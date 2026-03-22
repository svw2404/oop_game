#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

#include <unordered_map>

namespace {
std::unordered_map<HeadBodyCharacter*, glm::vec2> s_LastPos;
constexpr float IDLE_EPSILON = 0.5f;
}

void App::Update() {
    HandleFireboyInput();
    UpdateFireboyPhysics();
    CheckDiamondCollection();

    const glm::vec2 pos = m_Fireboy->GetPosition();
    const glm::vec2 prev = s_LastPos[m_Fireboy.get()];
    const float moved = glm::length(pos - prev);
    m_Fireboy->SetIdleState(moved < IDLE_EPSILON);
    s_LastPos[m_Fireboy.get()] = pos;

    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    m_Root.Update();
}

void App::HandleFireboyInput() {
    glm::vec2 oldPos = m_Fireboy->GetPosition();
    glm::vec2 newPos = oldPos;

    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        newPos.x -= m_MoveSpeed;
        m_Fireboy->SetScale({-1.0f, 1.0f});
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        newPos.x += m_MoveSpeed;
        m_Fireboy->SetScale({1.0f, 1.0f});
    }

    ResolveHorizontalCollisions(oldPos, newPos);
    m_Fireboy->SetPosition(newPos);

    if (Util::Input::IsKeyPressed(Util::Keycode::W) && m_FireboyOnGround) {
        m_FireboyVelocity.y = m_JumpSpeed;
        m_FireboyOnGround = false;
    }
}

void App::UpdateFireboyPhysics() {
    glm::vec2 oldPos = m_Fireboy->GetPosition();
    glm::vec2 newPos = oldPos;

    m_FireboyVelocity.y -= m_Gravity;
    newPos.y += m_FireboyVelocity.y;

    ResolveVerticalCollisions(oldPos, newPos);
    m_Fireboy->SetPosition(newPos);
}

void App::CheckDiamondCollection() {
    if (!m_Diamond || !m_Diamond->GetVisibility()) {
        return;
    }

    if (CheckAABB(
            m_Fireboy->GetPosition(), m_FireboyHitboxSize,
            m_Diamond->GetPosition(), m_DiamondHitboxSize)) {
        m_Diamond->SetVisible(false);
    }
}

void App::ResolveHorizontalCollisions(const glm::vec2& oldPos, glm::vec2& newPos) {
    for (const auto& block : m_SolidBlocks) {
        if (!CheckAABB(newPos, m_FireboyHitboxSize, block.center, block.size)) {
            continue;
        }

        const float oldRight = oldPos.x + m_FireboyHitboxSize.x * 0.5f;
        const float oldLeft = oldPos.x - m_FireboyHitboxSize.x * 0.5f;
        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;

        if (newPos.x > oldPos.x && oldRight <= blockLeft) {
            newPos.x = blockLeft - m_FireboyHitboxSize.x * 0.5f;
        }

        if (newPos.x < oldPos.x && oldLeft >= blockRight) {
            newPos.x = blockRight + m_FireboyHitboxSize.x * 0.5f;
        }
    }
}

void App::ResolveVerticalCollisions(const glm::vec2& oldPos, glm::vec2& newPos) {
    m_FireboyOnGround = false;

    for (const auto& block : m_SolidBlocks) {
        if (!CheckAABB(newPos, m_FireboyHitboxSize, block.center, block.size)) {
            continue;
        }

        const float oldBottom = oldPos.y - m_FireboyHitboxSize.y * 0.5f;
        const float oldTop = oldPos.y + m_FireboyHitboxSize.y * 0.5f;
        const float blockTop = block.center.y + block.size.y * 0.5f;
        const float blockBottom = block.center.y - block.size.y * 0.5f;

        if (m_FireboyVelocity.y <= 0.0f && oldBottom >= blockTop) {
            newPos.y = blockTop + m_FireboyHitboxSize.y * 0.5f;
            m_FireboyVelocity.y = 0.0f;
            m_FireboyOnGround = true;
        } else if (m_FireboyVelocity.y > 0.0f && oldTop <= blockBottom) {
            newPos.y = blockBottom - m_FireboyHitboxSize.y * 0.5f;
            m_FireboyVelocity.y = 0.0f;
        }
    }
}

bool App::CheckAABB(
    const glm::vec2& centerA, const glm::vec2& sizeA,
    const glm::vec2& centerB, const glm::vec2& sizeB) const {
    const float leftA = centerA.x - sizeA.x * 0.5f;
    const float rightA = centerA.x + sizeA.x * 0.5f;
    const float bottomA = centerA.y - sizeA.y * 0.5f;
    const float topA = centerA.y + sizeA.y * 0.5f;

    const float leftB = centerB.x - sizeB.x * 0.5f;
    const float rightB = centerB.x + sizeB.x * 0.5f;
    const float bottomB = centerB.y - sizeB.y * 0.5f;
    const float topB = centerB.y + sizeB.y * 0.5f;

    return !(rightA < leftB || leftA > rightB || topA < bottomB || bottomA > topB);
}

App::RectObject App::ImageRectToWorldRect(float x1, float y1, float x2, float y2) const {
    const float scaleX = m_BackgroundDisplayedSize.x / m_BackgroundOriginalSize.x;
    const float scaleY = m_BackgroundDisplayedSize.y / m_BackgroundOriginalSize.y;

    const float width = std::abs(x2 - x1) * scaleX;
    const float height = std::abs(y2 - y1) * scaleY;

    const float imageCenterX = (x1 + x2) * 0.5f * scaleX;
    const float imageCenterY = (y1 + y2) * 0.5f * scaleY;

    RectObject rect;
    rect.center.x = imageCenterX - (m_BackgroundDisplayedSize.x * 0.5f);
    rect.center.y = (m_BackgroundDisplayedSize.y * 0.5f) - imageCenterY;
    rect.size = {width, height};
    return rect;
}
