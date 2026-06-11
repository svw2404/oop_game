#include "App.hpp"

void App::LoadLevelProgress() {
    if (m_LevelProgressLoaded) {
        return;
    }

    m_LevelProgressLoaded = true;
    m_CompletedLevels.fill(false);
    m_NewlyUnlockedLevelIndex = 0;
}

void App::MarkActiveLevelCompleted() {
    LoadLevelProgress();
    if (m_ActiveLevelIndex < 1 ||
        m_ActiveLevelIndex > static_cast<int>(m_CompletedLevels.size())) {
        return;
    }

    bool& completed = m_CompletedLevels[static_cast<std::size_t>(m_ActiveLevelIndex - 1)];
    if (completed) {
        return;
    }

    completed = true;
    const int nextLevelIndex = m_ActiveLevelIndex + 1;
    if (nextLevelIndex <= static_cast<int>(m_CompletedLevels.size())) {
        m_NewlyUnlockedLevelIndex = nextLevelIndex;
    }
}

bool App::IsLevelCompleted(int levelIndex) const {
    return levelIndex >= 1 &&
        levelIndex <= static_cast<int>(m_CompletedLevels.size()) &&
        m_CompletedLevels[static_cast<std::size_t>(levelIndex - 1)];
}

bool App::IsLevelUnlocked(int levelIndex) const {
    if (levelIndex <= 1) {
        return true;
    }
    return IsLevelCompleted(levelIndex - 1);
}
