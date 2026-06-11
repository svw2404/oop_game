#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

namespace {
constexpr glm::vec2 VOLUME_ON_SIZE = {54.0f, 44.0f};
constexpr glm::vec2 VOLUME_OFF_SIZE = {34.0f, 44.0f};
constexpr glm::vec2 VOLUME_HITBOX_SIZE = {64.0f, 56.0f};
constexpr float VOLUME_LEFT_MARGIN_FROM_CENTER = 72.0f;
constexpr float VOLUME_TOP_MARGIN = 16.0f;
constexpr float VOLUME_HOVER_SCALE = 1.08f;
}

void App::BuildVolumeControl() {
    if (!m_AudioStateInitialized) {
        m_UnmutedMusicVolume = Mix_VolumeMusic(-1);
        m_UnmutedSfxVolume = Mix_Volume(-1, -1);
        m_AudioStateInitialized = true;
    }

    const float left =
        m_BackgroundDisplayedSize.x * 0.5f - VOLUME_LEFT_MARGIN_FROM_CENTER;
    const float top = m_BackgroundDisplayedSize.y * 0.5f - VOLUME_TOP_MARGIN;
    m_VolumeControlRect = {
        {left + VOLUME_HITBOX_SIZE.x * 0.5f,
         top - VOLUME_HITBOX_SIZE.y * 0.5f},
        VOLUME_HITBOX_SIZE,
    };

    m_VolumeControlIcon = std::make_shared<Character>(
        m_AudioMuted
            ? GA_RESOURCE_DIR "/Image/Assets/volume_off.png"
            : GA_RESOURCE_DIR "/Image/Assets/volume_on.png"
    );
    m_VolumeControlIcon->SetZIndex(90.0f);
    m_Root.AddChild(m_VolumeControlIcon);

    m_VolumeControlMouseLatch =
        Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);
    UpdateVolumeControlVisual(false);
    ApplyVolumeState();
}

void App::ApplyVolumeState() {
    Mix_VolumeMusic(m_AudioMuted ? 0 : m_UnmutedMusicVolume);
    Mix_Volume(-1, m_AudioMuted ? 0 : m_UnmutedSfxVolume);
}

void App::UpdateVolumeControlVisual(bool hovered) {
    if (!m_VolumeControlIcon) {
        return;
    }

    const glm::vec2 iconSize = m_AudioMuted ? VOLUME_OFF_SIZE : VOLUME_ON_SIZE;
    const float left =
        m_BackgroundDisplayedSize.x * 0.5f - VOLUME_LEFT_MARGIN_FROM_CENTER;
    const float top = m_BackgroundDisplayedSize.y * 0.5f - VOLUME_TOP_MARGIN;

    const std::string imagePath =
        m_AudioMuted
            ? GA_RESOURCE_DIR "/Image/Assets/volume_off.png"
            : GA_RESOURCE_DIR "/Image/Assets/volume_on.png";
    if (m_VolumeControlIcon->GetImagePath() != imagePath) {
        m_VolumeControlIcon->SetImage(imagePath);
    }
    m_VolumeControlIcon->SetSize(iconSize);
    m_VolumeControlIcon->SetPosition({
        left + iconSize.x * 0.5f,
        top - iconSize.y * 0.5f,
    });

    const float scale = hovered ? VOLUME_HOVER_SCALE : 1.0f;
    m_VolumeControlIcon->m_Transform.scale = {scale, scale};
}

bool App::UpdateVolumeControl() {
    if (!m_VolumeControlIcon) {
        return false;
    }

    const bool hovered = CheckAABB(
        Util::Input::GetCursorPosition(),
        {1.0f, 1.0f},
        m_VolumeControlRect.center,
        m_VolumeControlRect.size
    );
    const bool mouseDown = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);
    const bool clicked = hovered && mouseDown && !m_VolumeControlMouseLatch;

    if (clicked) {
        m_AudioMuted = !m_AudioMuted;
        ApplyVolumeState();
    }

    UpdateVolumeControlVisual(hovered);
    m_VolumeControlMouseLatch = mouseDown;
    return clicked;
}
