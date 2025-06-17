#pragma once
#include "Players.h"



std::uintptr_t utils::Players::get_local_player_pawn() {
	if (!client_dll_base || driver_handle == INVALID_HANDLE_VALUE)
		return 0;
	std::uintptr_t local_player_pawn = driver::read_memory<std::uintptr_t>(driver_handle, client_dll_base + offsets::client_dll::dwLocalPlayerPawn);

	if (!local_player_pawn)
		return 0;

	return local_player_pawn;
}

bool utils::Players::player_instance::is_enemy() {
	if (!client_dll_base || driver_handle == INVALID_HANDLE_VALUE)
		return false;

	int local_player_team = 0;

	std::uintptr_t local_player_pawn = driver::read_memory<std::uintptr_t>(driver_handle, client_dll_base + offsets::client_dll::dwLocalPlayerPawn);

	if (!local_player_pawn)
		return false;

	local_player_team = driver::read_memory<int>(driver_handle, local_player_pawn + schemas::client_dll::C_BaseEntity::m_iTeamNum);

	if (local_player_team == this->team)
		return false;

	return true;
}

bool utils::Players::player_instance::is_local_player() {
	if (!client_dll_base || driver_handle == INVALID_HANDLE_VALUE)
		return false;

	std::uintptr_t local_player_pawn = get_local_player_pawn();

	if (!local_player_pawn || this->player_pawn != local_player_pawn)
		return false;

	return true;
}

void utils::Players::delete_player_by_id(int index) {
	if (players.empty())
		return;

	for (auto it = players.begin(); it != players.end(); ++it) {
		if ((*it)->get_index() == index) {
			delete* it;
			players.erase(it); 
			break; 
		}
	}
}

void utils::Players::refresh_player_list() {
	if (!client_dll_base || driver_handle == INVALID_HANDLE_VALUE)
		return;

	const std::uintptr_t entity_list = driver::read_memory<std::uintptr_t>(driver_handle, client_dll_base + offsets::client_dll::dwEntityList);

	if (!entity_list)
		return;

	for (int i = 1; i < 65; i++) {

		const std::uintptr_t list_entry = driver::read_memory<std::uintptr_t>(driver_handle, entity_list + 0x8 * (i >> 9) + 0x10);

		if (!list_entry)
			return;

		const std::uintptr_t player_controller = driver::read_memory<std::uintptr_t>(driver_handle, list_entry + 0x78 * (i & 0x1FF));
		if (!player_controller)
		{
			delete_player_by_id(i);
			continue;
		}

		const std::uintptr_t player_name_addr = driver::read_memory<std::uintptr_t>(driver_handle, player_controller + schemas::client_dll::CCSPlayerController::m_sSanitizedPlayerName);

		if (!player_name_addr)
		{
			delete_player_by_id(i);
			continue;
		}

		std::string player_name;

		for (int j = 0; j < 32; j++) {
			const char c = driver::read_memory<char>(driver_handle, player_name_addr + j);
			if (c == '\0')
				break;
			player_name += c;
		}
		if (player_name.empty())
		{
			delete_player_by_id(i);
			continue;
		}

		const std::uint32_t player_pawn_handle = driver::read_memory<std::uint32_t>(driver_handle, player_controller + schemas::client_dll::CCSPlayerController::m_hPlayerPawn);

		if (!player_pawn_handle)
		{
			delete_player_by_id(i);
			continue;
		}

		const std::uintptr_t list_entry2 = driver::read_memory<std::uintptr_t>(driver_handle, (entity_list + 0x8 * ((player_pawn_handle & 0x7FFF) >> 9) + 0x10));

		if (!list_entry2)
		{
			delete_player_by_id(i);
			continue;
		}

		const std::uintptr_t player_pawn = driver::read_memory<std::uintptr_t>(driver_handle, (list_entry2 + 0x78 * (player_pawn_handle & 0x1FF)));

		if (!player_pawn)
		{
			delete_player_by_id(i);
			continue;
		}

		const int player_health = driver::read_memory<int>(driver_handle, player_pawn + schemas::client_dll::C_BaseEntity::m_iHealth);

		const int player_armor = driver::read_memory<int>(driver_handle, player_pawn + schemas::client_dll::C_CSPlayerPawn::m_ArmorValue);

		std::uintptr_t item_services = driver::read_memory<std::uintptr_t>(driver_handle, player_pawn + schemas::client_dll::C_BasePlayerPawn::m_pItemServices);

		const bool has_helmet = driver::read_memory<int>(driver_handle, item_services + schemas::client_dll::CCSPlayer_ItemServices::m_bHasHelmet) & (1 << 0);

		const bool has_defuser = driver::read_memory<int>(driver_handle, item_services + schemas::client_dll::CCSPlayer_ItemServices::m_bHasDefuser) & (1 << 0);

		const int player_team = driver::read_memory<int>(driver_handle, player_pawn + schemas::client_dll::C_BaseEntity::m_iTeamNum);


		if (players.empty()) {
			players.push_back(new player_instance(player_controller, player_pawn, i, player_name, player_health, player_armor, has_helmet, has_defuser, player_team));
		}
		else {
			bool found = false;
			for (auto player : players) {
				if (player->get_index() == i) {
					player->set_player_controller(player_controller);
					player->set_player_pawn(player_pawn);
					player->set_index(i);
					player->set_player_name(player_name);
					player->set_health(player_health);
					player->set_armor(player_armor, has_helmet);
					player->set_defuser(has_defuser);
					player->set_team(player_team);
					found = true;
					break;
				}
			}
			if (!found) {
				players.push_back(new player_instance(player_controller, player_pawn, i, player_name, player_health, player_armor, has_helmet, has_defuser, player_team));
			}

		}

	}

}

utils::math::Vector3 utils::Players::player_instance::get_bone_position(bones bone) {
	std::uintptr_t game_sence = driver::read_memory<std::uintptr_t>(driver_handle, this->player_pawn + schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
	if (!game_sence)
		return NULL;

	std::uintptr_t bone_matrix = driver::read_memory<std::uintptr_t>(driver_handle, game_sence + schemas::client_dll::CSkeletonInstance::m_modelState + 0x80);
	if (!bone_matrix)
		return NULL;

	utils::math::Vector3 bone_pos = driver::read_memory<utils::math::Vector3>(driver_handle, bone_matrix + 0x20 * bone + 0x0);

	return bone_pos;
	
}

utils::math::Vector3 utils::Players::player_instance::get_vec_origin() {
	std::uintptr_t game_sence = driver::read_memory<std::uintptr_t>(driver_handle, this->player_pawn + schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
	if (!game_sence)
		return NULL;

	utils::math::Vector3 vec_origin = driver::read_memory<utils::math::Vector3>(driver_handle, game_sence + schemas::client_dll::CGameSceneNode::m_vecOrigin);

	return vec_origin;
}

utils::math::Vector3 utils::Players::player_instance::get_vec_origin(std::uintptr_t player_pawn) {
	std::uintptr_t game_sence = driver::read_memory<std::uintptr_t>(driver_handle, player_pawn + schemas::client_dll::C_BaseEntity::m_pGameSceneNode);
	if (!game_sence)
		return NULL;
	utils::math::Vector3 vec_origin = driver::read_memory<utils::math::Vector3>(driver_handle, game_sence + schemas::client_dll::CGameSceneNode::m_vecOrigin);
	return vec_origin;
}

utils::Players::player_instance* utils::Players::get_player_by_id(int index) {
	if (utils::Players::players.empty())
		return nullptr;
	for (auto player : utils::Players::players) {
		if (player->get_index() == index) {
			return player;
		}
	}
	return nullptr;
}

utils::Players::player_instance* utils::Players::get_player_by_controller(std::uintptr_t controller) {
	if (utils::Players::players.empty())
		return nullptr;
	for (auto player : utils::Players::players) {
		if (player->get_player_controller() == controller) {
			return player;
		}
	}
	return nullptr;
}

utils::Players::player_instance* utils::Players::get_player_by_pawn(std::uintptr_t pawn) {
	if (utils::Players::players.empty())
		return nullptr;
	for (auto player : utils::Players::players) {
		if (player->get_player_pawn() == pawn) {
			return player;
		}
	}
	return nullptr;
}

std::string  utils::Players::player_instance::get_weapon_name() {
	std::uintptr_t clipping_weapon = driver::read_memory<std::uintptr_t>(driver_handle, this->get_player_pawn() + schemas::client_dll::C_CSPlayerPawnBase::m_pClippingWeapon);

	if (!clipping_weapon)
		return "Unknown";

	std::uintptr_t weapon_ = driver::read_memory<std::uintptr_t>(driver_handle, clipping_weapon + 0x380 );

	if (!weapon_)
		return "Unknown";

	std::uintptr_t name_addr = driver::read_memory<std::uintptr_t>(driver_handle, weapon_ + schemas::client_dll::CCSWeaponBaseVData::m_szName);

	if (!name_addr)
		return "Unknown";

	std::string weapon_name;

	for (int i = 0; i < 32; i++) {
		const char c = driver::read_memory<char>(driver_handle, name_addr + i);
		if (c == '\0')
			break;
		weapon_name += c;
	}

	if (weapon_name.empty())
		return "Unknown";

	if (weapon_name == "weapon_glock")
		return "Glock-18";
	else if (weapon_name == "weapon_m4a1_silencer")
		return "M4A1-S";
	else if (weapon_name == "weapon_usp_silencer")
		return "USP-S";
	else if (weapon_name == "weapon_hkp2000")
		return "hkp2000";
	else if (weapon_name == "weapon_ak47")
		return "AK47";
	else if (weapon_name == "weapon_m249")
		return "M249";
	else if (weapon_name == "weapon_cz75a")
		return "CZ75";
	else if (weapon_name == "weapon_mp7")
		return "MP7";
	else if (weapon_name == "weapon_mag7")
		return "MAG7";
	else if (weapon_name == "weapon_deagle")
		return "deagle";
	else if (weapon_name == "weapon_xm1014")
		return "XM1014";
	else if (weapon_name == "weapon_nova")
		return "NOVA";
	else if (weapon_name == "weapon_sg556")
		return "SG556";
	else if (weapon_name == "weapon_awp")
		return "AWP";
	else if (weapon_name == "weapon_p90")
		return "P90";
	else if (weapon_name == "weapon_mac10")
		return "MAC10";
	else if (weapon_name == "weapon_mp9")
		return "MP9";
	else if (weapon_name == "weapon_aug")
		return "AUG";
	else if (weapon_name == "weapon_galilar")
		return "Galil-AR";
	else if (weapon_name == "weapon_ssg08")
		return "SSG08";
	else if (weapon_name == "weapon_famas")
		return "FAMAS";
	else
		return weapon_name;
}