#include "App.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
constexpr std::size_t kNoIgnoredBlock = static_cast<std::size_t>(-1);
constexpr float DESCENDING_PLATFORM_HEAD_CLEARANCE_LOCAL = 2.0f;
constexpr float HANGING_PLATFORM_SLIDE_ACCELERATION = 0.24f;
constexpr float HANGING_PLATFORM_MAX_SLIDE_SPEED = 4.0f;
constexpr float HANGING_PLATFORM_SLIDE_POSITION_NUDGE = 0.45f;
constexpr float HANGING_PLATFORM_MIN_SLIDE_ANGLE = 0.035f;

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
    GetCharacterBodyBox(bodyPos, profile, bodyCenter, bodySize);
    return bodyCenter.x - bodySize.x * 0.5f;
}

float App::GetCharacterRightEdge(const glm::vec2& bodyPos, const CharacterCollisionProfile& profile) const {
    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(bodyPos, profile, bodyCenter, bodySize);
    return bodyCenter.x + bodySize.x * 0.5f;
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
    const CharacterCollisionProfile& profile
) {
    const float oldLeft = GetCharacterLeftEdge(oldPos, profile);
    const float oldRight = GetCharacterRightEdge(oldPos, profile);

    for (const auto& block : m_SolidBlocks) {
        if (!CheckCharacterCollision(newPos, block, profile)) {
            continue;
        }

        if (TrySnapToSlopeTopTransition(oldPos, newPos, block, profile)) {
            continue;
        }

        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;
        if (newPos.x > oldPos.x && oldRight <= blockLeft + m_GroundSnapTolerance) {
            const float rightOffset = oldPos.x - oldRight;
            newPos.x = blockLeft + rightOffset;
        }
        else if (newPos.x < oldPos.x && oldLeft >= blockRight - m_GroundSnapTolerance) {
            const float leftOffset = oldPos.x - oldLeft;
            newPos.x = blockRight + leftOffset;
        }
    }
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

    for (const auto& block : m_SolidBlocks) {
        if (!CheckCharacterCollision(newPos, block, profile)) {
            continue;
        }

        glm::vec2 bodyCenter;
        glm::vec2 bodySize;
        glm::vec2 headCenter;
        glm::vec2 headSize;
        GetCharacterBodyBox(oldPos, profile, bodyCenter, bodySize);
        GetCharacterHeadBox(oldPos, profile, headCenter, headSize);

        const float oldBottom = bodyCenter.y - bodySize.y * 0.5f;
        const float oldTop = headCenter.y + headSize.y * 0.5f;
        const float blockTop = block.center.y + block.size.y * 0.5f;
        const float blockBottom = block.center.y - block.size.y * 0.5f;

        if (velocity.y <= 0.0f && oldBottom >= blockTop - m_GroundSnapTolerance) {
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
            if (crossesSlope && (!foundSlope || slopeY > bestGroundY)) {
                bestGroundY = slopeY;
                foundSlope = true;
            }
        }
    }

    if (!foundSlope) {
        return false;
    }

    const float bottomOffset = newPos.y - newBottom;
    newPos.y = bestGroundY + bottomOffset;
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
    (void)oldPos;

    if (velocity.y <= 0.0f) {
        return false;
    }

    glm::vec2 headCenter;
    glm::vec2 headSize;
    GetCharacterHeadBox(newPos, profile, headCenter, headSize);
    const float headTop = headCenter.y + headSize.y * 0.5f;
    const float headOffset = headTop - newPos.y;

    for (const auto& slope : m_CeilingSlopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        const float t = (headCenter.x - slope.start.x) / delta.x;
        if (t < -0.10f || t > 1.10f) {
            continue;
        }

        const float ceilingY = slope.start.y + delta.y * t;
        if (headTop >= ceilingY - m_SlopeSnapTolerance) {
            newPos.y = ceilingY - headOffset;
            velocity.y = 0.0f;
            return true;
        }
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
    newPos.y = groundY + (newPos.y - (bodyCenter.y - bodySize.y * 0.5f));

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

    bool foundSlope = false;
    float bestSlopeY = -std::numeric_limits<float>::infinity();

    for (const auto& slope : m_Slopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        for (const float footX : {leftFootX, desiredX, rightFootX}) {
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

void App::ApplySlopeFollow(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const CharacterCollisionProfile& profile
) const {
    float slopeY = 0.0f;
    if (!FindBestSlopeYAtX(oldPos, newPos.x, slopeY, profile)) {
        return;
    }

    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(newPos, profile, bodyCenter, bodySize);
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

    SolidRect hazardRect;
    hazardRect.center = hazard.center;
    hazardRect.size = hazard.size;
    return CharacterTouchesRect(character, profile, hazardRect);
}

bool App::IsCharacterInLiquid(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile
) const {
    if (!character || !character->IsAlive()) {
        return false;
    }

    glm::vec2 bodyCenter;
    glm::vec2 bodySize;
    GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);
    const glm::vec2 footCenter = {
        bodyCenter.x,
        bodyCenter.y - bodySize.y * 0.40f,
    };
    const glm::vec2 footSize = {
        std::max(2.0f, bodySize.x * 0.70f),
        std::max(2.0f, bodySize.y * 0.20f),
    };

    for (const auto& hazard : m_Hazards) {
        if (CheckAABB(footCenter, footSize, hazard.center, hazard.size)) {
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
    const glm::vec2 pressCenter = {bodyCenter.x, bodyBottom};
    const glm::vec2 pressSize = {std::max(2.0f, bodySize.x * 0.75f), m_GroundSnapTolerance + 2.0f};

    return bodyBottom >= rectTop - m_GroundSnapTolerance &&
           CheckAABB(pressCenter, pressSize, rect.center, rect.size);
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
    const glm::vec2 pressCenter = {m_CubeRect.center.x, cubeBottom};
    const glm::vec2 pressSize = {std::max(2.0f, m_CubeRect.size.x * 0.85f), m_GroundSnapTolerance + 2.0f};

    return cubeBottom >= rectTop - m_GroundSnapTolerance &&
           CheckAABB(pressCenter, pressSize, rect.center, rect.size);
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

float App::ClampDescendingPlatformDeltaAgainstCharacter(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const SolidRect& oldPlatform,
    float requestedDeltaY,
    std::size_t platformBlockIndex
) const {
    if (!character || !character->IsAlive() || requestedDeltaY >= 0.0f) {
        return requestedDeltaY;
    }

    glm::vec2 headCenter;
    glm::vec2 headSize;
    GetCharacterHeadBox(character->GetPosition(), profile, headCenter, headSize);
    const float headTop = headCenter.y + headSize.y * 0.5f;
    const float platformBottom = oldPlatform.center.y - oldPlatform.size.y * 0.5f;
    const bool horizontalOverlap =
        headCenter.x + headSize.x * 0.5f > oldPlatform.center.x - oldPlatform.size.x * 0.5f &&
        headCenter.x - headSize.x * 0.5f < oldPlatform.center.x + oldPlatform.size.x * 0.5f;

    if (!horizontalOverlap || headTop > platformBottom) {
        return requestedDeltaY;
    }

    const float allowedDelta = headTop + DESCENDING_PLATFORM_HEAD_CLEARANCE_LOCAL - platformBottom;
    if (allowedDelta > requestedDeltaY) {
        return allowedDelta;
    }

    return requestedDeltaY + static_cast<float>(platformBlockIndex == kNoIgnoredBlock) * 0.0f;
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

    const SolidRect oldPlatform = m_GreenPlatformCurrentRect;
    const SolidRect& target = m_GreenSwitchOn ? m_GreenPlatformPressedRect : m_GreenPlatformRestRect;
    m_GreenPlatformCurrentRect.center.y = MoveToward(
        m_GreenPlatformCurrentRect.center.y,
        target.center.y,
        m_GreenPlatformSpeed
    );
    m_GreenPlatformCurrentRect.center.x = MoveToward(
        m_GreenPlatformCurrentRect.center.x,
        target.center.x,
        m_GreenPlatformSpeed
    );

    const float deltaY = m_GreenPlatformCurrentRect.center.y - oldPlatform.center.y;
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

    const bool pressed = IsGreenButtonPressed();
    m_GreenButtonPressed = pressed;
    const SolidRect oldPlatform = m_GreenPlatformCurrentRect2;
    const SolidRect& target = pressed ? m_GreenPlatformPressedRect2 : m_GreenPlatformRestRect2;
    m_GreenPlatformCurrentRect2.center.y = MoveToward(
        m_GreenPlatformCurrentRect2.center.y,
        target.center.y,
        m_GreenPlatformSpeed
    );
    m_GreenPlatformCurrentRect2.center.x = MoveToward(
        m_GreenPlatformCurrentRect2.center.x,
        target.center.x,
        m_GreenPlatformSpeed
    );

    const float deltaY = m_GreenPlatformCurrentRect2.center.y - oldPlatform.center.y;
    m_SolidBlocks[m_GreenPlatformBlockIndex2] = m_GreenPlatformCurrentRect2;
    if (m_GreenPlatform2) {
        m_GreenPlatform2->SetPosition(m_GreenPlatformCurrentRect2.center);
    }

    m_GreenButtonAfterPressVisual = MoveToward(
        m_GreenButtonAfterPressVisual,
        pressed ? m_GreenButtonPressDepth : 0.0f,
        m_GreenButtonAnimSpeed
    );
    m_GreenButtonAfterPressVisual2 = MoveToward(
        m_GreenButtonAfterPressVisual2,
        pressed ? m_GreenButtonPressDepth : 0.0f,
        m_GreenButtonAnimSpeed
    );
    if (m_GreenButtonAfter) {
        m_GreenButtonAfter->SetPosition(m_GreenButtonAfterBasePosition + glm::vec2{0.0f, -m_GreenButtonAfterPressVisual});
    }
    if (m_GreenButtonAfter2) {
        m_GreenButtonAfter2->SetPosition(m_GreenButtonAfterBasePosition2 + glm::vec2{0.0f, -m_GreenButtonAfterPressVisual2});
    }

    CarryCharacterWithPlatform(m_Fireboy, m_FireboyCollision, oldPlatform, deltaY);
    CarryCharacterWithPlatform(m_Watergirl, m_WatergirlCollision, oldPlatform, deltaY);
    CarryCubeWithPlatform(oldPlatform, deltaY);
}

void App::UpdateLevel2HiddenPlatform() {
    if (!m_HasLevel2HiddenPlatformBlock || m_Level2HiddenPlatformBlockIndex >= m_SolidBlocks.size()) {
        return;
    }

    const bool shown = CharacterPressesRectFromAbove(m_Fireboy, m_FireboyCollision, m_Level2TopButtonLeftHitbox) ||
                       CharacterPressesRectFromAbove(m_Watergirl, m_WatergirlCollision, m_Level2TopButtonLeftHitbox) ||
                       CharacterPressesRectFromAbove(m_Fireboy, m_FireboyCollision, m_Level2TopButtonRightHitbox) ||
                       CharacterPressesRectFromAbove(m_Watergirl, m_WatergirlCollision, m_Level2TopButtonRightHitbox) ||
                       CubePressesRectFromAbove(m_Level2TopButtonLeftHitbox) ||
                       CubePressesRectFromAbove(m_Level2TopButtonRightHitbox);

    const SolidRect oldPlatform = m_Level2HiddenPlatformCurrentRect;
    const SolidRect& target = shown ? m_Level2HiddenPlatformShownRect : m_Level2HiddenPlatformRestRect;
    m_Level2HiddenPlatformCurrentRect.center.x = MoveToward(
        m_Level2HiddenPlatformCurrentRect.center.x,
        target.center.x,
        m_GreenPlatformSpeed
    );
    m_Level2HiddenPlatformCurrentRect.center.y = MoveToward(
        m_Level2HiddenPlatformCurrentRect.center.y,
        target.center.y,
        m_GreenPlatformSpeed
    );

    const float deltaY = m_Level2HiddenPlatformCurrentRect.center.y - oldPlatform.center.y;
    m_SolidBlocks[m_Level2HiddenPlatformBlockIndex] = m_Level2HiddenPlatformCurrentRect;
    if (m_Level2HiddenPlatform) {
        m_Level2HiddenPlatform->SetPosition(m_Level2HiddenPlatformCurrentRect.center);
    }

    m_Level2TopButtonLeftPressVisual = MoveToward(
        m_Level2TopButtonLeftPressVisual,
        shown ? m_GreenButtonPressDepth : 0.0f,
        m_GreenButtonAnimSpeed
    );
    m_Level2TopButtonRightPressVisual = MoveToward(
        m_Level2TopButtonRightPressVisual,
        shown ? m_GreenButtonPressDepth : 0.0f,
        m_GreenButtonAnimSpeed
    );
    if (m_Level2TopButtonLeft) {
        m_Level2TopButtonLeft->SetPosition(m_Level2TopButtonLeftBasePosition + glm::vec2{0.0f, -m_Level2TopButtonLeftPressVisual});
    }
    if (m_Level2TopButtonRight) {
        m_Level2TopButtonRight->SetPosition(m_Level2TopButtonRightBasePosition + glm::vec2{0.0f, -m_Level2TopButtonRightPressVisual});
    }

    CarryCharacterWithPlatform(m_Fireboy, m_FireboyCollision, oldPlatform, deltaY);
    CarryCharacterWithPlatform(m_Watergirl, m_WatergirlCollision, oldPlatform, deltaY);
    CarryCubeWithPlatform(oldPlatform, deltaY);
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

        auto slideCharacter = [&](
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

            const float angleMagnitude = std::abs(angle);
            if (angleMagnitude < HANGING_PLATFORM_MIN_SLIDE_ANGLE) {
                return;
            }

            const float downhillDirection = angle < 0.0f ? 1.0f : -1.0f;
            const float slideStrength = std::clamp(
                angleMagnitude / std::max(0.001f, m_Level2HangingPlatformMaxAngle),
                0.0f,
                1.0f
            );
            velocity.x = std::clamp(
                velocity.x + downhillDirection * HANGING_PLATFORM_SLIDE_ACCELERATION * slideStrength,
                -HANGING_PLATFORM_MAX_SLIDE_SPEED,
                HANGING_PLATFORM_MAX_SLIDE_SPEED
            );
            velocity.y = std::min(velocity.y, 0.0f);
            onGround = true;

            glm::vec2 pos = character->GetPosition();
            pos.x += downhillDirection * HANGING_PLATFORM_SLIDE_POSITION_NUDGE * slideStrength;
            const float bottomOffset = pos.y - (bodyCenter.y - bodySize.y * 0.5f);
            pos.y = platformTopYAt(pos.x + profile.bodyHitboxOffset.x, angle) + bottomOffset;
            character->SetPosition(pos);
        };

        slideCharacter(m_Fireboy, m_FireboyCollision, m_FireboyVelocity, m_FireboyOnGround);
        slideCharacter(m_Watergirl, m_WatergirlCollision, m_WatergirlVelocity, m_WatergirlOnGround);
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
