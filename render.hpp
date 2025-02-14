#pragma once

// Dear ImGui: standalone example application for DirectX 11

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#define IMGUI_DEFINE_MATH_OPERATORS


#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <Windows.h>
#include <dwmapi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <TlHelp32.h>

#include "settings.hpp"
#include "game.hpp"
#include "particles.hpp"
#include "xorstr.h"  // Include the necessary header if available
#include "myfont.h"


// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

float Clamp(const float& value, const float& min, const float& max) {
    return (value < min) ? min : (value > max) ? max : value;
}

static float menuAlpha = 0.0f;

bool Keybind(CKeybind* keybind, const ImVec2& size_arg = ImVec2(0, 0), bool clicked = false, ImGuiButtonFlags flags = 0)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;
    // SetCursorPosX(window->Size.x - 14 - size_arg.x);
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(keybind->get_name().c_str());
    const ImVec2 label_size = ImGui::CalcTextSize(keybind->get_name().c_str(), NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) &&
        style.FramePadding.y <
        window->DC.CurrLineTextBaseOffset)  // Try to vertically align buttons that are smaller/have no padding so that
        // text baseline matches (bit hacky, since it shouldn't be a flag)
        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
    ImVec2 size = ImGui::CalcItemSize(
        size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    if (g.CurrentItemFlags & ImGuiItemFlags_ButtonRepeat)
        flags |= ImGuiButtonFlags_Repeat;
    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

    bool value_changed = false;
    int key = keybind->key;

    auto io = ImGui::GetIO();

    std::string name = keybind->get_key_name();

    if (keybind->waiting_for_input)
        name = "waiting";

    if (ImGui::GetIO().MouseClicked[0] && hovered)
    {
        if (g.ActiveId == id)
        {
            keybind->waiting_for_input = true;
        }
    }
    else if (ImGui::GetIO().MouseClicked[1] && hovered)
    {
        ImGui::OpenPopup(keybind->get_name().c_str());
    }
    else if (ImGui::GetIO().MouseClicked[0] && !hovered)
    {
        if (g.ActiveId == id)
            ImGui::ClearActiveID();
    }

    if (keybind->waiting_for_input)
    {
        if (ImGui::GetIO().MouseClicked[0] && !hovered)
        {
            keybind->key = VK_LBUTTON;

            ImGui::ClearActiveID();
            keybind->waiting_for_input = false;
        }
        else
        {
            if (keybind->set_key())
            {
                ImGui::ClearActiveID();
                keybind->waiting_for_input = false;
            }
        }
    }

    // Render
    ImVec4 textcolor = ImLerp(ImVec4(201 / 255.f, 204 / 255.f, 210 / 255.f, 1.f), ImVec4(1.0f, 1.0f, 1.0f, 1.f), 1.f);

    window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(33 / 255.0f, 33 / 255.0f, 33 / 255.0f, 1.f), 2.f);
    //window->DrawList->AddRect( bb.Min, bb.Max, ImColor( 0.f, 0.f, 0.f, 1.f ) );

    window->DrawList->AddText(
        bb.Min +
        ImVec2(
            size_arg.x / 2 - ImGui::CalcTextSize(name.c_str()).x / 2,
            size_arg.y / 2 - ImGui::CalcTextSize(name.c_str()).y / 2),
        ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)),
        name.c_str());

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar;
    //ImGui::SetNextWindowPos( pos + ImVec2( 0, size_arg.y - 1 ) );
    //ImGui::SetNextWindowSize( ImVec2( size_arg.x, 47 * 1.f ) );

    {
        if (ImGui::BeginPopup(keybind->get_name().c_str(), 0))
        {

            {
                {
                    ImGui::BeginGroup();
                    {
                        if (ImGui::Selectable("Hold", keybind->type == CKeybind::HOLD))
                            keybind->type = CKeybind::HOLD;
                        if (ImGui::Selectable("Toggle", keybind->type == CKeybind::TOGGLE))
                            keybind->type = CKeybind::TOGGLE;
                    }
                    ImGui::EndGroup();
                }
            }

            ImGui::EndPopup();
        }
    }

    return pressed;
}



// Main code
int render()
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    
    WNDCLASSEX wc;

    wc.cbClsExtra = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = NULL;
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = TEXT("D3D11 Overlay ImGui");
    wc.lpszMenuName = nullptr;
    wc.style = CS_VREDRAW | CS_HREDRAW;

    ::RegisterClassEx(&wc);
    const HWND hwnd = ::CreateWindowExA_Spoofed(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, wc.lpszClassName, "D3D11 Overlay ImGui", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, wc.hInstance, nullptr);

    SetLayeredWindowAttributes_Spoofed(hwnd, 0, 255, LWA_ALPHA);
    const MARGINS margin = { -1, 0, 0, 0 };
    DwmExtendFrameIntoClientArea(hwnd, &margin);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(L"D3D11 Overlay ImGui", wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// Load Font

    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;

    io.Fonts->AddFontFromMemoryTTF(oldfont, sizeof oldfont, 15.0f, &font_cfg);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 2.00;

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0, 0, 0, 0);

    // Main loop
    bool done = false;

    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (GetForegroundWindow_Spoofed() == FindWindowA_Spoofed(0, "Fortnite  ") || GetForegroundWindow_Spoofed() == hwnd)
        {
            if (settings::aimbot::enable)
            {
                settings::aimbot::aimbotkey.update();

                if (settings::aimbot::aimbotkey.enabled)
                {
                    aimbot(cache::closest_pawn);
                }
            }
        }

        game_loop();

        if (settings::aimbot::show_fov) {
            auto* drawList = ImGui::GetBackgroundDrawList();
            ImVec2 screenCenter = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);

            // Draw the outer outline
            drawList->AddCircle(
                screenCenter,
                settings::aimbot::fov + 1.0f, // Slightly larger radius for the outer outline
                ImColor(0, 0, 0, 200), // Outline color (black in this case)
                100, // Number of sides
                0.65f // Thinner outline thickness
            );

            // Draw the inner inline
            drawList->AddCircle(
                screenCenter,
                settings::aimbot::fov - 1.0f, // Slightly smaller radius for the inner inline
                ImColor(0, 0, 0, 200), // Inline color (black in this case)
                100, // Number of sides
                0.65f // Thinner inline thickness
            );

            // Check if the filled circle option is enabled
            if (settings::aimbot::fill_fov) {
                drawList->AddCircleFilled(
                    screenCenter,
                    settings::aimbot::fov,
                    ImColor(0, 0, 0, settings::aimbot::fill_opacity), // Dark color with adjustable opacity
                    100 // Number of sides
                );
            }

            // Draw the main circle
            drawList->AddCircle(
                screenCenter,
                settings::aimbot::fov,
                ImColor(250, 250, 250, 250), // Main circle color (white)
                100, // Number of sides
                1.0f // Thickness of the main circle
            );
        }

        float alphaSpeed = 4.0f * ImGui::GetIO().DeltaTime;

        if (settings::overlay::show_menu)
            menuAlpha = Clamp(menuAlpha + alphaSpeed, 0.0f, 1.0f);
        else
            menuAlpha = Clamp(menuAlpha - alphaSpeed, 0.0f, 1.0f);

        if (menuAlpha > 0.0f)
        {
            ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImGui::GetIO().DisplaySize, ImColor(0, 0, 0, static_cast<int>(200 * menuAlpha)));

            switch (settings::overlay::particle_type)
            {
            case 0:
                M::Particle.render(M_Particle_System::prt_type::rain);
                break;
            case 1:
                M::Particle.render(M_Particle_System::prt_type::snow);
                break;
            }


            ImGuiStyle* style = &ImGui::GetStyle();
            style->WindowRounding = 5.0f;
            style->FrameRounding = 3.0f;
            style->PopupRounding = 5.0f;
            style->ScrollbarRounding = 3.0f;
            style->GrabRounding = 3.0f;
            style->WindowBorderSize = 0.0f; // Remove window borders
            style->FrameBorderSize = 0.0f;
            style->WindowPadding = ImVec2(10, 10);
            style->FramePadding = ImVec2(5, 5);

            // Set colors for dark theme with black and red accent
            style->Colors[ImGuiCol_WindowBg] = ImColor(14, 14, 14);      // Dark background
            style->Colors[ImGuiCol_Border] = ImColor(24, 23, 25);        // Border color (dark grey)
            style->Colors[ImGuiCol_Separator] = ImColor(255, 0, 0);       // Red separator
            style->Colors[ImGuiCol_SeparatorActive] = ImColor(255, 0, 0);
            style->Colors[ImGuiCol_SeparatorHovered] = ImColor(255, 0, 0);
            style->Colors[ImGuiCol_Tab] = ImColor(14, 14, 14);             // Dark background for tabs
            style->Colors[ImGuiCol_TabHovered] = ImColor(100, 0, 0);      // Hovered tab dark red
            style->Colors[ImGuiCol_TabActive] = ImColor(200, 0, 0);       // Active tab bright red
            style->Colors[ImGuiCol_PopupBg] = ImColor(14, 14, 14);         // Dark popup background
            style->Colors[ImGuiCol_ScrollbarBg] = ImColor(14, 14, 14);     // Scrollbar background
            style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(255, 0, 0);    // Red scrollbar grab handle

            style->Colors[ImGuiCol_SliderGrab] = ImColor(255, 0, 0, 255); // Red slider grab handle
            style->Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 60, 60, 255); // Active slider grab handle (lighter red)

            // Set colors for slider background (frame background)
            style->Colors[ImGuiCol_FrameBg] = ImColor(40, 40, 40); // Darker gray for slider background
            style->Colors[ImGuiCol_FrameBgHovered] = ImColor(60, 60, 60); // Hovered frame background (slightly lighter gray)
            style->Colors[ImGuiCol_FrameBgActive] = ImColor(80, 80, 80); // Active frame background (even lighter gray)

            // Set colors for button background
            style->Colors[ImGuiCol_Button] = ImColor(50, 50, 50);    // Darker gray for button background
            style->Colors[ImGuiCol_ButtonHovered] = ImColor(70, 70, 70); // Slightly brighter gray for hovered button
            style->Colors[ImGuiCol_ButtonActive] = ImColor(90, 90, 90); // Brighter gray for active button

            // Set colors for checkbox and checkmark
            style->Colors[ImGuiCol_CheckMark] = ImColor(255, 0, 0);        // Red checkmark color

            // Set colors for checkbox hover states
            style->Colors[ImGuiCol_ButtonHovered] = ImColor(70, 70, 70); // Slightly brighter gray for checkbox hover (treated as button)
            style->Colors[ImGuiCol_ButtonActive] = ImColor(90, 90, 90); // Brighter gray for checkbox active (treated as button)

            // Set colors for keybind (input fields)
            style->Colors[ImGuiCol_TextSelectedBg] = ImColor(255, 0, 0);   // Red keybind background when selected

            static int activeTab = 0; // Track the active tab (0 = Aimbot, 1 = Visuals, 2 = Misc)
            static float glowEffect = 0.0f;
            static bool increaseGlow = true;
            static float aimbotEnableValue = 0.0f;

            // Glow animation for hover effects
            if (increaseGlow) {
                glowEffect += 0.01f;
                if (glowEffect >= 1.0f) increaseGlow = false;
            }
            else {
                glowEffect -= 0.01f;
                if (glowEffect <= 0.0f) increaseGlow = true;
            }



            ImGui::SetNextWindowBgAlpha(menuAlpha);
            ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::SetNextWindowSize(ImVec2(500, 500));
            {
                if (ImGui::BeginTabBar("Main"))
                {
                    if (ImGui::BeginTabItem("Nigger Bot"))
                    {
                        ImGui::BeginChild("Aimbot", ImVec2(400, 400), true);

                        ImGui::Checkbox("Enable Noxar.WTF Aimbot", &settings::aimbot::enable);
                        ImGui::SameLine();

                        ImGui::SetCursorPosY(11.5);
                        Keybind(&settings::aimbot::aimbotkey, ImVec2(60, 20));

                        ImGui::Checkbox("Enable Smoothness", &settings::aimbot::enable_smoothness);
                        ImGui::Checkbox("Visible Only", &settings::aimbot::visible_check);

                        int combinedSmoothness = static_cast<int>((settings::aimbot::smoothness_x + settings::aimbot::smoothness_y) / 2.0f);
                        if (ImGui::SliderInt("Smoothness", &combinedSmoothness, 1, 20))
                        {
                            settings::aimbot::smoothness_x = static_cast<float>(combinedSmoothness);
                            settings::aimbot::smoothness_y = static_cast<float>(combinedSmoothness);
                        }

                        ImGui::Checkbox("Fill FOV Circle", &settings::aimbot::fill_fov);
                        ImGui::Checkbox("FOV Circle", &settings::aimbot::show_fov);
                        ImGui::SliderFloat("FOV Size", &settings::aimbot::fov, 50, 500);

                        ImGui::Text("type");

                        ImGui::Combo(
                            "##type",
                            &settings::aimbot::aimbot_type,
                            settings::aimbot::aimbot_types,
                            IM_ARRAYSIZE(settings::aimbot::aimbot_types),
                            IM_ARRAYSIZE(settings::aimbot::aimbot_types)
                        );

                        ImGui::Text("Aim Part");

                        ImGui::Combo(
                            "##part",
                            &settings::aimbot::aimbot_part,
                            settings::aimbot::aimbot_parts,
                            IM_ARRAYSIZE(settings::aimbot::aimbot_parts),
                            IM_ARRAYSIZE(settings::aimbot::aimbot_parts)
                        );



                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Visuals"))
                    {
                        ImGui::BeginChild("ESP", ImVec2(400, 400), true);

                        ImGui::Checkbox("Enable", &settings::visuals::enable);


                        ImGui::Checkbox("Box", &settings::visuals::box);
                        ImGui::SameLine();
                        ImGui::Combo("##boxtype", &settings::visuals::box_type, settings::visuals::box_types, IM_ARRAYSIZE(settings::visuals::box_types), IM_ARRAYSIZE(settings::visuals::box_types));

                        ImGui::Checkbox("Skeleton", &settings::visuals::skeleton);
                        ImGui::Checkbox("Snaplines", &settings::visuals::line);

                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Colors"))
                    {

                        ImGui::BeginChild("Colors", ImVec2(400, 400), true);

                        ImGui::Text("Visible Box");
                        ImGui::SameLine();
                        if (ImGui::ColorButton("##VisibleBoxColor", visible_color, ImGuiColorEditFlags_NoTooltip, ImVec2(25, 25)))
                        {
                            ImGui::OpenPopup("VisibleBoxColorPicker");
                        }
                        if (ImGui::BeginPopup("VisibleBoxColorPicker"))
                        {
                            ImGui::ColorPicker3("##VisibleBoxPicker", (float*)&visible_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                            ImGui::EndPopup();
                        }

                        // Visible skeleton color
                        ImGui::SameLine(160); // Adjust spacing between the columns as needed
                        ImGui::Text("Visible Skeleton");
                        ImGui::SameLine();
                        if (ImGui::ColorButton("##VisibleSkeletonColor", visible_skeleton_color, ImGuiColorEditFlags_NoTooltip, ImVec2(25, 25)))
                        {
                            ImGui::OpenPopup("VisibleSkeletonColorPicker");
                        }
                        if (ImGui::BeginPopup("VisibleSkeletonColorPicker"))
                        {
                            ImGui::ColorPicker3("##VisibleSkeletonPicker", (float*)&visible_skeleton_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                            ImGui::EndPopup();
                        }

                        // Not visible skeleton color
                        ImGui::SameLine(160); // Adjust spacing between the columns as needed
                        ImGui::Text("Unvisible Skeleton");
                        ImGui::SameLine();
                        if (ImGui::ColorButton("##NotVisibleSkeletonColor", not_visible_skeleton_color, ImGuiColorEditFlags_NoTooltip, ImVec2(25, 25)))
                        {
                            ImGui::OpenPopup("NotVisibleSkeletonColorPicker");
                        }
                        if (ImGui::BeginPopup("NotVisibleSkeletonColorPicker"))
                        {
                            ImGui::ColorPicker3("##NotVisibleSkeletonPicker", (float*)&not_visible_skeleton_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                            ImGui::EndPopup();
                        }

                        ImGui::Separator();
                        ImGui::Text("Box Thickness");
                        ImGui::SliderFloat("##BoxThicknessSlider", &regular_box_thickness, 1.5f, 3.0f, "%.1f");

                        ImGui::Text("Skeleton Thickness");
                        ImGui::SliderFloat("##SkeletonThicknessSlider", &skeleton_thickness, 1.5f, 5.5f, "%.1f");

                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Misc"))
                    {
                        ImGui::BeginChild("Settings", ImVec2(400, 400), true);
                        ;
                        ImGui::Checkbox("Watermark Clock", &settings::features::showtime);
                        ImGui::Checkbox("Watermark FPS", &settings::features::showfps);

                        ImGui::EndChild();
                        ImGui::EndTabItem();
                    }

                    ImGui::EndTabBar();
                }
            }

            ImGui::End();
        }

        if (GetAsyncKeyState(VK_F2) & 1)
            settings::overlay::show_menu = !settings::overlay::show_menu;

        if (settings::overlay::show_menu)
            SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
        else
            SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
        #define xorstr_(str) (str)

        // Top Left
        std::string text1_part1 = xorstr_("Noxar");
        std::string text1_part2 = xorstr_("Free");

        // Get current local time (user's PC time: hour, minute, second, AM/PM)
        auto now = std::chrono::system_clock::now();
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm local_tm = *std::localtime(&now_time_t);  // Converts to user's local time

        // Format the time as "HH:MM:SS AM/PM"
        std::ostringstream time_stream;
        time_stream << std::put_time(&local_tm, "%I:%M:%S %p");  // %I for 12-hour format, %p for AM/PM

        // Generate the time and FPS string (conditionally displayed based on showtime and showfps)
        std::string text3;

        if (settings::features::showtime && settings::features::showfps) {
            text3 = xorstr_(" | ") + time_stream.str() + xorstr_(" | FPS: ") + std::to_string(static_cast<int>(ImGui::GetIO().Framerate));  // With both Time and FPS
        }
        else if (settings::features::showtime) {
            text3 = xorstr_(" | ") + time_stream.str();  // Only Time
        }
        else if (settings::features::showfps) {
            text3 = xorstr_(" | FPS: ") + std::to_string(static_cast<int>(ImGui::GetIO().Framerate));  // Only FPS
        }
        else {
            text3 = "";  // Neither Time nor FPS
        }

        // Measure text sizes
        ImVec2 text1_part1_size = ImGui::CalcTextSize(text1_part1.c_str());
        ImVec2 text1_part2_size = ImGui::CalcTextSize(text1_part2.c_str());
        ImVec2 text3_size = ImGui::CalcTextSize(text3.c_str());

        // Determine total width and height
        float max_height = text1_part1_size.y;
        float total_width = text1_part1_size.x + text1_part2_size.x + text3_size.x + 25.0f;
        float total_height = max_height + 10.0f;

        // Rectangle position and size
        ImVec2 rect_pos(10, 10);
        ImVec2 rect_size(total_width, total_height);

        // Draw the texts
        ImGui::GetForegroundDrawList()->AddText(
            rect_pos + ImVec2(10, (rect_size.y - text1_part1_size.y) / 2),
            ImColor(255, 255, 255, 255),  // White for "Elite"
            text1_part1.c_str()
        );

        ImGui::GetForegroundDrawList()->AddText(
            rect_pos + ImVec2(10 + text1_part1_size.x, (rect_size.y - text1_part2_size.y) / 2),
            IM_COL32(255, 0, 0, 255),  // Pink color using RGBA values for "Fn"
            text1_part2.c_str()
        );

        // Draw time/FPS string (text3) if it's not empty
        if (!text3.empty()) {
            ImGui::GetForegroundDrawList()->AddText(
                rect_pos + ImVec2(text1_part1_size.x + text1_part2_size.x + 10, (rect_size.y - text3_size.y) / 2),
                ImColor(255, 255, 255, 255),  // White for Time/FPS
                text3.c_str()
            );
        }


        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(L"D3D11 Overlay ImGui", wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}