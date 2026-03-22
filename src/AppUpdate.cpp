#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

#include <limits>
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

    // Keep the player glued to slope surfaces while walking so the terrain
    // behaves like a real ramp instead of a staircase of tiny rectangles.
    if (newPos.x != oldPos.x && m_FireboyVelocity.y <= 0.0f) {
        ApplySlopeFollow(oldPos, newPos);
    }

    ResolveHorizontalCollisions(oldPos, newPos);
    if (newPos.x != oldPos.x && m_FireboyVelocity.y <= 0.0f) {
        ApplySlopeFollow(oldPos, newPos);
    }
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
        const float blockTop = block.center.y + block.size.y * 0.5f;

        auto canStepUp = [&](float targetTopY) {
            const float raisedY = targetTopY + m_FireboyHitboxSize.y * 0.5f + 0.1f;
            const float liftAmount = raisedY - oldPos.y;
            if (liftAmount <= 0.0f || liftAmount > m_StepUpHeight) {
                return false;
            }

            const glm::vec2 candidatePos = {newPos.x, raisedY};
            for (const auto& other : m_SolidBlocks) {
                if (&other == &block) {
                    continue;
                }
                if (CheckAABB(candidatePos, m_FireboyHitboxSize, other.center, other.size)) {
                    return false;
                }
            }

            newPos.y = raisedY;
            return true;
        };

        if (newPos.x > oldPos.x && oldRight <= blockLeft) {
            if (!canStepUp(blockTop) && !TrySnapToSlopeTopTransition(oldPos, newPos, block)) {
                newPos.x = blockLeft - m_FireboyHitboxSize.x * 0.5f;
            }
        }

        if (newPos.x < oldPos.x && oldLeft >= blockRight) {
            if (!canStepUp(blockTop) && !TrySnapToSlopeTopTransition(oldPos, newPos, block)) {
                newPos.x = blockRight + m_FireboyHitboxSize.x * 0.5f;
            }
        }
    }
}

void App::ResolveVerticalCollisions(const glm::vec2& oldPos, glm::vec2& newPos) {
    m_FireboyOnGround = false;

    bool foundGround = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();

    for (const auto& block : m_SolidBlocks) {
        if (!CheckAABB(newPos, m_FireboyHitboxSize, block.center, block.size)) {
            continue;
        }

        const float oldBottom = oldPos.y - m_FireboyHitboxSize.y * 0.5f;
        const float oldTop = oldPos.y + m_FireboyHitboxSize.y * 0.5f;
        const float newBottom = newPos.y - m_FireboyHitboxSize.y * 0.5f;
        const float blockTop = block.center.y + block.size.y * 0.5f;
        const float blockBottom = block.center.y - block.size.y * 0.5f;

        if (m_FireboyVelocity.y <= 0.0f &&
            oldBottom >= blockTop - m_GroundSnapTolerance &&
            newBottom <= blockTop + m_GroundSnapTolerance) {
            if (!foundGround || blockTop > bestGroundY) {
                bestGroundY = blockTop;
                foundGround = true;
            }
        } else if (m_FireboyVelocity.y > 0.0f && oldTop <= blockBottom) {
            newPos.y = blockBottom - m_FireboyHitboxSize.y * 0.5f;
            m_FireboyVelocity.y = 0.0f;
        }
    }

    if (ResolveSlopeGrounding(oldPos, newPos)) {
        const float slopeGroundY = newPos.y - m_FireboyHitboxSize.y * 0.5f;
        if (!foundGround || slopeGroundY > bestGroundY) {
            bestGroundY = slopeGroundY;
            foundGround = true;
        }
    }

    if (!foundGround && m_FireboyVelocity.y <= 0.0f) {
        float stickyGroundY = 0.0f;
        if (FindNearbyGroundY(newPos, m_GroundStickTolerance, stickyGroundY)) {
            bestGroundY = stickyGroundY;
            foundGround = true;
        }
    }

    if (foundGround) {
        newPos.y = bestGroundY + m_FireboyHitboxSize.y * 0.5f;
        m_FireboyVelocity.y = 0.0f;
        m_FireboyOnGround = true;
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

SolidRect App::ImageRectToWorldRect(float x1, float y1, float x2, float y2) const {
    const float scaleX = m_BackgroundDisplayedSize.x / m_BackgroundOriginalSize.x;
    const float scaleY = m_BackgroundDisplayedSize.y / m_BackgroundOriginalSize.y;

    const float width = std::abs(x2 - x1) * scaleX;
    const float height = std::abs(y2 - y1) * scaleY;

    const float imageCenterX = (x1 + x2) * 0.5f * scaleX;
    const float imageCenterY = (y1 + y2) * 0.5f * scaleY;

    SolidRect rect;
    rect.center.x = imageCenterX - (m_BackgroundDisplayedSize.x * 0.5f);
    rect.center.y = (m_BackgroundDisplayedSize.y * 0.5f) - imageCenterY;
    rect.size = {width, height};
    return rect;
}

glm::vec2 App::ImagePointToWorldPoint(float x, float y) const {
    const float scaleX = m_BackgroundDisplayedSize.x / m_BackgroundOriginalSize.x;
    const float scaleY = m_BackgroundDisplayedSize.y / m_BackgroundOriginalSize.y;

    return {
        x * scaleX - (m_BackgroundDisplayedSize.x * 0.5f),
        (m_BackgroundDisplayedSize.y * 0.5f) - y * scaleY,
    };
}

bool App::ResolveSlopeGrounding(const glm::vec2& oldPos, glm::vec2& newPos) {
    if (m_FireboyVelocity.y > 1.0f) {
        return false;
    }

    const float oldBottom = oldPos.y - m_FireboyHitboxSize.y * 0.5f;
    const float newBottom = newPos.y - m_FireboyHitboxSize.y * 0.5f;
    const float leftFootX = newPos.x - m_FireboyHitboxSize.x * 0.35f;
    const float rightFootX = newPos.x + m_FireboyHitboxSize.x * 0.35f;

    bool foundSlope = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();

    for (const auto& slope : m_Slopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        for (const float footX : {leftFootX, rightFootX}) {
            const float minX = std::min(slope.start.x, slope.end.x) - m_SlopeSnapTolerance;
            const float maxX = std::max(slope.start.x, slope.end.x) + m_SlopeSnapTolerance;
            if (footX < minX || footX > maxX) {
                continue;
            }

            const float t = (footX - slope.start.x) / delta.x;
            if (t < -0.15f || t > 1.15f) {
                continue;
            }

            const float slopeY = slope.start.y + delta.y * t;
            const bool crossesSlope =
                oldBottom >= slopeY - m_SlopeSnapTolerance &&
                newBottom <= slopeY + m_SlopeSnapTolerance;

            if (!crossesSlope) {
                continue;
            }

            if (!foundSlope || slopeY > bestGroundY) {
                bestGroundY = slopeY;
                foundSlope = true;
            }
        }
    }

    if (!foundSlope) {
        return false;
    }

    newPos.y = bestGroundY + m_FireboyHitboxSize.y * 0.5f;
    m_FireboyVelocity.y = 0.0f;
    m_FireboyOnGround = true;
    return true;
}

bool App::TrySnapToSlopeTopTransition(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const SolidRect& blockingBlock
) const {
    if (newPos.x == oldPos.x) {
        return false;
    }

    const float direction = (newPos.x > oldPos.x) ? 1.0f : -1.0f;
    const float blockTop = blockingBlock.center.y + blockingBlock.size.y * 0.5f;
    const float oldBottom = oldPos.y - m_FireboyHitboxSize.y * 0.5f;
    const float frontFootOldX = oldPos.x + direction * m_FireboyHitboxSize.x * 0.35f;
    const float frontFootNewX = newPos.x + direction * m_FireboyHitboxSize.x * 0.35f;

    for (const auto& slope : m_Slopes) {
        const glm::vec2 upper = (slope.start.y >= slope.end.y) ? slope.start : slope.end;
        const glm::vec2 lower = (slope.start.y >= slope.end.y) ? slope.end : slope.start;

        const float uphillDirection = (upper.x > lower.x) ? 1.0f : -1.0f;
        if (direction != uphillDirection) {
            continue;
        }

        if (std::abs(blockTop - upper.y) > m_SlopeTopTransitionHeight) {
            continue;
        }

        const float transitionMinX = upper.x - m_SlopeTopTransitionWidth;
        const float transitionMaxX = upper.x + m_SlopeTopTransitionWidth;
        const bool reachesTopZone =
            std::max(frontFootOldX, frontFootNewX) >= transitionMinX &&
            std::min(frontFootOldX, frontFootNewX) <= transitionMaxX;

        if (!reachesTopZone) {
            continue;
        }

        const float liftAmount = upper.y - oldBottom;
        if (liftAmount < -m_SlopeSnapTolerance || liftAmount > m_SlopeTopTransitionHeight) {
            continue;
        }

        newPos.y = upper.y + m_FireboyHitboxSize.y * 0.5f + 0.1f;
        return true;
    }

    return false;
}

bool App::FindBestSlopeYAtX(const glm::vec2& oldPos, float desiredX, float& outSlopeY) const {
    const float oldBottom = oldPos.y - m_FireboyHitboxSize.y * 0.5f;
    const float leftFootX = desiredX - m_FireboyHitboxSize.x * 0.35f;
    const float rightFootX = desiredX + m_FireboyHitboxSize.x * 0.35f;

    bool foundSlope = false;
    float bestSlopeY = -std::numeric_limits<float>::infinity();

    for (const auto& slope : m_Slopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        for (const float footX : {leftFootX, rightFootX}) {
            const float minX = std::min(slope.start.x, slope.end.x) - m_SlopeTopTransitionWidth;
            const float maxX = std::max(slope.start.x, slope.end.x) + m_SlopeTopTransitionWidth;
            if (footX < minX || footX > maxX) {
                continue;
            }

            const float t = (footX - slope.start.x) / delta.x;
            if (t < -0.15f || t > 1.15f) {
                continue;
            }

            const float slopeY = slope.start.y + delta.y * t;
            const float deltaFromFeet = slopeY - oldBottom;
            if (deltaFromFeet < -m_SlopeFollowTolerance ||
                deltaFromFeet > m_SlopeTopTransitionHeight) {
                continue;
            }

            if (!foundSlope || slopeY > bestSlopeY) {
                bestSlopeY = slopeY;
                foundSlope = true;
            }
        }
    }

    if (!foundSlope) {
        return false;
    }

    outSlopeY = bestSlopeY;
    return true;
}

void App::ApplySlopeFollow(const glm::vec2& oldPos, glm::vec2& newPos) const {
    float bestSlopeY = 0.0f;
    if (!FindBestSlopeYAtX(oldPos, newPos.x, bestSlopeY)) {
        return;
    }

    newPos.y = bestSlopeY + m_FireboyHitboxSize.y * 0.5f;
}

bool App::FindNearbyGroundY(const glm::vec2& pos, float maxDistance, float& outGroundY) const {
    const float left = pos.x - m_FireboyHitboxSize.x * 0.5f;
    const float right = pos.x + m_FireboyHitboxSize.x * 0.5f;
    const float feetY = pos.y - m_FireboyHitboxSize.y * 0.5f;

    bool foundGround = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();

    for (const auto& block : m_SolidBlocks) {
        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;
        if (right <= blockLeft || left >= blockRight) {
            continue;
        }

        const float blockTop = block.center.y + block.size.y * 0.5f;
        if (std::abs(blockTop - feetY) > maxDistance) {
            continue;
        }

        if (!foundGround || blockTop > bestGroundY) {
            bestGroundY = blockTop;
            foundGround = true;
        }
    }

    float slopeGroundY = 0.0f;
    if (FindBestSlopeYAtX(pos, pos.x, slopeGroundY) &&
        std::abs(slopeGroundY - feetY) <= maxDistance &&
        (!foundGround || slopeGroundY > bestGroundY)) {
        bestGroundY = slopeGroundY;
        foundGround = true;
    }

    if (!foundGround) {
        return false;
    }

    outGroundY = bestGroundY;
    return true;
}
