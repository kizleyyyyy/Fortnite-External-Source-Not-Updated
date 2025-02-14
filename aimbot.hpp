#pragma once

#include <Windows.h>

#include "render.hpp"
#include "sdk.hpp"
#include "settings.hpp"

#include "spoof.h"

void aimbot(uintptr_t pawn_private)
{
	hook_mouse->HOOK();

	uintptr_t target_mesh = read<uintptr_t>(pawn_private + MESH);

	if (!target_mesh)
		return;

	if (settings::aimbot::magic_bullet)
	{
		//auto TraceThroughtWorld = 0xe1d;
		//auto TraceThroughtBuildingLimit = 0xe7c;
		//auto TraceThroughtLandscapeLimit = 0xe80;
		//auto ShouldForceCheckCollisions = 0x7ca;
		//auto CheckCollisionsWithoutAbility = 0x38b;

		//uintptr_t CurrentWeapon = read<uintptr_t>( 0x350  + 0xa68);    //0x350 acknowledgedpawn


		//write<bool>(CurrentWeapon + TraceThroughtWorld, true);

		//write<int32_t>(CurrentWeapon + TraceThroughtBuildingLimit, 5);

		//write<int32_t>(CurrentWeapon + TraceThroughtLandscapeLimit, 5);

		//write<bool>(CurrentWeapon + ShouldForceCheckCollisions, true);

		//write<bool>(CurrentWeapon + CheckCollisionsWithoutAbility, true);
	}

	if (settings::aimbot::visible_check)
	{
		if (!is_visible(target_mesh))
			return;

		if (!read<uintptr_t>(cache::player_controller + 0x30))
			return;
	}

	Vector3 head3d{ };

	switch (settings::aimbot::aimbot_part)
	{
	case 0:
		head3d = get_entity_bone(target_mesh, 110);
		break;
	case 1:
		head3d = get_entity_bone(target_mesh, 67);
		break;
	case 2:
		head3d = get_entity_bone(target_mesh, 37);
		break;
	}

	Vector2 head2d = project_world_to_screen(head3d);

	Vector2 relative{ };

	if (settings::aimbot::enable_smoothness)
	{
		relative.x = (head2d.x - settings::width / 2) * 1 / settings::aimbot::smoothness_x;
		relative.y = (head2d.y - settings::height / 2) * 1 / settings::aimbot::smoothness_y;
	}
	else
	{
		relative.x = head2d.x - settings::width / 2;
		relative.y = head2d.y - settings::height / 2;
	}

	MOVE_MOUSE->Move(relative.x, relative.y); // moves the mouse to our relative position of the player

	cache::closest_distance = FLT_MAX;
	cache::closest_pawn = NULL;
}
