#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

void App::HandleFireboyInput() {
    glm::vec2 oldPos = m_Fireboy->GetPosition();
    glm::vec2 newPos = oldPos;

    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        newPos.x -= m_MoveSpeed;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        newPos.x += m_MoveSpeed;
    }

    ResolveHorizontalCollisions(oldPos, newPos);
    m_Fireboy->SetPosition(newPos);

    if (Util::Input::IsKeyPressed(Util::Keycode::W) && m_FireboyOnGround) {
        m_FireboyVelocity.y = m_JumpSpeed;
        m_FireboyOnGround = false;
    }
}

App::RectObject App::ImageRectToWorldRect(float x1, float y1, float x2, float y2) const {
    float scaleX = m_BackgroundDisplayedSize.x / m_BackgroundOriginalSize.x;
    float scaleY = m_BackgroundDisplayedSize.y / m_BackgroundOriginalSize.y;

    float left = x1 * scaleX - m_BackgroundDisplayedSize.x * 0.5f;
    float right = x2 * scaleX - m_BackgroundDisplayedSize.x * 0.5f;
    float top = m_BackgroundDisplayedSize.y * 0.5f - y1 * scaleY;
    float bottom = m_BackgroundDisplayedSize.y * 0.5f - y2 * scaleY;

    RectObject rect;
    rect.center = { (left + right) * 0.5f, (top + bottom) * 0.5f };
    rect.size = { right - left, top - bottom };

    return rect;
}

bool App::CheckAABB(
    const glm::vec2& centerA, const glm::vec2& sizeA,
    const glm::vec2& centerB, const glm::vec2& sizeB
) const {
    float leftA = centerA.x - sizeA.x * 0.5f;
    float rightA = centerA.x + sizeA.x * 0.5f;
    float bottomA = centerA.y - sizeA.y * 0.5f;
    float topA = centerA.y + sizeA.y * 0.5f;

    float leftB = centerB.x - sizeB.x * 0.5f;
    float rightB = centerB.x + sizeB.x * 0.5f;
    float bottomB = centerB.y - sizeB.y * 0.5f;
    float topB = centerB.y + sizeB.y * 0.5f;

    return !(rightA < leftB || leftA > rightB || topA < bottomB || bottomA > topB);
}

void App::ResolveHorizontalCollisions(const glm::vec2& oldPos, glm::vec2& newPos) {
    for (const auto& block : m_SolidBlocks) {
        if (!CheckAABB(newPos, m_FireboyHitboxSize, block.center, block.size)) {
            continue;
        }

        float oldRight = oldPos.x + m_FireboyHitboxSize.x * 0.5f;
        float oldLeft = oldPos.x - m_FireboyHitboxSize.x * 0.5f;

        float blockLeft = block.center.x - block.size.x * 0.5f;
        float blockRight = block.center.x + block.size.x * 0.5f;

        // Moving right into a block
        if (newPos.x > oldPos.x && oldRight <= blockLeft) {
            newPos.x = blockLeft - m_FireboyHitboxSize.x * 0.5f;
        }

        // Moving left into a block
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

        float oldBottom = oldPos.y - m_FireboyHitboxSize.y * 0.5f;
        float oldTop = oldPos.y + m_FireboyHitboxSize.y * 0.5f;

        float blockTop = block.center.y + block.size.y * 0.5f;
        float blockBottom = block.center.y - block.size.y * 0.5f;

        // Falling onto a block
        if (m_FireboyVelocity.y <= 0.0f && oldBottom >= blockTop) {
            newPos.y = blockTop + m_FireboyHitboxSize.y * 0.5f;
            m_FireboyVelocity.y = 0.0f;
            m_FireboyOnGround = true;
        }

        // Jumping into underside of a block
        else if (m_FireboyVelocity.y > 0.0f && oldTop <= blockBottom) {
            newPos.y = blockBottom - m_FireboyHitboxSize.y * 0.5f;
            m_FireboyVelocity.y = 0.0f;
        }
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
        m_Diamond->GetPosition(), m_DiamondHitboxSize
    )) {
        m_Diamond->SetVisible(false);
    }
}

void App::Update() {
    HandleFireboyInput();
    UpdateFireboyPhysics();
    CheckDiamondCollection();

    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    m_Root.Update();
}