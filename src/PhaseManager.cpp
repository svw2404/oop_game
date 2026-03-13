#include "PhaseResourceManger.hpp"

#include "Util/Logger.hpp"

PhaseResourceManger::PhaseResourceManger() {
    m_TaskText = std::make_shared<TaskText>();
    m_Background = std::make_shared<BackgroundImage>();
   
    // 在這裡調整縮放比例
    // {1.0f, 1.0f} 是原圖大小
    // 如果圖片太大，試試 {0.5f, 0.5f}；如果太小，試試 {2.0f, 2.0f}
    m_Background->SetSize({ 0.8f, 0.8f });
}

void PhaseResourceManger::NextPhase() {
    if (m_Phase == 7) return;
    LOG_DEBUG("Passed! Next phase: {}", m_Phase);
    m_Background->NextPhase(m_Phase);
    m_TaskText->NextPhase(m_Phase++);
}
