#pragma once
#include "imgui_freetype.h"
#include <vector>
#include <cmath>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include "imgui_freetype.h"
#include <d3d11.h>
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include <D3DX11tex.h>
#include <unordered_map>
#include <random>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <imgui_settings.h>
#include "Fonts.h"
#include "images.h"

using namespace ImGui;

static inline ImVec2 operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }

static int slider_int[25];
static bool checkbox[25];
float color_edit[10][4];
int keybind[10];
static int combo[10];
static int iTabs = 0;
static int iSubTabs = 0;

#define COLOR_EDIT_FLAGS (ImGuiColorEditFlags_NoSidePreview | \
                         ImGuiColorEditFlags_PickerHueBar | \
                         ImGuiColorEditFlags_NoOptions | \
                         ImGuiColorEditFlags_NoInputs | \
                         ImGuiColorEditFlags_NoAlpha)

const char* cheat_name = "Noxar";
const char* developer = "External";
const char* role = "FN";

ID3D11ShaderResourceView* clouds = nullptr;
ID3D11ShaderResourceView* backgrounds = nullptr;
ID3D11ShaderResourceView* lb = nullptr;
ID3D11ShaderResourceView* logos = nullptr;
static bool is_logged_in = false;

namespace images {
    inline ID3D11ShaderResourceView* mouses = nullptr;
    inline ID3D11ShaderResourceView* eyes = nullptr;
    inline ID3D11ShaderResourceView* gear = nullptr;
    inline ID3D11ShaderResourceView* folder = nullptr;
    inline ID3D11ShaderResourceView* risky = nullptr;
    inline ID3D11ShaderResourceView* saving = nullptr;
}

struct GIFFrame {
    ID3D11ShaderResourceView* texture;
    float delay; // delay time for each frame
};

std::vector<GIFFrame> gifFrames;
int currentFrame = 0;
float frameTime = 0.0f;

ID3D11ShaderResourceView* CreateTextureFromMemory(ID3D11Device* device, unsigned char* data, int width, int height) {
    ID3D11Texture2D* texture = nullptr;
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;
    initData.SysMemPitch = width * 4;

    HRESULT hr = device->CreateTexture2D(&desc, &initData, &texture);

    ID3D11ShaderResourceView* textureView = nullptr;
    hr = device->CreateShaderResourceView(texture, nullptr, &textureView);
    texture->Release();

    return textureView;
}
int* delays = nullptr;
void LoadGIF(ID3D11Device* device, const unsigned char* gif_data, size_t gif_size) {
    int width, height, frames, components;
    unsigned char* frame_data = stbi_load_gif_from_memory(gif_data, gif_size, &delays, &width, &height, &frames, &components, 4);

    for (int i = 0; i < frames; i++) {
        unsigned char* frame = frame_data + (i * width * height * 4);
        ID3D11ShaderResourceView* texture = CreateTextureFromMemory(device, frame, width, height);
        GIFFrame gifFrame = { texture, 0.1f };
        gifFrames.push_back(gifFrame);
    }

    stbi_image_free(frame_data);
}

void RenderGIFAnimation(float deltaTime) {
    if (gifFrames.empty()) {
        ImGui::Text("No gif loaded.");
        return;
    }

    frameTime += deltaTime;
    if (frameTime >= gifFrames[currentFrame].delay) {
        frameTime = 0.0f;
        currentFrame = (currentFrame + 1) % gifFrames.size();
    }

    // Get the cursor position for positioning
    ImGui::SetCursorPos(ImVec2(10, -35));
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(140, 120);  // Size of the image
    float rounding = 20.0f;  // Rounding radius (half of width/height for full circle)

    // Get the draw list for custom rendering
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Add the image with rounded corners
    drawList->AddImageRounded(
        (void*)gifFrames[currentFrame].texture,
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        ImVec2(0, 0),
        ImVec2(1, 1),
        IM_COL32_BLACK,
        rounding
    );

    // Advance the cursor past the image
    ImGui::Dummy(size);
}

void RenderGIFAnimationLogin(float deltaTime) {
    if (gifFrames.empty()) {
        ImGui::Text("No gif loaded.");
        return;
    }

    frameTime += deltaTime;
    if (frameTime >= gifFrames[currentFrame].delay) {
        frameTime = 0.0f;
        currentFrame = (currentFrame + 1) % gifFrames.size();
    }

    // Get the cursor position for positioning
    ImGui::SetCursorPos(ImVec2(70, -10));
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(280, 240);  // Size of the image
    float rounding = 20.0f;  // Rounding radius (half of width/height for full circle)

    // Get the draw list for custom rendering
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Add the image with rounded corners
    drawList->AddImageRounded(
        (void*)gifFrames[currentFrame].texture,
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        ImVec2(0, 0),
        ImVec2(1, 1),
        IM_COL32_BLACK,
        rounding
    );

    // Advance the cursor past the image
    ImGui::Dummy(size);
}

class KeybindHelper {
private:
    struct KeybindInfo {
        std::string label;
        ImGuiKey key;
        float showTime;
        bool isActive;
    };

    static std::vector<KeybindInfo> activeBinds;
    static constexpr float DISPLAY_DURATION = 2.0f;
    static constexpr float FADE_DURATION = 0.3f;

    static ImGuiKey ConvertKeyCode(int key) {
        if (key >= 0 && key < 512) {
            return static_cast<ImGuiKey>(key);
        }
        return ImGuiKey_None;
    }

    static const char* GetKeyString(ImGuiKey key) {
        if (key == ImGuiKey_None) return "None";

        const char* name = ImGui::GetKeyName(key);
        return name ? name : "Unknown";
    }

public:
    static void ShowBindHelper() {
        ImGuiIO& io = ImGui::GetIO();
        float deltaTime = io.DeltaTime;

        for (auto& bind : activeBinds) {
            if (bind.isActive) {
                bind.showTime = DISPLAY_DURATION;
            }
            else {
                bind.showTime -= deltaTime;
            }
        }

        activeBinds.erase(
            std::remove_if(activeBinds.begin(), activeBinds.end(),
                [](const KeybindInfo& bind) { return bind.showTime <= 0.0f; }),
            activeBinds.end()
        );

        if (activeBinds.empty()) return;

        const float WINDOW_WIDTH = 200.0f;
        const float WINDOW_HEIGHT = 35.0f;
        const float WINDOW_PADDING = 10.0f;
        const ImVec2 SCREEN_SIZE = ImGui::GetIO().DisplaySize;

        float currentY = 120.0f;

        for (const auto& bind : activeBinds) {
            float alpha = 1.0f;
            if (bind.showTime < FADE_DURATION) {
                alpha = bind.showTime / FADE_DURATION;
            }

            ImGui::SetNextWindowPos(ImVec2(20.0f, currentY));
            ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 0));

            std::string windowName = "##BindHelper" + std::to_string(&bind - &activeBinds[0]);

            if (ImGui::Begin(windowName.c_str(), nullptr,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoDecoration
            )) {
                auto* drawList = ImGui::GetWindowDrawList();
                ImVec2 pos = ImGui::GetWindowPos();

                drawList->AddRectFilled(
                    pos,
                    ImVec2(pos.x + WINDOW_WIDTH, pos.y + WINDOW_HEIGHT),
                    ImColor(13, 14, 16, static_cast<int>(200 * alpha)),
                    4.0f
                );

                drawList->AddRectFilled(
                    ImVec2(pos.x, pos.y + WINDOW_HEIGHT - 2),
                    ImVec2(pos.x + WINDOW_WIDTH, pos.y + WINDOW_HEIGHT),
                    ImColor(main_color.Value.x, main_color.Value.y, main_color.Value.z, alpha)
                );

                ImGui::PushFont(medium_font);
                const float text_y = (WINDOW_HEIGHT - ImGui::GetTextLineHeight()) * 0.5f;
                ImGui::SetCursorPosY(text_y);

                ImGui::TextColored(
                    ImVec4(0.8f, 0.8f, 0.8f, alpha),
                    "%s", bind.label.c_str()
                );

                const char* keyName = GetKeyString(bind.key);
                float keyWidth = ImGui::CalcTextSize(keyName).x;
                ImGui::SameLine(WINDOW_WIDTH - keyWidth - 20);
                ImGui::TextColored(
                    ImVec4(main_color.Value.x, main_color.Value.y, main_color.Value.z, alpha),
                    "%s", keyName
                );
                ImGui::PopFont();
            }
            ImGui::End();

            ImGui::PopStyleVar(3);
            currentY += WINDOW_HEIGHT + WINDOW_PADDING;
        }
    }

    static void OnKeybindPressed(const char* label, int key) {
        ImGuiKey imKey = ConvertKeyCode(key);

        auto it = std::find_if(activeBinds.begin(), activeBinds.end(),
            [label](const KeybindInfo& bind) { return bind.label == label; });

        if (it != activeBinds.end()) {
            it->isActive = true;
            it->showTime = DISPLAY_DURATION;
        }
        else {
            KeybindInfo newBind;
            newBind.label = label;
            newBind.key = imKey;
            newBind.showTime = DISPLAY_DURATION;
            newBind.isActive = true;
            activeBinds.push_back(newBind);
        }
    }

    static void OnKeybindReleased(const char* label) {
        auto it = std::find_if(activeBinds.begin(), activeBinds.end(),
            [label](const KeybindInfo& bind) { return bind.label == label; });

        if (it != activeBinds.end()) {
            it->isActive = false;
        }
    }
};

std::vector<KeybindHelper::KeybindInfo> KeybindHelper::activeBinds;

void CheckInput(const char* label, int* key) {
    if (*key != 0) {

        bool isMouseButton = *key >= 0 && *key <= 4;

        if (isMouseButton) {
            if (ImGui::GetIO().MouseDown[*key]) {
                KeybindHelper::OnKeybindPressed(label, *key);
            }
            else {
                KeybindHelper::OnKeybindReleased(label);
            }
        }
        else {
            if (ImGui::GetIO().KeysDown[*key]) {
                KeybindHelper::OnKeybindPressed(label, *key);
            }
            else {
                KeybindHelper::OnKeybindReleased(label);
            }
        }
    }
}

namespace blur {
    struct BlurParameters {
        float intensity = 1.0f;
        ImColor color = ImColor(0, 0, 0, 30);
        int layers = 3;
        float feather = 0.3f;
        bool gaussian = true;
        float sigma = 1.0f;
    };

    namespace detail {
        float gaussian_weight(float x, float sigma) {
            const float PI = 3.14159265358979323846f;
            return (1.0f / (sigma * sqrt(2.0f * PI))) * exp(-(x * x) / (2.0f * sigma * sigma));
        }

        ImColor create_gradient(const ImColor& center_color, float alpha) {
            return ImColor(
                center_color.Value.x,
                center_color.Value.y,
                center_color.Value.z,
                center_color.Value.w * alpha
            );
        }
    }

    void add_blur(
        ImDrawList* draw_list,
        const ImVec2& min,
        const ImVec2& max,
        const BlurParameters& params = BlurParameters()
    ) {
        if (!draw_list) return;

        ImVec2 size = ImVec2(max.x - min.x, max.y - min.y);
        float baseOffset = 5.0f * params.intensity;

        draw_list->AddRectFilled(min, max, params.color);

        for (int i = 0; i < params.layers; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(params.layers - 1);
            float offset = baseOffset * (params.gaussian ?
                detail::gaussian_weight(t, params.sigma) * params.layers :
                (1.0f - t));

            ImVec2 inward_min = ImVec2(
                min.x + offset,
                min.y + offset
            );
            ImVec2 inward_max = ImVec2(
                max.x - offset,
                max.y - offset
            );

            float alpha = params.color.Value.w *
                (params.gaussian ?
                    detail::gaussian_weight(t, params.sigma) * 2.0f :
                    (1.0f - t)) *
                params.feather;

            ImColor layerColor = params.color;
            layerColor.Value.w = alpha;

            draw_list->AddRectFilledMultiColor(
                inward_min,
                inward_max,
                layerColor,
                layerColor,
                layerColor,
                layerColor
            );
        }

        float shadowOffset = baseOffset * 0.2f;
        ImColor shadowColor = (0, 0, 0, params.color.Value.w * 0.2f);
        draw_list->AddRect(
            ImVec2(min.x + shadowOffset, min.y + shadowOffset),
            ImVec2(max.x - shadowOffset, max.y - shadowOffset),
            shadowColor,
            0.0f,
            ImDrawFlags_None,
            1.5f
        );
    }

    void add_teal_blur(
        ImDrawList* draw_list,
        const ImVec2& min,
        const ImVec2& max,
        float intensity = 1.0f
    ) {
        BlurParameters params;
        params.intensity = intensity;
        params.color = ImColor(0, 128, 128, 30);
        params.gaussian = true;
        params.sigma = 1.2f;
        add_blur(draw_list, min, max, params);
    }

    void add_color_blur(
        ImDrawList* draw_list,
        const ImVec2& min,
        const ImVec2& max,
        ImColor color,
        float intensity = 1.0f
    ) {
        BlurParameters params;
        params.intensity = intensity;
        params.color = color;
        add_blur(draw_list, min, max, params);
    }

    void add_dark_blur(
        ImDrawList* draw_list,
        const ImVec2& min,
        const ImVec2& max,
        float intensity = 1.0f
    ) {
        BlurParameters params;
        params.intensity = intensity;
        params.color = ImColor(0, 0, 0, 40);
        params.layers = 4;
        params.feather = 0.4f;
        params.gaussian = true;
        params.sigma = 0.8f;
        add_blur(draw_list, min, max, params);
    }
}

struct CloudAnimation {
    bool isAnimating = false;
    float progress = 0.0f;
    float startTime = 0.0f;
    float duration = 0.0f;

    const float MIN_DURATION = 0.4f;
    const float MAX_DURATION = 0.8f;

    void StartAnimation() {
        isAnimating = true;
        progress = 0.0f;
        startTime = ImGui::GetTime();

        duration = MIN_DURATION + static_cast<float>(rand()) /
            (static_cast<float>(RAND_MAX / (MAX_DURATION - MIN_DURATION)));
    }

    void Update() {
        if (!isAnimating) return;

        float currentTime = ImGui::GetTime();
        progress = (currentTime - startTime) / duration;

        if (progress >= 1.0f) {
            isAnimating = false;
            progress = 1.0f;
        }
    }

    float GetEasedProgress() {
        return progress < 0.5f ?
            2.0f * progress * progress :
            1.0f - pow(-2.0f * progress + 2.0f, 2.0f) / 2.0f;
    }
};

CloudAnimation g_CloudAnim;

void StartCloudAnimation() {
    g_CloudAnim.StartAnimation();
}

void RenderCloudAnimation(ID3D11ShaderResourceView* clouds, ImColor main_color) {
    if (g_CloudAnim.isAnimating) {
        g_CloudAnim.Update();

        if (!g_CloudAnim.isAnimating) {
            ImGui::Image((ImTextureID)clouds, ImVec2(25, 25));
        }
        else {
            ImVec2 center = ImGui::GetCursorScreenPos() + ImVec2(12.5f, 12.5f);
            auto* draw_list = ImGui::GetWindowDrawList();

            const float radius = 8.5f;
            draw_list->AddCircleFilled(center, radius, ImColor(0, 0, 0, 50));

            const int segments = 32;
            float startAngle = -IM_PI / 2.0f;
            float endAngle = startAngle + (2 * IM_PI * g_CloudAnim.GetEasedProgress());

            draw_list->PathClear();
            for (int i = 0; i <= segments; i++) {
                float t = (float)i / (float)segments;
                float angle = startAngle + (endAngle - startAngle) * t;
                draw_list->PathLineTo(ImVec2(
                    center.x + cosf(angle) * radius,
                    center.y + sinf(angle) * radius
                ));
            }
            draw_list->PathStroke(main_color, false, 2.0f);
            ImGui::Dummy(ImVec2(25, 25));
        }
    }
    else {
        ImGui::Image((ImTextureID)clouds, ImVec2(25, 25));
    }
}

void infobar() {
    ImGui::PushFont(medium_font);

    const float cheat_name_size = ImGui::CalcTextSize(cheat_name).x;
    const float developer_size = ImGui::CalcTextSize(developer).x;
    const float role_size = ImGui::CalcTextSize(role).x;
    const float bar_size = ImGui::CalcTextSize("|").x;
    const float padding = 35.0f;  // Adjust this value to increase/decrease width
    const float ibar_size = cheat_name_size + bar_size + developer_size + bar_size +
        role_size + developer_size + bar_size + bar_size * 2 + padding;
    ImGui::SetNextWindowPos(ImVec2(15, 15));
    ImGui::SetNextWindowSize(ImVec2(ibar_size, 45));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    if (ImGui::Begin("##infobar", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings))
    {
        ImGuiStyle* style = &ImGui::GetStyle();
        const ImVec2& pos = ImGui::GetWindowPos();
        const ImVec2& region = ImGui::GetContentRegionMax();

        blur::BlurParameters params;
        params.intensity = 1.0f;
        blur::add_blur(ImGui::GetWindowDrawList(), pos, pos + region, params);

        ImColor bg_color = winbg_color;
        bg_color.Value.w = 0.65f;

        GetWindowDrawList()->AddRectFilled(
            pos,
            pos + region,
            bg_color,
            10.0f
        );

        ImColor gradientTop(1.0f, 1.0f, 1.0f, 0.03f);
        ImColor gradientBottom(1.0f, 1.0f, 1.0f, 0.0f);
        GetWindowDrawList()->AddRectFilledMultiColor(
            pos,
            pos + region,
            gradientTop,
            gradientTop,
            gradientBottom,
            gradientBottom
        );

        ImColor border_color = winbg_color;
        border_color.Value.w = 0.7f;
        GetWindowDrawList()->AddRect(
            pos,
            pos + region,
            border_color,
            10.0f
        );

        const char* info_set[3] = { cheat_name, developer, role };
        ImGui::SetCursorPos(ImVec2(style->ItemSpacing.x, (45 - ImGui::CalcTextSize(developer).y) / 2));

        ImGui::BeginGroup();
        {
            for (int i = 0; i < 3; i++) {
                ImGui::TextColored(
                    i < 1 ? main_color : text_color[1],
                    info_set[i]
                );

                if (i < 2) {
                    ImGui::SameLine();
                    ImGui::TextColored(ImColor(text_color[2]), "|");
                    ImGui::SameLine();
                }
            }
        }
        ImGui::EndGroup();

        ImGui::End();
    }

    ImGui::PopStyleColor();
    ImGui::PopFont();
}

// Notification system
struct Notification {
    std::string message;
    ImColor color;
    float animationProgress;
    float timeLeft;
    bool shouldRemove;
};

class NotificationManager {
private:
    static std::vector<Notification> notifications;
    static constexpr float NOTIFICATION_LIFETIME = 3.0f;
    static constexpr float ANIMATION_SPEED = 1.0f / 0.15f;

public:
    enum class NotificationType {
        SUCCESS,
        WARNING,
        ERRORS,
        INFO
    };

    static void AddNotification(const std::string& message, NotificationType type) {
        Notification notif;
        notif.message = message;
        notif.timeLeft = NOTIFICATION_LIFETIME;
        notif.animationProgress = 0.0f;
        notif.shouldRemove = false;
        notif.color = ImColor(main_color);

        notifications.push_back(notif);
    }

    static void Render() {
        ImGuiIO& io = ImGui::GetIO();
        float deltaTime = io.DeltaTime;

        const float NOTIFICATION_WIDTH = 240.0f;
        const float NOTIFICATION_HEIGHT = 40.0f;
        const float NOTIFICATION_PADDING = 8.0f;
        const float LINE_THICKNESS = 1.0f;

        float currentY = 10.0f;

        for (auto& notification : notifications) {
            if (notification.animationProgress < 1.0f) {
                notification.animationProgress += deltaTime * ANIMATION_SPEED;
                if (notification.animationProgress > 1.0f)
                    notification.animationProgress = 1.0f;
            }

            notification.timeLeft -= deltaTime;
            if (notification.timeLeft <= 0.0f) {
                notification.shouldRemove = true;
                continue;
            }

            float alpha = 1.0f;
            if (notification.timeLeft < 0.5f) {
                alpha = notification.timeLeft / 0.5f;
            }

            float targetX = 1920.0f - NOTIFICATION_WIDTH - 20.0f;
            float startX = 1920.0f + NOTIFICATION_WIDTH;
            float currentX = startX + (targetX - startX) * notification.animationProgress;

            ImGui::SetNextWindowPos(ImVec2(currentX, currentY));
            ImGui::SetNextWindowSize(ImVec2(NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT));

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(13, 14, 16, 200).Value);

            std::string windowName = "##Notification" + std::to_string(&notification - &notifications[0]);

            if (ImGui::Begin(windowName.c_str(), nullptr,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoDecoration
            )) {
                auto* drawList = ImGui::GetWindowDrawList();
                ImVec2 pos = ImGui::GetWindowPos();

                blur::BlurParameters params;
                params.intensity = 1.0f;
                blur::add_blur(drawList, pos, ImVec2(pos.x + NOTIFICATION_WIDTH, pos.y + NOTIFICATION_HEIGHT), params);

                drawList->AddRectFilled(pos,
                    ImVec2(pos.x + NOTIFICATION_WIDTH, pos.y + NOTIFICATION_HEIGHT),
                    ImColor(winbg_color), 4.0f);

                float progressWidth = (notification.timeLeft / NOTIFICATION_LIFETIME) * NOTIFICATION_WIDTH;
                drawList->AddRectFilled(
                    ImVec2(pos.x, pos.y + NOTIFICATION_HEIGHT - LINE_THICKNESS),
                    ImVec2(pos.x + progressWidth, pos.y + NOTIFICATION_HEIGHT),
                    notification.color);

                ImGui::PushFont(medium_font);
                ImGui::SetCursorPos(ImVec2(12.0f, NOTIFICATION_HEIGHT * 0.5f - ImGui::GetTextLineHeight() * 0.5f));
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", notification.message.c_str());
                ImGui::PopFont();
            }
            ImGui::End();

            ImGui::PopStyleColor();
            ImGui::PopStyleVar(3);

            currentY += NOTIFICATION_HEIGHT + NOTIFICATION_PADDING;
        }

        notifications.erase(
            std::remove_if(notifications.begin(), notifications.end(),
                [](const Notification& n) { return n.shouldRemove; }),
            notifications.end()
        );
    }
};

std::vector<Notification> NotificationManager::notifications;

// Menu animation variables
namespace GUI {
    float menu_animation = 100;
    float border_animation = 0;
    float menu_slide = 0;
    bool small_tab = false;
    float do_tab_anim = 100;
    float child_change_pos_on_tab_change = 111;
    bool domenuopen = true;
}

void UpdateMenuAnimations() {
    // Menu open/close animation
    if (GUI::menu_animation > 100)
        GUI::menu_animation = 100;

    if (GUI::menu_animation < 1) {
        GUI::menu_animation = 100;
        GUI::domenuopen = false;
    }

    if (GUI::menu_animation > 120)
        GUI::domenuopen = true;

    // Border animation
    if (GUI::border_animation > 3.4)
        GUI::border_animation = 3.5;

    if (GUI::border_animation < 1)
        GUI::border_animation = 0;

    // Menu slide animation
    if (GUI::menu_slide > 100)
        GUI::menu_slide = 250;

    if (GUI::menu_slide < 20)
        GUI::menu_slide = 0;

    // Tab animations
    if (GUI::small_tab) {
        GUI::do_tab_anim = GUI::do_tab_anim - 6;
        if (GUI::do_tab_anim < 31)
            GUI::do_tab_anim = 30;

        GUI::child_change_pos_on_tab_change = GUI::child_change_pos_on_tab_change - 1000;
        if (GUI::child_change_pos_on_tab_change < 1000)
            GUI::child_change_pos_on_tab_change = 10;
    }
    else {
        GUI::do_tab_anim = GUI::do_tab_anim + 6;
        if (GUI::do_tab_anim > 99)
            GUI::do_tab_anim = 100;

        GUI::child_change_pos_on_tab_change = GUI::child_change_pos_on_tab_change + 10;
        if (GUI::child_change_pos_on_tab_change > 110)
            GUI::child_change_pos_on_tab_change = 111;
    }
}

void RenderMenuAnimations(ImDrawList* draw, ImVec2 p) {
    UpdateMenuAnimations();

    float menu_alpha = GUI::menu_animation / 100.0f;
    float border_thickness = GUI::border_animation;
    float slide_offset = GUI::menu_slide;
    float tab_width = GUI::do_tab_anim;
    float child_pos = GUI::child_change_pos_on_tab_change;

    if (GUI::domenuopen) {
        draw->AddRectFilled(p, p + ImVec2(670, 604),
            ImColor(0.f, 0.f, 0.f, menu_alpha * 0.8f),
            ImGui::GetStyle().WindowRounding);

        if (border_thickness > 0) {
            draw->AddRect(p, p + ImVec2(670, 604),
                main_color,
                ImGui::GetStyle().WindowRounding,
                ImDrawFlags_None,
                border_thickness);
        }

        ImGui::SetCursorPos(ImVec2(25 + slide_offset, 72));
    }
}

void menu() {
    // When the menu is open, render the particle system first using the background draw list.
    // This ensures the particles are behind the menu.
    if (GUI::domenuopen) {
        // You can choose the particle type (snow, rain, or ash)
        M::Particle.render(M_Particle_System::prt_type::snow);
    }

    ImGui::Begin("General", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);
    {
        anim_speed = ImGui::GetIO().DeltaTime * 12.f;

        auto draw = ImGui::GetWindowDrawList();
        const auto& p = ImGui::GetWindowPos();

        ImGuiStyle& s = ImGui::GetStyle();

        // Main window blur and background
        blur::BlurParameters params;
        params.intensity = 0.0f;
        blur::add_blur(ImGui::GetBackgroundDrawList(), p, ImVec2(p.x + 670, p.y + 604), params);
        draw->AddRectFilled(p + ImVec2(0, 55), p + ImVec2(670, 604), experiment_color, s.WindowRounding, ImDrawFlags_RoundCornersBottom);

        ImGui::PushClipRect(p, p + ImVec2(670, 55), true);
        blur::add_blur(ImGui::GetWindowDrawList(), p, ImVec2(p.x + 670, p.y + 604), params);
        ImGui::PopClipRect();

        draw->AddRectFilled(p, p + ImVec2(670, 55), ImColor(5, 5, 5, 5), s.WindowRounding, ImDrawFlags_RoundCornersTop);
        draw->AddRectFilled(p + ImVec2(0, 52), p + ImVec2(670, 55), main_color, s.WindowRounding);

        RenderGIFAnimation(1.0);

        ImGui::SetCursorPosX(155);
        ImGui::SetCursorPosY(20);

        RenderCloudAnimation(clouds, main_color);

        const float tab_width = 40.0f;
        const float spacing = 10.0f;
        const int num_tabs = 6;
        const float total_width = (tab_width * num_tabs) + (spacing * (num_tabs - 1));

        const float right_padding = 190.0f;
        ImGui::SetCursorPos(ImVec2(827.0f - total_width - right_padding, 7.0f));

        ImGui::BeginGroup();

        if (ImGui::Tab(images::mouses, &iTabs, 0)) {
            NotificationManager::AddNotification("Switched to Aimbot", NotificationManager::NotificationType::INFO);
        }
        ImGui::SameLine(0, spacing);

        if (ImGui::Tab(images::eyes, &iTabs, 1)) {
            NotificationManager::AddNotification("Switched to Visuals", NotificationManager::NotificationType::INFO);
        }
        ImGui::SameLine(0, spacing);

        if (ImGui::Tab(images::risky, &iTabs, 2)) {
            NotificationManager::AddNotification("Switched to Info", NotificationManager::NotificationType::INFO);
        }
        ImGui::SameLine(0, spacing);

        if (ImGui::Tab(images::gear, &iTabs, 3)) {
            NotificationManager::AddNotification("Switched to Settings", NotificationManager::NotificationType::INFO);
        }
        ImGui::SameLine(0, spacing);

        ImGui::EndGroup();
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::SetCursorPos(ImVec2(25, 72));
        ImGui::BeginGroup();
        {
            if (iTabs == 0) {
                ImGui::BeginChild("Legitbot", ImVec2(frame_size.x * 2 + style.ItemSpacing.x, 532), true);
                {
                    if (ImGui::Checkbox("Enable", "Enable Aim", &settings::aimbot::enable)) {
                        StartCloudAnimation();
                    }
                    ImGui::Checkbox("RGB Fov", &settings::aimbot::show_fov);
                    ImGui::Combo("Aim Key", &settings::aimbot::current_aimkey, settings::aimbot::aimkey, IM_ARRAYSIZE(settings::aimbot::aimkey));
                }
                ImGui::EndChild();

                ImGui::SameLine();

                ImGui::SetCursorPosX(350);
                ImGui::BeginChild("Frame##2", ImVec2(frame_size.x * 2 + style.ItemSpacing.x, 532), true);
                {
                    ImGui::SliderFloat("FOV", &settings::aimbot::fov, 50, 300, "Fov: %.2f");
                    ImGui::SliderFloat("Smoothness", &settings::aimbot::smoothness, 1, 10, "Smoothness: %.2f");
                }
                ImGui::EndChild();
            }

            if (iTabs == 1) {
                ImGui::BeginChild("Ragebot", ImVec2(frame_size.x * 2.0f + style.ItemSpacing.x, 532.0f), true);
                {
                    ImGui::Columns(2, nullptr, false);
                    ImGui::Checkbox("Enable", &settings::visuals::enable);
                    ImGui::Checkbox("Skeleton", &settings::visuals::skeleton);
                    ImGui::Checkbox("Box", &settings::visuals::Box);
                    ImGui::Checkbox("Line", &settings::visuals::line);
                    ImGui::Checkbox("Outlines", &settings::visuals::outlines);
                    ImGui::Checkbox("Head Circle", &settings::visuals::headcircle);
                    ImGui::NextColumn();
                    ImGui::Checkbox("Distance", &settings::visuals::distance);
                    ImGui::Checkbox("Nazi Crosshair", &crosshair::Crosshair);
                    ImGui::Checkbox("Filled Box", &settings::visuals::fill_box);
                    ImGui::Checkbox("Cornered Box", &settings::visuals::CorneredBox);
                    ImGui::Checkbox("StreamProof", &settings::visuals::streamer);
                    ImGui::Checkbox("Gay Mode", &settings::visuals::rainbow_mode);
                }
                ImGui::EndChild();
            }

            if (iTabs == 2) {
                ImGui::BeginChild("Wallhack", ImVec2(frame_size.x * 2 + style.ItemSpacing.x, 532), true);
                {
                    if (ImGui::Button("Noxar.WTF Discord"))
                        ShellExecuteA(0, "open", "https://discord.gg/noxarrwtf", 0, 0, SW_SHOWNORMAL);
                    if (ImGui::Button("Noxar.WTF Website"))
                        ShellExecuteA(0, "open", "https://noxar.wtf", 0, 0, SW_SHOWNORMAL);
                    if (ImGui::Button("Exit Cheat"))
                        exit(0);
                }
                ImGui::EndChild();
            }

            if (iTabs == 3) {
                ImGui::BeginChild("Others", ImVec2(frame_size.x * 2 + style.ItemSpacing.x, 532), true);
                {
                    ImGui::Text("ESP Settings");
                    if (ImGui::ColorEdit4("CrossHair Color", crosshair::crosshair_color)) {
                        // Handle color change
                    }
                }
                ImGui::EndChild();
            }
        }
        ImGui::EndGroup();

        foremenu_drawlist = ImGui::GetWindowDrawList();
    }
    ImGui::End();
}

//
// ??? PARTICLE SYSTEM CODE ????????????????????????????????????????????????????????
//
#include "imgui.h" // adjust path as needed
#include <vector>
#include <random>

class M_Particle_System {
public:
    enum class prt_type {
        snow,
        rain,
        ash
    };

    void render(prt_type type);

private:
    struct Particle {
        float x = 0.0f, y = 0.0f;
        float fallSpeed = 0.0f, driftSpeed = 0.0f;
        uint8_t alpha = 0;
        int width = 0, height = 0;
    };

    std::vector<Particle> prts;

    Particle generate(prt_type type, bool firstTime = false);
    inline static int rand_int(int min, int max);
};

int M_Particle_System::rand_int(int min, int max) {
    static std::mt19937 gen(std::random_device{}());
    return std::uniform_int_distribution<>(min, max)(gen);
}

M_Particle_System::Particle M_Particle_System::generate(prt_type type, bool firstTime) {
    Particle p;
    auto io = ImGui::GetIO();
    p.x = static_cast<float>(rand_int(-static_cast<int>(io.DisplaySize.x / 2), static_cast<int>(io.DisplaySize.x)));
    p.y = static_cast<float>(rand_int(firstTime ? -static_cast<int>(io.DisplaySize.y * 2) : -100, -50));

    switch (type) {
    case prt_type::snow:
        p.fallSpeed = rand_int(50, 100) / 100.0f;
        p.driftSpeed = rand_int(10, 70) / 100.0f;
        p.alpha = static_cast<uint8_t>(rand_int(5, 255));
        p.width = p.height = rand_int(1, 2);
        break;
    case prt_type::rain:
        p.fallSpeed = rand_int(300, 600) / 100.0f;
        p.driftSpeed = rand_int(1, 10) / 100.0f;
        p.alpha = static_cast<uint8_t>(rand_int(100, 200));
        p.width = 1;
        p.height = rand_int(5, 10);
        break;
    case prt_type::ash:
        p.fallSpeed = rand_int(50, 100) / 100.0f;
        p.driftSpeed = rand_int(30, 100) / 100.0f;
        p.alpha = static_cast<uint8_t>(rand_int(50, 150));
        p.width = rand_int(2, 3);
        p.height = rand_int(4, 8);
        break;
    }
    return p;
}

void M_Particle_System::render(prt_type type) {
    float deltaTime = ImGui::GetIO().DeltaTime;
    if (prts.empty()) {
        prts.reserve(1400);
        for (int i = 0; i < 1400; ++i) {
            prts.push_back(generate(type, true));
        }
    }

    for (auto& p : prts) {
        if (p.y > ImGui::GetIO().DisplaySize.y + 50) {
            p = generate(type);
            continue;
        }
        p.x += p.driftSpeed * deltaTime * 100.0f;
        p.y += p.fallSpeed * deltaTime * 100.0f;

        ImU32 color;
        switch (type) {
        case prt_type::snow:
            color = ImColor(230, 230, 230, p.alpha);
            break;
        case prt_type::rain:
            color = ImColor(173, 216, 230, p.alpha);
            break;
        case prt_type::ash:
            color = ImColor(105, 105, 105, p.alpha);
            break;
        }
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + p.width, p.y + p.height), color);
    }
}

namespace M { inline M_Particle_System Particle; }
#include <imgui.h>
#include <imgui_internal.h>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <string>
#include <cstdlib>

// ?????????????????????????????????????????????????????????????????????????????
// Dummy Definitions & Global Variables (Replace these with your real ones)
// ?????????????????????????????????????????????????????????????????????????????
static ImVec4 main_color = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);   // Pastel blue (for accents)
static ImVec4 winbg_color = ImVec4(0.9f, 0.9f, 1.0f, 0.8f);   // Light pastel background
static ImVec4 experiment_color = ImVec4(0.8f, 0.8f, 0.9f, 0.5f);
static ImVec4 text_color[3] = {
    ImVec4(0.1f, 0.1f, 0.1f, 1.0f),
    ImVec4(0.2f, 0.2f, 0.2f, 1.0f),
    ImVec4(0.3f, 0.3f, 0.3f, 1.0f)
};

static ImVec2 frame_size = ImVec2(300, 532);
static ImDrawList* foremenu_drawlist = nullptr;
static ImFont* medium_font = nullptr; // Assume you load a font elsewhere

// Dummy settings (replace with your own settings system)
namespace settings {
    namespace aimbot {
        bool enable = false;
        bool show_fov = false;
        float fov = 100;
        float smoothness = 5;
        int current_aimkey = 0;
        const char* aimkey[3] = { "None", "Key1", "Key2" };
    }
    namespace visuals {
        bool enable = false;
        bool skeleton = false;
        bool Box = false;
        bool line = false;
        bool outlines = false;
        bool headcircle = false;
        bool distance = false;
        bool fill_box = false;
        bool CorneredBox = false;
        bool streamer = false;
        bool rainbow_mode = false;
    }
}
namespace crosshair {
    bool Crosshair = false;
    float crosshair_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
}

// Cheat info (for your infobar, etc.)
const char* cheat_name = "Noxar";
const char* developer = "External";
const char* role = "FN";

// Dummy images (replace with your loaded textures)
namespace images {
    ImTextureID mouses = nullptr;
    ImTextureID eyes = nullptr;
    ImTextureID gear = nullptr;
    ImTextureID folder = nullptr;
    ImTextureID risky = nullptr;
    ImTextureID saving = nullptr;
}

// ?????????????????????????????????????????????????????????????????????????????
// Notification System
// ?????????????????????????????????????????????????????????????????????????????
struct Notification {
    std::string message;
    ImColor color;
    float animationProgress;
    float timeLeft;
    bool shouldRemove;
};

class NotificationManager {
private:
    static std::vector<Notification> notifications;
    static constexpr float NOTIFICATION_LIFETIME = 3.0f;
    static constexpr float ANIMATION_SPEED = 1.0f / 0.15f;
public:
    enum class NotificationType { SUCCESS, WARNING, ERRORS, INFO };
    static void AddNotification(const std::string& message, NotificationType type) {
        Notification notif;
        notif.message = message;
        notif.timeLeft = NOTIFICATION_LIFETIME;
        notif.animationProgress = 0.0f;
        notif.shouldRemove = false;
        notif.color = ImColor(main_color);
        notifications.push_back(notif);
    }
    static void Render() {
        ImGuiIO& io = ImGui::GetIO();
        float deltaTime = io.DeltaTime;
        const float NOTIFICATION_WIDTH = 240.0f;
        const float NOTIFICATION_HEIGHT = 40.0f;
        const float NOTIFICATION_PADDING = 8.0f;
        float currentY = 10.0f;
        for (auto& notification : notifications) {
            if (notification.animationProgress < 1.0f) {
                notification.animationProgress += deltaTime * ANIMATION_SPEED;
                if (notification.animationProgress > 1.0f)
                    notification.animationProgress = 1.0f;
            }
            notification.timeLeft -= deltaTime;
            if (notification.timeLeft <= 0.0f) {
                notification.shouldRemove = true;
                continue;
            }
            float alpha = notification.timeLeft < 0.5f ? notification.timeLeft / 0.5f : 1.0f;
            float targetX = 1920.0f - NOTIFICATION_WIDTH - 20.0f;
            float startX = 1920.0f + NOTIFICATION_WIDTH;
            float currentX = startX + (targetX - startX) * notification.animationProgress;
            ImGui::SetNextWindowPos(ImVec2(currentX, currentY));
            ImGui::SetNextWindowSize(ImVec2(NOTIFICATION_WIDTH, NOTIFICATION_HEIGHT));
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(13, 14, 16, 200).Value);
            std::string windowName = "##Notification" + std::to_string(&notification - &notifications[0]);
            if (ImGui::Begin(windowName.c_str(), nullptr,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration)) {
                ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", notification.message.c_str());
            }
            ImGui::End();
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(3);
            currentY += NOTIFICATION_HEIGHT + NOTIFICATION_PADDING;
        }
        notifications.erase(std::remove_if(notifications.begin(), notifications.end(),
            [](const Notification& n) { return n.shouldRemove; }), notifications.end());
    }
};
std::vector<Notification> NotificationManager::notifications;

// ?????????????????????????????????????????????????????????????????????????????
// Particle System ("Snow" effect)
// ?????????????????????????????????????????????????????????????????????????????
class M_Particle_System {
public:
    enum class prt_type { snow, rain, ash };
    void render(prt_type type);
private:
    struct Particle {
        float x = 0.0f, y = 0.0f;
        float fallSpeed = 0.0f, driftSpeed = 0.0f;
        uint8_t alpha = 0;
        int width = 0, height = 0;
    };
    std::vector<Particle> prts;
    Particle generate(prt_type type, bool firstTime = false);
    inline static int rand_int(int min, int max);
};

int M_Particle_System::rand_int(int min, int max) {
    static std::mt19937 gen(std::random_device{}());
    return std::uniform_int_distribution<>(min, max)(gen);
}

M_Particle_System::Particle M_Particle_System::generate(prt_type type, bool firstTime) {
    Particle p;
    auto io = ImGui::GetIO();
    p.x = static_cast<float>(rand_int(-static_cast<int>(io.DisplaySize.x / 2), static_cast<int>(io.DisplaySize.x)));
    p.y = static_cast<float>(rand_int(firstTime ? -static_cast<int>(io.DisplaySize.y * 2) : -100, -50));
    switch (type) {
    case prt_type::snow:
        p.fallSpeed = rand_int(50, 100) / 100.0f;
        p.driftSpeed = rand_int(10, 70) / 100.0f;
        p.alpha = static_cast<uint8_t>(rand_int(5, 255));
        p.width = p.height = rand_int(1, 2);
        break;
    case prt_type::rain:
        p.fallSpeed = rand_int(300, 600) / 100.0f;
        p.driftSpeed = rand_int(1, 10) / 100.0f;
        p.alpha = static_cast<uint8_t>(rand_int(100, 200));
        p.width = 1;
        p.height = rand_int(5, 10);
        break;
    case prt_type::ash:
        p.fallSpeed = rand_int(50, 100) / 100.0f;
        p.driftSpeed = rand_int(30, 100) / 100.0f;
        p.alpha = static_cast<uint8_t>(rand_int(50, 150));
        p.width = rand_int(2, 3);
        p.height = rand_int(4, 8);
        break;
    }
    return p;
}

void M_Particle_System::render(prt_type type) {
    float deltaTime = ImGui::GetIO().DeltaTime;
    if (prts.empty()) {
        prts.reserve(1400);
        for (int i = 0; i < 1400; ++i) {
            prts.push_back(generate(type, true));
        }
    }
    for (auto& p : prts) {
        if (p.y > ImGui::GetIO().DisplaySize.y + 50) {
            p = generate(type);
            continue;
        }
        p.x += p.driftSpeed * deltaTime * 100.0f;
        p.y += p.fallSpeed * deltaTime * 100.0f;
        ImU32 color;
        switch (type) {
        case prt_type::snow:
            color = ImColor(230, 230, 230, p.alpha);
            break;
        case prt_type::rain:
            color = ImColor(173, 216, 230, p.alpha);
            break;
        case prt_type::ash:
            color = ImColor(105, 105, 105, p.alpha);
            break;
        }
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + p.width, p.y + p.height), color);
    }
}
namespace M { inline M_Particle_System Particle; }

// ?????????????????????????????????????????????????????????????????????????????
// Snowy GUI Menu (Improved Style)
// ?????????????????????????????????????????????????????????????????????????????
void menu() {
    ImGui::Begin("Snowy Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    {
        // Set a snowy pastel background
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.9f, 0.9f, 1.0f, 0.95f));
        ImGui::TextColored(ImVec4(0.1f, 0.1f, 0.3f, 1.0f), "Snowy GUI Menu");
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Aimbot Settings")) {
            ImGui::Checkbox("Enable Aimbot", &settings::aimbot::enable);
            ImGui::Checkbox("RGB FOV", &settings::aimbot::show_fov);
            ImGui::SliderFloat("FOV", &settings::aimbot::fov, 50, 300);
            ImGui::SliderFloat("Smoothness", &settings::aimbot::smoothness, 1, 10);
            ImGui::Combo("Aim Key", &settings::aimbot::current_aimkey, settings::aimbot::aimkey, 3);
        }
        if (ImGui::CollapsingHeader("Visuals Settings")) {
            ImGui::Checkbox("Enable Visuals", &settings::visuals::enable);
            ImGui::Checkbox("Skeleton", &settings::visuals::skeleton);
            ImGui::Checkbox("Box", &settings::visuals::Box);
            ImGui::Checkbox("Line", &settings::visuals::line);
            ImGui::Checkbox("Outlines", &settings::visuals::outlines);
            ImGui::Checkbox("Head Circle", &settings::visuals::headcircle);
            ImGui::Checkbox("Distance", &settings::visuals::distance);
            ImGui::Checkbox("StreamProof", &settings::visuals::streamer);
            ImGui::Checkbox("Rainbow Mode", &settings::visuals::rainbow_mode);
        }
        if (ImGui::CollapsingHeader("Misc")) {
            if (ImGui::Button("Open Discord"))
                NotificationManager::AddNotification("Discord Button Pressed", NotificationManager::NotificationType::INFO);
            if (ImGui::Button("Open Website"))
                NotificationManager::AddNotification("Website Button Pressed", NotificationManager::NotificationType::INFO);
            if (ImGui::Button("Exit"))
                exit(0);
        }
        ImGui::PopStyleColor();
    }
    ImGui::End();
}

// ?????????????????????????????????????????????????????????????????????????????
// Main Render Loop (with Toggle on Insert)
// ?????????????????????????????????????????????????????????????????????????????
void RenderFrame() {
    // Toggle the menu with the Insert key
    static bool showMenu = false;
    if (ImGui::IsKeyPressed(VK_INSERT))
        showMenu = !showMenu;

    // Always render the snow particle effect in the background
    M::Particle.render(M_Particle_System::prt_type::snow);

    // Render the GUI menu if toggled on
    if (showMenu)
        menu();

    // Render notifications (if any)
    NotificationManager::Render();
}

// ?????????????????????????????????????????????????????????????????????????????
// Main Entry Point (Simplified Example)
// ?????????????????????????????????????????????????????????????????????????????
int main(int, char**)
{
    // NOTE: You must initialize your platform/renderer (GLFW, SDL, DX11, etc.) and ImGui context here.
    // This is a simplified example main loop.

    // For example:
    //   - Initialize your window
    //   - Create ImGui context, set up bindings, load fonts, etc.

    // Main loop:
    while (true) {
        // Start the Dear ImGui frame
        ImGui::NewFrame();

        // Call our render function (this will check for Insert and draw the menu)
        RenderFrame();

        // Render ImGui
        ImGui::Render();

        // Your platform-specific render call here (e.g. swap buffers)
    }
    return 0;
}
