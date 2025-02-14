#pragma once

#include <cstdint>
#include "keybind.hpp"

namespace settings {
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	inline int screen_center_x = width / 2;
	inline int screen_center_y = height / 2;

	namespace caching {
		uintptr_t base_address = 0;
	}

	inline int tab = 0;

	namespace aimbot
	{
		inline bool enable = false;
		inline bool show_fov = false;
		inline bool fill_fov = false;
		inline int fill_opacity = 60;
		inline bool visible_check = false;

		inline float fov = 50;
		inline float smoothness_x = 1.55;
		inline float smoothness_y = 1.55;

		inline bool prediction = false;
		inline bool magic_bullet = false;
		inline bool enable_smoothness = false;
		inline bool closest_part = false;
		inline bool disable_outside_fov = false;

		inline CKeybind aimbotkey{ "aimkey" };

		inline int aimbot_type = 0;
		static const char* aimbot_types[1] = { "Camera" };

		inline int aimbot_part = 0;
		static const char* aimbot_parts[3] = { "Head", "Neck", "Torso" };
	}

	namespace triggerbot {
		inline bool enable_triggerbot = false;
		inline int triggerbot_delay = 1;
		inline int triggerbot_distance = 20;

		inline CKeybind triggerbot_key{ "tbkey" };
	}

	namespace visuals
	{
		inline bool enable = true;

		inline bool box = true;
		inline int box_type = 0;
		static const char* box_types[2] = { "2D Box", "2D Filled" };

		inline bool line = false;
		inline bool skeleton = true;
		inline bool team_check = true;
		inline bool show_crosshair = false;
		inline bool distance = true;
		inline bool username = false;
		inline bool FOVRadar = false;
		inline bool weapon = false;
		inline bool platform = false;
		inline bool rank = false;
	}

	namespace features
	{
		inline bool showtime = true;
		inline bool showfps = true;
	}

	namespace checks
	{
		inline bool death_check = false;
	}

	namespace exploits {
		inline bool magic_bullet = false;
		inline bool no_spread = false;
	}

	namespace overlay 
	{
		inline int particle_type = 1;
		static const char* particle_types[2] = { "Rain", "Snow" };

		inline bool show_menu = true;
	}

	namespace colors
	{
		ImColor crosshair_color = ImColor(255, 255, 255, 255);
	}
}
