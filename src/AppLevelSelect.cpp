#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

#include <array>
#include <string>

namespace {
    constexpr glm::vec2 LEVEL_SELECT_HEXAGON_HOTSPOT_SIZE = {78.0f, 72.0f};
    constexpr glm::vec2 LEVEL_SELECT_TRIANGLE_HOTSPOT_SIZE = {88.0f, 78.0f};
    constexpr glm::vec2 LEVEL_SELECT_DIAMOND_HOTSPOT_SIZE = {70.0f, 60.0f};
    constexpr glm::vec2 LEVEL_SELECT_HEXAGON_MARKER_SIZE = {116.0f, 104.0f};
    constexpr glm::vec2 LEVEL_SELECT_TRIANGLE_MARKER_SIZE = {112.0f, 102.0f};
    constexpr glm::vec2 LEVEL_SELECT_DIAMOND_MARKER_SIZE = {118.0f, 100.0f};
}

void App::UpdateLevelSelect() {
    if (!m_LevelSelectBuilt) {
        m_Root = Util::Renderer();
        m_LevelSelectButtons.clear();

        m_LevelSelectBackground = std::make_shared<BackgroundImage>(
            std::string(GA_RESOURCE_DIR) + "/Image/Background/manu.png"
        );
        m_LevelSelectBackground->SetPosition({0.0f, 0.0f});
        m_Root.AddChild(m_LevelSelectBackground);

        struct ButtonLayout {
            int levelIndex;
            glm::vec2 center;
            glm::vec2 hotspotSize;
            const char* hoverImagePath;
            glm::vec2 hoverSize;
        };

        const std::array<ButtonLayout, 5> layouts = {{
            {
                1,
                {-14.0f, -311.0f},
                LEVEL_SELECT_HEXAGON_HOTSPOT_SIZE,
                GA_RESOURCE_DIR "/Image/Assets/level_hexagon.png",
                LEVEL_SELECT_HEXAGON_MARKER_SIZE,
            },
            {
                2,
                {22.0f, -234.0f},
                LEVEL_SELECT_HEXAGON_HOTSPOT_SIZE,
                GA_RESOURCE_DIR "/Image/Assets/level_hexagon.png",
                LEVEL_SELECT_HEXAGON_MARKER_SIZE,
            },
            {
                3,
                {3.0f, -148.0f},
                LEVEL_SELECT_TRIANGLE_HOTSPOT_SIZE,
                GA_RESOURCE_DIR "/Image/Assets/level_triangle.png",
                LEVEL_SELECT_TRIANGLE_MARKER_SIZE,
            },
            {
                4,
                {3.0f, -48.0f},
                LEVEL_SELECT_HEXAGON_HOTSPOT_SIZE,
                GA_RESOURCE_DIR "/Image/Assets/level_hexagon.png",
                LEVEL_SELECT_HEXAGON_MARKER_SIZE,
            },
            {
                5,
                {-180.5f, -91.5f},
                LEVEL_SELECT_DIAMOND_HOTSPOT_SIZE,
                GA_RESOURCE_DIR "/Image/Assets/level_diamond.png",
                LEVEL_SELECT_DIAMOND_MARKER_SIZE,
            },
        }};

        for (const auto& layout : layouts) {
            LevelSelectButton button;
            button.levelIndex = layout.levelIndex;
            button.rect = {layout.center, layout.hotspotSize};
            button.sprite = std::make_shared<Character>(layout.hoverImagePath);
            button.sprite->SetSize(layout.hoverSize);
            button.sprite->SetPosition(layout.center);
            button.sprite->SetZIndex(2.0f);
            button.sprite->SetVisible(false);
            m_Root.AddChild(button.sprite);
            m_LevelSelectButtons.push_back(button);
        }

        m_LevelSelectMouseLatch = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);
        m_LevelSelectBuilt = true;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
        m_Root.Update();
        return;
    }

    const glm::vec2 cursor = Util::Input::GetCursorPosition();
    const bool mouseDown = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);

    for (const auto& button : m_LevelSelectButtons) {
        const bool hovered = CheckAABB(
            cursor,
            {1.0f, 1.0f},
            button.rect.center,
            button.rect.size
        );

        if (button.sprite) {
            button.sprite->SetVisible(hovered);
        }

        if (hovered && mouseDown && !m_LevelSelectMouseLatch) {
            m_ActiveLevelIndex = button.levelIndex;
            m_LevelSelectMouseLatch = true;
            m_LevelSelectBuilt = false;
            m_CurrentState = State::START;
            m_Root.Update();
            return;
        }
    }

    m_LevelSelectMouseLatch = mouseDown;
    m_Root.Update();
}
