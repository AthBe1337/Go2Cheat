#pragma once
#include <iostream>
#include <vector>

#include "offsets.h"
#include "../driver.h"
#include "math.h"

using namespace cs2_dumper;

namespace utils {
	namespace Players {
		std::uintptr_t get_local_player_pawn();

		enum bones : int {
			head = 6,
			neck = 5,
			spine = 4,
			spine_1 = 2,
			left_shoulder = 8,
			left_arm = 9,
			left_hand = 11,
			cock = 0,
			right_shoulder = 13,
			right_arm = 14,
			right_hand = 16,
			left_hip = 22,
			left_knee = 23,
			left_feet = 24,
			right_hip = 25,
			right_knee = 26,
			right_feet = 27

		};

		class player_instance {

		private:
			std::uintptr_t player_controller;
			std::uintptr_t player_pawn;
			int index;
			std::string player_name;
			int health;
			int armor;
			bool has_helmet;
			bool has_defuser;
			int team;

		public:
			player_instance(std::uintptr_t player_controller, std::uintptr_t player_pawn, int index, std::string player_name, int health, int armor, bool has_helmet, bool has_defuser, int team) {
				this->player_controller = player_controller;
				this->player_pawn = player_pawn;
				this->index = index;
				this->player_name = player_name;
				this->health = health;
				this->armor = armor;
				this->has_helmet = has_helmet;
				this->has_defuser = has_defuser;
				this->team = team;
			}
			std::uintptr_t get_player_controller() {
				return this->player_controller;
			}
			std::uintptr_t get_player_pawn() {
				return this->player_pawn;
			}
			int get_index() {
				return this->index;
			}
			std::string get_player_name() {
				return this->player_name;
			}
			int get_health() {
				return this->health;
			}
			int get_armor() {
				return this->armor;
			}
			int get_team() {
				return this->team;
			}
			bool get_helmet() {
				return this->has_helmet;
			}
			bool get_defuser() {
				return this->has_defuser;
			}
			void set_player_controller(std::uintptr_t player_controller) {
				this->player_controller = player_controller;
			}
			void set_player_pawn(std::uintptr_t player_pawn) {
				this->player_pawn = player_pawn;
			}
			void set_index(int index) {
				this->index = index;
			}
			void set_player_name(std::string player_name) {
				this->player_name = player_name;
			}
			void set_health(int health) {
				this->health = health;
			}
			void set_armor(int amor, bool has_helmet) {
				this->armor = amor;
				this->has_helmet = has_helmet;
			}
			void set_team(int team) {
				this->team = team;
			}
			void set_defuser(bool has_defuser) {
				this->has_defuser = has_defuser;
			}

			bool is_local_player();

			bool is_enemy();

			utils::math::Vector3 get_bone_position(bones bones);

			utils::math::Vector3 get_vec_origin();

			static utils::math::Vector3 get_vec_origin(std::uintptr_t player_pawn);

			std::string get_weapon_name();

		};

		inline std::vector<player_instance*> players;

		void delete_player_by_id(int index);

		player_instance* get_player_by_id(int index);

		player_instance* get_player_by_controller(std::uintptr_t controller);

		player_instance* get_player_by_pawn(std::uintptr_t pawn);

		void refresh_player_list();
	}
}