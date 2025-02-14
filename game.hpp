#pragma once

#include <Windows.h>

#include "render.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "sdk.hpp"
#include "render.hpp"
#include "settings.hpp"
#include "spoof.h"
#include "aimbot.hpp"

ImVec4 visible_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);   // Green (RGBA format)
ImVec4 not_visible_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red (RGBA format)

ImVec4 visible_skeleton_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);    // Default green for visible skeleton
ImVec4 not_visible_skeleton_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Default red for non-visible skeleton

static float regular_box_thickness = 1.5f; // Default thickness for regular box
float skeleton_thickness = 1.0f; // Default thickness for the skeleton


#define IMGUI_DEFINE_MATH_OPERATORS

static auto cr3loop() -> void
{
	if (cr3 != mem::fetch_cr3())
	{
		cr3 = mem::fetch_cr3();
	}
}

void addToFovRadar(Vector3 WorldLocation, ImVec2 drawPos, float FOVScale, ImColor color) {

	Vector3 vAngle = cache::local_camera.rotation;
	float fYaw = vAngle.y * 3.14159265358979323846 / 180.0f;
	float dx = WorldLocation.x - cache::local_camera.location.x;
	float dy = WorldLocation.y - cache::local_camera.location.y;
	float fsin_yaw = sinf(fYaw);
	float fminus_cos_yaw = -cosf(fYaw);

	float x = -(dy * fminus_cos_yaw + dx * fsin_yaw);
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;

	float fovRadius = FOVScale;
	float angle = atan2f(y, x);
	float triangleSize = 12.0f;
	float widthFactor = 8.0f;
	float outlineThickness = 2.0f;

	ImVec2 triangleCenter = ImVec2(drawPos.x + cosf(angle) * (fovRadius + triangleSize),
		drawPos.y + sinf(angle) * (fovRadius + triangleSize));

	ImVec2 point1 = ImVec2(triangleCenter.x + cosf(angle) * triangleSize,
		triangleCenter.y + sinf(angle) * triangleSize);
	ImVec2 point2 = ImVec2(triangleCenter.x + cosf(angle + widthFactor) * triangleSize,
		triangleCenter.y + sinf(angle + widthFactor) * triangleSize);
	ImVec2 point3 = ImVec2(triangleCenter.x + cosf(angle - widthFactor) * triangleSize,
		triangleCenter.y + sinf(angle - widthFactor) * triangleSize);

	ImU32 outlineColor = ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 1.0));

	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	drawList->AddTriangleFilled(point1, point2, point3, color);
	drawList->AddTriangle(point1, point2, point3, outlineColor, outlineThickness);
}

void draw_regular_box(int x, int y, int w, int h, uintptr_t mesh)
{
	// Determine visibility using the is_visible function
	bool visible = is_visible(mesh);

	// Set color based on visibility
	ImColor color = visible ? visible_color : not_visible_color;

	// Outline thickness is slightly larger than the regular box thickness
	float outline_thickness = regular_box_thickness + 1.0f;

	// Draw the black outline (slightly larger than the box)
	ImGui::GetBackgroundDrawList()->AddRect(
		ImVec2(x, y), ImVec2(x + w, y + h),
		ImColor(0, 0, 0), 0, 0, outline_thickness
	);

	// Draw the regular box with the adjustable thickness
	ImGui::GetBackgroundDrawList()->AddRect(
		ImVec2(x, y), ImVec2(x + w, y + h),
		color, 0, 0, regular_box_thickness
	);
}



//void draw_cornered_box(int x, int y, int w, int h, uintptr_t mesh)
//{
//	// Check if the enemy is visible using the is_visible function
//	bool visible = is_visible(mesh);
//
//	// Set the box color based on visibility (green if visible, red if not visible)
//	ImColor color = visible ? ImColor(0, 250, 0) : ImColor(250, 0, 0);  // Green if visible, red if not visible
//
//	// Black outline
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), ImColor(0, 0, 0), 2);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), ImColor(0, 0, 0), 2);
//
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), ImColor(0, 0, 0), 2);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), ImColor(0, 0, 0), 2);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), ImColor(0, 0, 0), 2);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), ImColor(0, 0, 0), 2);
//
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), ImColor(0, 0, 0), 2);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), ImColor(0, 0, 0), 2);
//
//	// Box color (changes with visibility)
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, 0.50);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, 0.50);
//
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, 0.50);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, 0.50);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, 0.50);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, 0.50);
//
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, 0.50);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, 0.50);
//}

//void draw_bounding_cornered_box(const Vector3& position, const Vector3& size, float thickness)
//{
//	float x = position.x;
//	float y = position.y;
//	float w = size.x;
//	float h = size.y;
//
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), ImColor(0, 0, 0), 2);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), ImColor(0, 0, 0), 2);
//
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), ImColor(0, 0, 0), 2);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), ImColor(0, 0, 0), 2);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), ImColor(0, 0, 0), 2);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), ImColor(0, 0, 0), 2);
//
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), ImColor(0, 0, 0), 2);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), ImColor(0, 0, 0), 2);
//
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), ImColor(250, 250, 250), thickness);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), ImColor(250, 250, 250), thickness);
//
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), ImColor(250, 250, 250), thickness);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), ImColor(250, 250, 250), thickness);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), ImColor(250, 250, 250), thickness);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), ImColor(250, 250, 250), thickness);
//
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), ImColor(250, 250, 250), thickness);
//	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), ImColor(250, 250, 250), thickness);
//}
//
//void draw_bounding_box(const Vector3& position, const Vector3& size, float thickness)
//{
//	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(position.x, position.y), ImVec2(size.x + position.x, size.y + position.y), ImColor(0, 0, 0), 0.0f, 0, 2);
//	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(position.x, position.y), ImVec2(size.x + position.x, size.y + position.y), ImColor(250, 250, 250), 0.0f, 0, thickness);
//}

void draw_filled_rect(int x, int y, int w, int h, uintptr_t mesh)
{
	// Check if the enemy is visible
	bool visible = is_visible(mesh);

	// Set the color based on visibility using global variables
	ImVec4 color = visible ? visible_color : not_visible_color;

	// Adjust alpha for a translucent effect (optional)
	ImVec4 color_with_alpha = ImVec4(color.x, color.y, color.z, 0.3f); // Use 30% opacity

	// Draw the filled rectangle with the determined color
	ImGui::GetBackgroundDrawList()->AddRectFilled(
		ImVec2(x, y), ImVec2(x + w, y + h),
		ImGui::ColorConvertFloat4ToU32(color_with_alpha),
		0, 0
	);
}


void draw_line(Vector2 location)
{
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(settings::width / 2, settings::height), ImVec2(location.x, location.y), ImColor(0, 0, 0), 0.5f);
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(settings::width / 2, settings::height), ImVec2(location.x, location.y), ImColor(250, 250, 250), 0.2f);
}

void draw_player_name(Vector2 location, std::string player_name)
{
	location.y -= 20;

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_name.c_str()).x / 2 + -1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_name.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_name.c_str()).x / 2 + 1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_name.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_name.c_str()).x / 2 + -1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_name.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_name.c_str()).x / 2 + 1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_name.c_str());

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_name.c_str()).x / 2, location.y), ImColor(0, 0, 0), player_name.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_name.c_str()).x / 2, location.y), ImColor(250, 250, 250), player_name.c_str());
}

void draw_player_platform(Vector2 location, std::string player_platform)
{
	if (!settings::visuals::username)
	{
		location.y -= 20;

		ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_platform.c_str()).x / 2 + -1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_platform.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_platform.c_str()).x / 2 + 1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_platform.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_platform.c_str()).x / 2 + -1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_platform.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_platform.c_str()).x / 2 + 1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_platform.c_str());

		ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_platform.c_str()).x / 2, location.y), ImColor(250, 250, 250), player_platform.c_str());
	}
	else
	{
		location.y -= 50;

		ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_platform.c_str()).x / 2 + -1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_platform.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_platform.c_str()).x / 2 + 1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_platform.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_platform.c_str()).x / 2 + -1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_platform.c_str());
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_platform.c_str()).x / 2 + 1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), player_platform.c_str());

		ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(player_platform.c_str()).x / 2, location.y), ImColor(250, 250, 250), player_platform.c_str());
	}
}

enum class EFortRarity : uint8_t
{
	EFortRarity__Common = 0,
	EFortRarity__Uncommon = 1,
	EFortRarity__Rare = 2,
	EFortRarity__Epic = 3,
	EFortRarity__Legendary = 4,
	EFortRarity__Mythic = 5,
	EFortRarity__Transcendent = 6,
	EFortRarity__Unattainable = 7,
	EFortRarity__NumRarityValues = 8,
};

inline ImVec4 get_loot_color(EFortRarity tier) {
	if (tier == EFortRarity::EFortRarity__Common)
		return ImVec4(180 / 255.0f, 180 / 255.0f, 180 / 255.0f, 1.f);
	else if (tier == EFortRarity::EFortRarity__Uncommon)
		return ImVec4(100 / 255.0f, 200 / 255.0f, 50 / 255.0f, 1.f);
	else if (tier == EFortRarity::EFortRarity__Rare)
		return ImVec4(50 / 255.0f, 150 / 255.0f, 255 / 255.0f, 1.f);
	else if (tier == EFortRarity::EFortRarity__Epic)
		return ImVec4(255 / 255.0f, 100 / 255.0f, 255 / 255.0f, 1.f);
	else if (tier == EFortRarity::EFortRarity__Legendary)
		return ImVec4(255 / 255.0f, 180 / 255.0f, 50 / 255.0f, 1.f);
	else if (tier == EFortRarity::EFortRarity__Mythic)
		return ImVec4(255 / 255.0f, 220 / 255.0f, 80 / 255.0f, 1.f);
	else if (tier == EFortRarity::EFortRarity__Transcendent)
		return ImVec4(50 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.f);

	return ImVec4(180 / 255.0f, 180 / 255.0f, 180 / 255.0f, 1.f);
}

void draw_player_weapon(Vector2 location, std::string weapon_name, uint64_t current_weapon, uint64_t weapon_data, EFortRarity tier)
{
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(weapon_name.c_str()).x / 2 + -1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), weapon_name.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(weapon_name.c_str()).x / 2 + 1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), weapon_name.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(weapon_name.c_str()).x / 2 + -1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), weapon_name.c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(weapon_name.c_str()).x / 2 + 1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), weapon_name.c_str());

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(weapon_name.c_str()).x / 2, location.y), ImGui::ColorConvertFloat4ToU32(get_loot_color(tier)), weapon_name.c_str());
}

std::string getRank(int tier) {
	switch (tier) {
	case 0:  return "Unranked";
	case 1:  return "Bronze 2";
	case 2:  return "Bronze 3";
	case 3:  return "Silver 1";
	case 4:  return "Silver 2";
	case 5:  return "Silver 3";
	case 6:  return "Gold 1";
	case 7:  return "Gold 2";
	case 8:  return "Gold 3";
	case 9:  return "Platinum 1";
	case 10: return "Platinum 2";
	case 11: return "Platinum 3";
	case 12: return "Diamond 1";
	case 13: return "Diamond 2";
	case 14: return "Diamond 3";
	case 15: return "Elite";
	case 16: return "Champion";
	case 17: return "Unreal";
	default: return "Unranked";
	}
}

ImVec4 getRankColor(int tier) {
	switch (tier) {
	case 0:  return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);    // Unranked
	case 1:
	case 2:  return ImVec4(0.902f, 0.580f, 0.227f, 1.0f); // Bronze
	case 3:
	case 4:
	case 5:  return ImVec4(0.843f, 0.843f, 0.843f, 1.0f); // Silver
	case 6:
	case 7:
	case 8:  return ImVec4(1.0f, 0.871f, 0.0f, 1.0f); // Gold
	case 9:
	case 10:
	case 11: return ImVec4(0.0f, 0.7f, 0.7f, 1.0f);  // Platinum
	case 12:
	case 13:
	case 14: return ImVec4(0.1686f, 0.3294f, 0.8235f, 1.0f); // Diamond
	case 15: return ImVec4(0.7f, 0.7f, 0.7f, 1.0f);   // Elite
	case 16: return ImVec4(1.0f, 0.6f, 0.0f, 1.0f);   // Champion
	case 17: return ImVec4(0.6f, 0.0f, 0.6f, 1.0f);   // Unreal
	default: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);    // Unranked
	}
}

void draw_player_rank(Vector2 location, int tier)
{
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(getRank(tier).c_str()).x / 2 + -1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), getRank(tier).c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(getRank(tier).c_str()).x / 2 + 1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), getRank(tier).c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(getRank(tier).c_str()).x / 2 + -1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), getRank(tier).c_str());
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(getRank(tier).c_str()).x / 2 + 1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), getRank(tier).c_str());

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(getRank(tier).c_str()).x / 2, location.y), ImGui::ColorConvertFloat4ToU32(getRankColor(tier)), getRank(tier).c_str());
}

void draw_distance(Vector2 location, float distance)
{
	char dist[64];
	sprintf_s(dist, "(%.fm)", distance);

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(dist).x / 2 + -1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), dist);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(dist).x / 2 + 1, location.y + -1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), dist);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(dist).x / 2 + -1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), dist);
	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(dist).x / 2 + 1, location.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), dist);

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(location.x - ImGui::CalcTextSize(dist).x / 2, location.y), ImColor(250, 250, 250), dist);
}

void draw_line(Vector2 start, Vector2 end, ImU32 color, float thickness)
{
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), color, thickness);
}

void draw_skeleton(uintptr_t mesh)
{
	std::vector<Vector3> bones = {
		get_entity_bone(mesh, 66),  // neck
		get_entity_bone(mesh, 9),   // left shoulder
		get_entity_bone(mesh, 10),  // left elbow
		get_entity_bone(mesh, 11),  // left hand
		get_entity_bone(mesh, 38),  // right shoulder
		get_entity_bone(mesh, 39),  // right elbow
		get_entity_bone(mesh, 40),  // right hand
		get_entity_bone(mesh, 2),   // pelvis
		get_entity_bone(mesh, 71),  // left hip
		get_entity_bone(mesh, 72),  // left knee
		get_entity_bone(mesh, 78),  // right hip
		get_entity_bone(mesh, 79),  // right knee
		get_entity_bone(mesh, 75),  // left foot
		get_entity_bone(mesh, 82),  // right foot
		get_entity_bone(mesh, 110)  // head
	};

	std::vector<Vector2> screenPositions(bones.size());
	for (size_t i = 0; i < bones.size(); ++i) {
		screenPositions[i] = project_world_to_screen(bones[i]);
	}

	// Check if the player is visible
	bool visible = is_visible(mesh);

	// Set skeleton color based on visibility
	ImVec4 skeletonColor = visible ? visible_skeleton_color : not_visible_skeleton_color;

	// Outline thickness slightly larger than skeleton thickness
	float outline_thickness = skeleton_thickness + 1.0f;

	// Draw outline (black color for better visibility)
	draw_line(screenPositions[1], screenPositions[4], ImColor(0, 0, 0), outline_thickness);   // left shoulder to right shoulder
	draw_line(screenPositions[1], screenPositions[2], ImColor(0, 0, 0), outline_thickness);   // left shoulder to left elbow
	draw_line(screenPositions[2], screenPositions[3], ImColor(0, 0, 0), outline_thickness);   // left elbow to left hand
	draw_line(screenPositions[4], screenPositions[5], ImColor(0, 0, 0), outline_thickness);   // right shoulder to right elbow
	draw_line(screenPositions[5], screenPositions[6], ImColor(0, 0, 0), outline_thickness);   // right elbow to right hand
	draw_line(screenPositions[0], screenPositions[7], ImColor(0, 0, 0), outline_thickness);   // neck to pelvis
	draw_line(screenPositions[7], screenPositions[8], ImColor(0, 0, 0), outline_thickness);   // pelvis to left hip
	draw_line(screenPositions[8], screenPositions[9], ImColor(0, 0, 0), outline_thickness);   // left hip to left knee
	draw_line(screenPositions[9], screenPositions[12], ImColor(0, 0, 0), outline_thickness);  // left knee to left foot
	draw_line(screenPositions[7], screenPositions[10], ImColor(0, 0, 0), outline_thickness);  // pelvis to right hip
	draw_line(screenPositions[10], screenPositions[11], ImColor(0, 0, 0), outline_thickness); // right hip to right knee
	draw_line(screenPositions[11], screenPositions[13], ImColor(0, 0, 0), outline_thickness); // right knee to right foot

	// Draw skeleton with adjustable thickness
	draw_line(screenPositions[1], screenPositions[4], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness);   // left shoulder to right shoulder
	draw_line(screenPositions[1], screenPositions[2], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness);   // left shoulder to left elbow
	draw_line(screenPositions[2], screenPositions[3], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness);   // left elbow to left hand
	draw_line(screenPositions[4], screenPositions[5], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness);   // right shoulder to right elbow
	draw_line(screenPositions[5], screenPositions[6], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness);   // right elbow to right hand
	draw_line(screenPositions[0], screenPositions[7], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness);   // neck to pelvis
	draw_line(screenPositions[7], screenPositions[8], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness);   // pelvis to left hip
	draw_line(screenPositions[8], screenPositions[9], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness);   // left hip to left knee
	draw_line(screenPositions[9], screenPositions[12], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness);  // left knee to left foot
	draw_line(screenPositions[7], screenPositions[10], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness);  // pelvis to right hip
	draw_line(screenPositions[10], screenPositions[11], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness); // right hip to right knee
	draw_line(screenPositions[11], screenPositions[13], ImGui::ColorConvertFloat4ToU32(skeletonColor), skeleton_thickness); // right knee to right foot
}


struct fbox_sphere_bounds final
{
public:
	struct Vector3       orgin;             // 0x0000(0x0018)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	struct Vector3       box_extent;        // 0x0018(0x0018)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	double                       sphere_radius;     // 0x0030(0x0008)(Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

struct box_t {
	Vector3 min, max, size;
};

#include <array>
#include <algorithm>

bool get_bounding_box(uintptr_t mesh, box_t& box) {
	fbox_sphere_bounds bounds = read<fbox_sphere_bounds>(mesh + 0xE8);

	bounds.box_extent *= 2.0f;
	bounds.orgin -= bounds.box_extent / 2.f;

	std::array<Vector3, 8> points = {
		Vector3(bounds.orgin.x, bounds.orgin.y, bounds.orgin.z),
		Vector3(bounds.orgin.x, bounds.orgin.y, bounds.orgin.z + bounds.box_extent.z),

		Vector3(bounds.orgin.x + bounds.box_extent.x, bounds.orgin.y, bounds.orgin.z + bounds.box_extent.z),
		Vector3(bounds.orgin.x + bounds.box_extent.x, bounds.orgin.y, bounds.orgin.z),
		Vector3(bounds.orgin.x + bounds.box_extent.x, bounds.orgin.y + bounds.box_extent.y, bounds.orgin.z),

		Vector3(bounds.orgin.x, bounds.orgin.y + bounds.box_extent.y, bounds.orgin.z),
		Vector3(bounds.orgin.x, bounds.orgin.y + bounds.box_extent.y, bounds.orgin.z + bounds.box_extent.z),

		Vector3(bounds.orgin.x + bounds.box_extent.x, bounds.orgin.y + bounds.box_extent.y, bounds.orgin.z + bounds.box_extent.z)
	};

	box.min = { DBL_MAX, DBL_MAX, DBL_MAX };
	box.max = { -DBL_MAX, -DBL_MAX, -DBL_MAX };

	std::array<Vector2, 8> screen{};

	auto custom_min = [](double a, double b) { return a < b ? a : b; };
	auto custom_max = [](double a, double b) { return a > b ? a : b; };

	for (auto i = 0; i < points.size(); i++) {
		screen.at(i) = project_world_to_screen(points.at(i));

		box.min.x = custom_min(box.min.x, screen.at(i).x);
		box.min.y = custom_min(box.min.y, screen.at(i).y);

		box.max.x = custom_max(box.max.x, screen.at(i).x);
		box.max.y = custom_max(box.max.y, screen.at(i).y);
	}

	box.size = box.min - box.max;

	return true;
}

void game_loop()
{
	cache::uworld = read<uintptr_t>(settings::caching::base_address + UWORLD);
	cache::game_instance = read<uintptr_t>(cache::uworld + GAME_INSTANCE);
	cache::local_players = read<uintptr_t>(read<uintptr_t>(cache::game_instance + LOCAL_PLAYERS));
	cache::player_controller = read<uintptr_t>(cache::local_players + PLAYER_CONTROLLER);
	cache::local_pawn = read<uintptr_t>(cache::player_controller + LOCAL_PAWN);

	if (cache::local_pawn != 0)
	{
		cache::root_component = read<uintptr_t>(cache::local_pawn + ROOT_COMPONENT);
		cache::relative_location = read<Vector3>(cache::root_component + RELATIVE_LOCATION);
		cache::player_state = read<uintptr_t>(cache::local_pawn + PLAYER_STATE);
		cache::my_team_id = read<int>(cache::player_state + TEAM_INDEX);
	}

	cache::current_weapon = read<uintptr_t>(cache::local_pawn + CURRENT_WEAPON);
	cache::game_state = read<uintptr_t>(cache::uworld + GAME_STATE);
	cache::player_array = read<uintptr_t>(cache::game_state + PLAYER_ARRAY);
	cache::player_count = read<int>(cache::game_state + (PLAYER_ARRAY + sizeof(uintptr_t)));

	cache::relative_location = read<Vector3>(cache::root_component + RELATIVE_LOCATION);

	cache::closest_distance = FLT_MAX;
	cache::closest_pawn = NULL;

	/*std::cout << "UWorld: " << cache::uworld << std::endl;
	std::cout << "Game Instance: " << cache::game_instance << std::endl;
	std::cout << "Local Players: " << cache::local_players << std::endl;
	std::cout << "Player Controller: " << cache::player_controller << std::endl;
	std::cout << "Local Pawn: " << cache::local_pawn << std::endl;

	if (cache::local_pawn != 0) {
		std::cout << "Root Component: " << cache::root_component << std::endl;
		std::cout << "Relative Location: (" << cache::relative_location.x << ", "
			<< cache::relative_location.y << ", "
			<< cache::relative_location.z << ")" << std::endl;
		std::cout << "Player State: " << cache::player_state << std::endl;
		std::cout << "Team ID: " << cache::my_team_id << std::endl;
	}

	std::cout << "Current Weapon: " << cache::current_weapon << std::endl;
	std::cout << "Game State: " << cache::game_state << std::endl;
	std::cout << "Player Array: " << cache::player_array << std::endl;
	std::cout << "Player Count: " << cache::player_count << std::endl;*/

	for (int i = 0; i < cache::player_count; i++)
	{
		uintptr_t player_state = read<uintptr_t>(cache::player_array + (i * sizeof(uintptr_t)));
		if (!player_state)
			continue;

		int my_team_id = read<int>(cache::player_state + TEAM_INDEX);
		int player_team_id = read<int>(player_state + TEAM_INDEX);

		if (my_team_id && player_team_id == my_team_id)
			continue;

		uintptr_t pawn_private = read<uintptr_t>(player_state + PAWN_PRIVATE);
		if (!pawn_private || pawn_private == cache::local_pawn)
			continue;

		uintptr_t mesh = read<uintptr_t>(pawn_private + MESH);
		if (!mesh)
			continue;

		Vector3 head3d = get_entity_bone(mesh, 110);
		Vector2 head2d = project_world_to_screen(Vector3(head3d.x, head3d.y, head3d.z + 17));

		Vector3 bottom3d = get_entity_bone(mesh, 0);
		Vector2 bottom2d = project_world_to_screen(Vector3(bottom3d.x, bottom3d.y, bottom3d.z - 10));

		float box_height = abs(head2d.y - bottom2d.y);
		float box_width = box_height * 0.50f;
		float distance = cache::relative_location.distance(bottom3d) / 100;

		visible_cached vc_local = {};
		float last_render_time_on_screen = read<float>(mesh + LAST_SUBMIT_TIME_ON_SCREEN);
		vc_local.last_submit_time_on_screen = last_render_time_on_screen;
		vc_local.mesh = mesh;

		visible_vec.push_back(vc_local);

		ImColor ESPColor = ImColor(255, 255, 255);


		if (settings::visuals::enable)
		{
			if (settings::visuals::box)
			{
				box_t bounds{};
				get_bounding_box(mesh, bounds); // Assuming this function calculates the bounding box

				switch (settings::visuals::box_type)
				{
				case 0:
					// Pass mesh pointer as the last argument to the function
					draw_regular_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, mesh);
					break;
				case 1:
					draw_filled_rect(head2d.x - (box_width / 2), head2d.y, box_width, box_height, mesh);
					break;
				}
			}

			if (settings::visuals::show_crosshair)
			{
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(settings::screen_center_x - 6, settings::screen_center_y), ImVec2(settings::screen_center_x + 6, settings::screen_center_y), settings::colors::crosshair_color, 1.0f);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(settings::screen_center_x, settings::screen_center_y - 6), ImVec2(settings::screen_center_x, settings::screen_center_y + 6), settings::colors::crosshair_color, 1.0f);
			}

			if (settings::visuals::skeleton)
			{
				draw_skeleton(mesh);
			}

			if (settings::visuals::username)
			{
				draw_player_name(head2d, get_player_name(player_state));

				head2d.y += ImGui::GetFontSize() + 2;
			}

			if (settings::visuals::FOVRadar)
			{
				addToFovRadar(head3d, ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fov, ESPColor);
			}

			if (settings::visuals::platform)
			{
				draw_player_platform(head2d, get_player_platform(player_state));

				head2d.y += ImGui::GetFontSize() + 2;
			}

			if (settings::visuals::rank)
			{
				int32_t rank_progress = read<int32_t>(
					read<uintptr_t>(player_state + HABANERO_COMPONENT) + RANKED_PROGRESS + 0x10
				);

				draw_player_rank(bottom2d, rank_progress);

				bottom2d.y += ImGui::GetFontSize() + 2;
			}

			auto team_id = read<char>(player_state + 0x1261);
			if (settings::visuals::team_check && cache::local_pawn)
			{
				if (team_id == cache::my_team_id) continue;
			}


			if (settings::visuals::distance)
			{
				draw_distance(bottom2d, distance);

				bottom2d.y += ImGui::GetFontSize() + 2;
			}

			if (settings::visuals::line)
			{
				draw_line(bottom2d);
			}
		}

		double dx = head2d.x - settings::width / 2;
		double dy = head2d.y - settings::height / 2;

		float dist = sqrtf(dx * dx + dy * dy);

		if (dist <= settings::aimbot::fov && dist < cache::closest_distance)
		{
			cache::closest_distance = dist;
			cache::closest_pawn = pawn_private;
		}
	}

	//static bool reset_spread = false;
	//if (settings::exploits::no_spread)
	//{
	//	write<float>(cache::current_weapon + 0x1f00, -1); // TimeRemainingForBulletSpread
	//	write<float>(cache::current_weapon + 0x1efc, -1); // BulletCountForPerBulletSpread
	//	write<float>(cache::current_weapon + 0x1ef4, -1); // FirstShotAccuracyMinWaitTime
	//}
	//else
	//{
	//	if (!reset_spread)
	//	{
	//		write<float>(cache::current_weapon + 0x1f00, 1); // TimeRemainingForBulletSpread
	//		write<float>(cache::current_weapon + 0x1efc, 1); // BulletCountForPerBulletSpread
	//		write<float>(cache::current_weapon + 0x1ef4, 1); // FirstShotAccuracyMinWaitTime

	//		reset_spread = true;
	//	}
	//}
}