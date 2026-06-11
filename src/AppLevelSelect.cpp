#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

#include <array>
#include <string>

namespace {
    // TESTING ONLY: keep every level selectable while the levels are audited.
    // Restore this to true before release to re-enable sequential unlocking and NEW!.
    constexpr bool LEVEL_PROGRESSION_LOCKS_ENABLED = false;
    constexpr glm::vec2 LEVEL_SELECT_HEXAGON_HOTSPOT_SIZE = {78.0f, 72.0f};
    constexpr glm::vec2 LEVEL_SELECT_TRIANGLE_HOTSPOT_SIZE = {88.0f, 78.0f};
    constexpr glm::vec2 LEVEL_SELECT_HEXAGON_MARKER_SIZE = {116.0f, 104.0f};
    constexpr glm::vec2 LEVEL_SELECT_TRIANGLE_MARKER_SIZE = {112.0f, 102.0f};
    constexpr glm::vec2 LEVEL_SELECT_NEW_LABEL_OFFSET = {58.0f, 28.0f};
}

void App::UpdateLevelSelect() {
    LoadLevelProgress();

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
                LEVEL_SELECT_HEXAGON_HOTSPOT_SIZE,
                GA_RESOURCE_DIR "/Image/Assets/level_hexagon.png",
                LEVEL_SELECT_HEXAGON_MARKER_SIZE,
            },
        }};

        for (const auto& layout : layouts) {
            LevelSelectButton button;
            button.levelIndex = layout.levelIndex;
            button.rect = {layout.center, layout.hotspotSize};
            button.unlocked =
                !LEVEL_PROGRESSION_LOCKS_ENABLED || IsLevelUnlocked(layout.levelIndex);

            button.sprite = std::make_shared<Character>(layout.hoverImagePath);
            button.sprite->SetSize(layout.hoverSize);
            button.sprite->SetPosition(layout.center);
            button.sprite->SetZIndex(2.0f);
            button.sprite->SetVisible(IsLevelCompleted(layout.levelIndex));
            m_Root.AddChild(button.sprite);

            if (LEVEL_PROGRESSION_LOCKS_ENABLED &&
                layout.levelIndex == m_NewlyUnlockedLevelIndex &&
                button.unlocked &&
                !IsLevelCompleted(layout.levelIndex)) {
                button.newLabel = std::make_shared<OverlayText>(
                    "NEW!",
                    20,
                    Util::Color(255, 218, 92, 255),
                    3.0f
                );
                button.newLabel->m_Transform.translation =
                    layout.center + LEVEL_SELECT_NEW_LABEL_OFFSET;
                m_Root.AddChild(button.newLabel);
            }

            m_LevelSelectButtons.push_back(button);
        }

        BuildVolumeControl();
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
    const bool volumeClicked = UpdateVolumeControl();

    for (const auto& button : m_LevelSelectButtons) {
        const bool hovered = CheckAABB(
            cursor,
            {1.0f, 1.0f},
            button.rect.center,
            button.rect.size
        );

        if (button.sprite) {
            button.sprite->SetVisible(
                IsLevelCompleted(button.levelIndex) ||
                (hovered && button.unlocked)
            );
        }

        if (!volumeClicked &&
            hovered &&
            button.unlocked &&
            mouseDown &&
            !m_LevelSelectMouseLatch) {
            if (button.levelIndex == m_NewlyUnlockedLevelIndex) {
                m_NewlyUnlockedLevelIndex = 0;
            }
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
