#include "Util/Animation.hpp"

#include "Util/Logger.hpp"
#include "Core/Texture.hpp"
#include <SDL.h>

namespace Util {

Animation::Animation(const std::vector<std::string> &paths, bool play,
                     std::size_t interval, bool looping, std::size_t cooldown)
    : m_State(State::PAUSE), m_Interval(static_cast<double>(interval)),
      m_Looping(looping), m_Cooldown(cooldown), m_Index(0),
      m_LastUpdateTime(0), m_TimeBetweenFrameUpdate(0) {
    m_Frames.reserve(paths.size());
    for (const auto &p : paths) {
        try {
            auto img = std::make_shared<Util::Image>(p);
            m_Frames.push_back(img);
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to load animation frame: {}", p);
        }
    }
    if (play) {
        Play();
    }
}

void Animation::SetCurrentFrame(std::size_t index) {
    if (index < m_Frames.size()) {
        m_Index = index;
    }
}

void Animation::Draw(const Core::Matrices &data) {
    if (m_Frames.empty()) return;

    // 每次繪製前先更新動畫進度（這保證在沒有外部 Update 呼叫時也會前進）
    Update();

    // 畫目前幀
    m_Frames[m_Index]->Draw(data);
}

void Animation::Play() {
    // 如果已結束且不循環，從頭開始播放
    if (m_State == State::ENDED && !m_Looping) {
        m_Index = 0;
    }
    m_State = State::PLAY;
    m_LastUpdateTime = SDL_GetTicks();
    m_TimeBetweenFrameUpdate = 0;
}

void Animation::Pause() {
    m_State = State::PAUSE;
}

// Update 負責以 m_Interval（毫秒）推進幀
void Animation::Update() {
    if (m_State != State::PLAY) return;
    if (m_Frames.empty()) return;

    unsigned long now = SDL_GetTicks();

    if (m_LastUpdateTime == 0) {
        m_LastUpdateTime = now;
        return;
    }

    unsigned long delta = now - m_LastUpdateTime;
    m_TimeBetweenFrameUpdate += static_cast<double>(delta);

    while (m_TimeBetweenFrameUpdate >= m_Interval && m_Frames.size() > 0) {
        m_TimeBetweenFrameUpdate -= m_Interval;
        // 推進幀
        m_Index++;
        if (m_Index >= m_Frames.size()) {
            if (m_Looping) {
                m_Index = 0;
            } else {
                // 到末幀並停止（或進入 ENDED）
                m_Index = m_Frames.size() - 1;
                m_State = State::ENDED;
                // 若 cooldown 設定 >0，可設定 m_CooldownEndTime = now + m_Cooldown;
                break;
            }
        }
    }

    m_LastUpdateTime = now;
}

void Animation::SetSize(const glm::vec2 &size) {
    for (auto &frame : m_Frames) {
        if (frame) {
            frame->SetSize(size);
        }
    }
}

} // namespace Util