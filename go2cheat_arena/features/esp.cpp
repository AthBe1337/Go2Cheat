
#include "esp.h"

void features::esp::draw_esp(bool render_friend) {

	//utils::math::ViewMatrix view_matrix = driver::read_memory<utils::math::ViewMatrix>(driver_handle, client_dll_base + offsets::client_dll::dwViewMatrix);
	utils::math::ViewMatrix view_matrix = driver::read_memory<utils::math::ViewMatrix>(driver_handle, client_dll_base + 0x1A73028);

	for (auto player : utils::Players::players) {
		if ((render_friend || player->is_enemy()) && player->get_health() > 0 && !player->is_local_player())
		{

			utils::math::Vector3 local_vec = utils::Players::player_instance::get_vec_origin(utils::Players::get_local_player_pawn());
			
			utils::math::Vector3 vec_origin = player->get_vec_origin();

			float distance = utils::math::GetDistance(vec_origin, local_vec);

			utils::math::Vector3 top = { vec_origin.x, vec_origin.y, vec_origin.z + 72.f };

			utils::math::Vector3 bottom = { vec_origin.x, vec_origin.y, vec_origin.z - 10.f };

			utils::math::Vector3 screen_head_pos;
			utils::math::Vector3 screen_feet_pos;
			if (utils::math::WTS(top, screen_head_pos, view_matrix) && utils::math::WTS(bottom, screen_feet_pos, view_matrix))
			{
				float height = screen_feet_pos.y - screen_head_pos.y;
				float width = height * 0.29f;

				//player name and weapon name
				ImGui::GetBackgroundDrawList()->AddText(NULL, width / 3, { screen_head_pos.x - width, screen_head_pos.y - width / 3 }, IM_COL32(255, 0, 0, 255), player->get_player_name().c_str());

				ImGui::GetBackgroundDrawList()->AddRectFilled({ screen_head_pos.x - width, screen_head_pos.y }, { screen_head_pos.x + width, screen_feet_pos.y }, ImColor(0.f, 0.f, 0.f, 0.1f));

				ImGui::GetBackgroundDrawList()->AddText(NULL, width / 2, { screen_head_pos.x - width + 2.0f, screen_head_pos.y - width / 2 + height}, IM_COL32(255, 255, 255, 255), player->get_weapon_name().c_str());


				ImGui::GetBackgroundDrawList()->AddRect({ screen_head_pos.x - width - 1.f, screen_head_pos.y - 1.f }, { screen_head_pos.x + width - 1.f, screen_feet_pos.y + 1.f }, ImColor(0.f, 0.f, 0.f));
				ImGui::GetBackgroundDrawList()->AddRect({ screen_head_pos.x - width, screen_feet_pos.y }, { screen_head_pos.x + width, screen_feet_pos.y }, ImColor(1.f, 0.f, 0.f));
				ImGui::GetBackgroundDrawList()->AddRect({ screen_head_pos.x - width + 1.f, screen_head_pos.y + 1.f }, { screen_head_pos.x + width + 1.f, screen_feet_pos.y + 1.f }, ImColor(0.f, 0.f, 0.f));

				int health = player->get_health();
				int armor = player->get_armor();
				if (health > 0) {
					ImGui::GetBackgroundDrawList()->AddLine({ screen_head_pos.x - width - 5.f, screen_head_pos.y + (100.f - (float)health) / 100.f * height }, { screen_head_pos.x - width - 5.f, screen_feet_pos.y - 1.f }, ImColor(0.0f, 1.0f, 0.f), 2.f);
				}
				if (armor > 0) {
					ImGui::GetBackgroundDrawList()->AddLine({ screen_head_pos.x - width - 10.f, screen_head_pos.y + (100.f - (float)armor) / 100.f * height }, { screen_head_pos.x - width - 10.f, screen_feet_pos.y - 1.f }, ImColor(0.0f, 0.0f, 1.0f), 2.f);
				}

				utils::math::Vector3 head, neck, spine, spine_1, left_shoulder, left_arm, left_hand, cock, right_shoulder, right_arm, right_hand, left_hip, left_knee, left_feet, right_hip, right_knee, right_feet;

				head = player->get_bone_position(utils::Players::head);
				neck = player->get_bone_position(utils::Players::neck);
				spine = player->get_bone_position(utils::Players::spine);
				spine_1 = player->get_bone_position(utils::Players::spine_1);
				left_shoulder = player->get_bone_position(utils::Players::left_shoulder);
				left_arm = player->get_bone_position(utils::Players::left_arm);
				left_hand = player->get_bone_position(utils::Players::left_hand);
				cock = player->get_bone_position(utils::Players::cock);
				right_shoulder = player->get_bone_position(utils::Players::right_shoulder);
				right_arm = player->get_bone_position(utils::Players::right_arm);
				right_hand = player->get_bone_position(utils::Players::right_hand);
				left_hip = player->get_bone_position(utils::Players::left_hip);
				left_knee = player->get_bone_position(utils::Players::left_knee);
				left_feet = player->get_bone_position(utils::Players::left_feet);
				right_hip = player->get_bone_position(utils::Players::right_hip);
				right_knee = player->get_bone_position(utils::Players::right_knee);
				right_feet = player->get_bone_position(utils::Players::right_feet);

				utils::math::Vector3 _head, _neck, _spine, _spine_1, _left_shoulder, _left_arm, _left_hand, _cock, _right_shoulder, _right_arm, _right_hand, _left_hip, _left_knee, _left_feet, _right_hip, _right_knee, _right_feet;

				if (utils::math::WTS(head, _head, view_matrix) &&
					utils::math::WTS(neck, _neck, view_matrix) &&
					utils::math::WTS(spine, _spine, view_matrix) &&
					utils::math::WTS(spine_1, _spine_1, view_matrix) &&
					utils::math::WTS(left_shoulder, _left_shoulder, view_matrix) &&
					utils::math::WTS(left_arm, _left_arm, view_matrix) &&
					utils::math::WTS(left_hand, _left_hand, view_matrix) &&
					utils::math::WTS(cock, _cock, view_matrix) &&
					utils::math::WTS(right_shoulder, _right_shoulder, view_matrix) &&
					utils::math::WTS(right_arm, _right_arm, view_matrix) &&
					utils::math::WTS(right_hand, _right_hand, view_matrix) &&
					utils::math::WTS(left_hip, _left_hip, view_matrix) &&
					utils::math::WTS(left_knee, _left_knee, view_matrix) &&
					utils::math::WTS(left_feet, _left_feet, view_matrix) &&
					utils::math::WTS(right_hip, _right_hip, view_matrix) &&
					utils::math::WTS(right_knee, _right_knee, view_matrix) &&
					utils::math::WTS(right_feet, _right_feet, view_matrix))
				{
					// 绘制骨骼线
					if (player->get_helmet())
						ImGui::GetBackgroundDrawList()->AddCircle({ _head.x, _head.y }, width / 4.0f, IM_COL32(255, 255, 255, 255));

					if(player->get_defuser())
						ImGui::GetBackgroundDrawList()->AddCircle({ _cock.x, _cock.y }, width / 6.f, IM_COL32(255, 255, 255, 255));

					ImGui::GetBackgroundDrawList()->AddLine({ _head.x, _head.y }, { _neck.x, _neck.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _neck.x, _neck.y }, { _spine.x, _spine.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _spine.x, _spine.y }, { _spine_1.x, _spine_1.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _spine_1.x, _spine_1.y }, { _cock.x, _cock.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _neck.x, _neck.y }, { _left_shoulder.x, _left_shoulder.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _left_shoulder.x, _left_shoulder.y }, { _left_arm.x, _left_arm.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _left_arm.x, _left_arm.y }, { _left_hand.x, _left_hand.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _neck.x, _neck.y }, { _right_shoulder.x, _right_shoulder.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _right_shoulder.x, _right_shoulder.y }, { _right_arm.x, _right_arm.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _right_arm.x, _right_arm.y }, { _right_hand.x, _right_hand.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _cock.x, _cock.y }, { _left_hip.x, _left_hip.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _left_hip.x, _left_hip.y }, { _left_knee.x, _left_knee.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _left_knee.x, _left_knee.y }, { _left_feet.x, _left_feet.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _cock.x, _cock.y }, { _right_hip.x, _right_hip.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _right_hip.x, _right_hip.y }, { _right_knee.x, _right_knee.y }, IM_COL32(255, 255, 255, 255), 2.0f);
					ImGui::GetBackgroundDrawList()->AddLine({ _right_knee.x, _right_knee.y }, { _right_feet.x, _right_feet.y }, IM_COL32(255, 255, 255, 255), 2.0f);

				}
			}
		}
	}
}

void features::esp::C4_info() {
	std::uintptr_t game_rules = driver::read_memory<std::uintptr_t>(driver_handle, client_dll_base + offsets::client_dll::dwGameRules);

	if (!game_rules)
		return;

	bool bomb_planted = driver::read_memory<byte>(driver_handle, game_rules + schemas::client_dll::C_CSGameRules::m_bBombPlanted) & 0x1;

	if (!bomb_planted)
		return;

	ImGui::GetBackgroundDrawList()->AddText(NULL, 30, { (float)screenW / 2 - 30, (float)screenH / 3 * 2.5f }, IM_COL32(255, 0, 0, 255), "Bomb planted!");

	std::uintptr_t plantedC4 = driver::read_memory<std::uintptr_t>(driver_handle, client_dll_base + offsets::client_dll::dwPlantedC4);

	if (!plantedC4)
		return;

	plantedC4 = driver::read_memory<std::uintptr_t>(driver_handle, plantedC4);

	if (!plantedC4)
		return;

	bool is_defusing = driver::read_memory<byte>(driver_handle, plantedC4 + schemas::client_dll::C_PlantedC4::m_bBeingDefused) & 0x1;

	if (!is_defusing)
		return;

	float blow_time = driver::read_memory<float>(driver_handle, plantedC4 + schemas::client_dll::C_PlantedC4::m_flC4Blow);

	float defuse_time = driver::read_memory<float>(driver_handle, plantedC4 + schemas::client_dll::C_PlantedC4::m_flDefuseCountDown);

	if(blow_time >= defuse_time)
		ImGui::GetBackgroundDrawList()->AddText(NULL, 30, { (float)screenW / 2 - 30, (float)screenH / 3 * 2.5f + 35}, IM_COL32(0, 255, 0, 255), "Defusing!");
	else
		ImGui::GetBackgroundDrawList()->AddText(NULL, 30, { (float)screenW / 2 - 30, (float)screenH / 3 * 2.5f + 35 }, IM_COL32(255, 0, 0, 255), "Defusing!");
}