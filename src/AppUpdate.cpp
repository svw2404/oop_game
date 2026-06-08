#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Time.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <unordered_map>

namespace {
    // These constants tune how animation state is inferred from physics. They are
    // intentionally separate from movement speed so visual state changes remain
    // stable even if gameplay numbers are retuned.
    std::unordered_map<HeadBodyCharacter*, glm::vec2> s_LastPos;
    constexpr float IDLE_EPSILON = 0.5f;
    constexpr float AIR_STATE_EPSILON = 0.15f;
    constexpr float AIR_RUN_EPSILON = 0.25f;
    constexpr float MAX_AIR_HEAD_ROTATION = 0.45f;
    constexpr float FIXED_TIME_STEP = 1.0f / 60.0f;
    constexpr float DESCENDING_PLATFORM_HEAD_CLEARANCE = 2.0f;
}

void App::Update() {
    // Main frame orchestration:
    // 1. Early-out to fail overlay if both characters are already dead
    // 2. Update dynamic world objects first (cube/platform/switch)
    // 3. Process character input + physics
    // 4. Resolve hazards, pickups, doors, and victory
    if (IsFailOverlayVisible()) {
        m_VictoryOverlayVisible = false;
        UpdateVictoryOverlayVisuals();
        UpdateFailOverlay();

        if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
            m_CurrentState = State::END;
        }

        m_Root.Update();
        return;
    }

    if (m_VictoryPhase == VictoryPhase::None) {
        UpdateCubePhysics();

        HandleFireboyInput();
        HandleWatergirlInput();

        UpdateFireboyPhysics();
        UpdateWatergirlPhysics();

        UpdateGreenSwitch();
        UpdateGreenPlatform();
        UpdateGreenPlatform2();
        UpdateLevel2HiddenPlatform();
        UpdateLevel2HangingPlatform();
        UpdateLevel4ChainPlatforms();
        UpdateFans();
        CheckHazards();
        UpdateDeathSequence();

        if (AreBothCharactersDead()) {
            UpdateFailOverlay();
            m_Root.Update();
            return;
        }

        CheckDiamondCollection();
        UpdateExitDoors();

        if (m_VictoryPhase == VictoryPhase::None) {
            UpdateCharacterMotionState(m_Fireboy, m_FireboyVelocity, m_FireboyOnGround);
            UpdateCharacterMotionState(m_Watergirl, m_WatergirlVelocity, m_WatergirlOnGround);
        }
        else {
            UpdateVictorySequence();
        }
    }
    else {
        UpdateExitDoors();
        UpdateVictorySequence();
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    UpdateVictoryOverlayVisuals();
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
        Util::Keycode::W,
        m_FireboyCeilingCarryTimer,
        m_FireboyCeilingCarrySpeed
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
        Util::Keycode::UP,
        m_WatergirlCeilingCarryTimer,
        m_WatergirlCeilingCarrySpeed
    );
}

void App::UpdateFans() {
    if (m_Fans.empty()) {
        return;
    }

    constexpr float FAN_MAX_HORIZONTAL_SPEED = 2.8f;
    constexpr float FAN_MAX_VERTICAL_SPEED = 3.2f;

    auto applyWind = [&](const std::shared_ptr<HeadBodyCharacter>& character,
                         const CharacterCollisionProfile& profile,
                         glm::vec2& velocity) {
        if (!character || !character->IsAlive()) {
            return;
        }

        for (const auto& fan : m_Fans) {
            if (!CharacterTouchesRect(character, profile, fan.windZone)) {
                continue;
            }

            velocity.x = std::clamp(
                velocity.x + fan.windVelocityDelta.x,
                -FAN_MAX_HORIZONTAL_SPEED,
                FAN_MAX_HORIZONTAL_SPEED
            );
            velocity.y = std::clamp(
                velocity.y + fan.windVelocityDelta.y,
                -FAN_MAX_VERTICAL_SPEED,
                FAN_MAX_VERTICAL_SPEED
            );
        }
    };

    applyWind(m_Fireboy, m_FireboyCollision, m_FireboyVelocity);
    applyWind(m_Watergirl, m_WatergirlCollision, m_WatergirlVelocity);
}

void App::HandleCharacterInput(
    const std::shared_ptr<HeadBodyCharacter>& character,
    glm::vec2& velocity,
    bool& onGround,
    const CharacterCollisionProfile& profile,
    Util::Keycode leftKey,
    Util::Keycode rightKey,
    Util::Keycode jumpKey,
    float& ceilingCarryTimer,
    float& ceilingCarrySpeed
) {
    if (!character || !character->IsAlive()) {
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
        character->SetScale({ -1.0f, 1.0f });
    }
    else if (inputDir > 0.0f) {
        character->SetScale({ 1.0f, 1.0f });
    }

    // Liquids do not use a separate movement mode. Instead we scale the normal
    // controller's speed/acceleration/jump so water/lava feel heavier.
    const bool inLiquid = IsCharacterInLiquid(character, profile);
    const float liquidSpeedScale = inLiquid ? m_LiquidMoveSpeedScale : 1.0f;
    const float liquidAccelerationScale = inLiquid ? m_LiquidAccelerationScale : 1.0f;
    const float liquidDecelerationScale = inLiquid ? m_LiquidDecelerationScale : 1.0f;

    const float acceleration =
        (onGround ? m_GroundAcceleration : m_AirAcceleration) * liquidAccelerationScale;
    const float deceleration =
        (onGround ? m_GroundDeceleration : m_AirDeceleration) * liquidDecelerationScale;

    if (inputDir != 0.0f) {
        const float targetSpeed = inputDir * m_MoveSpeed * liquidSpeedScale;
        if (velocity.x < targetSpeed) {
            velocity.x = std::min(targetSpeed, velocity.x + acceleration);
        }
        else if (velocity.x > targetSpeed) {
            velocity.x = std::max(targetSpeed, velocity.x - acceleration);
        }
    }
    else if (velocity.x > 0.0f) {
        velocity.x = std::max(0.0f, velocity.x - deceleration);
    }
    else if (velocity.x < 0.0f) {
        velocity.x = std::min(0.0f, velocity.x + deceleration);
    }

    if (std::abs(velocity.x) < 0.01f) {
        velocity.x = 0.0f;
    }

    // After a head-bonk on a ceiling, preserve horizontal travel briefly so
    // roof collision does not kill the whole jump arc's x-distance.
    if (ceilingCarryTimer > 0.0f && std::abs(ceilingCarrySpeed) > 0.01f) {
        const float timerRatio = std::clamp(
            ceilingCarryTimer / m_CeilingMomentumCarryDuration,
            0.0f,
            1.0f
        );
        const float minCarryAbs =
            std::abs(ceilingCarrySpeed) *
            (m_CeilingMomentumCarryFloor + (1.0f - m_CeilingMomentumCarryFloor) * timerRatio);

        if (ceilingCarrySpeed > 0.0f) {
            velocity.x = std::max(velocity.x, minCarryAbs);
        }
        else {
            velocity.x = std::min(velocity.x, -minCarryAbs);
        }

        ceilingCarryTimer = std::max(0.0f, ceilingCarryTimer - FIXED_TIME_STEP);
        if (ceilingCarryTimer <= 0.0f) {
            ceilingCarrySpeed = 0.0f;
        }
    }

    newPos.x += velocity.x;

    // Keep the player glued to slope surfaces while walking so the terrain
    // behaves like a real ramp instead of a staircase of tiny rectangles.
    if (onGround && newPos.x != oldPos.x) {
        ApplySlopeFollow(oldPos, newPos, profile);
    }

    const float attemptedX = newPos.x;
    ResolveHorizontalCollisions(oldPos, newPos, profile);
    if (std::abs(newPos.x - attemptedX) > 0.001f) {
        velocity.x = 0.0f;
    }
    if (onGround && newPos.x != oldPos.x) {
        const float preSnapY = newPos.y;
        ApplySlopeFollow(oldPos, newPos, profile);
        // Call 1 already handled downward slope follow. This second call exists
        // to snap UP onto a slope after horizontal collision corrects X. Prevent
        // it from undoing a step-up that ResolveHorizontalCollisions just performed.
        if (newPos.y < preSnapY) {
            newPos.y = preSnapY;
        }
    }
    character->SetPosition(newPos);

    // Jump injects vertical velocity plus a controlled horizontal launch band.
    // That gives a meaningful jump even without a huge run-up.
    if (Util::Input::IsKeyPressed(jumpKey) && onGround) {
        velocity.y = m_JumpSpeed * (inLiquid ? m_LiquidJumpScale : 1.0f);
        if (inputDir != 0.0f) {
            const float jumpLaunchMin = m_JumpHorizontalLaunchMin * liquidSpeedScale;
            const float jumpLaunchMax = m_JumpHorizontalLaunchMax * liquidSpeedScale;

            if (inputDir > 0.0f) {
                velocity.x = std::clamp(
                    std::max(velocity.x, jumpLaunchMin),
                    0.0f,
                    jumpLaunchMax
                );
            }
            else {
                velocity.x = std::clamp(
                    std::min(velocity.x, -jumpLaunchMin),
                    -jumpLaunchMax,
                    0.0f
                );
            }
        }
        onGround = false;
    }
}

void App::UpdateFireboyPhysics() {
    UpdateCharacterPhysics(
        m_Fireboy,
        m_FireboyVelocity,
        m_FireboyOnGround,
        m_FireboyCollision,
        m_FireboyCeilingCarryTimer,
        m_FireboyCeilingCarrySpeed
    );
}

void App::UpdateWatergirlPhysics() {
    UpdateCharacterPhysics(
        m_Watergirl,
        m_WatergirlVelocity,
        m_WatergirlOnGround,
        m_WatergirlCollision,
        m_WatergirlCeilingCarryTimer,
        m_WatergirlCeilingCarrySpeed
    );
}

void App::UpdateCharacterPhysics(
    const std::shared_ptr<HeadBodyCharacter>& character,
    glm::vec2& velocity,
    bool& onGround,
    const CharacterCollisionProfile& profile,
    float& ceilingCarryTimer,
    float& ceilingCarrySpeed
) {
    if (!character || !character->IsAlive()) {
        return;
    }

    glm::vec2 oldPos = character->GetPosition();
    glm::vec2 newPos = oldPos;
    const float preVerticalVelocityX = velocity.x;

    // Vertical motion is resolved separately from horizontal motion. This is
    // why head collisions should stop y-motion but only indirectly shorten x.
    const bool inLiquid = IsCharacterInLiquid(character, profile);
    velocity.y -= m_Gravity * (inLiquid ? m_LiquidGravityScale : 1.0f);
    if (inLiquid) {
        velocity.x *= m_LiquidVelocityDrag;
        velocity.y *= m_LiquidVelocityDrag;
    }
    newPos.y += velocity.y;

    bool hitCeiling = false;
    ResolveVerticalCollisions(oldPos, newPos, profile, velocity, onGround, hitCeiling);

    // Remember horizontal momentum after a ceiling hit so the next few frames
    // keep the jump feeling "floaty" instead of reflecting/killing x carry.
    if (hitCeiling && std::abs(preVerticalVelocityX) > 0.01f) {
        ceilingCarryTimer = m_CeilingMomentumCarryDuration;
        ceilingCarrySpeed = preVerticalVelocityX * m_CeilingMomentumCarryBoost;
    }

    if (onGround) {
        ceilingCarryTimer = 0.0f;
        ceilingCarrySpeed = 0.0f;
    }

    character->SetPosition(newPos);
}

void App::UpdateCharacterMotionState(
    const std::shared_ptr<HeadBodyCharacter>& character,
    const glm::vec2& velocity,
    bool onGround
) {
    // Animation state is driven from movement outcome, not raw input. This
    // keeps jump/fall/run visuals consistent even when slopes or collisions
    // modify the actual motion.
    if (!character || !character->IsAlive()) {
        return;
    }

    const glm::vec2 pos = character->GetPosition();
    const auto prevIter = s_LastPos.find(character.get());
    const glm::vec2 prev = (prevIter == s_LastPos.end()) ? pos : prevIter->second;
    const float moved = glm::length(pos - prev);

    if (!onGround) {
        if (std::abs(velocity.x) > AIR_RUN_EPSILON) {
            character->SetMotionState(HeadBodyCharacter::MotionState::Move);
        }
        else if (velocity.y > AIR_STATE_EPSILON) {
            character->SetMotionState(HeadBodyCharacter::MotionState::Jump);
        }
        else {
            character->SetMotionState(HeadBodyCharacter::MotionState::Fall);
        }
    }
    else if (moved < IDLE_EPSILON) {
        character->SetMotionState(HeadBodyCharacter::MotionState::Idle);
    }
    else {
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
    }
    else {
        character->SetHeadRotation(0.0f);
    }

    s_LastPos[character.get()] = pos;
}

void App::UpdateExitDoors() {
    // Doors open independently for their matching characters, but the level
    // only transitions into victory once both doors are occupied together.
    auto updateDoorSprite = [&](ExitDoor& door) {
        if (!door.sprite) {
            return;
        }

        const float maxProgress =
            static_cast<float>(door.openingImagePaths.size()) * m_DoorFrameDuration;
        if (door.occupied) {
            door.openProgress = std::min(maxProgress, door.openProgress + FIXED_TIME_STEP);
        }
        else {
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
    }
    else {
        m_FireboyDoor.occupied = true;
        m_WatergirlDoor.occupied = true;
    }

    updateDoorSprite(m_FireboyDoor);
    updateDoorSprite(m_WatergirlDoor);

    const bool fireboyReadyForVictory =
        m_Fireboy &&
        m_Fireboy->IsAlive() &&
        m_FireboyDoor.occupied &&
        m_FireboyOnGround;
    const bool watergirlReadyForVictory =
        m_Watergirl &&
        m_Watergirl->IsAlive() &&
        m_WatergirlDoor.occupied &&
        m_WatergirlOnGround;

    if (m_VictoryPhase == VictoryPhase::None &&
        fireboyReadyForVictory &&
        watergirlReadyForVictory) {
        m_VictoryPhase = VictoryPhase::RunIntoDoor;
        m_VictoryTimer = 0.0f;
        if (m_LevelCompleteTimeMs <= 0.0f) {
            m_LevelCompleteTimeMs = std::max(
                0.0f,
                Util::Time::GetElapsedTimeMs() - m_LevelStartTimeMs
            );
        }
        m_FireboyVelocity = { 0.0f, 0.0f };
        m_WatergirlVelocity = { 0.0f, 0.0f };

        if (!m_FinishMusicPlayed && m_FinishMusic) {
            m_FinishMusic->Play(0);
            m_FinishMusicPlayed = true;
        }
    }
}

void App::UpdateVictorySequence() {
    // Victory has two phases:
    // - a short auto-run into the doors
    // - then a stationary win animation
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
            const ExitDoor& door,
            glm::vec2& velocity,
            bool& onGround) {
                if (!character || !door.sprite) {
                    return true;
                }

                velocity = { 0.0f, 0.0f };
                onGround = true;
                zeroAirVisuals(character);

                glm::vec2 pos = character->GetPosition();
                const float deltaX = door.sprite->GetPosition().x - pos.x;
                const bool centered = std::abs(deltaX) <= m_VictoryDoorCenterTolerance;

                if (!centered) {
                    const float moveX = std::clamp(deltaX, -m_VictoryAlignSpeed, m_VictoryAlignSpeed);
                    pos.x += moveX;
                    character->SetScale({ moveX < 0.0f ? -1.0f : 1.0f, 1.0f });
                    character->SetMotionState(HeadBodyCharacter::MotionState::Move);
                }
                else {
                    pos.x = door.sprite->GetPosition().x;
                    character->SetMotionState(HeadBodyCharacter::MotionState::Idle);
                }

                character->SetPosition(pos);
                return centered;
            };

        const bool fireboyCentered = runIntoDoor(
            m_Fireboy,
            m_FireboyDoor,
            m_FireboyVelocity,
            m_FireboyOnGround
        );
        const bool watergirlCentered = runIntoDoor(
            m_Watergirl,
            m_WatergirlDoor,
            m_WatergirlVelocity,
            m_WatergirlOnGround
        );

        if (m_VictoryTimer >= m_VictoryRunDuration &&
            fireboyCentered &&
            watergirlCentered) {
            m_VictoryPhase = VictoryPhase::Celebrate;
            m_VictoryTimer = 0.0f;
            if (m_Fireboy) {
                m_FireboyVictoryStartPosition = m_Fireboy->GetPosition();
                m_FireboyVictoryStartScale = m_Fireboy->GetTransform().scale;
            }
            if (m_Watergirl) {
                m_WatergirlVictoryStartPosition = m_Watergirl->GetPosition();
                m_WatergirlVictoryStartScale = m_Watergirl->GetTransform().scale;
            }
            m_FireboyVelocity = { 0.0f, 0.0f };
            m_WatergirlVelocity = { 0.0f, 0.0f };
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
        m_VictoryTimer += FIXED_TIME_STEP;
        m_FireboyVelocity = { 0.0f, 0.0f };
        m_WatergirlVelocity = { 0.0f, 0.0f };
        zeroAirVisuals(m_Fireboy);
        zeroAirVisuals(m_Watergirl);
        const float progress = std::clamp(
            m_VictoryTimer / std::max(0.001f, m_VictoryCelebrateDuration),
            0.0f,
            1.0f
        );
        auto sinkIntoDoor = [&](const std::shared_ptr<HeadBodyCharacter>& character,
                                const ExitDoor& door,
                                const glm::vec2& startPos,
                                const glm::vec2& startScale) {
            if (!character || !door.sprite) {
                return;
            }

            character->SetMotionState(HeadBodyCharacter::MotionState::Win);

            const glm::vec2 targetPos = door.triggerRect.center + glm::vec2{
                0.0f,
                m_VictoryDoorSinkYOffset
            };
            const glm::vec2 currentPos = {
                targetPos.x,
                startPos.y + (targetPos.y - startPos.y) * progress,
            };

            const float scaleXSign = startScale.x < 0.0f ? -1.0f : 1.0f;
            const float startScaleMagnitude = std::abs(startScale.y);
            const float currentScaleMagnitude =
                startScaleMagnitude +
                (m_VictoryDoorSinkScale - startScaleMagnitude) * progress;

            character->SetScale({
                scaleXSign * currentScaleMagnitude,
                currentScaleMagnitude,
            });
            character->SetPosition(currentPos);
        };

        sinkIntoDoor(
            m_Fireboy,
            m_FireboyDoor,
            m_FireboyVictoryStartPosition,
            m_FireboyVictoryStartScale
        );
        sinkIntoDoor(
            m_Watergirl,
            m_WatergirlDoor,
            m_WatergirlVictoryStartPosition,
            m_WatergirlVictoryStartScale
        );
    }
}

bool App::AreBothCharactersDead() const {
    return m_Fireboy &&
        m_Watergirl &&
        m_Fireboy->GetLifeState() == HeadBodyCharacter::LifeState::Dead &&
        m_Watergirl->GetLifeState() == HeadBodyCharacter::LifeState::Dead;
}

bool App::IsFailOverlayVisible() const {
    return m_FailOverlayVisible;
}

App::FailOverlayAction App::GetFailOverlayAction() const {
    if (!m_FailOverlayVisible) {
        return FailOverlayAction::None;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::R) ||
        Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
        return FailOverlayAction::Restart;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::H)) {
        return FailOverlayAction::Home;
    }

    if (Util::Input::IsKeyDown(Util::Keycode::E)) {
        return FailOverlayAction::Exit;
    }

    if (!Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) || m_FailOverlayMouseLatch) {
        return FailOverlayAction::None;
    }

    const glm::vec2 cursor = Util::Input::GetCursorPosition();
    const auto cursorInside = [&](const SolidRect& rect) {
        return CheckAABB(cursor, { 1.0f, 1.0f }, rect.center, rect.size);
        };

    if (cursorInside(m_FailRestartButton.rect)) {
        return FailOverlayAction::Restart;
    }
    if (cursorInside(m_FailHomeButton.rect)) {
        return FailOverlayAction::Home;
    }
    if (cursorInside(m_FailExitButton.rect)) {
        return FailOverlayAction::Exit;
    }

    return FailOverlayAction::None;
}

void App::ResetFailOverlayLatch() {
    if (!Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB)) {
        m_FailOverlayMouseLatch = false;
    }
}

void App::UpdateFailOverlayVisuals() {
    for (const auto& object : m_FailOverlayObjects) {
        if (object) {
            object->SetVisible(m_FailOverlayVisible);
        }
    }

    const glm::vec2 cursor = Util::Input::GetCursorPosition();
    const auto hoveredColor = Util::Color(255, 220, 140, 255);
    const auto normalColor = Util::Color(255, 255, 255, 255);

    if (m_FailOverlayTitle) {
        m_FailOverlayTitle->SetColor(Util::Color(255, 245, 220, 255));
    }

    auto updateButtonText = [&](FailOverlayButton& button) {
        if (!button.labelObject) {
            return;
        }

        const bool hovered = m_FailOverlayVisible &&
            CheckAABB(cursor, { 1.0f, 1.0f }, button.rect.center, button.rect.size);

        button.labelObject->SetColor(hovered ? hoveredColor : normalColor);
        };

    updateButtonText(m_FailRestartButton);
    updateButtonText(m_FailHomeButton);
    updateButtonText(m_FailExitButton);
}

void App::UpdateFailOverlay() {
    // Fail overlay is the current "game over" state. Restart reloads the same
    // level, while Home returns to the level select screen.
    m_FailOverlayVisible = AreBothCharactersDead();
    UpdateFailOverlayVisuals();

    if (!m_FailOverlayVisible) {
        ResetFailOverlayLatch();
        return;
    }

    const FailOverlayAction action = GetFailOverlayAction();
    if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
        m_FailOverlayMouseLatch = true;
    }
    else {
        m_FailOverlayMouseLatch = false;
    }

    switch (action) {
    case FailOverlayAction::Restart:
        m_CurrentState = State::START;
        break;
    case FailOverlayAction::Home:
        m_CurrentState = State::LEVEL_SELECT;
        break;
    case FailOverlayAction::Exit:
        m_CurrentState = State::END;
        break;
    case FailOverlayAction::None:
        break;
    }
}

void App::UpdateVictoryOverlayVisuals() {
    const bool fireboyWinFinished = !m_Fireboy || m_Fireboy->IsWinAnimationFinished();
    const bool watergirlWinFinished = !m_Watergirl || m_Watergirl->IsWinAnimationFinished();
    const bool celebrationFinished =
        (fireboyWinFinished && watergirlWinFinished) ||
        (m_VictoryTimer >= std::max(0.0f, m_VictoryCelebrateDuration));

    m_VictoryOverlayVisible =
        (m_VictoryPhase == VictoryPhase::Celebrate) &&
        celebrationFinished;

    for (const auto& object : m_VictoryOverlayObjects) {
        if (object) {
            object->SetVisible(m_VictoryOverlayVisible);
        }
    }

    if (!m_VictoryOverlayVisible) {
        return;
    }

    const auto formatTimeText = [](float elapsedMs) {
        const int totalSeconds = std::max(0, static_cast<int>(std::round(elapsedMs / 1000.0f)));
        const int minutes = totalSeconds / 60;
        const int seconds = totalSeconds % 60;

        std::ostringstream stream;
        stream << std::setfill('0')
            << std::setw(2) << minutes
            << ":"
            << std::setw(2) << seconds;
        return stream.str();
        };

    // The original games clearly rank faster / more complete runs higher, but
    // exact public thresholds were not available in our source pass. This is
    // an inferred grade scale so the overlay already communicates progress.
    const int collectedDiamonds =
        m_FireDiamondsCollected + m_WaterDiamondsCollected + m_GreenDiamondsCollected;
    const int totalDiamonds =
        std::max(1, m_FireDiamondsTotal + m_WaterDiamondsTotal + m_GreenDiamondsTotal);
    const float collectedRatio =
        static_cast<float>(collectedDiamonds) / static_cast<float>(totalDiamonds);
    const float elapsedSeconds = std::max(0.0f, m_LevelCompleteTimeMs / 1000.0f);

    std::string rank = "C";
    if (collectedRatio >= 0.999f && elapsedSeconds <= 65.0f) {
        rank = "S";
    }
    else if (collectedRatio >= 0.85f && elapsedSeconds <= 95.0f) {
        rank = "A";
    }
    else if (collectedRatio >= 0.60f || elapsedSeconds <= 140.0f) {
        rank = "B";
    }

    if (m_VictoryOverlayTitle) {
        m_VictoryOverlayTitle->SetColor(Util::Color(255, 245, 220, 255));
    }
    if (m_VictoryTimeText) {
        m_VictoryTimeText->SetText(formatTimeText(m_LevelCompleteTimeMs));
        m_VictoryTimeText->SetColor(Util::Color(255, 255, 255, 255));
    }
    if (m_VictoryBlueDiamondText) {
        m_VictoryBlueDiamondText->SetText("x " + std::to_string(m_WaterDiamondsCollected));
        m_VictoryBlueDiamondText->SetColor(Util::Color(240, 248, 255, 255));
    }
    if (m_VictoryRedDiamondText) {
        m_VictoryRedDiamondText->SetText("x " + std::to_string(m_FireDiamondsCollected));
        m_VictoryRedDiamondText->SetColor(Util::Color(255, 235, 230, 255));
    }
    if (m_VictoryRankText) {
        m_VictoryRankText->SetText("Rank: " + rank);
        m_VictoryRankText->SetColor(Util::Color(255, 230, 170, 255));
    }

    if (m_VictoryContinueButton.labelObject) {
        const glm::vec2 cursor = Util::Input::GetCursorPosition();
        const bool hovered = CheckAABB(
            cursor,
            { 1.0f, 1.0f },
            m_VictoryContinueButton.rect.center,
            m_VictoryContinueButton.rect.size
        );
        m_VictoryContinueButton.labelObject->SetColor(
            hovered
            ? Util::Color(255, 220, 140, 255)
            : Util::Color(255, 255, 255, 255)
        );

        if (hovered && Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            m_CurrentState = State::LEVEL_SELECT;
            return;
        }
    }

    if (Util::Input::IsKeyDown(Util::Keycode::H)) {
        m_CurrentState = State::LEVEL_SELECT;
    } else if (Util::Input::IsKeyDown(Util::Keycode::R)) {
        m_CurrentState = State::START;
    }
}

void App::CheckHazards() {
    // Hazard rules currently match the classic element logic:
    // Fireboy dies in water, Watergirl dies in lava, and venom kills both.
    auto isFatal = [](HazardRect::Type type, bool isFireboy) {
        switch (type) {
        case HazardRect::Type::Lava:
            return !isFireboy;
        case HazardRect::Type::Water:
            return isFireboy;
        case HazardRect::Type::Venom:
            return true;
        }

        return false;
        };

    auto playDeathSfx = [&]() {
        if (m_DeathMusic) {
            m_DeathMusic->Play(0);
        }
        };

    for (const auto& hazard : m_Hazards) {
        if (m_Fireboy &&
            m_Fireboy->GetLifeState() == HeadBodyCharacter::LifeState::Alive &&
            isFatal(hazard.type, true) &&
            CharacterTouchesHazard(m_Fireboy, m_FireboyCollision, hazard)) {
            m_FireboyVelocity = { 0.0f, 0.0f };
            m_FireboyOnGround = false;
            m_Fireboy->SetAirborneRunHeadOffsetEnabled(false);
            m_Fireboy->SetHeadRotation(0.0f);
            m_Fireboy->SetMotionState(HeadBodyCharacter::MotionState::Fall);
            m_FireboyDeathStartScale = m_Fireboy->GetTransform().scale;
            m_FireboyDeathTimer = 0.0f;
            m_Fireboy->SetLifeState(HeadBodyCharacter::LifeState::Dying);
            playDeathSfx();
        }

        if (m_Watergirl &&
            m_Watergirl->GetLifeState() == HeadBodyCharacter::LifeState::Alive &&
            isFatal(hazard.type, false) &&
            CharacterTouchesHazard(m_Watergirl, m_WatergirlCollision, hazard)) {
            m_WatergirlVelocity = { 0.0f, 0.0f };
            m_WatergirlOnGround = false;
            m_Watergirl->SetAirborneRunHeadOffsetEnabled(false);
            m_Watergirl->SetHeadRotation(0.0f);
            m_Watergirl->SetMotionState(HeadBodyCharacter::MotionState::Fall);
            m_WatergirlDeathStartScale = m_Watergirl->GetTransform().scale;
            m_WatergirlDeathTimer = 0.0f;
            m_Watergirl->SetLifeState(HeadBodyCharacter::LifeState::Dying);
            playDeathSfx();
        }
    }
}

void App::UpdateDeathSequence() {
    // Death is a short sink/shrink transition before the character becomes
    // fully non-interactive and invisible.
    auto updateCharacterDeath = [&](const std::shared_ptr<HeadBodyCharacter>& character,
        float& deathTimer,
        const glm::vec2& deathStartScale,
        glm::vec2& velocity) {
            if (!character || character->GetLifeState() != HeadBodyCharacter::LifeState::Dying) {
                return;
            }

            deathTimer = std::min(m_DeathAnimationDuration, deathTimer + FIXED_TIME_STEP);
            const float t = std::clamp(deathTimer / m_DeathAnimationDuration, 0.0f, 1.0f);
            const float scaleMagnitude = 1.0f + (m_DeathEndScale - 1.0f) * t;
            const float scaleXSign = deathStartScale.x < 0.0f ? -1.0f : 1.0f;
            const float scaleYSign = deathStartScale.y < 0.0f ? -1.0f : 1.0f;

            character->SetScale({
                scaleXSign * scaleMagnitude,
                scaleYSign * scaleMagnitude,
                });
            character->SetHeadRotation(0.0f);

            glm::vec2 pos = character->GetPosition();
            pos.y -= m_DeathSinkSpeed * (0.85f + 0.15f * t);
            character->SetPosition(pos);

            velocity = { 0.0f, 0.0f };

            if (t >= 1.0f) {
                character->SetLifeState(HeadBodyCharacter::LifeState::Dead);
            }
        };

    updateCharacterDeath(m_Fireboy, m_FireboyDeathTimer, m_FireboyDeathStartScale, m_FireboyVelocity);
    updateCharacterDeath(m_Watergirl, m_WatergirlDeathTimer, m_WatergirlDeathStartScale, m_WatergirlVelocity);
}

void App::UpdateGreenSwitch() {
    // Switches are toggle-based and independent from the hold-to-press buttons.
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
    // Diamonds are optional collectibles right now, but they already track
    // per-color totals so later win conditions / UI can use the same data.
    if (m_Diamonds.empty()) {
        return;
    }

    auto touchesDiamond = [&](const std::shared_ptr<HeadBodyCharacter>& character,
        const CharacterCollisionProfile& profile,
        const CollectibleDiamond& diamond) {
            if (!character || !character->IsAlive()) {
                return false;
            }

            glm::vec2 bodyCenter = { 0.0f, 0.0f };
            glm::vec2 bodySize = { 0.0f, 0.0f };
            glm::vec2 headCenter = { 0.0f, 0.0f };
            glm::vec2 headSize = { 0.0f, 0.0f };
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
    // The cube is treated as a pushable solid block that:
    // - accelerates from character push intent
    // - collides with terrain
    // - can ride moving platforms
    // - now follows slopes without rotating
    if (!m_Cube || !m_HasCubeBlock || m_CubeBlockIndex >= m_SolidBlocks.size()) {
        return;
    }

    const SolidRect oldCube = m_CubeRect;

    // Push intent counts both characters. If both push in the same direction,
    // the target speed/acceleration naturally becomes stronger.
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

                glm::vec2 bodyCenter = { 0.0f, 0.0f };
                glm::vec2 bodySize = { 0.0f, 0.0f };
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
        }
        else if (m_CubeVelocity.x > targetSpeed) {
            m_CubeVelocity.x = std::max(targetSpeed, m_CubeVelocity.x - pushAcceleration);
        }
    }
    else if (m_CubeVelocity.x > 0.0f) {
        m_CubeVelocity.x = std::max(0.0f, m_CubeVelocity.x - m_CubeDeceleration);
    }
    else if (m_CubeVelocity.x < 0.0f) {
        m_CubeVelocity.x = std::min(0.0f, m_CubeVelocity.x + m_CubeDeceleration);
    }

    if (std::abs(m_CubeVelocity.x) < 0.01f) {
        m_CubeVelocity.x = 0.0f;
    }

    // Horizontal pass first, with slope-follow before and after collision
    // resolution. This mirrors the character controller and avoids stair-step
    // movement on ramps.
    SolidRect cubeAfterHorizontal = m_CubeRect;
    cubeAfterHorizontal.center.x += m_CubeVelocity.x;

    if (cubeAfterHorizontal.center.x != oldCube.center.x && m_CubeVelocity.y <= 0.0f) {
        ApplyCubeSlopeFollow(oldCube, cubeAfterHorizontal);
    }

    ResolveCubeHorizontalCollisions(oldCube, cubeAfterHorizontal, m_CubeVelocity);

    if (cubeAfterHorizontal.center.x != oldCube.center.x && m_CubeVelocity.y <= 0.0f) {
        ApplyCubeSlopeFollow(oldCube, cubeAfterHorizontal);
    }

    // Vertical pass next: regular block collision + explicit slope grounding.
    m_CubeRect = cubeAfterHorizontal;
    const SolidRect cubeBeforeVertical = m_CubeRect;
    m_CubeVelocity.y -= m_Gravity;
    m_CubeRect.center.y += m_CubeVelocity.y;
    m_CubeOnGround = false;

    bool foundGround = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();
    bool hitCeiling = false;
    for (std::size_t i = 0; i < m_SolidBlocks.size(); ++i) {
        if (i == m_CubeBlockIndex) {
            continue;
        }

        const auto& block = m_SolidBlocks[i];
        if (!CheckAABB(m_CubeRect.center, m_CubeRect.size, block.center, block.size)) {
            continue;
        }

        const float oldBottom = cubeBeforeVertical.center.y - cubeBeforeVertical.size.y * 0.5f;
        const float oldTop = cubeBeforeVertical.center.y + cubeBeforeVertical.size.y * 0.5f;
        const float newBottom = m_CubeRect.center.y - m_CubeRect.size.y * 0.5f;
        const float newTop = m_CubeRect.center.y + m_CubeRect.size.y * 0.5f;
        const float blockTop = block.center.y + block.size.y * 0.5f;
        const float blockBottom = block.center.y - block.size.y * 0.5f;

        if (m_CubeVelocity.y <= 0.0f &&
            oldBottom >= blockTop - m_GroundSnapTolerance &&
            newBottom <= blockTop + m_GroundSnapTolerance) {
            bestGroundY = std::max(bestGroundY, blockTop);
            foundGround = true;
        }
        else if (block.blockBottom &&
            m_CubeVelocity.y > 0.0f &&
            oldTop <= blockBottom + m_GroundSnapTolerance &&
            newTop >= blockBottom - m_GroundSnapTolerance) {
            m_CubeRect.center.y = blockBottom - m_CubeRect.size.y * 0.5f;
            m_CubeVelocity.y = 0.0f;
            hitCeiling = true;
        }
    }

    if (!hitCeiling && ResolveCubeSlopeGrounding(
        cubeBeforeVertical,
        m_CubeRect,
        m_CubeVelocity,
        m_CubeOnGround
    )) {
        const float slopeGroundY = m_CubeRect.center.y - m_CubeRect.size.y * 0.5f;
        if (!foundGround || slopeGroundY > bestGroundY) {
            bestGroundY = slopeGroundY;
            foundGround = true;
        }
    }

    if (!foundGround && m_CubeVelocity.y <= 0.0f) {
        float stickyGroundY = 0.0f;
        if (FindNearbyCubeGroundY(m_CubeRect, m_GroundStickTolerance, stickyGroundY)) {
            bestGroundY = stickyGroundY;
            foundGround = true;
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

void App::ResolveCubeHorizontalCollisions(
    const SolidRect& oldCube,
    SolidRect& newCube,
    glm::vec2& velocity
) const {
    // Cube horizontal collision allows a small step-up / nearby-ground snap so
    // slope and flat transitions do not behave like tiny vertical walls.
    const float oldBottom = oldCube.center.y - oldCube.size.y * 0.5f;

    auto cubeCollides = [&](const SolidRect& candidate, const SolidRect& block) {
        return CheckAABB(candidate.center, candidate.size, block.center, block.size);
        };

    for (std::size_t i = 0; i < m_SolidBlocks.size(); ++i) {
        if (i == m_CubeBlockIndex) {
            continue;
        }

        const auto& block = m_SolidBlocks[i];
        if (!cubeCollides(newCube, block)) {
            continue;
        }

        const float oldLeft = oldCube.center.x - oldCube.size.x * 0.5f;
        const float oldRight = oldCube.center.x + oldCube.size.x * 0.5f;
        const float newLeft = newCube.center.x - newCube.size.x * 0.5f;
        const float newRight = newCube.center.x + newCube.size.x * 0.5f;
        const float blockLeft = block.center.x - block.size.x * 0.5f;
        const float blockRight = block.center.x + block.size.x * 0.5f;
        const float blockTop = block.center.y + block.size.y * 0.5f;

        auto canStepUp = [&](float targetTopY) {
            const float raisedY = targetTopY + newCube.size.y * 0.5f + 0.1f;
            const float liftAmount = raisedY - oldCube.center.y;
            if (liftAmount <= 0.0f || liftAmount > m_StepUpHeight) {
                return false;
            }

            SolidRect candidate = newCube;
            candidate.center.y = raisedY;
            const float candidateBottom = candidate.center.y - candidate.size.y * 0.5f;

            for (std::size_t j = 0; j < m_SolidBlocks.size(); ++j) {
                if (j == m_CubeBlockIndex || j == i) {
                    continue;
                }

                const auto& other = m_SolidBlocks[j];
                if (!cubeCollides(candidate, other)) {
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

            newCube.center.y = raisedY;
            return true;
            };

        auto canStepToNearbyGround = [&]() {
            SolidRect probeCube = newCube;
            probeCube.center.y = oldCube.center.y;

            float targetGroundY = 0.0f;
            if (!FindNearbyCubeGroundY(
                probeCube,
                m_StepUpHeight + m_GroundSnapTolerance,
                targetGroundY
            )) {
                return false;
            }

            const float liftAmount = targetGroundY - oldBottom;
            if (liftAmount < -m_GroundSnapTolerance ||
                liftAmount > m_StepUpHeight + m_GroundSnapTolerance) {
                return false;
            }

            SolidRect candidate = newCube;
            candidate.center.y = targetGroundY + candidate.size.y * 0.5f + 0.1f;
            const float candidateBottom = candidate.center.y - candidate.size.y * 0.5f;

            for (std::size_t j = 0; j < m_SolidBlocks.size(); ++j) {
                if (j == m_CubeBlockIndex) {
                    continue;
                }

                const auto& other = m_SolidBlocks[j];
                if (!cubeCollides(candidate, other)) {
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

            newCube.center.y = candidate.center.y;
            return true;
            };

        if (velocity.x > 0.0f && oldRight <= blockLeft && newRight >= blockLeft) {
            if (!canStepUp(blockTop) && !canStepToNearbyGround()) {
                newCube.center.x = blockLeft - newCube.size.x * 0.5f;
                velocity.x = 0.0f;
            }
        }
        else if (velocity.x < 0.0f && oldLeft >= blockRight && newLeft <= blockRight) {
            if (!canStepUp(blockTop) && !canStepToNearbyGround()) {
                newCube.center.x = blockRight + newCube.size.x * 0.5f;
                velocity.x = 0.0f;
            }
        }
    }
}

bool App::ResolveCubeSlopeGrounding(
    const SolidRect& oldCube,
    SolidRect& newCube,
    glm::vec2& velocity,
    bool& onGround
) const {
    // The cube samples left/center/right bottom points against floor slopes.
    // This gives smooth ramp travel without needing to rotate the sprite.
    if (velocity.y > 1.0f) {
        return false;
    }

    const float oldBottom = oldCube.center.y - oldCube.size.y * 0.5f;
    const float newBottom = newCube.center.y - newCube.size.y * 0.5f;
    const float footInset = std::min(m_FootProbeInset, newCube.size.x * 0.25f);
    const float leftFootX = newCube.center.x - newCube.size.x * 0.5f + footInset;
    const float rightFootX = newCube.center.x + newCube.size.x * 0.5f - footInset;

    bool foundSlope = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();

    for (const auto& slope : m_Slopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        for (const float footX : {leftFootX, newCube.center.x, rightFootX}) {
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

    newCube.center.y = bestGroundY + newCube.size.y * 0.5f;
    velocity.y = 0.0f;
    onGround = true;
    return true;
}

bool App::FindBestCubeSlopeYAtX(
    const SolidRect& oldCube,
    float desiredCenterX,
    float& outSlopeY
) const {
    const float oldBottom = oldCube.center.y - oldCube.size.y * 0.5f;
    const float footInset = std::min(m_FootProbeInset, oldCube.size.x * 0.25f);
    const float leftFootX = desiredCenterX - oldCube.size.x * 0.5f + footInset;
    const float rightFootX = desiredCenterX + oldCube.size.x * 0.5f - footInset;

    bool foundSlope = false;
    float bestSlopeY = -std::numeric_limits<float>::infinity();

    for (const auto& slope : m_Slopes) {
        const glm::vec2 delta = slope.end - slope.start;
        if (std::abs(delta.x) < 0.001f) {
            continue;
        }

        for (const float footX : {leftFootX, desiredCenterX, rightFootX}) {
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

void App::ApplyCubeSlopeFollow(
    const SolidRect& oldCube,
    SolidRect& newCube
) const {
    float bestSlopeY = 0.0f;
    if (!FindBestCubeSlopeYAtX(oldCube, newCube.center.x, bestSlopeY)) {
        return;
    }

    newCube.center.y = bestSlopeY + newCube.size.y * 0.5f;
}

bool App::FindNearbyCubeGroundY(
    const SolidRect& cube,
    float maxDistance,
    float& outGroundY
) const {
    const float left = cube.center.x - cube.size.x * 0.5f;
    const float right = cube.center.x + cube.size.x * 0.5f;
    const float bottom = cube.center.y - cube.size.y * 0.5f;

    bool foundGround = false;
    float bestGroundY = -std::numeric_limits<float>::infinity();

    for (std::size_t i = 0; i < m_SolidBlocks.size(); ++i) {
        if (i == m_CubeBlockIndex) {
            continue;
        }

        const auto& block = m_SolidBlocks[i];
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

    float slopeGroundY = 0.0f;
    if (FindBestCubeSlopeYAtX(cube, cube.center.x, slopeGroundY) &&
        std::abs(slopeGroundY - bottom) <= maxDistance &&
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
