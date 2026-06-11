#include "App.hpp"

#include "Util/Logger.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
constexpr float DESCENDING_PLATFORM_HEAD_CLEARANCE_LOCAL = 2.0f;

float MoveToward(float current, float target, float maxDelta) {
    if (current < target) {
        return std::min(target, current + maxDelta);
    }
    if (current > target) {
        return std::max(target, current - maxDelta);
    }
    return current;
}
} // namespace

glm::vec2 App::ImagePointToWorldPoint(float x, float y) const {
    const float scaleX = m_BackgroundDisplayedSize.x / m_BackgroundOriginalSize.x;
    const float scaleY = m_BackgroundDisplayedSize.y / m_BackgroundOriginalSize.y;

    return {
        (x - m_BackgroundOriginalSize.x * 0.5f) * scaleX,
        (m_BackgroundOriginalSize.y * 0.5f - y) * scaleY,
    };
}

SolidRect App::ImageRectToWorldRect(
    float x1,
    float y1,
    float x2,
    float y2,
    bool blockBottom
) const {
    const glm::vec2 p1 = ImagePointToWorldPoint(x1, y1);
    const glm::vec2 p2 = ImagePointToWorldPoint(x2, y2);

    SolidRect rect;
    rect.center = {(p1.x + p2.x) * 0.5f, (p1.y + p2.y) * 0.5f};
    rect.size = {std::abs(p2.x - p1.x), std::abs(p2.y - p1.y)};
    rect.blockBottom = blockBottom;
    return rect;
}

bool App::CheckAABB(
    const glm::vec2& centerA,
    const glm::vec2& sizeA,
    const glm::vec2& centerB,
    const glm::vec2& sizeB
) const {
    return std::abs(centerA.x - centerB.x) <= (sizeA.x + sizeB.x) * 0.5f &&
           std::abs(centerA.y - centerB.y) <= (sizeA.y + sizeB.y) * 0.5f;
}

void App::GetCharacterBodyBox(
    const glm::vec2& bodyPos,
    const CharacterCollisionProfile& profile,
    glm::vec2& outCenter,
    glm::vec2& outSize
) const {
    outCenter = bodyPos + profile.bodyHitboxOffset;
    outSize = {
        std::max(1.0f, profile.bodyHitboxSize.x * profile.bodyHitboxScale.x + profile.bodyHitboxPadding.x),
        std::max(1.0f, profile.bodyHitboxSize.y * profile.bodyHitboxScale.y + profile.bodyHitboxPadding.y),
    };
}

void App::GetCharacterHeadBox(
    const glm::vec2& bodyPos,
    const CharacterCollisionProfile& profile,
    glm::vec2& outCenter,
    glm::vec2& outSize
) const {
    outCenter = bodyPos + profile.headHitboxOffset;
    outSize = {
        std::max(1.0f, profile.headHitboxSize.x * profile.headHitboxScale.x + profile.headHitboxPadding.x),
        std::max(1.0f, profile.headHitboxSize.y * profile.headHitboxScale.y + profile.headHitboxPadding.y),
    };
}

bool App::CheckCharacterCollision(
    const glm::vec2& bodyPos,
    const SolidRect& block,
    const CharacterCollisionProfile& profile
) const {
    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    glm::vec2 headCenter;
    glm::vec2 headSize;
    GetCharacterBodyBox(bodyPos, profile, bodyCenter, bodySize);
    GetCharacterHeadBox(bodyPos, profile, headCenter, headSize);

    return CheckAABB(bodyCenter, bodySize, block.center, block.size) ||
           CheckAABB(headCenter, headSize, block.center, block.size);
}

float App::GetCharacterLeftEdge(const glm::vec2& bodyPos, const CharacterCollisionProfile& profile) const {
    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    glm::vec2 headCenter;
    glm::vec2 headSize;
    GetCharacterBodyBox(bodyPos, profile, bodyCenter, bodySize);
    GetCharacterHeadBox(bodyPos, profile, headCenter, headSize);
    return std::min(
        bodyCenter.x - bodySize.x * 0.5f,
        headCenter.x - headSize.x * 0.5f
    );
}

float App::GetCharacterRightEdge(const glm::vec2& bodyPos, const CharacterCollisionProfile& profile) const {
    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    glm::vec2 headCenter;
    glm::vec2 headSize;
    GetCharacterBodyBox(bodyPos, profile, bodyCenter, bodySize);
    GetCharacterHeadBox(bodyPos, profile, headCenter, headSize);
    return std::max(
        bodyCenter.x + bodySize.x * 0.5f,
        headCenter.x + headSize.x * 0.5f
    );
}

float App::GetCharacterBodyBottom(const glm::vec2& bodyPos, const CharacterCollisionProfile& profile) const {
    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(bodyPos, profile, bodyCenter, bodySize);
    return bodyCenter.y - bodySize.y * 0.5f;
}

void App::GetFireboyBodyBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const {
    GetCharacterBodyBox(bodyPos, m_FireboyCollision, outCenter, outSize);
}

void App::GetFireboyHeadBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const {
    GetCharacterHeadBox(bodyPos, m_FireboyCollision, outCenter, outSize);
}

bool App::CheckFireboyCollision(const glm::vec2& bodyPos, const SolidRect& block) const {
    return CheckCharacterCollision(bodyPos, block, m_FireboyCollision);
}

float App::GetFireboyLeftEdge(const glm::vec2& bodyPos) const {
    return GetCharacterLeftEdge(bodyPos, m_FireboyCollision);
}

float App::GetFireboyRightEdge(const glm::vec2& bodyPos) const {
    return GetCharacterRightEdge(bodyPos, m_FireboyCollision);
}

float App::GetFireboyHeadTop(const glm::vec2& bodyPos) const {
    glm::vec2 headCenter;
    glm::vec2 headSize;
    GetFireboyHeadBox(bodyPos, headCenter, headSize);
    return headCenter.y + headSize.y * 0.5f;
}

float App::GetFireboyBodyBottom(const glm::vec2& bodyPos) const {
    return GetCharacterBodyBottom(bodyPos, m_FireboyCollision);
}

void App::GetWatergirlBodyBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const {
    GetCharacterBodyBox(bodyPos, m_WatergirlCollision, outCenter, outSize);
}

void App::GetWatergirlHeadBox(const glm::vec2& bodyPos, glm::vec2& outCenter, glm::vec2& outSize) const {
    GetCharacterHeadBox(bodyPos, m_WatergirlCollision, outCenter, outSize);
}

bool App::CheckWatergirlCollision(const glm::vec2& bodyPos, const SolidRect& block) const {
    return CheckCharacterCollision(bodyPos, block, m_WatergirlCollision);
}

float App::GetWatergirlLeftEdge(const glm::vec2& bodyPos) const {
    return GetCharacterLeftEdge(bodyPos, m_WatergirlCollision);
}

float App::GetWatergirlRightEdge(const glm::vec2& bodyPos) const {
    return GetCharacterRightEdge(bodyPos, m_WatergirlCollision);
}

float App::GetWatergirlBodyBottom(const glm::vec2& bodyPos) const {
    return GetCharacterBodyBottom(bodyPos, m_WatergirlCollision);
}

void App::RecalculateCharacterCollisionBoxes(
    const std::shared_ptr<HeadBodyCharacter>& character,
    CharacterCollisionProfile& profile
) {
    if (!character) {
        return;
    }

    const glm::vec2 bodySize = character->GetBodySize();
    const glm::vec2 headSize = character->GetHeadSize();
    if (bodySize.x > 0.0f && bodySize.y > 0.0f) {
        profile.bodyHitboxSize = bodySize;
    }
    if (headSize.x > 0.0f && headSize.y > 0.0f) {
        profile.headHitboxSize = headSize;
    }
    profile.headHitboxOffset = character->GetHeadCenterOffset();
}

void App::RecalculateFireboyCollisionBoxes() {
    RecalculateCharacterCollisionBoxes(m_Fireboy, m_FireboyCollision);
}

void App::RecalculateWatergirlCollisionBoxes() {
    RecalculateCharacterCollisionBoxes(m_Watergirl, m_WatergirlCollision);
}

bool App::WouldCharacterHitTerrainAt(
    const glm::vec2& bodyPos,
    const CharacterCollisionProfile& profile,
    bool allowSupportContacts,
    std::size_t ignoredSolidBlockIndex
) const {
    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(bodyPos, profile, bodyCenter, bodySize);
    const float bodyBottom = bodyCenter.y - bodySize.y * 0.5f;

    for (std::size_t i = 0; i < m_SolidBlocks.size(); ++i) {
        if (i == ignoredSolidBlockIndex) {
            continue;
        }

        const SolidRect& block = m_SolidBlocks[i];
        if (block.isSlopeGuard) {
            continue;
        }
        if (!CheckCharacterCollision(bodyPos, block, profile)) {
            continue;
        }

        const float blockTop = block.center.y + block.size.y * 0.5f;
        const bool supportOnly =
            allowSupportContacts &&
            bodyBottom >= blockTop - m_GroundSnapTolerance &&
            bodyBottom <= blockTop + m_GroundSnapTolerance;
        if (!supportOnly) {
            return true;
        }
    }

    return false;
}

void App::ResolveHorizontalCollisions(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const CharacterCollisionProfile& profile,
    bool onGround
) {
    const float oldLeft = GetCharacterLeftEdge(oldPos, profile);
    const float oldRight = GetCharacterRightEdge(oldPos, profile);
    const float oldBottom = GetCharacterBodyBottom(oldPos, profile);
    glm::vec2 oldBodyCenter;
    glm::vec2 oldBodySize;
    GetCharacterBodyBox(oldPos, profile, oldBodyCenter, oldBodySize);
    const float bodyBottomOffset = oldPos.y - oldBottom;
    float currentGroundY = 0.0f;
    const bool nearGroundForStepAssist =
        onGround &&
        FindNearbyGroundY(oldPos, m_GroundStickTolerance + m_GroundSnapTolerance, currentGroundY, profile) &&
        oldBottom >= currentGroundY - m_GroundSnapTolerance &&
        oldBottom <= currentGroundY + m_GroundStickTolerance;
    auto followsFloorSlope = [&]() {
        float slopeY = 0.0f;
        if (!FindBestSlopeYAtX(oldPos, newPos.x, slopeY, profile)) {
            return false;
        }

        glm::vec2 newBodyCenter;
        glm::vec2 newBodySize;
        GetCharacterBodyBox(newPos, profile, newBodyCenter, newBodySize);
        const float newBottom = newBodyCenter.y - newBodySize.y * 0.5f;
        return std::abs(newBottom - slopeY) <= m_GroundSnapTolerance + 0.5f;
    };

    for (const auto& block : m_SolidBlocks) {
        if (block.isSlopeGuard) {
            continue;
        }
        if (!CheckCharacterCollision(newPos, block, profile)) {
            continue;
        }
        if (onGround &&
            TryStepFromSlopeHighEnd(oldPos, newPos, block, profile)) {
            continue;
        }
        if (block.isSlopeFill && nearGroundForStepAssist && followsFloorSlope()) {
            continue;
        }

        if (nearGroundForStepAssist && TrySnapToSlopeTopTransition(oldPos, newPos, block, profile)) {
            continue;
        }

        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;

        const float blockTop = block.center.y + block.size.y * 0.5f;
        auto canStepUp = [&](float targetTopY) {
            const float raisedY = targetTopY + bodyBottomOffset + 0.1f;
            const float liftAmount = raisedY - oldPos.y;
            if (liftAmount <= 0.0f || liftAmount > m_StepUpHeight) {
                return false;
            }

            const glm::vec2 candidatePos = {newPos.x, raisedY};
            glm::vec2 candidateBodyCenter;
            glm::vec2 candidateBodySize;
            glm::vec2 candidateHeadCenter;
            glm::vec2 candidateHeadSize;
            GetCharacterBodyBox(candidatePos, profile, candidateBodyCenter, candidateBodySize);
            GetCharacterHeadBox(candidatePos, profile, candidateHeadCenter, candidateHeadSize);
            const float candidateBottom = candidateBodyCenter.y - candidateBodySize.y * 0.5f;

            for (const auto& other : m_SolidBlocks) {
                if (&other == &block) {
                    continue;
                }
                if (other.isSlopeGuard) {
                    continue;
                }

                const bool bodyOverlap = CheckAABB(
                    candidateBodyCenter,
                    candidateBodySize,
                    other.center,
                    other.size
                );
                const bool headOverlap = CheckAABB(
                    candidateHeadCenter,
                    candidateHeadSize,
                    other.center,
                    other.size
                );
                if (!bodyOverlap && !headOverlap) {
                    continue;
                }

                const float otherTop = other.center.y + other.size.y * 0.5f;
                const bool embeddedSupportMass =
                    !other.blockBottom &&
                    candidateBottom > otherTop + m_GroundSnapTolerance;
                if (embeddedSupportMass) {
                    continue;
                }
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

        auto canStepToNearbyGround = [&]() {
            float targetGroundY = 0.0f;
            const glm::vec2 probePos = {newPos.x, oldPos.y};
            if (!FindNearbyGroundY(
                probePos,
                m_StepUpHeight + m_GroundSnapTolerance,
                targetGroundY,
                profile
            )) {
                return false;
            }

            const float liftAmount = targetGroundY - oldBottom;
            if (liftAmount < -m_GroundSnapTolerance ||
                liftAmount > m_StepUpHeight + m_GroundSnapTolerance) {
                return false;
            }

            const glm::vec2 candidatePos = {
                newPos.x,
                targetGroundY + bodyBottomOffset + 0.1f,
            };
            glm::vec2 candidateBodyCenter;
            glm::vec2 candidateBodySize;
            glm::vec2 candidateHeadCenter;
            glm::vec2 candidateHeadSize;
            GetCharacterBodyBox(candidatePos, profile, candidateBodyCenter, candidateBodySize);
            GetCharacterHeadBox(candidatePos, profile, candidateHeadCenter, candidateHeadSize);
            const float candidateBottom = candidateBodyCenter.y - candidateBodySize.y * 0.5f;

            for (const auto& other : m_SolidBlocks) {
                if (other.isSlopeGuard) {
                    continue;
                }
                const bool bodyOverlap = CheckAABB(
                    candidateBodyCenter,
                    candidateBodySize,
                    other.center,
                    other.size
                );
                const bool headOverlap = CheckAABB(
                    candidateHeadCenter,
                    candidateHeadSize,
                    other.center,
                    other.size
                );
                if (!bodyOverlap && !headOverlap) {
                    continue;
                }

                const float otherTop = other.center.y + other.size.y * 0.5f;
                const bool embeddedSupportMass =
                    !other.blockBottom &&
                    candidateBottom > otherTop + m_GroundSnapTolerance;
                if (embeddedSupportMass) {
                    continue;
                }
                const bool isSupportContact =
                    candidateBottom >= otherTop - m_GroundSnapTolerance &&
                    candidateBottom <= otherTop + m_SlopeSnapTolerance;
                if (!isSupportContact) {
                    return false;
                }
            }

            newPos.y = candidatePos.y;
            return true;
        };

        if (newPos.x > oldPos.x && oldRight <= blockLeft + m_GroundSnapTolerance) {
            if (!nearGroundForStepAssist || (!canStepUp(blockTop) && !canStepToNearbyGround())) {
                const float rightOffset = oldPos.x - oldRight;
                newPos.x = blockLeft + rightOffset;
            }
        }
        else if (newPos.x < oldPos.x && oldLeft >= blockRight - m_GroundSnapTolerance) {
            if (!nearGroundForStepAssist || (!canStepUp(blockTop) && !canStepToNearbyGround())) {
                const float leftOffset = oldPos.x - oldLeft;
                newPos.x = blockRight + leftOffset;
            }
        }
    }
}

bool App::TryStepFromSlopeHighEnd(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const SolidRect& blockingBlock,
    const CharacterCollisionProfile& profile
) const {
    if (blockingBlock.isSlopeGuard || blockingBlock.isSlopeFill) {
        return false;
    }

    const float requestedDeltaX = newPos.x - oldPos.x;
    if (std::abs(requestedDeltaX) < 0.001f) {
        return false;
    }

    glm::vec2 oldBodyCenter;
    glm::vec2 oldBodySize;
    GetCharacterBodyBox(oldPos, profile, oldBodyCenter, oldBodySize);
    const float oldBottom = oldBodyCenter.y - oldBodySize.y * 0.5f;
    const float bodyBottomOffset = oldPos.y - oldBottom;

    const bool movingRight = requestedDeltaX > 0.0f;
    const float oldLeadingEdge = movingRight
        ? GetCharacterRightEdge(oldPos, profile)
        : GetCharacterLeftEdge(oldPos, profile);
    const float blockSide = movingRight
        ? blockingBlock.center.x - blockingBlock.size.x * 0.5f
        : blockingBlock.center.x + blockingBlock.size.x * 0.5f;
    const float blockTop =
        blockingBlock.center.y + blockingBlock.size.y * 0.5f;

    if (std::abs(oldLeadingEdge - blockSide) >
        m_SlopeTopTransitionWidth + m_GroundSnapTolerance) {
        return false;
    }

    for (const auto& slope : m_Slopes) {
        const glm::vec2 highPoint =
            slope.start.y >= slope.end.y ? slope.start : slope.end;
        const glm::vec2 lowPoint =
            slope.start.y >= slope.end.y ? slope.end : slope.start;
        const float uphillDirection = highPoint.x > lowPoint.x ? 1.0f : -1.0f;

        if (requestedDeltaX * uphillDirection <= 0.0f) {
            continue;
        }
        if (std::abs(highPoint.x - blockSide) > m_SlopeTopTransitionWidth) {
            continue;
        }
        if (std::abs(oldLeadingEdge - highPoint.x) >
            m_SlopeTopTransitionWidth + m_GroundSnapTolerance) {
            continue;
        }

        const float distanceBelowHighPoint = highPoint.y - oldBottom;
        if (distanceBelowHighPoint < -m_GroundSnapTolerance ||
            distanceBelowHighPoint >
                m_SlopeTopTransitionHeight + m_GroundSnapTolerance) {
            continue;
        }

        const float blockContinuationHeight = blockTop - highPoint.y;
        if (blockContinuationHeight < -m_GroundSnapTolerance ||
            blockContinuationHeight >
                m_StepUpHeight + m_GroundSnapTolerance) {
            continue;
        }

        const float liftAmount = blockTop - oldBottom;
        if (liftAmount < -m_GroundSnapTolerance ||
            liftAmount > m_StepUpHeight + m_GroundSnapTolerance) {
            continue;
        }

        const glm::vec2 candidatePos = {
            newPos.x,
            blockTop + bodyBottomOffset + 0.1f,
        };
        glm::vec2 candidateBodyCenter;
        glm::vec2 candidateBodySize;
        glm::vec2 candidateHeadCenter;
        glm::vec2 candidateHeadSize;
        GetCharacterBodyBox(
            candidatePos,
            profile,
            candidateBodyCenter,
            candidateBodySize
        );
        GetCharacterHeadBox(
            candidatePos,
            profile,
            candidateHeadCenter,
            candidateHeadSize
        );
        const float candidateBottom =
            candidateBodyCenter.y - candidateBodySize.y * 0.5f;

        bool blocked = false;
        for (const auto& other : m_SolidBlocks) {
            if (&other == &blockingBlock || other.isSlopeGuard) {
                continue;
            }

            const bool bodyOverlap = CheckAABB(
                candidateBodyCenter,
                candidateBodySize,
                other.center,
                other.size
            );
            const bool headOverlap = CheckAABB(
                candidateHeadCenter,
                candidateHeadSize,
                other.center,
                other.size
            );
            if (!bodyOverlap && !headOverlap) {
                continue;
            }

            const float otherTop = other.center.y + other.size.y * 0.5f;
            const bool embeddedSupportMass =
                (!other.blockBottom || other.isSlopeFill) &&
                candidateBottom > otherTop + m_GroundSnapTolerance;
            const bool supportContact =
                candidateBottom >= otherTop - m_GroundSnapTolerance &&
                candidateBottom <= otherTop + m_SlopeSnapTolerance;
            if (!embeddedSupportMass && !supportContact) {
                blocked = true;
                break;
            }
        }

        if (!blocked) {
            newPos.y = candidatePos.y;
            return true;
        }
    }

    return false;
}

bool App::RunSlopeTopTransitionRegressionAudit() const {
    int candidateCount = 0;
    int failureCount = 0;

    for (const auto& slope : m_Slopes) {
        const glm::vec2 highPoint =
            slope.start.y >= slope.end.y ? slope.start : slope.end;
        const glm::vec2 lowPoint =
            slope.start.y >= slope.end.y ? slope.end : slope.start;
        const float uphillDirection = highPoint.x > lowPoint.x ? 1.0f : -1.0f;

        for (const auto& block : m_SolidBlocks) {
            if (block.isSlopeGuard || block.isSlopeFill) {
                continue;
            }

            const float blockSide = uphillDirection > 0.0f
                ? block.center.x - block.size.x * 0.5f
                : block.center.x + block.size.x * 0.5f;
            const float blockTop =
                block.center.y + block.size.y * 0.5f;
            const float continuationHeight = blockTop - highPoint.y;
            if (std::abs(blockSide - highPoint.x) > m_SlopeTopTransitionWidth ||
                continuationHeight < -m_GroundSnapTolerance ||
                continuationHeight >
                    m_StepUpHeight + m_GroundSnapTolerance) {
                continue;
            }

            for (const CharacterCollisionProfile* profile : {
                     &m_FireboyCollision,
                     &m_WatergirlCollision,
                 }) {
                glm::vec2 originBodyCenter;
                glm::vec2 originBodySize;
                GetCharacterBodyBox(
                    {0.0f, 0.0f},
                    *profile,
                    originBodyCenter,
                    originBodySize
                );
                const float originBottom =
                    originBodyCenter.y - originBodySize.y * 0.5f;
                const float bottomOffset = -originBottom;
                const float edgeOffset = uphillDirection > 0.0f
                    ? GetCharacterRightEdge({0.0f, 0.0f}, *profile)
                    : GetCharacterLeftEdge({0.0f, 0.0f}, *profile);

                glm::vec2 oldPos = {
                    blockSide - edgeOffset,
                    highPoint.y + bottomOffset,
                };
                glm::vec2 newPos = oldPos + glm::vec2{
                    uphillDirection * 0.2f,
                    0.0f,
                };
                const float requestedX = newPos.x;

                const_cast<App*>(this)->ResolveHorizontalCollisions(
                    oldPos,
                    newPos,
                    *profile,
                    true
                );
                ++candidateCount;

                const float resolvedBottom =
                    GetCharacterBodyBottom(newPos, *profile);
                const bool keptProgress =
                    std::abs(newPos.x - requestedX) <= 0.001f;
                const bool reachedTop =
                    std::abs(resolvedBottom - blockTop) <=
                    m_GroundSnapTolerance;
                if (!keptProgress || !reachedTop) {
                    ++failureCount;
                    LOG_ERROR(
                        "[SLOPE AUDIT] failed high=({}, {}) side={} top={} resolved=({}, {})",
                        highPoint.x,
                        highPoint.y,
                        blockSide,
                        blockTop,
                        newPos.x,
                        resolvedBottom
                    );
                }
            }
        }
    }

    LOG_INFO(
        "[SLOPE AUDIT] level {} candidates={} failures={}",
        m_ActiveLevelIndex,
        candidateCount,
        failureCount
    );
    return candidateCount > 0 && failureCount == 0;
}

void App::ResolveVerticalCollisions(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const CharacterCollisionProfile& profile,
    glm::vec2& velocity,
    bool& onGround,
    bool& hitCeiling
) {
    onGround = false;
    hitCeiling = false;

    bool foundGround = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();
    glm::vec2 oldBodyCenter;
    glm::vec2 oldBodySize;
    glm::vec2 oldHeadCenter;
    glm::vec2 oldHeadSize;
    glm::vec2 newBodyCenter;
    glm::vec2 newBodySize;
    GetCharacterBodyBox(oldPos, profile, oldBodyCenter, oldBodySize);
    GetCharacterHeadBox(oldPos, profile, oldHeadCenter, oldHeadSize);
    GetCharacterBodyBox(newPos, profile, newBodyCenter, newBodySize);

    for (const auto& block : m_SolidBlocks) {
        if (!CheckCharacterCollision(newPos, block, profile)) {
            continue;
        }

        const float oldBottom = oldBodyCenter.y - oldBodySize.y * 0.5f;
        const float newBottom = newBodyCenter.y - newBodySize.y * 0.5f;
        const float oldTop = oldHeadCenter.y + oldHeadSize.y * 0.5f;
        const float blockTop = block.center.y + block.size.y * 0.5f;
        const float blockBottom = block.center.y - block.size.y * 0.5f;

        if (velocity.y <= 0.0f &&
            oldBottom >= blockTop - m_GroundSnapTolerance &&
            newBottom <= blockTop + m_GroundSnapTolerance) {
            bestGroundY = std::max(bestGroundY, blockTop);
            foundGround = true;
        }
        else if (block.blockBottom && velocity.y > 0.0f && oldTop <= blockBottom + m_GroundSnapTolerance) {
            const float topOffset = oldPos.y - oldTop;
            newPos.y = blockBottom + topOffset;
            velocity.y = 0.0f;
            hitCeiling = true;
        }
    }

    if (!hitCeiling) {
        ResolveCeilingSlopeCollision(oldPos, newPos, profile, velocity);
    }

    if (ResolveSlopeGrounding(oldPos, newPos, profile, velocity, onGround)) {
        return;
    }

    if (!foundGround && velocity.y <= 0.0f) {
        float stickyGroundY = 0.0f;
        if (FindNearbyGroundY(newPos, m_GroundStickTolerance, stickyGroundY, profile)) {
            bestGroundY = stickyGroundY;
            foundGround = true;
        }
    }

    if (foundGround) {
        glm::vec2 bodyCenter;
        glm::vec2 bodySize;
        GetCharacterBodyBox(newPos, profile, bodyCenter, bodySize);
        const float bottomOffset = newPos.y - (bodyCenter.y - bodySize.y * 0.5f);
        newPos.y = bestGroundY + bottomOffset;
        velocity.y = 0.0f;
        onGround = true;
    }
}

bool App::ResolveSlopeGrounding(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const CharacterCollisionProfile& profile,
    glm::vec2& velocity,
    bool& onGround
) {
    if (velocity.y > 1.0f) {
        return false;
    }

    glm::vec2 oldBodyCenter;
    glm::vec2 oldBodySize;
    glm::vec2 newBodyCenter;
    glm::vec2 newBodySize;
    GetCharacterBodyBox(oldPos, profile, oldBodyCenter, oldBodySize);
    GetCharacterBodyBox(newPos, profile, newBodyCenter, newBodySize);

    const float oldBottom = oldBodyCenter.y - oldBodySize.y * 0.5f;
    const float newBottom = newBodyCenter.y - newBodySize.y * 0.5f;
    const float footInset = std::min(m_FootProbeInset, newBodySize.x * 0.25f);
    const float leftFootX = newBodyCenter.x - newBodySize.x * 0.5f + footInset;
    const float rightFootX = newBodyCenter.x + newBodySize.x * 0.5f - footInset;

    bool foundExactSlope = false;
    float bestExactGroundY = -std::numeric_limits<float>::infinity();
    bool foundTransitionSlope = false;
    float bestTransitionGroundY = -std::numeric_limits<float>::infinity();

    for (const auto& slope : m_Slopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        for (const float footX : {leftFootX, newBodyCenter.x, rightFootX}) {
            const float actualMinX = std::min(slope.start.x, slope.end.x);
            const float actualMaxX = std::max(slope.start.x, slope.end.x);
            const float transitionMinX = actualMinX - m_SlopeSnapTolerance;
            const float transitionMaxX = actualMaxX + m_SlopeSnapTolerance;
            if (footX < transitionMinX || footX > transitionMaxX) {
                continue;
            }

            const float t = (footX - slope.start.x) / delta.x;
            if (t < -0.15f || t > 1.15f) {
                continue;
            }

            const bool withinActualSpan =
                footX >= actualMinX && footX <= actualMaxX && t >= 0.0f && t <= 1.0f;
            const float sampledT = withinActualSpan ? t : std::clamp(t, 0.0f, 1.0f);
            const float slopeY = slope.start.y + delta.y * sampledT;
            const bool crossesSlope =
                oldBottom >= slopeY - m_SlopeSnapTolerance &&
                newBottom <= slopeY + m_SlopeSnapTolerance;
            if (!crossesSlope) {
                continue;
            }

            if (withinActualSpan) {
                if (!foundExactSlope || slopeY > bestExactGroundY) {
                    bestExactGroundY = slopeY;
                    foundExactSlope = true;
                }
            }
            else if (!foundTransitionSlope || slopeY > bestTransitionGroundY) {
                bestTransitionGroundY = slopeY;
                foundTransitionSlope = true;
            }
        }
    }

    if (foundExactSlope) {
        const float bottomOffset = newPos.y - newBottom;
        newPos.y = bestExactGroundY + bottomOffset;
        velocity.y = 0.0f;
        onGround = true;
        return true;
    }

    if (!foundTransitionSlope) {
        return false;
    }

    const float bottomOffset = newPos.y - newBottom;
    newPos.y = bestTransitionGroundY + bottomOffset;
    velocity.y = 0.0f;
    onGround = true;
    return true;
}

bool App::ResolveCeilingSlopeCollision(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const CharacterCollisionProfile& profile,
    glm::vec2& velocity
) {
    if (velocity.y <= 0.0f) {
        return false;
    }

    glm::vec2 oldHeadCenter;
    glm::vec2 oldHeadSize;
    glm::vec2 headCenter;
    glm::vec2 headSize;
    GetCharacterHeadBox(oldPos, profile, oldHeadCenter, oldHeadSize);
    GetCharacterHeadBox(newPos, profile, headCenter, headSize);
    const float oldHeadTop = oldHeadCenter.y + oldHeadSize.y * 0.5f;
    const float headTop = headCenter.y + headSize.y * 0.5f;
    const float headOffset = headTop - newPos.y;
    const float undersideTolerance = std::min(m_SlopeSnapTolerance, m_GroundSnapTolerance + 1.0f);

    auto crossesIndexedSlopesFromBelow = [&](const auto& slopes, float tolerance, auto&& shouldSkip) {
        for (std::size_t i = 0; i < slopes.size(); ++i) {
            if (shouldSkip(i)) {
                continue;
            }

            const auto& slope = slopes[i];
            const glm::vec2 delta = slope.end - slope.start;
            if (std::abs(delta.x) < 0.001f) {
                continue;
            }

            const float t = (headCenter.x - slope.start.x) / delta.x;
            if (t < -0.10f || t > 1.10f) {
                continue;
            }

            const float surfaceY = slope.start.y + delta.y * t;
            const bool crossesFromBelow =
                oldHeadTop <= surfaceY + tolerance &&
                headTop >= surfaceY - tolerance;
            if (!crossesFromBelow) {
                continue;
            }

            newPos.y = surfaceY - headOffset;
            velocity.y = 0.0f;
            return true;
        }

        return false;
    };

    auto crossesFloorSlopeFromBelowNearHighEnd = [&](std::size_t i) {
        if (m_HasLevel2HangingPlatformSlope &&
            (i == m_Level2HangingPlatformSlopeIndex ||
             i == m_Level2HangingPlatformSlopeIndex2)) {
            return false;
        }

        const auto& slope = m_Slopes[i];
        const glm::vec2 highPoint = (slope.start.y >= slope.end.y) ? slope.start : slope.end;
        return std::abs(headCenter.x - highPoint.x) <= m_SlopeTopTransitionWidth;
    };

    if (crossesIndexedSlopesFromBelow(
            m_CeilingSlopes,
            m_SlopeSnapTolerance,
            [](std::size_t) { return false; }
        )) {
        return true;
    }

    // Floor slopes also need an underside barrier so characters cannot jump
    // up through the ramp shell into the higher block behind it.
    if (crossesIndexedSlopesFromBelow(
            m_Slopes,
            undersideTolerance,
            [&](std::size_t i) {
                return !crossesFloorSlopeFromBelowNearHighEnd(i);
            }
        )) {
        return true;
    }

    return false;
}

bool App::TrySnapToSlopeTopTransition(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const SolidRect& blockingBlock,
    const CharacterCollisionProfile& profile
) const {
    float groundY = 0.0f;
    if (!FindNearbyGroundY(newPos, m_StepUpHeight + m_GroundSnapTolerance, groundY, profile)) {
        return false;
    }

    const float oldBottom = GetCharacterBodyBottom(oldPos, profile);
    const float lift = groundY - oldBottom;
    if (lift < -m_GroundSnapTolerance || lift > m_StepUpHeight + m_GroundSnapTolerance) {
        return false;
    }

    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(newPos, profile, bodyCenter, bodySize);
    newPos.y = groundY + (newPos.y - (bodyCenter.y - bodySize.y * 0.5f)) + 0.1f;

    return !CheckCharacterCollision(newPos, blockingBlock, profile);
}

bool App::FindBestSlopeYAtX(
    const glm::vec2& oldPos,
    float desiredX,
    float& outSlopeY,
    const CharacterCollisionProfile& profile
) const {
    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(oldPos, profile, bodyCenter, bodySize);

    const float oldBottom = bodyCenter.y - bodySize.y * 0.5f;
    const float footInset = std::min(m_FootProbeInset, bodySize.x * 0.25f);
    const float leftFootX = desiredX + profile.bodyHitboxOffset.x - bodySize.x * 0.5f + footInset;
    const float rightFootX = desiredX + profile.bodyHitboxOffset.x + bodySize.x * 0.5f - footInset;

    bool foundExactSlope = false;
    float bestExactSlopeY = -std::numeric_limits<float>::infinity();
    bool foundTransitionSlope = false;
    float bestTransitionSlopeY = -std::numeric_limits<float>::infinity();

    for (const auto& slope : m_Slopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        for (const float footX : {leftFootX, desiredX, rightFootX}) {
            const float actualMinX = std::min(slope.start.x, slope.end.x);
            const float actualMaxX = std::max(slope.start.x, slope.end.x);
            const float transitionMinX = actualMinX - m_SlopeTopTransitionWidth;
            const float transitionMaxX = actualMaxX + m_SlopeTopTransitionWidth;
            if (footX < transitionMinX || footX > transitionMaxX) {
                continue;
            }

            const float t = (footX - slope.start.x) / delta.x;
            if (t < -0.15f || t > 1.15f) {
                continue;
            }

            const bool withinActualSpan =
                footX >= actualMinX && footX <= actualMaxX && t >= 0.0f && t <= 1.0f;
            const float sampledT = withinActualSpan ? t : std::clamp(t, 0.0f, 1.0f);
            const float slopeY = slope.start.y + delta.y * sampledT;
            const float deltaFromFeet = slopeY - oldBottom;
            if (deltaFromFeet < -m_SlopeFollowTolerance ||
                deltaFromFeet > m_SlopeTopTransitionHeight) {
                continue;
            }

            if (withinActualSpan) {
                if (!foundExactSlope || slopeY > bestExactSlopeY) {
                    bestExactSlopeY = slopeY;
                    foundExactSlope = true;
                }
            }
            else if (!foundTransitionSlope || slopeY > bestTransitionSlopeY) {
                bestTransitionSlopeY = slopeY;
                foundTransitionSlope = true;
            }
        }
    }

    if (foundExactSlope) {
        outSlopeY = bestExactSlopeY;
        return true;
    }

    if (!foundTransitionSlope) {
        return false;
    }

    outSlopeY = bestTransitionSlopeY;
    return true;
}

void App::ApplySlopeFollow(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const CharacterCollisionProfile& profile
) const {
    float slopeY = 0.0f;
    if (!FindBestSlopeYAtX(oldPos, newPos.x, slopeY, profile)) {
        return;
    }

    glm::vec2 oldBodyCenter;
    glm::vec2 oldBodySize;
    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(oldPos, profile, oldBodyCenter, oldBodySize);
    GetCharacterBodyBox(newPos, profile, bodyCenter, bodySize);
    const float oldBottom = oldBodyCenter.y - oldBodySize.y * 0.5f;
    const float upwardLift = slopeY - oldBottom;
    if (upwardLift > m_StepUpHeight + m_GroundSnapTolerance) {
        return;
    }
    const float bottomOffset = newPos.y - (bodyCenter.y - bodySize.y * 0.5f);
    newPos.y = slopeY + bottomOffset;
}

bool App::FindNearbyGroundY(
    const glm::vec2& pos,
    float maxDistance,
    float& outGroundY,
    const CharacterCollisionProfile& profile
) const {
    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(pos, profile, bodyCenter, bodySize);
    const float left = bodyCenter.x - bodySize.x * 0.5f;
    const float right = bodyCenter.x + bodySize.x * 0.5f;
    const float bottom = bodyCenter.y - bodySize.y * 0.5f;

    bool foundGround = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();

    for (const auto& block : m_SolidBlocks) {
        if (block.isSlopeGuard) {
            continue;
        }
        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;
        if (right <= blockLeft - m_FootProbeInset || left >= blockRight + m_FootProbeInset) {
            continue;
        }

        const float blockTop = block.center.y + block.size.y * 0.5f;
        if (std::abs(blockTop - bottom) > maxDistance) {
            continue;
        }

        if (!foundGround || blockTop > bestGroundY) {
            bestGroundY = blockTop;
            foundGround = true;
        }
    }

    float slopeY = 0.0f;
    if (FindBestSlopeYAtX(pos, pos.x, slopeY, profile) &&
        std::abs(slopeY - bottom) <= maxDistance &&
        (!foundGround || slopeY > bestGroundY)) {
        bestGroundY = slopeY;
        foundGround = true;
    }

    if (!foundGround) {
        return false;
    }

    outGroundY = bestGroundY;
    return true;
}

bool App::FindNearbyCeilingY(
    const glm::vec2& pos,
    float maxDistance,
    float& outCeilingY,
    const CharacterCollisionProfile& profile
) const {
    glm::vec2 headCenter;
    glm::vec2 headSize;
    GetCharacterHeadBox(pos, profile, headCenter, headSize);
    const float top = headCenter.y + headSize.y * 0.5f;

    bool foundCeiling = false;
    float bestCeilingY = std::numeric_limits<float>::infinity();

    for (const auto& block : m_SolidBlocks) {
        if (!block.blockBottom) {
            continue;
        }

        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;
        if (headCenter.x < blockLeft || headCenter.x > blockRight) {
            continue;
        }

        const float blockBottom = block.center.y - block.size.y * 0.5f;
        if (blockBottom >= top && blockBottom - top <= maxDistance) {
            bestCeilingY = std::min(bestCeilingY, blockBottom);
            foundCeiling = true;
        }
    }

    if (!foundCeiling) {
        return false;
    }

    outCeilingY = bestCeilingY;
    return true;
}

bool App::CharacterTouchesRect(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const SolidRect& rect
) const {
    return character && character->IsAlive() && CheckCharacterCollision(character->GetPosition(), rect, profile);
}

bool App::CharacterTouchesHazard(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const HazardRect& hazard
) const {
    if (!character || !character->IsAlive()) {
        return false;
    }

    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);
    const glm::vec2 hazardProbeCenter = {
        bodyCenter.x,
        bodyCenter.y - bodySize.y * 0.22f,
    };
    const glm::vec2 hazardProbeSize = {
        std::max(2.0f, bodySize.x * 0.72f),
        std::max(2.0f, bodySize.y * 0.48f),
    };

    return CheckAABB(hazardProbeCenter, hazardProbeSize, hazard.center, hazard.size);
}

bool App::IsCharacterInLiquid(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile
) const {
    if (!character || !character->IsAlive()) {
        return false;
    }

    for (const auto& hazard : m_Hazards) {
        if (CharacterTouchesHazard(character, profile, hazard)) {
            return true;
        }
    }

    return false;
}

bool App::IsCharacterAtDoor(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const ExitDoor& door
) const {
    return character && character->IsAlive() && CharacterTouchesRect(character, profile, door.triggerRect);
}

bool App::CharacterPressesRectFromAbove(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const SolidRect& rect
) const {
    if (!character || !character->IsAlive()) {
        return false;
    }

    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);
    const float bodyBottom = bodyCenter.y - bodySize.y * 0.5f;
    const float rectTop = rect.center.y + rect.size.y * 0.5f;
    const float rectLeft = rect.center.x - rect.size.x * 0.5f;
    const float rectRight = rect.center.x + rect.size.x * 0.5f;
    const float footInset = std::min(m_FootProbeInset, bodySize.x * 0.25f);
    const float footLeft = bodyCenter.x - bodySize.x * 0.5f + footInset;
    const float footRight = bodyCenter.x + bodySize.x * 0.5f - footInset;
    const bool horizontalOverlap = footRight > rectLeft && footLeft < rectRight;

    return horizontalOverlap &&
           bodyBottom >= rectTop - m_GroundStickTolerance &&
           bodyBottom <= rectTop + m_GroundSnapTolerance;
}

bool App::CubeTouchesRect(const SolidRect& rect) const {
    return m_HasCubeBlock && CheckAABB(m_CubeRect.center, m_CubeRect.size, rect.center, rect.size);
}

bool App::CubePressesRectFromAbove(const SolidRect& rect) const {
    if (!m_HasCubeBlock) {
        return false;
    }

    const float cubeBottom = m_CubeRect.center.y - m_CubeRect.size.y * 0.5f;
    const float rectTop = rect.center.y + rect.size.y * 0.5f;
    const float rectLeft = rect.center.x - rect.size.x * 0.5f;
    const float rectRight = rect.center.x + rect.size.x * 0.5f;
    const float footInset = std::min(m_FootProbeInset, m_CubeRect.size.x * 0.20f);
    const float footLeft = m_CubeRect.center.x - m_CubeRect.size.x * 0.5f + footInset;
    const float footRight = m_CubeRect.center.x + m_CubeRect.size.x * 0.5f - footInset;
    const bool horizontalOverlap = footRight > rectLeft && footLeft < rectRight;

    return horizontalOverlap &&
           cubeBottom >= rectTop - m_GroundStickTolerance &&
           cubeBottom <= rectTop + m_GroundSnapTolerance;
}

bool App::IsGreenButtonPressed() const {
    return CharacterPressesRectFromAbove(m_Fireboy, m_FireboyCollision, m_GreenButtonAfterHitbox) ||
           CharacterPressesRectFromAbove(m_Watergirl, m_WatergirlCollision, m_GreenButtonAfterHitbox) ||
           CubePressesRectFromAbove(m_GreenButtonAfterHitbox) ||
           CharacterPressesRectFromAbove(m_Fireboy, m_FireboyCollision, m_GreenButtonAfterHitbox2) ||
           CharacterPressesRectFromAbove(m_Watergirl, m_WatergirlCollision, m_GreenButtonAfterHitbox2) ||
           CubePressesRectFromAbove(m_GreenButtonAfterHitbox2);
}

bool App::IsGreenSwitchTouched() const {
    return CharacterTouchesRect(m_Fireboy, m_FireboyCollision, m_GreenSwitchHitbox) ||
           CharacterTouchesRect(m_Watergirl, m_WatergirlCollision, m_GreenSwitchHitbox) ||
           CubeTouchesRect(m_GreenSwitchHitbox);
}

void App::CarryCharacterWithPlatform(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const SolidRect& oldPlatform,
    float platformDeltaY
) const {
    if (!character || !character->IsAlive() || std::abs(platformDeltaY) <= 0.001f) {
        return;
    }

    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);
    const float bodyBottom = bodyCenter.y - bodySize.y * 0.5f;
    const float oldTop = oldPlatform.center.y + oldPlatform.size.y * 0.5f;
    const bool stoodOnPlatform =
        bodyBottom >= oldTop - m_GroundSnapTolerance &&
        bodyBottom <= oldTop + m_GroundStickTolerance &&
        bodyCenter.x + bodySize.x * 0.5f > oldPlatform.center.x - oldPlatform.size.x * 0.5f &&
        bodyCenter.x - bodySize.x * 0.5f < oldPlatform.center.x + oldPlatform.size.x * 0.5f;

    if (stoodOnPlatform) {
        character->SetPosition(character->GetPosition() + glm::vec2{0.0f, platformDeltaY});
    }
}

float App::ClampPlatformDeltaAgainstCharacter(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const SolidRect& oldPlatform,
    float requestedDeltaY,
    std::size_t platformBlockIndex
) const {
    if (!character || !character->IsAlive() || std::abs(requestedDeltaY) <= 0.001f) {
        return requestedDeltaY;
    }

    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    glm::vec2 headCenter;
    glm::vec2 headSize;
    GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);
    GetCharacterHeadBox(character->GetPosition(), profile, headCenter, headSize);
    const float bodyBottom = bodyCenter.y - bodySize.y * 0.5f;
    const float headTop = headCenter.y + headSize.y * 0.5f;
    const float platformTop = oldPlatform.center.y + oldPlatform.size.y * 0.5f;
    const float platformBottom = oldPlatform.center.y - oldPlatform.size.y * 0.5f;
    const bool horizontalOverlap =
        std::max(
            bodyCenter.x + bodySize.x * 0.5f,
            headCenter.x + headSize.x * 0.5f
        ) > oldPlatform.center.x - oldPlatform.size.x * 0.5f &&
        std::min(
            bodyCenter.x - bodySize.x * 0.5f,
            headCenter.x - headSize.x * 0.5f
        ) < oldPlatform.center.x + oldPlatform.size.x * 0.5f;

    if (!horizontalOverlap) {
        return requestedDeltaY;
    }

    const bool stoodOnPlatform =
        bodyBottom >= platformTop - m_GroundSnapTolerance &&
        bodyBottom <= platformTop + m_GroundStickTolerance;

    if (requestedDeltaY < 0.0f) {
        if (stoodOnPlatform) {
            return requestedDeltaY;
        }
        if (headTop > platformBottom) {
            return CheckCharacterCollision(character->GetPosition(), oldPlatform, profile)
                ? 0.0f
                : requestedDeltaY;
        }

        const float allowedDelta =
            headTop + DESCENDING_PLATFORM_HEAD_CLEARANCE_LOCAL - platformBottom;
        return std::max(requestedDeltaY, std::min(0.0f, allowedDelta));
    }

    if (stoodOnPlatform) {
        const glm::vec2 startPos = character->GetPosition();
        const glm::vec2 requestedPos = startPos + glm::vec2{0.0f, requestedDeltaY};
        if (!WouldCharacterHitTerrainAt(
                requestedPos,
                profile,
                true,
                platformBlockIndex
            )) {
            return requestedDeltaY;
        }

        float safeDelta = 0.0f;
        float blockedDelta = requestedDeltaY;
        for (int i = 0; i < 8; ++i) {
            const float candidateDelta = (safeDelta + blockedDelta) * 0.5f;
            const glm::vec2 candidatePos =
                startPos + glm::vec2{0.0f, candidateDelta};
            if (WouldCharacterHitTerrainAt(
                    candidatePos,
                    profile,
                    true,
                    platformBlockIndex
                )) {
                blockedDelta = candidateDelta;
            }
            else {
                safeDelta = candidateDelta;
            }
        }
        return safeDelta;
    }

    if (CheckCharacterCollision(character->GetPosition(), oldPlatform, profile)) {
        return 0.0f;
    }
    if (bodyBottom <= platformTop) {
        return requestedDeltaY;
    }

    const float allowedDelta =
        bodyBottom - DESCENDING_PLATFORM_HEAD_CLEARANCE_LOCAL - platformTop;
    return std::min(requestedDeltaY, std::max(0.0f, allowedDelta));
}

void App::CarryCharacterWithCube(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const SolidRect& oldCube,
    const glm::vec2& cubeDelta
) const {
    if (!character || !character->IsAlive()) {
        return;
    }

    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);
    const float bodyBottom = bodyCenter.y - bodySize.y * 0.5f;
    const float oldCubeTop = oldCube.center.y + oldCube.size.y * 0.5f;
    const bool stoodOnCube =
        bodyBottom >= oldCubeTop - m_GroundSnapTolerance &&
        bodyBottom <= oldCubeTop + m_GroundStickTolerance &&
        bodyCenter.x + bodySize.x * 0.5f > oldCube.center.x - oldCube.size.x * 0.5f &&
        bodyCenter.x - bodySize.x * 0.5f < oldCube.center.x + oldCube.size.x * 0.5f;

    if (stoodOnCube) {
        character->SetPosition(character->GetPosition() + cubeDelta);
    }
}

void App::CarryCubeWithPlatform(const SolidRect& oldPlatform, float platformDeltaY) {
    if (!m_HasCubeBlock || std::abs(platformDeltaY) <= 0.001f) {
        return;
    }

    const float cubeBottom = m_CubeRect.center.y - m_CubeRect.size.y * 0.5f;
    const float oldTop = oldPlatform.center.y + oldPlatform.size.y * 0.5f;
    const bool stoodOnPlatform =
        cubeBottom >= oldTop - m_GroundSnapTolerance &&
        cubeBottom <= oldTop + m_GroundStickTolerance &&
        m_CubeRect.center.x + m_CubeRect.size.x * 0.5f > oldPlatform.center.x - oldPlatform.size.x * 0.5f &&
        m_CubeRect.center.x - m_CubeRect.size.x * 0.5f < oldPlatform.center.x + oldPlatform.size.x * 0.5f;

    if (stoodOnPlatform) {
        m_CubeRect.center.y += platformDeltaY;
        m_CubeVelocity.y = 0.0f;
        m_CubeOnGround = true;
        if (m_CubeBlockIndex < m_SolidBlocks.size()) {
            m_SolidBlocks[m_CubeBlockIndex] = m_CubeRect;
        }
        if (m_Cube) {
            m_Cube->SetPosition(m_CubeRect.center);
        }
    }
}

void App::UpdateGreenPlatform() {
    if (!m_HasGreenPlatformBlock || m_GreenPlatformBlockIndex >= m_SolidBlocks.size()) {
        return;
    }

    const bool level4ButtonPressed =
        m_ActiveLevelIndex == 4 &&
        (CharacterPressesRectFromAbove(m_Fireboy, m_FireboyCollision, m_GreenButtonAfterHitbox) ||
         CharacterPressesRectFromAbove(m_Watergirl, m_WatergirlCollision, m_GreenButtonAfterHitbox) ||
         CubePressesRectFromAbove(m_GreenButtonAfterHitbox));
    const SolidRect oldPlatform = m_GreenPlatformCurrentRect;
    const SolidRect& target =
        ((m_ActiveLevelIndex == 4) ? level4ButtonPressed : m_GreenSwitchOn)
            ? m_GreenPlatformPressedRect
            : m_GreenPlatformRestRect;
    const float requestedY = MoveToward(
        oldPlatform.center.y,
        target.center.y,
        m_GreenPlatformSpeed
    );
    float deltaY = requestedY - oldPlatform.center.y;
    deltaY = ClampPlatformDeltaAgainstCharacter(
        m_Fireboy,
        m_FireboyCollision,
        oldPlatform,
        deltaY,
        m_GreenPlatformBlockIndex
    );
    deltaY = ClampPlatformDeltaAgainstCharacter(
        m_Watergirl,
        m_WatergirlCollision,
        oldPlatform,
        deltaY,
        m_GreenPlatformBlockIndex
    );
    m_GreenPlatformCurrentRect.center.y = oldPlatform.center.y + deltaY;
    m_GreenPlatformCurrentRect.center.x = MoveToward(
        m_GreenPlatformCurrentRect.center.x,
        target.center.x,
        m_GreenPlatformSpeed
    );

    m_SolidBlocks[m_GreenPlatformBlockIndex] = m_GreenPlatformCurrentRect;
    if (m_GreenPlatform) {
        m_GreenPlatform->SetPosition(m_GreenPlatformCurrentRect.center);
    }

    CarryCharacterWithPlatform(m_Fireboy, m_FireboyCollision, oldPlatform, deltaY);
    CarryCharacterWithPlatform(m_Watergirl, m_WatergirlCollision, oldPlatform, deltaY);
    CarryCubeWithPlatform(oldPlatform, deltaY);
}

void App::UpdateGreenPlatform2() {
    if (!m_HasGreenPlatformBlock2 || m_GreenPlatformBlockIndex2 >= m_SolidBlocks.size()) {
        return;
    }

    const float buttonAnimSpeed = m_GreenButtonAnimSpeed;

    const bool leftPressed =
        CharacterPressesRectFromAbove(m_Fireboy, m_FireboyCollision, m_GreenButtonAfterHitbox) ||
        CharacterPressesRectFromAbove(m_Watergirl, m_WatergirlCollision, m_GreenButtonAfterHitbox) ||
        CubePressesRectFromAbove(m_GreenButtonAfterHitbox);
    const bool rightPressed =
        CharacterPressesRectFromAbove(m_Fireboy, m_FireboyCollision, m_GreenButtonAfterHitbox2) ||
        CharacterPressesRectFromAbove(m_Watergirl, m_WatergirlCollision, m_GreenButtonAfterHitbox2) ||
        CubePressesRectFromAbove(m_GreenButtonAfterHitbox2);
    const bool pressed = (m_ActiveLevelIndex == 4)
        ? m_GreenSwitchOn
        : (leftPressed || rightPressed);
    m_GreenButtonPressed = pressed;
    const SolidRect oldPlatform = m_GreenPlatformCurrentRect2;
    const SolidRect& target = pressed ? m_GreenPlatformPressedRect2 : m_GreenPlatformRestRect2;
    const float requestedY = MoveToward(
        oldPlatform.center.y,
        target.center.y,
        m_GreenPlatformSpeed
    );
    float deltaY = requestedY - oldPlatform.center.y;
    deltaY = ClampPlatformDeltaAgainstCharacter(
        m_Fireboy,
        m_FireboyCollision,
        oldPlatform,
        deltaY,
        m_GreenPlatformBlockIndex2
    );
    deltaY = ClampPlatformDeltaAgainstCharacter(
        m_Watergirl,
        m_WatergirlCollision,
        oldPlatform,
        deltaY,
        m_GreenPlatformBlockIndex2
    );
    m_GreenPlatformCurrentRect2.center.y = oldPlatform.center.y + deltaY;
    m_GreenPlatformCurrentRect2.center.x = MoveToward(
        m_GreenPlatformCurrentRect2.center.x,
        target.center.x,
        m_GreenPlatformSpeed
    );

    m_SolidBlocks[m_GreenPlatformBlockIndex2] = m_GreenPlatformCurrentRect2;
    if (m_GreenPlatform2) {
        m_GreenPlatform2->SetPosition(m_GreenPlatformCurrentRect2.center);
    }

    auto animateButton = [&](const std::shared_ptr<Character>& button,
                             const glm::vec2& basePosition,
                             const glm::vec2& baseSize,
                             float& pressVisual,
                             bool isPressed) {
        pressVisual = MoveToward(
            pressVisual,
            isPressed ? m_GreenButtonPressDepth : 0.0f,
            buttonAnimSpeed
        );

        if (!button) {
            return;
        }

        const float visibleRatio = std::clamp(
            1.0f - (pressVisual / std::max(0.001f, m_GreenButtonPressDepth)),
            0.0f,
            1.0f
        );
        const float visibleHeight = baseSize.y * visibleRatio;

        button->SetVisible(visibleHeight > 0.5f);
        button->SetSize({baseSize.x, std::max(0.1f, visibleHeight)});
        button->SetPosition(basePosition + glm::vec2{0.0f, -pressVisual * 0.5f});
    };

    animateButton(
        m_GreenButtonAfter,
        m_GreenButtonAfterBasePosition,
        m_GreenButtonAfterBaseSize,
        m_GreenButtonAfterPressVisual,
        leftPressed
    );
    animateButton(
        m_GreenButtonAfter2,
        m_GreenButtonAfterBasePosition2,
        m_GreenButtonAfterBaseSize2,
        m_GreenButtonAfterPressVisual2,
        rightPressed
    );

    CarryCharacterWithPlatform(m_Fireboy, m_FireboyCollision, oldPlatform, deltaY);
    CarryCharacterWithPlatform(m_Watergirl, m_WatergirlCollision, oldPlatform, deltaY);
    CarryCubeWithPlatform(oldPlatform, deltaY);
}

void App::UpdateLevel2HiddenPlatform() {
    if (!m_HasLevel2HiddenPlatformBlock || m_Level2HiddenPlatformBlockIndex >= m_SolidBlocks.size()) {
        return;
    }

    const float buttonAnimSpeed = m_GreenButtonAnimSpeed;
    const bool leftPressed =
        CharacterPressesRectFromAbove(m_Fireboy, m_FireboyCollision, m_Level2TopButtonLeftHitbox) ||
        CharacterPressesRectFromAbove(m_Watergirl, m_WatergirlCollision, m_Level2TopButtonLeftHitbox) ||
        CubePressesRectFromAbove(m_Level2TopButtonLeftHitbox);
    const bool rightPressed =
        CharacterPressesRectFromAbove(m_Fireboy, m_FireboyCollision, m_Level2TopButtonRightHitbox) ||
        CharacterPressesRectFromAbove(m_Watergirl, m_WatergirlCollision, m_Level2TopButtonRightHitbox) ||
        CubePressesRectFromAbove(m_Level2TopButtonRightHitbox);
    const bool shown = leftPressed || rightPressed;

    const SolidRect oldPlatform = m_Level2HiddenPlatformCurrentRect;
    const SolidRect& target = shown ? m_Level2HiddenPlatformShownRect : m_Level2HiddenPlatformRestRect;
    m_Level2HiddenPlatformCurrentRect.center.x = MoveToward(
        m_Level2HiddenPlatformCurrentRect.center.x,
        target.center.x,
        m_GreenPlatformSpeed
    );
    m_Level2HiddenPlatformCurrentRect.center.y = target.center.y;

    const float deltaX = m_Level2HiddenPlatformCurrentRect.center.x - oldPlatform.center.x;
    m_SolidBlocks[m_Level2HiddenPlatformBlockIndex] = m_Level2HiddenPlatformCurrentRect;
    if (m_Level2HiddenPlatform) {
        m_Level2HiddenPlatform->SetPosition(m_Level2HiddenPlatformCurrentRect.center);
    }

    auto animateButton = [&](const std::shared_ptr<Character>& button,
                             const glm::vec2& basePosition,
                             float& pressVisual,
                             bool isPressed) {
        pressVisual = MoveToward(
            pressVisual,
            isPressed ? m_GreenButtonPressDepth : 0.0f,
            buttonAnimSpeed
        );

        if (!button) {
            return;
        }
        button->SetVisible(true);
        button->SetSize(m_Level2TopButtonBaseSize);
        button->SetPosition(basePosition + glm::vec2{0.0f, -pressVisual});
    };

    animateButton(
        m_Level2TopButtonLeft,
        m_Level2TopButtonLeftBasePosition,
        m_Level2TopButtonLeftPressVisual,
        leftPressed
    );
    animateButton(
        m_Level2TopButtonRight,
        m_Level2TopButtonRightBasePosition,
        m_Level2TopButtonRightPressVisual,
        rightPressed
    );

    auto carryCharacterHorizontally = [&](const std::shared_ptr<HeadBodyCharacter>& character,
                                          const CharacterCollisionProfile& profile) {
        if (!character || !character->IsAlive() || std::abs(deltaX) <= 0.001f) {
            return;
        }

        glm::vec2 bodyCenter;
        glm::vec2 bodySize;
        GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);
        const float bodyBottom = bodyCenter.y - bodySize.y * 0.5f;
        const float oldTop = oldPlatform.center.y + oldPlatform.size.y * 0.5f;
        const bool stoodOnPlatform =
            bodyBottom >= oldTop - m_GroundSnapTolerance &&
            bodyBottom <= oldTop + m_GroundStickTolerance &&
            bodyCenter.x + bodySize.x * 0.5f > oldPlatform.center.x - oldPlatform.size.x * 0.5f &&
            bodyCenter.x - bodySize.x * 0.5f < oldPlatform.center.x + oldPlatform.size.x * 0.5f;

        if (stoodOnPlatform) {
            character->SetPosition(character->GetPosition() + glm::vec2{deltaX, 0.0f});
        }
    };

    auto carryCubeHorizontally = [&]() {
        if (!m_HasCubeBlock || std::abs(deltaX) <= 0.001f) {
            return;
        }

        const float cubeBottom = m_CubeRect.center.y - m_CubeRect.size.y * 0.5f;
        const float oldTop = oldPlatform.center.y + oldPlatform.size.y * 0.5f;
        const bool stoodOnPlatform =
            cubeBottom >= oldTop - m_GroundSnapTolerance &&
            cubeBottom <= oldTop + m_GroundStickTolerance &&
            m_CubeRect.center.x + m_CubeRect.size.x * 0.5f > oldPlatform.center.x - oldPlatform.size.x * 0.5f &&
            m_CubeRect.center.x - m_CubeRect.size.x * 0.5f < oldPlatform.center.x + oldPlatform.size.x * 0.5f;

        if (stoodOnPlatform) {
            m_CubeRect.center.x += deltaX;
            if (m_CubeBlockIndex < m_SolidBlocks.size()) {
                m_SolidBlocks[m_CubeBlockIndex] = m_CubeRect;
            }
            if (m_Cube) {
                m_Cube->SetPosition(m_CubeRect.center);
            }
        }
    };

    carryCharacterHorizontally(m_Fireboy, m_FireboyCollision);
    carryCharacterHorizontally(m_Watergirl, m_WatergirlCollision);
    carryCubeHorizontally();
}

void App::UpdateLevel2HangingPlatform() {
    auto updateHangingPlatform = [&](
        const std::shared_ptr<Character>& platform,
        const SolidRect& platformRect,
        std::size_t slopeIndex,
        float& angle
    ) {
        if (!platform || platformRect.size.x <= 0.0f || platformRect.size.y <= 0.0f) {
            return;
        }

        float torque = 0.0f;
        const float halfWidth = platformRect.size.x * 0.5f;
        const float halfHeight = platformRect.size.y * 0.5f;

        auto platformTopYAt = [&](float worldX, float platformAngle) {
            const float localX = std::clamp(
                worldX - platformRect.center.x,
                -halfWidth,
                halfWidth
            );
            return platformRect.center.y +
                halfHeight * std::cos(platformAngle) +
                localX * std::sin(platformAngle);
        };

        auto characterStandsOnPlatform = [&](
            const std::shared_ptr<HeadBodyCharacter>& character,
            const CharacterCollisionProfile& profile,
            glm::vec2& outBodyCenter,
            glm::vec2& outBodySize
        ) {
            if (!character || !character->IsAlive()) {
                return false;
            }

            GetCharacterBodyBox(character->GetPosition(), profile, outBodyCenter, outBodySize);
            const float bottom = outBodyCenter.y - outBodySize.y * 0.5f;
            const float platformTop = platformTopYAt(outBodyCenter.x, angle);
            const bool horizontallyOverlaps =
                outBodyCenter.x + outBodySize.x * 0.5f > platformRect.center.x - halfWidth &&
                outBodyCenter.x - outBodySize.x * 0.5f < platformRect.center.x + halfWidth;

            const float platformContactTolerance = std::max(
                m_GroundStickTolerance,
                halfWidth * std::sin(std::abs(angle)) + m_GroundSnapTolerance
            );

            return horizontallyOverlaps &&
                bottom >= platformTop - m_GroundSnapTolerance &&
                bottom <= platformTop + platformContactTolerance;
        };

        auto addTorque = [&](const std::shared_ptr<HeadBodyCharacter>& character, const CharacterCollisionProfile& profile) {
            glm::vec2 bodyCenter;
            glm::vec2 bodySize;
            if (characterStandsOnPlatform(character, profile, bodyCenter, bodySize)) {
                torque += (bodyCenter.x - platformRect.center.x) *
                    m_Level2HangingPlatformTorqueScale;
            }
        };

        addTorque(m_Fireboy, m_FireboyCollision);
        addTorque(m_Watergirl, m_WatergirlCollision);

        const float targetAngle = std::clamp(
            -torque,
            -m_Level2HangingPlatformMaxAngle,
            m_Level2HangingPlatformMaxAngle
        );
        angle = MoveToward(
            angle,
            targetAngle,
            m_Level2HangingPlatformAngularSpeed * (1.0f / 60.0f)
        );

        platform->m_Transform.rotation = angle;
        if (m_HasLevel2HangingPlatformSlope && slopeIndex < m_Slopes.size()) {
            const float leftY =
                -halfWidth * std::sin(angle) +
                halfHeight * std::cos(angle);
            const float rightY =
                halfWidth * std::sin(angle) +
                halfHeight * std::cos(angle);
            m_Slopes[slopeIndex] = {
                platformRect.center + glm::vec2{-halfWidth, leftY},
                platformRect.center + glm::vec2{halfWidth, rightY},
            };
        }

        auto settleCharacterOnPlatform = [&](
            const std::shared_ptr<HeadBodyCharacter>& character,
            const CharacterCollisionProfile& profile,
            glm::vec2& velocity,
            bool& onGround
        ) {
            glm::vec2 bodyCenter;
            glm::vec2 bodySize;
            if (!characterStandsOnPlatform(character, profile, bodyCenter, bodySize)) {
                return;
            }

            velocity.y = std::min(velocity.y, 0.0f);
            onGround = true;

            glm::vec2 pos = character->GetPosition();
            const float bottomOffset = pos.y - (bodyCenter.y - bodySize.y * 0.5f);
            pos.y = platformTopYAt(pos.x + profile.bodyHitboxOffset.x, angle) + bottomOffset;
            character->SetPosition(pos);
        };

        settleCharacterOnPlatform(
            m_Fireboy,
            m_FireboyCollision,
            m_FireboyVelocity,
            m_FireboyOnGround
        );
        settleCharacterOnPlatform(
            m_Watergirl,
            m_WatergirlCollision,
            m_WatergirlVelocity,
            m_WatergirlOnGround
        );
    };

    updateHangingPlatform(
        m_Level2HangingPlatform,
        m_Level2HangingPlatformRect,
        m_Level2HangingPlatformSlopeIndex,
        m_Level2HangingPlatformAngle
    );
    updateHangingPlatform(
        m_Level2HangingPlatform2,
        m_Level2HangingPlatformRect2,
        m_Level2HangingPlatformSlopeIndex2,
        m_Level2HangingPlatformAngle2
    );
}

void App::UpdateLevel4ChainPlatforms() {
    if (!m_HasLevel4ChainPlatforms ||
        (!m_HasLevel4ChainPlatformTop && !m_HasLevel4ChainPlatformBottom) ||
        (m_HasLevel4ChainPlatformTop && m_Level4ChainPlatformTopBlockIndex >= m_SolidBlocks.size()) ||
        (m_HasLevel4ChainPlatformBottom && m_Level4ChainPlatformBottomBlockIndex >= m_SolidBlocks.size())) {
        return;
    }

    auto cubeStandsOn = [&](const SolidRect& platform) {
        if (!m_HasCubeBlock) {
            return false;
        }

        const float cubeBottom = m_CubeRect.center.y - m_CubeRect.size.y * 0.5f;
        const float platformTop = platform.center.y + platform.size.y * 0.5f;
        return cubeBottom >= platformTop - m_GroundSnapTolerance &&
            cubeBottom <= platformTop + m_GroundStickTolerance &&
            m_CubeRect.center.x + m_CubeRect.size.x * 0.5f > platform.center.x - platform.size.x * 0.5f &&
            m_CubeRect.center.x - m_CubeRect.size.x * 0.5f < platform.center.x + platform.size.x * 0.5f;
    };

    const bool topLoaded =
        m_HasLevel4ChainPlatformTop &&
        cubeStandsOn(m_Level4ChainPlatformTopCurrentRect);
    const bool bottomLoaded =
        m_HasLevel4ChainPlatformBottom &&
        cubeStandsOn(m_Level4ChainPlatformBottomCurrentRect);

    // The mechanism has one valid travel range: the upper platform descends
    // while the lower counter-platform rises. Loading the lower platform
    // returns the mechanism toward rest instead of driving both platforms
    // beyond the authored shafts.
    const float targetOffset =
        topLoaded && !bottomLoaded ? -m_Level4ChainPlatformMaxOffset : 0.0f;

    const float oldOffset = m_Level4ChainPlatformOffset;
    const float distanceToTarget = targetOffset - m_Level4ChainPlatformOffset;
    const float desiredVelocity = std::clamp(
        distanceToTarget * 0.08f,
        -m_Level4ChainPlatformSpeed,
        m_Level4ChainPlatformSpeed
    );
    m_Level4ChainPlatformVelocity = MoveToward(
        m_Level4ChainPlatformVelocity,
        desiredVelocity,
        m_Level4ChainPlatformAcceleration
    );

    float nextOffset = m_Level4ChainPlatformOffset + m_Level4ChainPlatformVelocity;
    const bool crossesTarget =
        (distanceToTarget > 0.0f && nextOffset >= targetOffset) ||
        (distanceToTarget < 0.0f && nextOffset <= targetOffset);
    if (crossesTarget ||
        (std::abs(distanceToTarget) <= 0.05f &&
         std::abs(m_Level4ChainPlatformVelocity) <= m_Level4ChainPlatformAcceleration)) {
        nextOffset = targetOffset;
        m_Level4ChainPlatformVelocity = 0.0f;
    }
    m_Level4ChainPlatformOffset = std::clamp(
        nextOffset,
        -m_Level4ChainPlatformMaxOffset,
        0.0f
    );

    const SolidRect oldTop = m_Level4ChainPlatformTopCurrentRect;
    const SolidRect oldBottom = m_Level4ChainPlatformBottomCurrentRect;

    if (m_HasLevel4ChainPlatformTop) {
        m_Level4ChainPlatformTopCurrentRect = m_Level4ChainPlatformTopRestRect;
        m_Level4ChainPlatformTopCurrentRect.center.y += m_Level4ChainPlatformOffset;
        m_SolidBlocks[m_Level4ChainPlatformTopBlockIndex] = m_Level4ChainPlatformTopCurrentRect;
    }
    if (m_HasLevel4ChainPlatformBottom) {
        m_Level4ChainPlatformBottomCurrentRect = m_Level4ChainPlatformBottomRestRect;
        m_Level4ChainPlatformBottomCurrentRect.center.y -=
            m_Level4ChainPlatformOffset * m_Level4ChainPlatformBottomTravelScale;
        m_SolidBlocks[m_Level4ChainPlatformBottomBlockIndex] = m_Level4ChainPlatformBottomCurrentRect;
    }

    const float topDeltaY = m_Level4ChainPlatformTopCurrentRect.center.y - oldTop.center.y;
    const float bottomDeltaY = m_Level4ChainPlatformBottomCurrentRect.center.y - oldBottom.center.y;
    const float offsetDelta = m_Level4ChainPlatformOffset - oldOffset;
    if (m_Level4HorizontalChainWidth > 0.0f) {
        m_Level4HorizontalChainAnimPhase = std::fmod(
            m_Level4HorizontalChainAnimPhase - offsetDelta * 1.8f,
            m_Level4HorizontalChainWidth
        );
        if (m_Level4HorizontalChainAnimPhase < 0.0f) {
            m_Level4HorizontalChainAnimPhase += m_Level4HorizontalChainWidth;
        }
    }

    for (std::size_t i = 0;
         i < m_Level4HorizontalChainLinks.size() &&
         i < m_Level4HorizontalChainBasePositions.size();
         ++i) {
        if (!m_Level4HorizontalChainLinks[i]) {
            continue;
        }

        float x = m_Level4HorizontalChainBasePositions[i].x;
        if (m_Level4HorizontalChainWidth > 0.0f &&
            m_Level4HorizontalChainSpacing > 0.0f) {
            x = m_Level4HorizontalChainMinX + std::fmod(
                (static_cast<float>(i) + 0.5f) * m_Level4HorizontalChainSpacing +
                    m_Level4HorizontalChainAnimPhase,
                m_Level4HorizontalChainWidth
            );
        }
        m_Level4HorizontalChainLinks[i]->SetVisible(true);
        m_Level4HorizontalChainLinks[i]->SetPosition({
            x,
            m_Level4HorizontalChainBasePositions[i].y
        });
        m_Level4HorizontalChainLinks[i]->m_Transform.rotation = 1.57079632679f;
    }

    constexpr float twoPi = 6.28318530718f;
    m_Level4ChainPulleyRotation = std::fmod(
        m_Level4ChainPulleyRotation - offsetDelta * 0.055f,
        twoPi
    );
    for (const auto& pulley : m_Level4ChainPulleys) {
        if (pulley) {
            pulley->m_Transform.rotation = m_Level4ChainPulleyRotation;
        }
    }

    if (m_HasLevel4ChainPlatformTop && m_Level4ChainPlatformTop) {
        m_Level4ChainPlatformTop->SetPosition(m_Level4ChainPlatformTopCurrentRect.center);
    }
    if (m_HasLevel4ChainPlatformBottom && m_Level4ChainPlatformBottom) {
        m_Level4ChainPlatformBottom->SetPosition(m_Level4ChainPlatformBottomCurrentRect.center);
    }
    if (m_HasLevel4ChainPlatformTop && m_Level4ChainConnectTop) {
        m_Level4ChainConnectTop->SetPosition({
            m_Level4ChainPlatformTopCurrentRect.center.x + m_Level4ChainTopXOffset,
            m_Level4ChainPlatformTopCurrentRect.center.y + m_Level4ChainTopConnectorYOffset,
        });
    }
    if (m_HasLevel4ChainPlatformBottom && m_Level4ChainConnectBottom) {
        m_Level4ChainConnectBottom->SetPosition({
            m_Level4ChainPlatformBottomCurrentRect.center.x + m_Level4ChainBottomXOffset,
            m_Level4ChainPlatformBottomCurrentRect.center.y +
                m_Level4ChainBottomConnectorYOffset,
        });
    }
    if (m_HasLevel4ChainPlatformTop && m_Level4ChainTop) {
        const float anchorY = m_HasLevel4ChainTopAnchor
            ? m_Level4ChainTopAnchorY
            : oldTop.center.y + oldTop.size.y * 2.0f;
        const float chainEndY = m_Level4ChainConnectTop
            ? m_Level4ChainConnectTop->GetPosition().y
            : m_Level4ChainPlatformTopCurrentRect.center.y +
                m_Level4ChainPlatformTopCurrentRect.size.y * 0.5f;
        const glm::vec2 chainSize = m_Level4ChainTop->GetSize();
        m_Level4ChainTop->SetSize({
            chainSize.x,
            std::max(1.0f, std::abs(anchorY - chainEndY))
        });
        m_Level4ChainTop->SetPosition({
            m_Level4ChainPlatformTopCurrentRect.center.x + m_Level4ChainTopXOffset,
            (anchorY + chainEndY) * 0.5f
        });
    }
    if (m_HasLevel4ChainPlatformBottom && m_Level4ChainBottom) {
        const float anchorY = m_HasLevel4ChainBottomAnchor
            ? m_Level4ChainBottomAnchorY
            : oldBottom.center.y + oldBottom.size.y * 2.0f;
        const float chainEndY = m_Level4ChainConnectBottom
            ? m_Level4ChainConnectBottom->GetPosition().y
            : m_Level4ChainPlatformBottomCurrentRect.center.y +
                m_Level4ChainPlatformBottomCurrentRect.size.y * 0.5f;
        const glm::vec2 chainSize = m_Level4ChainBottom->GetSize();
        m_Level4ChainBottom->SetSize({
            chainSize.x,
            std::max(1.0f, std::abs(anchorY - chainEndY))
        });
        m_Level4ChainBottom->SetPosition({
            m_Level4ChainPlatformBottomCurrentRect.center.x + m_Level4ChainBottomXOffset,
            (anchorY + chainEndY) * 0.5f
        });
    }

    if (m_HasLevel4ChainPlatformTop) {
        CarryCharacterWithPlatform(m_Fireboy, m_FireboyCollision, oldTop, topDeltaY);
        CarryCharacterWithPlatform(m_Watergirl, m_WatergirlCollision, oldTop, topDeltaY);
        CarryCubeWithPlatform(oldTop, topDeltaY);
    }
    if (m_HasLevel4ChainPlatformBottom) {
        CarryCharacterWithPlatform(m_Fireboy, m_FireboyCollision, oldBottom, bottomDeltaY);
        CarryCharacterWithPlatform(m_Watergirl, m_WatergirlCollision, oldBottom, bottomDeltaY);
        CarryCubeWithPlatform(oldBottom, bottomDeltaY);
    }
}
