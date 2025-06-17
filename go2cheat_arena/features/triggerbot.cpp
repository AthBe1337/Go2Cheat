#include "triggerbot.h"

void features::triggerbot::trigger() {
	if (!client_dll_base || driver_handle == INVALID_HANDLE_VALUE || utils::Players::players.empty())
		return;

	std::uintptr_t local_player_pawn = utils::Players::get_local_player_pawn();

	int crosshair_id = driver::read_memory<int>(driver_handle, local_player_pawn + schemas::client_dll::C_CSPlayerPawnBase::m_iIDEntIndex);

	std::uintptr_t entity_list = driver::read_memory<std::uintptr_t>(driver_handle, client_dll_base + offsets::client_dll::dwEntityList);

	std::uintptr_t list_entry = driver::read_memory<std::uintptr_t>(driver_handle, entity_list + 0x8 * (crosshair_id >> 9) + 0x10);

	std::uintptr_t base_entity = driver::read_memory<std::uintptr_t>(driver_handle, list_entry + 0x78 * (crosshair_id & 0x1FF));
	//it works
	
	utils::Players::player_instance* player = utils::Players::get_player_by_pawn(base_entity);

	if (!player)
		return;

	if (player->get_health() > 0 && player->is_enemy()) {

		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		Sleep(10);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		normal_print(FOREGROUND_RED | FOREGROUND_GREEN, "自动扳机已触发，目标: %s", player->get_player_name().c_str());
	}

}