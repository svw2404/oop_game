#include "App.hpp"

#include "Util\Input.hpp"
#include "Util\Keycode.hpp"
#include "Util\Time.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace {
std::string FormatClock(float elapsedMs) {
    const int totalSeconds =
        std::max(0, static_cast<int>(std::floor(elapsedMs / 1000.0f)));
    const int minutes = totalSeconds / 60;
    const int seconds = totalSeconds % 60;

    std::ostringstream stream;
    stream << std::setfill('0')
           << std::setw(2) << minutes
           << ":"
           << std::setw(2) << seconds;
    return stream.str();
}
}

void App::BuildGameplayUi() {
    const std::string panelPath =
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/ui_fail_overlay_panel.png";

    m_TimerHudPanel = std::make_shared<Character>(panelPath);
    m_TimerHudPanel->SetZIndex(40.0f);
    m_TimerHudPanel->SetSize({156.0f, 48.0f});
    m_TimerHudPanel->SetPosition({
        0.0f,
        m_BackgroundDisplayedSize.y * 0.5f - 34.0f,
    });
    m_Root.AddChild(m_TimerHudPanel);

    m_TimerHudText = std::make_shared<OverlayText>(
        "00:00",
        24,
        Util::Color(255, 245, 220, 255),
        41.0f
    );
    m_TimerHudText->m_Transform.translation = m_TimerHudPanel->GetPosition();
    m_Root.AddChild(m_TimerHudText);

    auto addPauseObject = [&](const std::shared_ptr<Util::GameObject>& object) {
        object->SetVisible(false);
        m_Root.AddChild(object);
        m_PauseOverlayObjects.push_back(object);
    };

    m_PauseDimmer = std::make_shared<Character>(panelPath);
    m_PauseDimmer->SetZIndex(60.0f);
    m_PauseDimmer->SetSize(m_BackgroundDisplayedSize);
    m_PauseDimmer->SetPosition({0.0f, 0.0f});
    addPauseObject(m_PauseDimmer);

    m_PauseOverlayPanel = std::make_shared<Character>(panelPath);
    m_PauseOverlayPanel->SetZIndex(61.0f);
    m_PauseOverlayPanel->SetSize({620.0f, 470.0f});
    m_PauseOverlayPanel->SetPosition({0.0f, 8.0f});
    addPauseObject(m_PauseOverlayPanel);

    auto makePauseText = [&](const std::string& text,
                             const glm::vec2& position,
                             int fontSize,
                             const Util::Color& color) {
        auto object = std::make_shared<OverlayText>(
            text,
            fontSize,
            color,
            62.0f
        );
        object->m_Transform.translation = position;
        addPauseObject(object);
        return object;
    };

    auto makePauseIcon = [&](const std::string& path,
                             const glm::vec2& position) {
        auto object = std::make_shared<Character>(path);
        object->SetZIndex(62.0f);
        object->SetSize({40.0f, 36.0f});
        object->SetPosition(position);
        addPauseObject(object);
        return object;
    };

    m_PauseOverlayTitle = makePauseText(
        "PAUSED",
        {0.0f, 166.0f},
        36,
        Util::Color(255, 245, 220, 255)
    );

    auto makePauseButton = [&](PauseAction action,
                               const std::string& label,
                               const glm::vec2& center) {
        PauseOverlayButton button;
        button.action = action;
        button.rect = {center, {300.0f, 58.0f}};
        button.labelObject = makePauseText(
            label,
            center,
            27,
            Util::Color(255, 255, 255, 255)
        );
        return button;
    };

    m_PauseMainMenuButton = makePauseButton(
        PauseAction::MainMenu,
        "MAIN MENU",
        {0.0f, 94.0f}
    );
    m_PauseRetryButton = makePauseButton(
        PauseAction::Retry,
        "RETRY LEVEL",
        {0.0f, 34.0f}
    );

    m_PauseBlueDiamondIcon = makePauseIcon(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_blue.png",
        {-112.0f, -38.0f}
    );
    m_PauseBlueDiamondText = makePauseText(
        "x 0",
        {-62.0f, -38.0f},
        24,
        Util::Color(240, 248, 255, 255)
    );
    m_PauseRedDiamondIcon = makePauseIcon(
        std::string(GA_RESOURCE_DIR) + "/Image/Assets/diamond_red.png",
        {52.0f, -38.0f}
    );
    m_PauseRedDiamondText = makePauseText(
        "x 0",
        {102.0f, -38.0f},
        24,
        Util::Color(255, 235, 230, 255)
    );

    m_PauseResumeButton = makePauseButton(
        PauseAction::Resume,
        "RESUME  >",
        {0.0f, -126.0f}
    );

    UpdateTimerHud();
    UpdatePauseOverlayVisuals();
}

float App::GetDisplayedLevelTimeMs() const {
    if (m_LevelCompleteTimeMs > 0.0f) {
        return m_LevelCompleteTimeMs;
    }

    const float now = m_GamePaused
        ? m_PauseStartedTimeMs
        : Util::Time::GetElapsedTimeMs();
    return std::max(0.0f, now - m_LevelStartTimeMs);
}

void App::UpdateTimerHud() {
    if (!m_TimerHudText) {
        return;
    }

    m_TimerHudText->SetText(FormatClock(GetDisplayedLevelTimeMs()));
}

bool App::HandlePauseToggle() {
    if (m_VictoryPhase != VictoryPhase::None || IsFailOverlayVisible()) {
        return false;
    }

    const bool togglePressed =
        Util::Input::IsKeyDown(Util::Keycode::P) ||
        Util::Input::IsKeyDown(Util::Keycode::ESCAPE);
    if (!togglePressed) {
        return false;
    }

    if (m_GamePaused) {
        ResumeGameplay();
    }
    else {
        m_GamePaused = true;
        m_PauseStartedTimeMs = Util::Time::GetElapsedTimeMs();
        m_PauseMouseLatch =
            Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);
        UpdatePauseOverlayVisuals();
    }
    return true;
}

void App::ResumeGameplay() {
    if (!m_GamePaused) {
        return;
    }

    const float pausedDuration = std::max(
        0.0f,
        Util::Time::GetElapsedTimeMs() - m_PauseStartedTimeMs
    );
    m_LevelStartTimeMs += pausedDuration;
    m_PauseStartedTimeMs = 0.0f;
    m_GamePaused = false;
    UpdatePauseOverlayVisuals();
}

void App::UpdatePauseOverlayVisuals() {
    for (const auto& object : m_PauseOverlayObjects) {
        if (object) {
            object->SetVisible(m_GamePaused);
        }
    }

    if (!m_GamePaused) {
        return;
    }

    if (m_PauseBlueDiamondText) {
        m_PauseBlueDiamondText->SetText(
            "x " + std::to_string(m_WaterDiamondsCollected)
        );
    }
    if (m_PauseRedDiamondText) {
        m_PauseRedDiamondText->SetText(
            "x " + std::to_string(m_FireDiamondsCollected)
        );
    }

    const glm::vec2 cursor = Util::Input::GetCursorPosition();
    const bool mouseDown = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);
    const auto normalColor = Util::Color(255, 255, 255, 255);
    const auto hoveredColor = Util::Color(255, 220, 140, 255);

    for (PauseOverlayButton* button : {
             &m_PauseMainMenuButton,
             &m_PauseRetryButton,
             &m_PauseResumeButton,
         }) {
        if (!button->labelObject) {
            continue;
        }

        const bool hovered = CheckAABB(
            cursor,
            {1.0f, 1.0f},
            button->rect.center,
            button->rect.size
        );
        button->labelObject->SetColor(hovered ? hoveredColor : normalColor);
        const float scale = hovered ? (mouseDown ? 0.98f : 1.05f) : 1.0f;
        button->labelObject->m_Transform.scale = {scale, scale};
    }
}

void App::UpdatePauseOverlay() {
    if (!m_GamePaused) {
        return;
    }

    UpdateTimerHud();
    UpdatePauseOverlayVisuals();

    const bool mouseDown = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);
    const bool clicked = mouseDown && !m_PauseMouseLatch;
    PauseAction action = PauseAction::None;

    if (clicked) {
        const glm::vec2 cursor = Util::Input::GetCursorPosition();
        for (const PauseOverlayButton* button : {
                 &m_PauseMainMenuButton,
                 &m_PauseRetryButton,
                 &m_PauseResumeButton,
             }) {
            if (CheckAABB(
                    cursor,
                    {1.0f, 1.0f},
                    button->rect.center,
                    button->rect.size
                )) {
                action = button->action;
                break;
            }
        }
    }

    m_PauseMouseLatch = mouseDown;

    switch (action) {
    case PauseAction::MainMenu:
        m_GamePaused = false;
        m_CurrentState = State::LEVEL_SELECT;
        break;
    case PauseAction::Retry:
        m_GamePaused = false;
        m_CurrentState = State::START;
        break;
    case PauseAction::Resume:
        ResumeGameplay();
        break;
    case PauseAction::None:
        break;
    }
}
