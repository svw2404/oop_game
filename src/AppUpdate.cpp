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
    float inputDir = 0.0f;

    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        inputDir -= 1.0f;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::D)) {
        inputDir += 1.0f;
    }

    if (inputDir < 0.0f) {
        m_Fireboy->SetScale({-1.0f, 1.0f});
    } else if (inputDir > 0.0f) {
        m_Fireboy->SetScale({1.0f, 1.0f});
    }

    const float acceleration = m_FireboyOnGround ? m_GroundAcceleration : m_AirAcceleration;
    const float deceleration = m_FireboyOnGround ? m_GroundDeceleration : m_AirDeceleration;

    if (inputDir != 0.0f) {
        const float targetSpeed = inputDir * m_MoveSpeed;
        if (m_FireboyVelocity.x < targetSpeed) {
            m_FireboyVelocity.x = std::min(targetSpeed, m_FireboyVelocity.x + acceleration);
        } else if (m_FireboyVelocity.x > targetSpeed) {
            m_FireboyVelocity.x = std::max(targetSpeed, m_FireboyVelocity.x - acceleration);
        }
    } else if (m_FireboyVelocity.x > 0.0f) {
        m_FireboyVelocity.x = std::max(0.0f, m_FireboyVelocity.x - deceleration);
    } else if (m_FireboyVelocity.x < 0.0f) {
        m_FireboyVelocity.x = std::min(0.0f, m_FireboyVelocity.x + deceleration);
    }

    if (std::abs(m_FireboyVelocity.x) < 0.01f) {
        m_FireboyVelocity.x = 0.0f;
    }

    newPos.x += m_FireboyVelocity.x;

    // Keep the player glued to slope surfaces while walking so the terrain
    // behaves like a real ramp instead of a staircase of tiny rectangles.
    if (newPos.x != oldPos.x && m_FireboyVelocity.y <= 0.0f) {
        ApplySlopeFollow(oldPos, newPos);
    }

    const float attemptedX = newPos.x;
    ResolveHorizontalCollisions(oldPos, newPos);
    if (std::abs(newPos.x - attemptedX) > 0.001f) {
        m_FireboyVelocity.x = 0.0f;
    }
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

    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetFireboyBodyBox(m_Fireboy->GetPosition(), bodyCenter, bodySize);
    GetFireboyHeadBox(m_Fireboy->GetPosition(), headCenter, headSize);

    if (CheckAABB(bodyCenter, bodySize, m_Diamond->GetPosition(), m_DiamondHitboxSize) ||
        CheckAABB(headCenter, headSize, m_Diamond->GetPosition(), m_DiamondHitboxSize)) {
        m_Diamond->SetVisible(false);
    }
}

void App::ResolveHorizontalCollisions(const glm::vec2& oldPos, glm::vec2& newPos) {
    for (const auto& block : m_SolidBlocks) {
        if (!CheckFireboyCollision(newPos, block)) {
            continue;
        }

        const float oldRight = GetFireboyRightEdge(oldPos);
        const float oldLeft = GetFireboyLeftEdge(oldPos);
        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;
        const float blockTop = block.center.y + block.size.y * 0.5f;

        auto canStepUp = [&](float targetTopY) {
            const glm::vec2 bodyOffset = m_FireboyBodyHitboxOffset;
            const float raisedY = targetTopY - bodyOffset.y + m_FireboyBodyHitboxSize.y * 0.5f + 0.1f;
            const float liftAmount = raisedY - oldPos.y;
            if (liftAmount <= 0.0f || liftAmount > m_StepUpHeight) {
                return false;
            }

            const glm::vec2 candidatePos = {newPos.x, raisedY};
            glm::vec2 candidateBodyCenter = {0.0f, 0.0f};
            glm::vec2 candidateBodySize = {0.0f, 0.0f};
            GetFireboyBodyBox(candidatePos, candidateBodyCenter, candidateBodySize);
            const float candidateBottom = candidateBodyCenter.y - candidateBodySize.y * 0.5f;

            for (const auto& other : m_SolidBlocks) {
                if (&other == &block) {
                    continue;
                }

                if (!CheckFireboyCollision(candidatePos, other)) {
                    continue;
                }

                const float otherTop = other.center.y + other.size.y * 0.5f;
                const bool isSupportContact =
                    candidateBottom >= otherTop - m_GroundSnapTolerance &&
                    candidateBottom <= otherTop + m_SlopeSnapTolerance;

                if (!isSupportContact) {
                    return false;
                }
            }

            newPos.y = raisedY;
            return true;
        };

        if (newPos.x > oldPos.x && oldRight <= blockLeft) {
            if (!canStepUp(blockTop) && !TrySnapToSlopeTopTransition(oldPos, newPos, block)) {
                const float rightExtent = GetFireboyRightEdge({0.0f, 0.0f});
                newPos.x = blockLeft - rightExtent;
            }
        }

        if (newPos.x < oldPos.x && oldLeft >= blockRight) {
            if (!canStepUp(blockTop) && !TrySnapToSlopeTopTransition(oldPos, newPos, block)) {
                const float leftExtent = -GetFireboyLeftEdge({0.0f, 0.0f});
                newPos.x = blockRight + leftExtent;
            }
        }
    }
}

void App::ResolveVerticalCollisions(const glm::vec2& oldPos, glm::vec2& newPos) {
    m_FireboyOnGround = false;

    bool foundGround = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();
    bool hitCeiling = false;

    glm::vec2 oldBodyCenter = {0.0f, 0.0f};
    glm::vec2 oldBodySize = {0.0f, 0.0f};
    glm::vec2 newBodyCenter = {0.0f, 0.0f};
    glm::vec2 newBodySize = {0.0f, 0.0f};
    glm::vec2 oldHeadCenter = {0.0f, 0.0f};
    glm::vec2 oldHeadSize = {0.0f, 0.0f};
    GetFireboyBodyBox(oldPos, oldBodyCenter, oldBodySize);
    GetFireboyBodyBox(newPos, newBodyCenter, newBodySize);
    GetFireboyHeadBox(oldPos, oldHeadCenter, oldHeadSize);

    for (const auto& block : m_SolidBlocks) {
        const bool bodyOverlap = CheckAABB(newBodyCenter, newBodySize, block.center, block.size);
        glm::vec2 newHeadCenter = {0.0f, 0.0f};
        glm::vec2 newHeadSize = {0.0f, 0.0f};
        GetFireboyHeadBox(newPos, newHeadCenter, newHeadSize);
        const bool headOverlap = CheckAABB(newHeadCenter, newHeadSize, block.center, block.size);
        if (!bodyOverlap && !headOverlap) {
            continue;
        }

        const float oldBottom = oldBodyCenter.y - oldBodySize.y * 0.5f;
        const float oldTop = oldHeadCenter.y + oldHeadSize.y * 0.5f;
        const float newBottom = newBodyCenter.y - newBodySize.y * 0.5f;
        const float newTop = newHeadCenter.y + newHeadSize.y * 0.5f;
        const float blockTop = block.center.y + block.size.y * 0.5f;
        const float blockBottom = block.center.y - block.size.y * 0.5f;

        if (bodyOverlap &&
            m_FireboyVelocity.y <= 0.0f &&
            oldBottom >= blockTop - m_GroundSnapTolerance &&
            newBottom <= blockTop + m_GroundSnapTolerance) {
            if (!foundGround || blockTop > bestGroundY) {
                bestGroundY = blockTop;
                foundGround = true;
            }
        } else if (block.blockBottom &&
                   headOverlap &&
                   m_FireboyVelocity.y > 0.0f &&
                   oldTop <= blockBottom + m_GroundSnapTolerance &&
                   newTop >= blockBottom - m_GroundSnapTolerance) {
            const float headTopOffset = m_FireboyHeadHitboxOffset.y + m_FireboyHeadHitboxSize.y * 0.5f;
            newPos.y = blockBottom - headTopOffset;
            m_FireboyVelocity.y = 0.0f;
            hitCeiling = true;
        }
    }

    if (!hitCeiling) {
        hitCeiling = ResolveCeilingSlopeCollision(oldPos, newPos);
    }

    if (!hitCeiling && m_FireboyVelocity.y > 0.0f) {
        float stickyCeilingY = 0.0f;
        if (FindNearbyCeilingY(newPos, m_CeilingStickTolerance, stickyCeilingY)) {
            newPos.y = stickyCeilingY - (m_FireboyHeadHitboxOffset.y + m_FireboyHeadHitboxSize.y * 0.5f);
            m_FireboyVelocity.y = 0.0f;
            hitCeiling = true;
        }
    }

    if (!hitCeiling && ResolveSlopeGrounding(oldPos, newPos)) {
        glm::vec2 groundedBodyCenter = {0.0f, 0.0f};
        glm::vec2 groundedBodySize = {0.0f, 0.0f};
        GetFireboyBodyBox(newPos, groundedBodyCenter, groundedBodySize);
        const float slopeGroundY = groundedBodyCenter.y - groundedBodySize.y * 0.5f;
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
        newPos.y = bestGroundY - m_FireboyBodyHitboxOffset.y + m_FireboyBodyHitboxSize.y * 0.5f;
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

SolidRect App::ImageRectToWorldRect(float x1, float y1, float x2, float y2, bool blockBottom) const {
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
    rect.blockBottom = blockBottom;
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

void App::RecalculateFireboyCollisionBoxes() {
    if (!m_Fireboy) {
        return;
    }

    const glm::vec2 bodySize = m_Fireboy->GetBodySize();
    const glm::vec2 headSize = m_Fireboy->GetHeadSize();
    const glm::vec2 headCenterOffset = m_Fireboy->GetHeadCenterOffset();

    m_FireboyBodyHitboxSize = {
        std::max(1.0f, bodySize.x * m_FireboyBodyHitboxScale.x - m_FireboyBodyHitboxPadding.x),
        std::max(1.0f, bodySize.y * m_FireboyBodyHitboxScale.y - m_FireboyBodyHitboxPadding.y),
    };
    m_FireboyBodyHitboxOffset = {0.0f, -2.0f};

    m_FireboyHeadHitboxSize = {
        std::max(1.0f, headSize.x * m_FireboyHeadHitboxScale.x - m_FireboyHeadHitboxPadding.x),
        std::max(1.0f, headSize.y * m_FireboyHeadHitboxScale.y - m_FireboyHeadHitboxPadding.y),
    };
    m_FireboyHeadHitboxOffset = headCenterOffset;
}

void App::GetFireboyBodyBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const {
    outCenter = bodyPos + m_FireboyBodyHitboxOffset;
    outSize = m_FireboyBodyHitboxSize;
}

void App::GetFireboyHeadBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const {
    outCenter = bodyPos + m_FireboyHeadHitboxOffset;
    outSize = m_FireboyHeadHitboxSize;
}

bool App::CheckFireboyCollision(const glm::vec2& bodyPos, const SolidRect& block) const {
    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetFireboyBodyBox(bodyPos, bodyCenter, bodySize);
    GetFireboyHeadBox(bodyPos, headCenter, headSize);
    return CheckAABB(bodyCenter, bodySize, block.center, block.size) ||
           CheckAABB(headCenter, headSize, block.center, block.size);
}

float App::GetFireboyLeftEdge(const glm::vec2& bodyPos) const {
    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetFireboyBodyBox(bodyPos, bodyCenter, bodySize);
    GetFireboyHeadBox(bodyPos, headCenter, headSize);
    return std::min(
        bodyCenter.x - bodySize.x * 0.5f,
        headCenter.x - headSize.x * 0.5f
    );
}

float App::GetFireboyRightEdge(const glm::vec2& bodyPos) const {
    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetFireboyBodyBox(bodyPos, bodyCenter, bodySize);
    GetFireboyHeadBox(bodyPos, headCenter, headSize);
    return std::max(
        bodyCenter.x + bodySize.x * 0.5f,
        headCenter.x + headSize.x * 0.5f
    );
}

float App::GetFireboyHeadTop(const glm::vec2& bodyPos) const {
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetFireboyHeadBox(bodyPos, headCenter, headSize);
    return headCenter.y + headSize.y * 0.5f;
}

float App::GetFireboyBodyBottom(const glm::vec2& bodyPos) const {
    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    GetFireboyBodyBox(bodyPos, bodyCenter, bodySize);
    return bodyCenter.y - bodySize.y * 0.5f;
}

bool App::ResolveSlopeGrounding(const glm::vec2& oldPos, glm::vec2& newPos) {
    if (m_FireboyVelocity.y > 1.0f) {
        return false;
    }

    glm::vec2 oldBodyCenter = {0.0f, 0.0f};
    glm::vec2 oldBodySize = {0.0f, 0.0f};
    glm::vec2 newBodyCenter = {0.0f, 0.0f};
    glm::vec2 newBodySize = {0.0f, 0.0f};
    GetFireboyBodyBox(oldPos, oldBodyCenter, oldBodySize);
    GetFireboyBodyBox(newPos, newBodyCenter, newBodySize);

    const float oldBottom = oldBodyCenter.y - oldBodySize.y * 0.5f;
    const float newBottom = newBodyCenter.y - newBodySize.y * 0.5f;
    const float footInset = std::min(m_FootProbeInset, newBodySize.x * 0.25f);
    const float leftFootX = newBodyCenter.x - newBodySize.x * 0.5f + footInset;
    const float rightFootX = newBodyCenter.x + newBodySize.x * 0.5f - footInset;

    bool foundSlope = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();

    for (const auto& slope : m_Slopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        for (const float footX : {leftFootX, newBodyCenter.x, rightFootX}) {
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

    newPos.y = bestGroundY - m_FireboyBodyHitboxOffset.y + m_FireboyBodyHitboxSize.y * 0.5f;
    m_FireboyVelocity.y = 0.0f;
    m_FireboyOnGround = true;
    return true;
}

bool App::ResolveCeilingSlopeCollision(const glm::vec2& oldPos, glm::vec2& newPos) {
    if (m_FireboyVelocity.y <= 0.0f) {
        return false;
    }

    glm::vec2 oldHeadCenter = {0.0f, 0.0f};
    glm::vec2 oldHeadSize = {0.0f, 0.0f};
    glm::vec2 newHeadCenter = {0.0f, 0.0f};
    glm::vec2 newHeadSize = {0.0f, 0.0f};
    GetFireboyHeadBox(oldPos, oldHeadCenter, oldHeadSize);
    GetFireboyHeadBox(newPos, newHeadCenter, newHeadSize);

    const float oldTop = oldHeadCenter.y + oldHeadSize.y * 0.5f;
    const float newTop = newHeadCenter.y + newHeadSize.y * 0.5f;
    const float leftX = newHeadCenter.x - newHeadSize.x * 0.5f;
    const float centerX = newHeadCenter.x;
    const float rightX = newHeadCenter.x + newHeadSize.x * 0.5f;

    bool foundCeiling = false;
    float lowestCeilingY = std::numeric_limits<float>::infinity();

    for (const auto& slope : m_CeilingSlopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        for (const float sampleX : {leftX, centerX, rightX}) {
            const float minX = std::min(slope.start.x, slope.end.x) - m_SlopeSnapTolerance;
            const float maxX = std::max(slope.start.x, slope.end.x) + m_SlopeSnapTolerance;
            if (sampleX < minX || sampleX > maxX) {
                continue;
            }

            const float t = (sampleX - slope.start.x) / delta.x;
            if (t < -0.15f || t > 1.15f) {
                continue;
            }

            const float slopeY = slope.start.y + delta.y * t;
            const bool crossesSlope =
                oldTop <= slopeY + m_SlopeSnapTolerance &&
                newTop >= slopeY - m_SlopeSnapTolerance;

            if (!crossesSlope) {
                continue;
            }

            if (!foundCeiling || slopeY < lowestCeilingY) {
                lowestCeilingY = slopeY;
                foundCeiling = true;
            }
        }
    }

    if (!foundCeiling) {
        return false;
    }

    newPos.y = lowestCeilingY - (m_FireboyHeadHitboxOffset.y + m_FireboyHeadHitboxSize.y * 0.5f);
    m_FireboyVelocity.y = 0.0f;
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

    glm::vec2 oldBodyCenter = {0.0f, 0.0f};
    glm::vec2 oldBodySize = {0.0f, 0.0f};
    glm::vec2 newBodyCenter = {0.0f, 0.0f};
    glm::vec2 newBodySize = {0.0f, 0.0f};
    GetFireboyBodyBox(oldPos, oldBodyCenter, oldBodySize);
    GetFireboyBodyBox(newPos, newBodyCenter, newBodySize);

    const float direction = (newPos.x > oldPos.x) ? 1.0f : -1.0f;
    const float blockTop = blockingBlock.center.y + blockingBlock.size.y * 0.5f;
    const float oldBottom = oldBodyCenter.y - oldBodySize.y * 0.5f;
    const float oldFootInset = std::min(m_FootProbeInset, oldBodySize.x * 0.25f);
    const float newFootInset = std::min(m_FootProbeInset, newBodySize.x * 0.25f);
    const float frontFootOldX = oldBodyCenter.x + direction * (oldBodySize.x * 0.5f - oldFootInset);
    const float frontFootNewX = newBodyCenter.x + direction * (newBodySize.x * 0.5f - newFootInset);
    const float centerOldX = oldBodyCenter.x;
    const float centerNewX = newBodyCenter.x;

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
        const bool frontReachesTopZone =
            std::max(frontFootOldX, frontFootNewX) >= transitionMinX &&
            std::min(frontFootOldX, frontFootNewX) <= transitionMaxX;
        const bool bodyReachesTopZone =
            std::max(centerOldX, centerNewX) >= transitionMinX &&
            std::min(centerOldX, centerNewX) <= transitionMaxX;

        if (!frontReachesTopZone && !bodyReachesTopZone) {
            continue;
        }

        const float liftAmount = upper.y - oldBottom;
        if (liftAmount < -m_SlopeSnapTolerance || liftAmount > m_SlopeTopTransitionHeight) {
            continue;
        }

        newPos.y = upper.y - m_FireboyBodyHitboxOffset.y + m_FireboyBodyHitboxSize.y * 0.5f + 0.1f;
        return true;
    }

    return false;
}

bool App::FindBestSlopeYAtX(const glm::vec2& oldPos, float desiredX, float& outSlopeY) const {
    glm::vec2 oldBodyCenter = {0.0f, 0.0f};
    glm::vec2 oldBodySize = {0.0f, 0.0f};
    GetFireboyBodyBox(oldPos, oldBodyCenter, oldBodySize);

    const float centerOffsetX = oldBodyCenter.x - oldPos.x;
    const float desiredBodyCenterX = desiredX + centerOffsetX;
    const float oldBottom = oldBodyCenter.y - oldBodySize.y * 0.5f;
    const float footInset = std::min(m_FootProbeInset, oldBodySize.x * 0.25f);
    const float leftFootX = desiredBodyCenterX - oldBodySize.x * 0.5f + footInset;
    const float rightFootX = desiredBodyCenterX + oldBodySize.x * 0.5f - footInset;

    bool foundSlope = false;
    float bestSlopeY = -std::numeric_limits<float>::infinity();

    for (const auto& slope : m_Slopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        for (const float footX : {leftFootX, desiredBodyCenterX, rightFootX}) {
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

    newPos.y = bestSlopeY - m_FireboyBodyHitboxOffset.y + m_FireboyBodyHitboxSize.y * 0.5f;
}

bool App::FindNearbyGroundY(const glm::vec2& pos, float maxDistance, float& outGroundY) const {
    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    GetFireboyBodyBox(pos, bodyCenter, bodySize);

    const float left = bodyCenter.x - bodySize.x * 0.5f;
    const float right = bodyCenter.x + bodySize.x * 0.5f;
    const float feetY = bodyCenter.y - bodySize.y * 0.5f;

    bool foundGround = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();

    for (const auto& block : m_SolidBlocks) {
        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;
        if (right <= blockLeft - m_FootProbeInset || left >= blockRight + m_FootProbeInset) {
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

bool App::FindNearbyCeilingY(const glm::vec2& pos, float maxDistance, float& outCeilingY) const {
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetFireboyHeadBox(pos, headCenter, headSize);

    const float left = headCenter.x - headSize.x * 0.5f;
    const float centerX = headCenter.x;
    const float right = headCenter.x + headSize.x * 0.5f;
    const float headTop = headCenter.y + headSize.y * 0.5f;

    bool foundCeiling = false;
    float bestCeilingY = std::numeric_limits<float>::infinity();

    for (const auto& block : m_SolidBlocks) {
        if (!block.blockBottom) {
            continue;
        }

        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;
        if (right <= blockLeft - m_FootProbeInset || left >= blockRight + m_FootProbeInset) {
            continue;
        }

        const float blockBottom = block.center.y - block.size.y * 0.5f;
        if (std::abs(blockBottom - headTop) > maxDistance) {
            continue;
        }

        if (!foundCeiling || blockBottom < bestCeilingY) {
            bestCeilingY = blockBottom;
            foundCeiling = true;
        }
    }

    for (const auto& slope : m_CeilingSlopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        for (const float sampleX : {left, centerX, right}) {
            const float minX = std::min(slope.start.x, slope.end.x) - m_SlopeSnapTolerance;
            const float maxX = std::max(slope.start.x, slope.end.x) + m_SlopeSnapTolerance;
            if (sampleX < minX || sampleX > maxX) {
                continue;
            }

            const float t = (sampleX - slope.start.x) / delta.x;
            if (t < -0.15f || t > 1.15f) {
                continue;
            }

            const float slopeY = slope.start.y + delta.y * t;
            if (std::abs(slopeY - headTop) > maxDistance) {
                continue;
            }

            if (!foundCeiling || slopeY < bestCeilingY) {
                bestCeilingY = slopeY;
                foundCeiling = true;
            }
        }
    }

    if (!foundCeiling) {
        return false;
    }

    outCeilingY = bestCeilingY;
    return true;
}
