#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_map>

namespace {
std::unordered_map<HeadBodyCharacter*, glm::vec2> s_LastPos;
constexpr float IDLE_EPSILON = 0.5f;
constexpr float AIR_STATE_EPSILON = 0.15f;
constexpr float AIR_RUN_EPSILON = 0.25f;
constexpr float MAX_AIR_HEAD_ROTATION = 0.45f;
constexpr float FIXED_TIME_STEP = 1.0f / 60.0f;
}

void App::Update() {
    if (m_VictoryPhase == VictoryPhase::None) {
        UpdateCubePhysics();

        HandleFireboyInput();
        HandleWatergirlInput();

        UpdateFireboyPhysics();
        UpdateWatergirlPhysics();

        UpdateGreenSwitch();
        UpdateGreenPlatform();
        if (CheckHazards()) {
            m_Root.Update();
            return;
        }
        CheckDiamondCollection();
        UpdateExitDoors();

        if (m_VictoryPhase == VictoryPhase::None) {
            UpdateCharacterMotionState(m_Fireboy, m_FireboyVelocity, m_FireboyOnGround);
            UpdateCharacterMotionState(m_Watergirl, m_WatergirlVelocity, m_WatergirlOnGround);
        } else {
            UpdateVictorySequence();
        }
    } else {
        UpdateExitDoors();
        UpdateVictorySequence();
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    m_Root.Update();
}

void App::HandleFireboyInput() {
    HandleCharacterInput(
        m_Fireboy,
        m_FireboyVelocity,
        m_FireboyOnGround,
        m_FireboyCollision,
        Util::Keycode::A,
        Util::Keycode::D,
        Util::Keycode::W
    );
}

void App::HandleWatergirlInput() {
    HandleCharacterInput(
        m_Watergirl,
        m_WatergirlVelocity,
        m_WatergirlOnGround,
        m_WatergirlCollision,
        Util::Keycode::LEFT,
        Util::Keycode::RIGHT,
        Util::Keycode::UP
    );
}

void App::HandleCharacterInput(
    const std::shared_ptr<HeadBodyCharacter>& character,
    glm::vec2& velocity,
    bool& onGround,
    const CharacterCollisionProfile& profile,
    Util::Keycode leftKey,
    Util::Keycode rightKey,
    Util::Keycode jumpKey
) {
    if (!character) {
        return;
    }

    glm::vec2 oldPos = character->GetPosition();
    glm::vec2 newPos = oldPos;
    float inputDir = 0.0f;

    if (Util::Input::IsKeyPressed(leftKey)) {
        inputDir -= 1.0f;
    }

    if (Util::Input::IsKeyPressed(rightKey)) {
        inputDir += 1.0f;
    }

    if (inputDir < 0.0f) {
        character->SetScale({-1.0f, 1.0f});
    } else if (inputDir > 0.0f) {
        character->SetScale({1.0f, 1.0f});
    }

    const float acceleration = onGround ? m_GroundAcceleration : m_AirAcceleration;
    const float deceleration = onGround ? m_GroundDeceleration : m_AirDeceleration;

    if (inputDir != 0.0f) {
        const float targetSpeed = inputDir * m_MoveSpeed;
        if (velocity.x < targetSpeed) {
            velocity.x = std::min(targetSpeed, velocity.x + acceleration);
        } else if (velocity.x > targetSpeed) {
            velocity.x = std::max(targetSpeed, velocity.x - acceleration);
        }
    } else if (velocity.x > 0.0f) {
        velocity.x = std::max(0.0f, velocity.x - deceleration);
    } else if (velocity.x < 0.0f) {
        velocity.x = std::min(0.0f, velocity.x + deceleration);
    }

    if (std::abs(velocity.x) < 0.01f) {
        velocity.x = 0.0f;
    }

    newPos.x += velocity.x;

    // Keep the player glued to slope surfaces while walking so the terrain
    // behaves like a real ramp instead of a staircase of tiny rectangles.
    if (newPos.x != oldPos.x && velocity.y <= 0.0f) {
        ApplySlopeFollow(oldPos, newPos, profile);
    }

    const float attemptedX = newPos.x;
    ResolveHorizontalCollisions(oldPos, newPos, profile);
    if (std::abs(newPos.x - attemptedX) > 0.001f) {
        velocity.x = 0.0f;
    }
    if (newPos.x != oldPos.x && velocity.y <= 0.0f) {
        ApplySlopeFollow(oldPos, newPos, profile);
    }
    character->SetPosition(newPos);

    if (Util::Input::IsKeyPressed(jumpKey) && onGround) {
        velocity.y = m_JumpSpeed;
        onGround = false;
    }
}

void App::UpdateFireboyPhysics() {
    UpdateCharacterPhysics(m_Fireboy, m_FireboyVelocity, m_FireboyOnGround, m_FireboyCollision);
}

void App::UpdateWatergirlPhysics() {
    UpdateCharacterPhysics(m_Watergirl, m_WatergirlVelocity, m_WatergirlOnGround, m_WatergirlCollision);
}

void App::UpdateCharacterPhysics(
    const std::shared_ptr<HeadBodyCharacter>& character,
    glm::vec2& velocity,
    bool& onGround,
    const CharacterCollisionProfile& profile
) {
    if (!character) {
        return;
    }

    glm::vec2 oldPos = character->GetPosition();
    glm::vec2 newPos = oldPos;

    velocity.y -= m_Gravity;
    newPos.y += velocity.y;

    ResolveVerticalCollisions(oldPos, newPos, profile, velocity, onGround);
    character->SetPosition(newPos);
}

void App::UpdateCharacterMotionState(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const glm::vec2& velocity,
    bool onGround
) {
    if (!character) {
        return;
    }

    const glm::vec2 pos = character->GetPosition();
    const auto prevIter = s_LastPos.find(character.get());
    const glm::vec2 prev = (prevIter == s_LastPos.end()) ? pos : prevIter->second;
    const float moved = glm::length(pos - prev);

    if (!onGround) {
        if (std::abs(velocity.x) > AIR_RUN_EPSILON) {
            character->SetMotionState(HeadBodyCharacter::MotionState::Move);
        } else if (velocity.y > AIR_STATE_EPSILON) {
            character->SetMotionState(HeadBodyCharacter::MotionState::Jump);
        } else {
            character->SetMotionState(HeadBodyCharacter::MotionState::Fall);
        }
    } else if (moved < IDLE_EPSILON) {
        character->SetMotionState(HeadBodyCharacter::MotionState::Idle);
    } else {
        character->SetMotionState(HeadBodyCharacter::MotionState::Move);
    }

    character->SetAirborneRunHeadOffsetEnabled(
        !onGround &&
        std::abs(velocity.x) > AIR_RUN_EPSILON &&
        velocity.y > AIR_STATE_EPSILON
    );

    if (!onGround && std::abs(velocity.x) > AIR_RUN_EPSILON) {
        const float facingSign = velocity.x < 0.0f ? -1.0f : 1.0f;
        const float headAngle = std::clamp(
            std::atan2(velocity.y, std::abs(velocity.x)) * 0.5f * facingSign,
            -MAX_AIR_HEAD_ROTATION,
            MAX_AIR_HEAD_ROTATION
        );
        character->SetHeadRotation(headAngle);
    } else {
        character->SetHeadRotation(0.0f);
    }

    s_LastPos[character.get()] = pos;
}

void App::UpdateExitDoors() {
    auto updateDoorSprite = [&](ExitDoor& door) {
        if (!door.sprite) {
            return;
        }

        const float maxProgress =
            static_cast<float>(door.openingImagePaths.size()) * m_DoorFrameDuration;
        if (door.occupied) {
            door.openProgress = std::min(maxProgress, door.openProgress + FIXED_TIME_STEP);
        } else {
            door.openProgress = std::max(0.0f, door.openProgress - FIXED_TIME_STEP);
        }

        if (door.openProgress <= 0.0f || door.openingImagePaths.empty()) {
            door.sprite->SetImage(door.closedImagePath);
            return;
        }

        const int frameIndex = std::clamp(
            static_cast<int>((door.openProgress - 0.0001f) / m_DoorFrameDuration),
            0,
            static_cast<int>(door.openingImagePaths.size()) - 1
        );
        door.sprite->SetImage(door.openingImagePaths[frameIndex]);
    };

    if (m_VictoryPhase == VictoryPhase::None) {
        m_FireboyDoor.occupied = IsCharacterAtDoor(m_Fireboy, m_FireboyCollision, m_FireboyDoor);
        m_WatergirlDoor.occupied = IsCharacterAtDoor(m_Watergirl, m_WatergirlCollision, m_WatergirlDoor);
    } else {
        m_FireboyDoor.occupied = true;
        m_WatergirlDoor.occupied = true;
    }

    updateDoorSprite(m_FireboyDoor);
    updateDoorSprite(m_WatergirlDoor);

    if (m_VictoryPhase == VictoryPhase::None &&
        m_FireboyDoor.occupied &&
        m_WatergirlDoor.occupied) {
        m_VictoryPhase = VictoryPhase::RunIntoDoor;
        m_VictoryTimer = 0.0f;
        m_FireboyVelocity = {0.0f, 0.0f};
        m_WatergirlVelocity = {0.0f, 0.0f};
    }
}

void App::UpdateVictorySequence() {
    auto zeroAirVisuals = [](const std::shared_ptr<HeadBodyCharacter>& character) {
        if (!character) {
            return;
        }

        character->SetHeadRotation(0.0f);
        character->SetAirborneRunHeadOffsetEnabled(false);
    };

    if (m_VictoryPhase == VictoryPhase::RunIntoDoor) {
        m_VictoryTimer += FIXED_TIME_STEP;

        auto runIntoDoor = [&](const std::shared_ptr<HeadBodyCharacter>& character,
                               glm::vec2& velocity,
                               bool& onGround) {
            if (!character) {
                return;
            }

            velocity = {0.0f, 0.0f};
            onGround = true;
            zeroAirVisuals(character);
            character->SetScale({1.0f, 1.0f});
            character->SetMotionState(HeadBodyCharacter::MotionState::Move);

            glm::vec2 pos = character->GetPosition();
            pos.x += m_MoveSpeed * 0.70f;
            character->SetPosition(pos);
        };

        runIntoDoor(m_Fireboy, m_FireboyVelocity, m_FireboyOnGround);
        runIntoDoor(m_Watergirl, m_WatergirlVelocity, m_WatergirlOnGround);

        if (m_VictoryTimer >= m_VictoryRunDuration) {
            m_VictoryPhase = VictoryPhase::Celebrate;
            m_FireboyVelocity = {0.0f, 0.0f};
            m_WatergirlVelocity = {0.0f, 0.0f};
            zeroAirVisuals(m_Fireboy);
            zeroAirVisuals(m_Watergirl);
            if (m_Fireboy) {
                m_Fireboy->SetMotionState(HeadBodyCharacter::MotionState::Win);
            }
            if (m_Watergirl) {
                m_Watergirl->SetMotionState(HeadBodyCharacter::MotionState::Win);
            }
        }
        return;
    }

    if (m_VictoryPhase == VictoryPhase::Celebrate) {
        m_FireboyVelocity = {0.0f, 0.0f};
        m_WatergirlVelocity = {0.0f, 0.0f};
        zeroAirVisuals(m_Fireboy);
        zeroAirVisuals(m_Watergirl);
        if (m_Fireboy) {
            m_Fireboy->SetMotionState(HeadBodyCharacter::MotionState::Win);
        }
        if (m_Watergirl) {
            m_Watergirl->SetMotionState(HeadBodyCharacter::MotionState::Win);
        }
    }
}

bool App::CheckHazards() {
    auto isFatal = [](HazardRect::Type type, bool isFireboy) {
        switch (type) {
        case HazardRect::Type::Lava:
            return !isFireboy;
        case HazardRect::Type::Water:
            return isFireboy;
        }

        return false;
    };

    for (const auto& hazard : m_Hazards) {
        if (isFatal(hazard.type, true) &&
            CharacterTouchesHazard(m_Fireboy, m_FireboyCollision, hazard)) {
            ResetLevelState();
            return true;
        }

        if (isFatal(hazard.type, false) &&
            CharacterTouchesHazard(m_Watergirl, m_WatergirlCollision, hazard)) {
            ResetLevelState();
            return true;
        }
    }

    return false;
}

void App::ResetLevelState() {
    m_VictoryPhase = VictoryPhase::None;
    m_VictoryTimer = 0.0f;

    m_FireboyVelocity = {0.0f, 0.0f};
    m_WatergirlVelocity = {0.0f, 0.0f};
    m_CubeVelocity = {0.0f, 0.0f};
    m_FireboyOnGround = true;
    m_WatergirlOnGround = true;
    m_CubeOnGround = true;

    if (m_Fireboy) {
        m_Fireboy->SetAirborneRunHeadOffsetEnabled(false);
        m_Fireboy->SetHeadRotation(0.0f);
        m_Fireboy->SetScale({1.0f, 1.0f});
        m_Fireboy->SetMotionState(HeadBodyCharacter::MotionState::Idle);
        m_Fireboy->SetPosition(m_FireboySpawnPosition);
        s_LastPos[m_Fireboy.get()] = m_FireboySpawnPosition;
    }

    if (m_Watergirl) {
        m_Watergirl->SetAirborneRunHeadOffsetEnabled(false);
        m_Watergirl->SetHeadRotation(0.0f);
        m_Watergirl->SetScale({1.0f, 1.0f});
        m_Watergirl->SetMotionState(HeadBodyCharacter::MotionState::Idle);
        m_Watergirl->SetPosition(m_WatergirlSpawnPosition);
        s_LastPos[m_Watergirl.get()] = m_WatergirlSpawnPosition;
    }

    m_GreenButtonPressed = false;
    m_GreenSwitchOn = false;
    m_GreenSwitchTouchLatch = false;
    if (m_GreenSwitch) {
        m_GreenSwitch->SetImage(std::string(GA_RESOURCE_DIR) + "/Image/Assets/switch_green_off.png");
    }

    if (m_HasGreenPlatformBlock && m_GreenPlatformBlockIndex < m_SolidBlocks.size()) {
        m_GreenPlatformCurrentRect = m_GreenPlatformRestRect;
        m_SolidBlocks[m_GreenPlatformBlockIndex] = m_GreenPlatformCurrentRect;
        if (m_GreenPlatform) {
            m_GreenPlatform->SetPosition(m_GreenPlatformCurrentRect.center);
        }
    }

    m_CubeRect = m_CubeSpawnRect;
    if (m_HasCubeBlock && m_CubeBlockIndex < m_SolidBlocks.size()) {
        m_SolidBlocks[m_CubeBlockIndex] = m_CubeRect;
    }
    if (m_Cube) {
        m_Cube->SetPosition(m_CubeSpawnPosition);
    }

    m_FireboyDoor.occupied = false;
    m_WatergirlDoor.occupied = false;
    m_FireboyDoor.openProgress = 0.0f;
    m_WatergirlDoor.openProgress = 0.0f;
    if (m_FireboyDoor.sprite) {
        m_FireboyDoor.sprite->SetImage(m_FireboyDoor.closedImagePath);
    }
    if (m_WatergirlDoor.sprite) {
        m_WatergirlDoor.sprite->SetImage(m_WatergirlDoor.closedImagePath);
    }

    m_FireDiamondsCollected = 0;
    m_WaterDiamondsCollected = 0;
    m_GreenDiamondsCollected = 0;
    for (auto& diamond : m_Diamonds) {
        diamond.collected = false;
        if (diamond.sprite) {
            diamond.sprite->SetVisible(true);
        }
    }
}

void App::UpdateGreenSwitch() {
    if (!m_GreenSwitch) {
        return;
    }

    const bool touched = IsGreenSwitchTouched();
    if (touched && !m_GreenSwitchTouchLatch) {
        m_GreenSwitchOn = !m_GreenSwitchOn;
        m_GreenSwitch->SetImage(
            std::string(GA_RESOURCE_DIR) +
            (m_GreenSwitchOn
                ? "/Image/Assets/switch_green_on.png"
                : "/Image/Assets/switch_green_off.png")
        );
    }

    m_GreenSwitchTouchLatch = touched;
}

void App::CheckDiamondCollection() {
    if (m_Diamonds.empty()) {
        return;
    }

    auto touchesDiamond = [&](const std::shared_ptr<HeadBodyCharacter>& character,
                              const CharacterCollisionProfile& profile,
                              const CollectibleDiamond& diamond) {
        if (!character) {
            return false;
        }

        glm::vec2 bodyCenter = {0.0f, 0.0f};
        glm::vec2 bodySize = {0.0f, 0.0f};
        glm::vec2 headCenter = {0.0f, 0.0f};
        glm::vec2 headSize = {0.0f, 0.0f};
        GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);
        GetCharacterHeadBox(character->GetPosition(), profile, headCenter, headSize);

        const glm::vec2 diamondHitboxSize = {
            std::max(1.0f, diamond.sprite->GetSize().x * m_DiamondHitboxScale.x),
            std::max(1.0f, diamond.sprite->GetSize().y * m_DiamondHitboxScale.y),
        };

        return CheckAABB(bodyCenter, bodySize, diamond.sprite->GetPosition(), diamondHitboxSize) ||
               CheckAABB(headCenter, headSize, diamond.sprite->GetPosition(), diamondHitboxSize);
    };

    auto collectDiamond = [&](CollectibleDiamond& diamond) {
        diamond.collected = true;
        diamond.sprite->SetVisible(false);

        switch (diamond.type) {
        case DiamondType::Fire:
            ++m_FireDiamondsCollected;
            break;
        case DiamondType::Water:
            ++m_WaterDiamondsCollected;
            break;
        case DiamondType::Neutral:
            ++m_GreenDiamondsCollected;
            break;
        }
    };

    for (auto& diamond : m_Diamonds) {
        if (!diamond.sprite || diamond.collected || !diamond.sprite->GetVisibility()) {
            continue;
        }

        const bool fireboyCanCollect =
            diamond.type == DiamondType::Fire || diamond.type == DiamondType::Neutral;
        const bool watergirlCanCollect =
            diamond.type == DiamondType::Water || diamond.type == DiamondType::Neutral;

        if (fireboyCanCollect && touchesDiamond(m_Fireboy, m_FireboyCollision, diamond)) {
            collectDiamond(diamond);
            continue;
        }

        if (watergirlCanCollect && touchesDiamond(m_Watergirl, m_WatergirlCollision, diamond)) {
            collectDiamond(diamond);
        }
    }
}

void App::UpdateCubePhysics() {
    if (!m_Cube || !m_HasCubeBlock || m_CubeBlockIndex >= m_SolidBlocks.size()) {
        return;
    }

    const SolidRect oldCube = m_CubeRect;

    auto getPushIntent = [&]() {
        float pushIntent = 0.0f;

        auto tryCharacterPush = [&](const std::shared_ptr<HeadBodyCharacter>& character,
                                    const CharacterCollisionProfile& profile,
                                    bool onGround,
                                    Util::Keycode leftKey,
                                    Util::Keycode rightKey) {
            if (!character || !onGround) {
                return;
            }

            float inputDir = 0.0f;
            if (Util::Input::IsKeyPressed(leftKey)) {
                inputDir -= 1.0f;
            }
            if (Util::Input::IsKeyPressed(rightKey)) {
                inputDir += 1.0f;
            }
            if (inputDir == 0.0f) {
                return;
            }

            glm::vec2 bodyCenter = {0.0f, 0.0f};
            glm::vec2 bodySize = {0.0f, 0.0f};
            GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);

            const float bodyLeft = bodyCenter.x - bodySize.x * 0.5f;
            const float bodyRight = bodyCenter.x + bodySize.x * 0.5f;
            const float bodyBottom = bodyCenter.y - bodySize.y * 0.5f;
            const float bodyTop = bodyCenter.y + bodySize.y * 0.5f;
            const float cubeLeft = oldCube.center.x - oldCube.size.x * 0.5f;
            const float cubeRight = oldCube.center.x + oldCube.size.x * 0.5f;
            const float cubeBottom = oldCube.center.y - oldCube.size.y * 0.5f;
            const float cubeTop = oldCube.center.y + oldCube.size.y * 0.5f;

            const bool verticalOverlap =
                bodyTop > cubeBottom + 2.0f &&
                bodyBottom < cubeTop - 2.0f;

            if (!verticalOverlap) {
                return;
            }

            const float pushTolerance = 10.0f;
            if (inputDir > 0.0f &&
                bodyRight >= cubeLeft - pushTolerance &&
                bodyRight <= cubeLeft + pushTolerance) {
                pushIntent += 1.0f;
            }

            if (inputDir < 0.0f &&
                bodyLeft <= cubeRight + pushTolerance &&
                bodyLeft >= cubeRight - pushTolerance) {
                pushIntent -= 1.0f;
            }
        };

        tryCharacterPush(
            m_Fireboy,
            m_FireboyCollision,
            m_FireboyOnGround,
            Util::Keycode::A,
            Util::Keycode::D
        );
        tryCharacterPush(
            m_Watergirl,
            m_WatergirlCollision,
            m_WatergirlOnGround,
            Util::Keycode::LEFT,
            Util::Keycode::RIGHT
        );

        return std::clamp(pushIntent, -2.0f, 2.0f);
    };

    const float pushIntent = getPushIntent();
    const float targetSpeed = pushIntent * m_CubePushSpeed;
    const float pushAcceleration = m_CubePushAcceleration * std::max(1.0f, std::abs(pushIntent));
    if (pushIntent != 0.0f) {
        if (m_CubeVelocity.x < targetSpeed) {
            m_CubeVelocity.x = std::min(targetSpeed, m_CubeVelocity.x + pushAcceleration);
        } else if (m_CubeVelocity.x > targetSpeed) {
            m_CubeVelocity.x = std::max(targetSpeed, m_CubeVelocity.x - pushAcceleration);
        }
    } else if (m_CubeVelocity.x > 0.0f) {
        m_CubeVelocity.x = std::max(0.0f, m_CubeVelocity.x - m_CubeDeceleration);
    } else if (m_CubeVelocity.x < 0.0f) {
        m_CubeVelocity.x = std::min(0.0f, m_CubeVelocity.x + m_CubeDeceleration);
    }

    if (std::abs(m_CubeVelocity.x) < 0.01f) {
        m_CubeVelocity.x = 0.0f;
    }

    m_CubeRect.center.x += m_CubeVelocity.x;
    for (std::size_t i = 0; i < m_SolidBlocks.size(); ++i) {
        if (i == m_CubeBlockIndex) {
            continue;
        }

        const auto& block = m_SolidBlocks[i];
        if (!CheckAABB(m_CubeRect.center, m_CubeRect.size, block.center, block.size)) {
            continue;
        }

        const float oldLeft = oldCube.center.x - oldCube.size.x * 0.5f;
        const float oldRight = oldCube.center.x + oldCube.size.x * 0.5f;
        const float newLeft = m_CubeRect.center.x - m_CubeRect.size.x * 0.5f;
        const float newRight = m_CubeRect.center.x + m_CubeRect.size.x * 0.5f;
        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;

        if (m_CubeVelocity.x > 0.0f && oldRight <= blockLeft && newRight >= blockLeft) {
            m_CubeRect.center.x = blockLeft - m_CubeRect.size.x * 0.5f;
            m_CubeVelocity.x = 0.0f;
        } else if (m_CubeVelocity.x < 0.0f && oldLeft >= blockRight && newLeft <= blockRight) {
            m_CubeRect.center.x = blockRight + m_CubeRect.size.x * 0.5f;
            m_CubeVelocity.x = 0.0f;
        }
    }

    const SolidRect cubeAfterHorizontal = m_CubeRect;
    m_CubeVelocity.y -= m_Gravity;
    m_CubeRect.center.y += m_CubeVelocity.y;
    m_CubeOnGround = false;

    bool foundGround = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();
    for (std::size_t i = 0; i < m_SolidBlocks.size(); ++i) {
        if (i == m_CubeBlockIndex) {
            continue;
        }

        const auto& block = m_SolidBlocks[i];
        if (!CheckAABB(m_CubeRect.center, m_CubeRect.size, block.center, block.size)) {
            continue;
        }

        const float oldBottom = cubeAfterHorizontal.center.y - cubeAfterHorizontal.size.y * 0.5f;
        const float oldTop = cubeAfterHorizontal.center.y + cubeAfterHorizontal.size.y * 0.5f;
        const float newBottom = m_CubeRect.center.y - m_CubeRect.size.y * 0.5f;
        const float newTop = m_CubeRect.center.y + m_CubeRect.size.y * 0.5f;
        const float blockTop = block.center.y + block.size.y * 0.5f;
        const float blockBottom = block.center.y - block.size.y * 0.5f;

        if (m_CubeVelocity.y <= 0.0f &&
            oldBottom >= blockTop - m_GroundSnapTolerance &&
            newBottom <= blockTop + m_GroundSnapTolerance) {
            bestGroundY = std::max(bestGroundY, blockTop);
            foundGround = true;
        } else if (m_CubeVelocity.y > 0.0f &&
                   oldTop <= blockBottom + m_GroundSnapTolerance &&
                   newTop >= blockBottom - m_GroundSnapTolerance) {
            m_CubeRect.center.y = blockBottom - m_CubeRect.size.y * 0.5f;
            m_CubeVelocity.y = 0.0f;
        }
    }

    if (foundGround) {
        m_CubeRect.center.y = bestGroundY + m_CubeRect.size.y * 0.5f;
        m_CubeVelocity.y = 0.0f;
        m_CubeOnGround = true;
    }

    const glm::vec2 cubeDelta = m_CubeRect.center - oldCube.center;
    m_SolidBlocks[m_CubeBlockIndex] = m_CubeRect;
    m_Cube->SetPosition(m_CubeRect.center);

    if (std::abs(cubeDelta.x) > 0.001f || std::abs(cubeDelta.y) > 0.001f) {
        CarryCharacterWithCube(m_Fireboy, m_FireboyCollision, oldCube, cubeDelta);
        CarryCharacterWithCube(m_Watergirl, m_WatergirlCollision, oldCube, cubeDelta);
    }
}

void App::UpdateGreenPlatform() {
    if (!m_GreenPlatform || !m_HasGreenPlatformBlock || m_GreenPlatformBlockIndex >= m_SolidBlocks.size()) {
        return;
    }

    m_GreenButtonPressed = IsGreenButtonPressed();

    const SolidRect oldPlatform = m_GreenPlatformCurrentRect;
    const float targetY = m_GreenButtonPressed
        ? m_GreenPlatformPressedRect.center.y
        : m_GreenPlatformRestRect.center.y;
    const float deltaToTarget = targetY - m_GreenPlatformCurrentRect.center.y;
    const float platformDeltaY = std::abs(deltaToTarget) <= m_GreenPlatformSpeed
        ? deltaToTarget
        : std::copysign(m_GreenPlatformSpeed, deltaToTarget);

    if (std::abs(platformDeltaY) < 0.001f) {
        return;
    }

    m_GreenPlatformCurrentRect.center.y += platformDeltaY;
    m_SolidBlocks[m_GreenPlatformBlockIndex] = m_GreenPlatformCurrentRect;
    m_GreenPlatform->SetPosition(m_GreenPlatformCurrentRect.center);

    CarryCharacterWithPlatform(m_Fireboy, m_FireboyCollision, oldPlatform, platformDeltaY);
    CarryCharacterWithPlatform(m_Watergirl, m_WatergirlCollision, oldPlatform, platformDeltaY);
    CarryCubeWithPlatform(oldPlatform, platformDeltaY);
}

bool App::IsGreenButtonPressed() const {
    return CharacterTouchesRect(m_Fireboy, m_FireboyCollision, m_GreenButtonHitbox) ||
           CharacterTouchesRect(m_Watergirl, m_WatergirlCollision, m_GreenButtonHitbox) ||
           CharacterTouchesRect(m_Fireboy, m_FireboyCollision, m_GreenButtonAfterHitbox) ||
           CharacterTouchesRect(m_Watergirl, m_WatergirlCollision, m_GreenButtonAfterHitbox) ||
           CubeTouchesRect(m_GreenButtonHitbox) ||
           CubeTouchesRect(m_GreenButtonAfterHitbox);
}

bool App::IsGreenSwitchTouched() const {
    return CharacterTouchesRect(m_Fireboy, m_FireboyCollision, m_GreenSwitchHitbox) ||
           CharacterTouchesRect(m_Watergirl, m_WatergirlCollision, m_GreenSwitchHitbox) ||
           CubeTouchesRect(m_GreenSwitchHitbox);
}

bool App::CubeTouchesRect(const SolidRect& rect) const {
    return m_Cube &&
           m_HasCubeBlock &&
           CheckAABB(m_CubeRect.center, m_CubeRect.size, rect.center, rect.size);
}

bool App::IsCharacterAtDoor(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const ExitDoor& door
) const {
    if (!character || !door.sprite) {
        return false;
    }

    return CharacterTouchesRect(character, profile, door.triggerRect);
}

bool App::CharacterTouchesRect(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const SolidRect& rect
) const {
    if (!character) {
        return false;
    }

    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);
    GetCharacterHeadBox(character->GetPosition(), profile, headCenter, headSize);

    return CheckAABB(bodyCenter, bodySize, rect.center, rect.size) ||
           CheckAABB(headCenter, headSize, rect.center, rect.size);
}

bool App::CharacterTouchesHazard(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const HazardRect& hazard
) const {
    if (!character) {
        return false;
    }

    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);
    GetCharacterHeadBox(character->GetPosition(), profile, headCenter, headSize);

    return CheckAABB(bodyCenter, bodySize, hazard.center, hazard.size) ||
           CheckAABB(headCenter, headSize, hazard.center, hazard.size);
}

void App::CarryCharacterWithPlatform(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const SolidRect& oldPlatform,
    float platformDeltaY
) const {
    if (!character || std::abs(platformDeltaY) < 0.001f) {
        return;
    }

    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);

    const float bodyLeft = bodyCenter.x - bodySize.x * 0.5f;
    const float bodyRight = bodyCenter.x + bodySize.x * 0.5f;
    const float feetY = bodyCenter.y - bodySize.y * 0.5f;
    const float platformLeft = oldPlatform.center.x - oldPlatform.size.x * 0.5f;
    const float platformRight = oldPlatform.center.x + oldPlatform.size.x * 0.5f;
    const float platformTop = oldPlatform.center.y + oldPlatform.size.y * 0.5f;

    const bool horizontallyOverlaps =
        bodyRight > platformLeft + m_FootProbeInset &&
        bodyLeft < platformRight - m_FootProbeInset;
    const bool standingOnPlatform =
        feetY >= platformTop - m_GroundStickTolerance &&
        feetY <= platformTop + m_GroundSnapTolerance;

    if (!horizontallyOverlaps || !standingOnPlatform) {
        return;
    }

    glm::vec2 pos = character->GetPosition();
    pos.y += platformDeltaY;
    character->SetPosition(pos);
}

void App::CarryCharacterWithCube(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const CharacterCollisionProfile& profile,
    const SolidRect& oldCube,
    const glm::vec2& cubeDelta
) const {
    if (!character || (std::abs(cubeDelta.x) < 0.001f && std::abs(cubeDelta.y) < 0.001f)) {
        return;
    }

    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    GetCharacterBodyBox(character->GetPosition(), profile, bodyCenter, bodySize);

    const float bodyLeft = bodyCenter.x - bodySize.x * 0.5f;
    const float bodyRight = bodyCenter.x + bodySize.x * 0.5f;
    const float bodyTop = bodyCenter.y + bodySize.y * 0.5f;
    const float feetY = bodyCenter.y - bodySize.y * 0.5f;
    const float bodyBottom = feetY;
    const float cubeLeft = oldCube.center.x - oldCube.size.x * 0.5f;
    const float cubeRight = oldCube.center.x + oldCube.size.x * 0.5f;
    const float cubeBottom = oldCube.center.y - oldCube.size.y * 0.5f;
    const float cubeTop = oldCube.center.y + oldCube.size.y * 0.5f;

    const bool horizontallyOverlaps =
        bodyRight > cubeLeft + m_FootProbeInset &&
        bodyLeft < cubeRight - m_FootProbeInset;
    const bool standingOnCube =
        feetY >= cubeTop - m_GroundStickTolerance &&
        feetY <= cubeTop + m_GroundSnapTolerance;
    const bool verticalOverlap =
        bodyTop > cubeBottom + 2.0f &&
        bodyBottom < cubeTop - 2.0f;
    const float sideTolerance = 8.0f;
    const bool pushedByCubeFromLeft =
        cubeDelta.x > 0.0f &&
        verticalOverlap &&
        bodyLeft >= cubeRight - sideTolerance &&
        bodyLeft <= cubeRight + sideTolerance;
    const bool pushedByCubeFromRight =
        cubeDelta.x < 0.0f &&
        verticalOverlap &&
        bodyRight <= cubeLeft + sideTolerance &&
        bodyRight >= cubeLeft - sideTolerance;

    if ((!horizontallyOverlaps || !standingOnCube) &&
        !pushedByCubeFromLeft &&
        !pushedByCubeFromRight) {
        return;
    }

    glm::vec2 pos = character->GetPosition();
    if (standingOnCube) {
        pos += cubeDelta;
    } else {
        pos.x += cubeDelta.x;
    }
    character->SetPosition(pos);
}

void App::CarryCubeWithPlatform(const SolidRect& oldPlatform, float platformDeltaY) {
    if (!m_Cube || !m_HasCubeBlock || std::abs(platformDeltaY) < 0.001f) {
        return;
    }

    const float cubeLeft = m_CubeRect.center.x - m_CubeRect.size.x * 0.5f;
    const float cubeRight = m_CubeRect.center.x + m_CubeRect.size.x * 0.5f;
    const float cubeBottom = m_CubeRect.center.y - m_CubeRect.size.y * 0.5f;
    const float platformLeft = oldPlatform.center.x - oldPlatform.size.x * 0.5f;
    const float platformRight = oldPlatform.center.x + oldPlatform.size.x * 0.5f;
    const float platformTop = oldPlatform.center.y + oldPlatform.size.y * 0.5f;

    const bool horizontallyOverlaps =
        cubeRight > platformLeft + m_FootProbeInset &&
        cubeLeft < platformRight - m_FootProbeInset;
    const bool standingOnPlatform =
        cubeBottom >= platformTop - m_GroundStickTolerance &&
        cubeBottom <= platformTop + m_GroundSnapTolerance;

    if (!horizontallyOverlaps || !standingOnPlatform) {
        return;
    }

    const SolidRect oldCube = m_CubeRect;
    m_CubeRect.center.y += platformDeltaY;
    m_SolidBlocks[m_CubeBlockIndex] = m_CubeRect;
    m_Cube->SetPosition(m_CubeRect.center);

    const glm::vec2 cubeDelta = m_CubeRect.center - oldCube.center;
    CarryCharacterWithCube(m_Fireboy, m_FireboyCollision, oldCube, cubeDelta);
    CarryCharacterWithCube(m_Watergirl, m_WatergirlCollision, oldCube, cubeDelta);
}

void App::ResolveHorizontalCollisions(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const CharacterCollisionProfile& profile
) {
    glm::vec2 oldBodyCenter = {0.0f, 0.0f};
    glm::vec2 oldBodySize = {0.0f, 0.0f};
    GetCharacterBodyBox(oldPos, profile, oldBodyCenter, oldBodySize);
    const float oldBottom = oldBodyCenter.y - oldBodySize.y * 0.5f;

    // Treat neighboring flat tops and slopes as one connected walkable surface
    // during horizontal movement, so uphill handoffs behave like terrain
    // transitions instead of tiny vertical walls.
    for (const auto& block : m_SolidBlocks) {
        if (!CheckCharacterCollision(newPos, block, profile)) {
            continue;
        }

        const float oldRight = GetCharacterRightEdge(oldPos, profile);
        const float oldLeft = GetCharacterLeftEdge(oldPos, profile);
        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;
        const float blockTop = block.center.y + block.size.y * 0.5f;

        auto canStepUp = [&](float targetTopY) {
            const float raisedY =
                targetTopY - profile.bodyHitboxOffset.y + profile.bodyHitboxSize.y * 0.5f + 0.1f;
            const float liftAmount = raisedY - oldPos.y;
            if (liftAmount <= 0.0f || liftAmount > m_StepUpHeight) {
                return false;
            }

            const glm::vec2 candidatePos = {newPos.x, raisedY};
            glm::vec2 candidateBodyCenter = {0.0f, 0.0f};
            glm::vec2 candidateBodySize = {0.0f, 0.0f};
            GetCharacterBodyBox(candidatePos, profile, candidateBodyCenter, candidateBodySize);
            const float candidateBottom = candidateBodyCenter.y - candidateBodySize.y * 0.5f;

            for (const auto& other : m_SolidBlocks) {
                if (&other == &block) {
                    continue;
                }

                if (!CheckCharacterCollision(candidatePos, other, profile)) {
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
                targetGroundY - profile.bodyHitboxOffset.y +
                    profile.bodyHitboxSize.y * 0.5f + 0.1f,
            };
            glm::vec2 candidateBodyCenter = {0.0f, 0.0f};
            glm::vec2 candidateBodySize = {0.0f, 0.0f};
            glm::vec2 candidateHeadCenter = {0.0f, 0.0f};
            glm::vec2 candidateHeadSize = {0.0f, 0.0f};
            GetCharacterBodyBox(candidatePos, profile, candidateBodyCenter, candidateBodySize);
            GetCharacterHeadBox(candidatePos, profile, candidateHeadCenter, candidateHeadSize);
            const float candidateBottom = candidateBodyCenter.y - candidateBodySize.y * 0.5f;

            for (const auto& other : m_SolidBlocks) {
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

        if (newPos.x > oldPos.x && oldRight <= blockLeft) {
            if (!canStepUp(blockTop) &&
                !canStepToNearbyGround() &&
                !TrySnapToSlopeTopTransition(oldPos, newPos, block, profile)) {
                const float rightExtent = GetCharacterRightEdge({0.0f, 0.0f}, profile);
                newPos.x = blockLeft - rightExtent;
            }
        }

        if (newPos.x < oldPos.x && oldLeft >= blockRight) {
            if (!canStepUp(blockTop) &&
                !canStepToNearbyGround() &&
                !TrySnapToSlopeTopTransition(oldPos, newPos, block, profile)) {
                const float leftExtent = -GetCharacterLeftEdge({0.0f, 0.0f}, profile);
                newPos.x = blockRight + leftExtent;
            }
        }
    }
}

void App::ResolveVerticalCollisions(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const CharacterCollisionProfile& profile,
    glm::vec2& velocity,
    bool& onGround
) {
    onGround = false;

    bool foundGround = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();
    bool hitCeiling = false;

    glm::vec2 oldBodyCenter = {0.0f, 0.0f};
    glm::vec2 oldBodySize = {0.0f, 0.0f};
    glm::vec2 newBodyCenter = {0.0f, 0.0f};
    glm::vec2 newBodySize = {0.0f, 0.0f};
    glm::vec2 oldHeadCenter = {0.0f, 0.0f};
    glm::vec2 oldHeadSize = {0.0f, 0.0f};
    GetCharacterBodyBox(oldPos, profile, oldBodyCenter, oldBodySize);
    GetCharacterBodyBox(newPos, profile, newBodyCenter, newBodySize);
    GetCharacterHeadBox(oldPos, profile, oldHeadCenter, oldHeadSize);

    for (const auto& block : m_SolidBlocks) {
        const bool bodyOverlap = CheckAABB(newBodyCenter, newBodySize, block.center, block.size);
        glm::vec2 newHeadCenter = {0.0f, 0.0f};
        glm::vec2 newHeadSize = {0.0f, 0.0f};
        GetCharacterHeadBox(newPos, profile, newHeadCenter, newHeadSize);
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
            velocity.y <= 0.0f &&
            oldBottom >= blockTop - m_GroundSnapTolerance &&
            newBottom <= blockTop + m_GroundSnapTolerance) {
            if (!foundGround || blockTop > bestGroundY) {
                bestGroundY = blockTop;
                foundGround = true;
            }
        } else if (block.blockBottom &&
                   headOverlap &&
                   velocity.y > 0.0f &&
                   oldTop <= blockBottom + m_GroundSnapTolerance &&
                   newTop >= blockBottom - m_GroundSnapTolerance) {
            const float headTopOffset = profile.headHitboxOffset.y + profile.headHitboxSize.y * 0.5f;
            newPos.y = blockBottom - headTopOffset;
            velocity.y = 0.0f;
            hitCeiling = true;
        }
    }

    if (!hitCeiling) {
        hitCeiling = ResolveCeilingSlopeCollision(oldPos, newPos, profile, velocity);
    }

    if (!hitCeiling && velocity.y > 0.0f) {
        float stickyCeilingY = 0.0f;
        if (FindNearbyCeilingY(newPos, m_CeilingStickTolerance, stickyCeilingY, profile)) {
            newPos.y = stickyCeilingY - (profile.headHitboxOffset.y + profile.headHitboxSize.y * 0.5f);
            velocity.y = 0.0f;
            hitCeiling = true;
        }
    }

    if (!hitCeiling && ResolveSlopeGrounding(oldPos, newPos, profile, velocity, onGround)) {
        glm::vec2 groundedBodyCenter = {0.0f, 0.0f};
        glm::vec2 groundedBodySize = {0.0f, 0.0f};
        GetCharacterBodyBox(newPos, profile, groundedBodyCenter, groundedBodySize);
        const float slopeGroundY = groundedBodyCenter.y - groundedBodySize.y * 0.5f;
        if (!foundGround || slopeGroundY > bestGroundY) {
            bestGroundY = slopeGroundY;
            foundGround = true;
        }
    }

    if (!foundGround && velocity.y <= 0.0f) {
        float stickyGroundY = 0.0f;
        if (FindNearbyGroundY(newPos, m_GroundStickTolerance, stickyGroundY, profile)) {
            bestGroundY = stickyGroundY;
            foundGround = true;
        }
    }

    if (foundGround) {
        newPos.y = bestGroundY - profile.bodyHitboxOffset.y + profile.bodyHitboxSize.y * 0.5f;
        velocity.y = 0.0f;
        onGround = true;
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

void App::RecalculateCharacterCollisionBoxes(
    const std::shared_ptr<HeadBodyCharacter>& character,
    CharacterCollisionProfile& profile
) {
    if (!character) {
        return;
    }

    const glm::vec2 bodySize = character->GetBodySize();
    const glm::vec2 headSize = character->GetHeadSize();
    const glm::vec2 headCenterOffset = character->GetHeadCenterOffset();

    profile.bodyHitboxSize = {
        std::max(1.0f, bodySize.x * profile.bodyHitboxScale.x - profile.bodyHitboxPadding.x),
        std::max(1.0f, bodySize.y * profile.bodyHitboxScale.y - profile.bodyHitboxPadding.y),
    };
    profile.bodyHitboxOffset = {0.0f, -2.0f};

    profile.headHitboxSize = {
        std::max(1.0f, headSize.x * profile.headHitboxScale.x - profile.headHitboxPadding.x),
        std::max(1.0f, headSize.y * profile.headHitboxScale.y - profile.headHitboxPadding.y),
    };
    profile.headHitboxOffset = headCenterOffset;
}

void App::RecalculateFireboyCollisionBoxes() {
    RecalculateCharacterCollisionBoxes(m_Fireboy, m_FireboyCollision);
}

void App::RecalculateWatergirlCollisionBoxes() {
    RecalculateCharacterCollisionBoxes(m_Watergirl, m_WatergirlCollision);
}

void App::GetCharacterBodyBox(
    const glm::vec2& bodyPos,
    const CharacterCollisionProfile& profile,
    glm::vec2& outCenter,
    glm::vec2& outSize
) const {
    outCenter = bodyPos + profile.bodyHitboxOffset;
    outSize = profile.bodyHitboxSize;
}

void App::GetCharacterHeadBox(
    const glm::vec2& bodyPos,
    const CharacterCollisionProfile& profile,
    glm::vec2& outCenter,
    glm::vec2& outSize
) const {
    outCenter = bodyPos + profile.headHitboxOffset;
    outSize = profile.headHitboxSize;
}

bool App::CheckCharacterCollision(
    const glm::vec2& bodyPos,
    const SolidRect& block,
    const CharacterCollisionProfile& profile
) const {
    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetCharacterBodyBox(bodyPos, profile, bodyCenter, bodySize);
    GetCharacterHeadBox(bodyPos, profile, headCenter, headSize);
    return CheckAABB(bodyCenter, bodySize, block.center, block.size) ||
           CheckAABB(headCenter, headSize, block.center, block.size);
}

float App::GetCharacterLeftEdge(const glm::vec2& bodyPos, const CharacterCollisionProfile& profile) const {
    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetCharacterBodyBox(bodyPos, profile, bodyCenter, bodySize);
    GetCharacterHeadBox(bodyPos, profile, headCenter, headSize);
    return std::min(
        bodyCenter.x - bodySize.x * 0.5f,
        headCenter.x - headSize.x * 0.5f
    );
}

float App::GetCharacterRightEdge(const glm::vec2& bodyPos, const CharacterCollisionProfile& profile) const {
    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetCharacterBodyBox(bodyPos, profile, bodyCenter, bodySize);
    GetCharacterHeadBox(bodyPos, profile, headCenter, headSize);
    return std::max(
        bodyCenter.x + bodySize.x * 0.5f,
        headCenter.x + headSize.x * 0.5f
    );
}

float App::GetCharacterBodyBottom(const glm::vec2& bodyPos, const CharacterCollisionProfile& profile) const {
    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
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
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
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

    glm::vec2 oldBodyCenter = {0.0f, 0.0f};
    glm::vec2 oldBodySize = {0.0f, 0.0f};
    glm::vec2 newBodyCenter = {0.0f, 0.0f};
    glm::vec2 newBodySize = {0.0f, 0.0f};
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

    newPos.y = bestGroundY - profile.bodyHitboxOffset.y + profile.bodyHitboxSize.y * 0.5f;
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

    glm::vec2 oldHeadCenter = {0.0f, 0.0f};
    glm::vec2 oldHeadSize = {0.0f, 0.0f};
    glm::vec2 newHeadCenter = {0.0f, 0.0f};
    glm::vec2 newHeadSize = {0.0f, 0.0f};
    GetCharacterHeadBox(oldPos, profile, oldHeadCenter, oldHeadSize);
    GetCharacterHeadBox(newPos, profile, newHeadCenter, newHeadSize);

    const float oldTop = oldHeadCenter.y + oldHeadSize.y * 0.5f;
    const float newTop = newHeadCenter.y + newHeadSize.y * 0.5f;
    const float leftX = newHeadCenter.x - newHeadSize.x * 0.5f;
    const float centerX = newHeadCenter.x;
    const float rightX = newHeadCenter.x + newHeadSize.x * 0.5f;

    bool foundCeiling = false;
    float lowestCeilingY = std::numeric_limits<float>::infinity();

    auto accumulateCeilingHit = [&](const auto& slopes) {
        for (const auto& slope : slopes) {
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
    };

    accumulateCeilingHit(m_CeilingSlopes);

    // Floor slopes must also behave like solid terrain boundaries when the
    // character jumps up into them from below, otherwise their endpoints act
    // like hollow gaps that can be exploited near pits and depressions.
    accumulateCeilingHit(m_Slopes);

    if (!foundCeiling) {
        return false;
    }

    newPos.y = lowestCeilingY - (profile.headHitboxOffset.y + profile.headHitboxSize.y * 0.5f);
    velocity.y = 0.0f;
    return true;
}

bool App::TrySnapToSlopeTopTransition(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const SolidRect& blockingBlock,
    const CharacterCollisionProfile& profile
) const {
    if (newPos.x == oldPos.x) {
        return false;
    }

    glm::vec2 oldBodyCenter = {0.0f, 0.0f};
    glm::vec2 oldBodySize = {0.0f, 0.0f};
    glm::vec2 newBodyCenter = {0.0f, 0.0f};
    glm::vec2 newBodySize = {0.0f, 0.0f};
    GetCharacterBodyBox(oldPos, profile, oldBodyCenter, oldBodySize);
    GetCharacterBodyBox(newPos, profile, newBodyCenter, newBodySize);

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

        newPos.y = upper.y - profile.bodyHitboxOffset.y + profile.bodyHitboxSize.y * 0.5f + 0.1f;
        return true;
    }

    return false;
}

bool App::FindBestSlopeYAtX(
    const glm::vec2& oldPos,
    float desiredX,
    float& outSlopeY,
    const CharacterCollisionProfile& profile
) const {
    glm::vec2 oldBodyCenter = {0.0f, 0.0f};
    glm::vec2 oldBodySize = {0.0f, 0.0f};
    GetCharacterBodyBox(oldPos, profile, oldBodyCenter, oldBodySize);

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

void App::ApplySlopeFollow(
    const glm::vec2& oldPos,
    glm::vec2& newPos,
    const CharacterCollisionProfile& profile
) const {
    float bestSlopeY = 0.0f;
    if (!FindBestSlopeYAtX(oldPos, newPos.x, bestSlopeY, profile)) {
        return;
    }

    newPos.y = bestSlopeY - profile.bodyHitboxOffset.y + profile.bodyHitboxSize.y * 0.5f;
}

bool App::FindNearbyGroundY(
    const glm::vec2& pos,
    float maxDistance,
    float& outGroundY,
    const CharacterCollisionProfile& profile
) const {
    glm::vec2 bodyCenter = {0.0f, 0.0f};
    glm::vec2 bodySize = {0.0f, 0.0f};
    GetCharacterBodyBox(pos, profile, bodyCenter, bodySize);

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
    if (FindBestSlopeYAtX(pos, pos.x, slopeGroundY, profile) &&
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

bool App::FindNearbyCeilingY(
    const glm::vec2& pos,
    float maxDistance,
    float& outCeilingY,
    const CharacterCollisionProfile& profile
) const {
    glm::vec2 headCenter = {0.0f, 0.0f};
    glm::vec2 headSize = {0.0f, 0.0f};
    GetCharacterHeadBox(pos, profile, headCenter, headSize);

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
