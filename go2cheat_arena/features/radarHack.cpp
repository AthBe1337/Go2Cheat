#pragma once
#include "radarHack.h"


void features::radarHack::set_spotted_state() {

	if (driver_handle == INVALID_HANDLE_VALUE || !client_dll_base)
		return;

	for (auto player : utils::Players::players){

		std::uintptr_t player_pawn = player->get_player_pawn();	

		const std::uintptr_t spotted_state = player_pawn + schemas::client_dll::C_CSPlayerPawn::m_entitySpottedState;

		if (!spotted_state)
			continue;

		driver::write_memory(driver_handle, (spotted_state + schemas::client_dll::EntitySpottedState_t::m_bSpotted), true);
		driver::write_memory(driver_handle, (spotted_state + schemas::client_dll::EntitySpottedState_t::m_bSpottedByMask), 1);

	}

}