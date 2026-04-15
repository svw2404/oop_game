#include "HeadBodyCharacter.hpp"

#include <algorithm>
#include <cmath>

namespace {
glm::vec2 GetDrawableSize(const std::shared_ptr<Core::Drawable>& drawable) {
    if (!drawable) {
        return {0.0f, 0.0f};
    }

    if (auto img = std::dynamic_pointer_cast<Util::Image>(drawable)) {
        return img->GetSize();
    }

    if (auto anim = std::dynamic_pointer_cast<Util::Animation>(drawable)) {
        return anim->GetSize();
    }

    return {0.0f, 0.0f};
}

void ApplyDrawableSize(const std::shared_ptr<Core::Drawable>& drawable, const glm::vec2& size) {
    if (!drawable) {
        return;
    }

    if (auto img = std::dynamic_pointer_cast<Util::Image>(drawable)) {
        img->SetSize(size);
    } else if (auto anim = std::dynamic_pointer_cast<Util::Animation>(drawable)) {
        anim->SetSize(size);
    }
}
} // namespace

// ----------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------

HeadBodyCharacter::HeadBodyCharacter(const std::string& bodyPath,
                                     const std::string& headPath,
                                     float zIndex)
    : Character(std::string(), true) {
    m_BodyDrawable = std::make_shared<Util::Image>(bodyPath);
    m_BodyObject = std::make_shared<Util::GameObject>(m_BodyDrawable, zIndex);
    m_BodyObject->m_Transform.translation = {0.0f, 0.0f};

    m_HeadDrawable = std::make_shared<Util::Image>(headPath);
    m_HeadObject = std::make_shared<Util::GameObject>(m_HeadDrawable, zIndex + 0.1f);
    m_HeadObject->m_Transform.translation = {0.0f, 0.0f};

    this->AddChild(m_BodyObject);
    this->AddChild(m_HeadObject);

    UpdateHeadTransform();
}

HeadBodyCharacter::HeadBodyCharacter(const std::string& bodyPath,
                                     const std::vector<std::string>& headPaths,
                                     float zIndex,
                                     std::size_t interval,
                                     bool looping)
    : Character(std::string(), true) {
    m_BodyDrawable = std::make_shared<Util::Image>(bodyPath);
    m_BodyObject = std::make_shared<Util::GameObject>(m_BodyDrawable, zIndex);
    m_BodyObject->m_Transform.translation = {0.0f, 0.0f};

    m_HeadDrawable = std::make_shared<Util::Animation>(headPaths, true, interval, looping, 0);
    m_HeadObject = std::make_shared<Util::GameObject>(m_HeadDrawable, zIndex + 0.1f);
    m_HeadObject->m_Transform.translation = {0.0f, 0.0f};

    this->AddChild(m_BodyObject);
    this->AddChild(m_HeadObject);

    UpdateHeadTransform();
}

HeadBodyCharacter::HeadBodyCharacter(const std::vector<std::string>& bodyPaths,
                                     const std::vector<std::string>& headPaths,
                                     float zIndex,
                                     std::size_t bodyInterval,
                                     bool bodyLooping,
                                     std::size_t headInterval,
                                     bool headLooping)
    : Character(std::string(), true) {
    m_BodyDrawable = std::make_shared<Util::Animation>(bodyPaths, true, bodyInterval, bodyLooping, 0);
    m_BodyObject = std::make_shared<Util::GameObject>(m_BodyDrawable, zIndex);
    m_BodyObject->m_Transform.translation = {0.0f, 0.0f};

    m_HeadDrawable = std::make_shared<Util::Animation>(headPaths, true, headInterval, headLooping, 0);
    m_HeadObject = std::make_shared<Util::GameObject>(m_HeadDrawable, zIndex + 0.1f);
    m_HeadObject->m_Transform.translation = {0.0f, 0.0f};

    this->AddChild(m_BodyObject);
    this->AddChild(m_HeadObject);

    UpdateHeadTransform();
}

// ----------------------------------------------------------------------------
// Runtime configuration
// ----------------------------------------------------------------------------

void HeadBodyCharacter::SetSize(const glm::vec2& size) {
    // Body 使用原尺寸，Head 使用 body 的 0.8 倍
    ApplyDrawableSize(m_BodyDrawable, size);
    ApplyDrawableSize(m_IdleBodyDrawable, size);

    const glm::vec2 headSize = size * 0.8f * m_HeadScale;
    ApplyDrawableSize(m_HeadDrawable, headSize);
    ApplyDrawableSize(m_IdleHeadDrawable, headSize);
    ApplyDrawableSize(m_JumpHeadDrawable, headSize);
    ApplyDrawableSize(m_FallHeadDrawable, headSize);
    ApplyDrawableSize(m_WinHeadDrawable, headSize);

    UpdateHeadTransform();
}

void HeadBodyCharacter::SetBodySize(const glm::vec2& size) {
    ApplyDrawableSize(m_BodyDrawable, size);
    ApplyDrawableSize(m_IdleBodyDrawable, size);
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetHeadOffset(const glm::vec2& offset) {
    m_HeadOffset = offset;
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetHeadScale(float scale) {
    m_HeadScale = scale;

    // 依目前 body 尺寸重新套一次，以更新 head 尺寸
    const glm::vec2 bodySize = GetDrawableSize(m_BodyDrawable);
    if (bodySize.x > 0.0f && bodySize.y > 0.0f) {
        SetSize(bodySize);
    }
}

void HeadBodyCharacter::SetMoveHeadWidthScale(float scale) {
    m_MoveHeadWidthScale = scale;
    UpdateHeadTransform();
    this->SetPosition(this->m_Transform.translation);
}

void HeadBodyCharacter::SetHeadAbsoluteSize(const glm::vec2& size) {
    ApplyDrawableSize(m_HeadDrawable, size);
    ApplyDrawableSize(m_IdleHeadDrawable, size);
    ApplyDrawableSize(m_JumpHeadDrawable, size);
    ApplyDrawableSize(m_FallHeadDrawable, size);
    ApplyDrawableSize(m_WinHeadDrawable, size);
    UpdateHeadTransform();
}

// ----------------------------------------------------------------------------
// Body / Head drawable swap
// ----------------------------------------------------------------------------

void HeadBodyCharacter::SetBodyImage(const std::string& path) {
    m_BodyDrawable = std::make_shared<Util::Image>(path);

    if (m_BodyObject) {
        m_BodyObject->SetDrawable(m_BodyDrawable);
        m_BodyObject->m_Transform.translation = {0.0f, 0.0f};
    } else {
        m_BodyObject = std::make_shared<Util::GameObject>(m_BodyDrawable, this->GetZIndex());
        m_BodyObject->m_Transform.translation = {0.0f, 0.0f};
        this->AddChild(m_BodyObject);
    }

    RefreshDrawables();
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetBodyImage(const std::vector<std::string>& paths,
                                     std::size_t interval,
                                     bool looping) {
    m_BodyDrawable = std::make_shared<Util::Animation>(paths, true, interval, looping, 0);

    if (m_BodyObject) {
        m_BodyObject->SetDrawable(m_BodyDrawable);
        m_BodyObject->m_Transform.translation = {0.0f, 0.0f};
    } else {
        m_BodyObject = std::make_shared<Util::GameObject>(m_BodyDrawable, this->GetZIndex());
        m_BodyObject->m_Transform.translation = {0.0f, 0.0f};
        this->AddChild(m_BodyObject);
    }

    RefreshDrawables();
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetHeadImage(const std::string& path) {
    m_HeadDrawable = std::make_shared<Util::Image>(path);

    if (m_HeadObject) {
        m_HeadObject->SetDrawable(m_HeadDrawable);
        m_HeadObject->m_Transform.translation = {0.0f, 0.0f};
    } else {
        m_HeadObject = std::make_shared<Util::GameObject>(m_HeadDrawable, this->GetZIndex() + 0.1f);
        m_HeadObject->m_Transform.translation = {0.0f, 0.0f};
        this->AddChild(m_HeadObject);
    }

    UpdateHeadTransform();
}

void HeadBodyCharacter::SetHeadImage(const std::vector<std::string>& paths,
                                     std::size_t interval,
                                     bool looping) {
    m_HeadDrawable = std::make_shared<Util::Animation>(paths, true, interval, looping, 0);

    if (m_HeadObject) {
        m_HeadObject->SetDrawable(m_HeadDrawable);
        m_HeadObject->m_Transform.translation = {0.0f, 0.0f};
    } else {
        m_HeadObject = std::make_shared<Util::GameObject>(m_HeadDrawable, this->GetZIndex() + 0.1f);
        m_HeadObject->m_Transform.translation = {0.0f, 0.0f};
        this->AddChild(m_HeadObject);
    }

    UpdateHeadTransform();
}

// ----------------------------------------------------------------------------
// Idle drawable configuration
// ----------------------------------------------------------------------------

void HeadBodyCharacter::SetIdleBodyImage(const std::string& path) {
    m_IdleBodyDrawable = std::make_shared<Util::Image>(path);

    glm::vec2 bodySize = GetDrawableSize(m_BodyDrawable);
    if (bodySize.x <= 0.0f || bodySize.y <= 0.0f) {
        bodySize = this->GetSize();
    }
    ApplyDrawableSize(m_IdleBodyDrawable, bodySize);

    m_UseIdleWhenIdle = true;

    RefreshDrawables();
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetIdleHeadImage(const std::string& path) {
    m_IdleHeadDrawable = std::make_shared<Util::Image>(path);

    const glm::vec2 headSize = GetDrawableSize(m_HeadDrawable);
    if (headSize.x > 0.0f && headSize.y > 0.0f) {
        ApplyDrawableSize(m_IdleHeadDrawable, headSize);
    }

    m_UseIdleWhenIdle = true;

    RefreshDrawables();
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetIdleHeadImage(const std::vector<std::string>& paths,
                                         std::size_t interval,
                                         bool looping) {
    m_IdleHeadDrawable = std::make_shared<Util::Animation>(paths, true, interval, looping, 0);

    const glm::vec2 headSize = GetDrawableSize(m_HeadDrawable);
    if (headSize.x > 0.0f && headSize.y > 0.0f) {
        ApplyDrawableSize(m_IdleHeadDrawable, headSize);
    }

    m_UseIdleWhenIdle = true;

    RefreshDrawables();
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetJumpHeadImage(const std::vector<std::string>& paths,
                                         std::size_t interval,
                                         bool looping) {
    m_JumpHeadDrawable = std::make_shared<Util::Animation>(paths, true, interval, looping, 0);

    const glm::vec2 headSize = GetDrawableSize(m_HeadDrawable);
    if (headSize.x > 0.0f && headSize.y > 0.0f) {
        ApplyDrawableSize(m_JumpHeadDrawable, headSize);
    }

    RefreshDrawables();
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetFallHeadImage(const std::vector<std::string>& paths,
                                         std::size_t interval,
                                         bool looping) {
    m_FallHeadDrawable = std::make_shared<Util::Animation>(paths, true, interval, looping, 0);

    const glm::vec2 headSize = GetDrawableSize(m_HeadDrawable);
    if (headSize.x > 0.0f && headSize.y > 0.0f) {
        ApplyDrawableSize(m_FallHeadDrawable, headSize);
    }

    RefreshDrawables();
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetWinHeadImage(const std::vector<std::string>& paths,
                                        std::size_t interval,
                                        bool looping) {
    m_WinHeadDrawable = std::make_shared<Util::Animation>(paths, true, interval, looping, 0);

    const glm::vec2 headSize = GetDrawableSize(m_HeadDrawable);
    if (headSize.x > 0.0f && headSize.y > 0.0f) {
        ApplyDrawableSize(m_WinHeadDrawable, headSize);
    }

    RefreshDrawables();
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetIdleState(bool idle) {
    if (!m_UseIdleWhenIdle || m_IsIdle == idle) {
        return;
    }

    m_IsIdle = idle;
    SetMotionState(idle ? MotionState::Idle : MotionState::Move);
}

void HeadBodyCharacter::SetMotionState(MotionState state) {
    if (m_MotionState == state) {
        m_IsIdle = (state == MotionState::Idle);
        return;
    }

    m_MotionState = state;
    m_IsIdle = (state == MotionState::Idle);
    RefreshDrawables();
    UpdateHeadTransform();
    this->SetPosition(this->m_Transform.translation);
}

// ----------------------------------------------------------------------------
// Transform
// ----------------------------------------------------------------------------

void HeadBodyCharacter::SetPosition(const glm::vec2& pos) {
    // 1) 更新容器與 body 座標
    this->m_Transform.translation = pos;
    if (m_BodyObject) {
        m_BodyObject->m_Transform.translation = pos;
    }

    // 2) 先算出 head 的相對位移
    UpdateHeadTransform();

    // 3) 再轉成世界座標
    if (m_HeadObject) {
        const glm::vec2 relativeOffset = m_HeadObject->m_Transform.translation;
        m_HeadObject->m_Transform.translation = pos + relativeOffset;
    }
}

void HeadBodyCharacter::SetScale(const glm::vec2& scale) {
    this->m_Transform.scale = scale;

    if (m_BodyObject) {
        m_BodyObject->m_Transform.scale = scale;
    }

    if (m_HeadObject) {
        const float headWidthScale =
            (m_MotionState == MotionState::Move) ? m_MoveHeadWidthScale : 1.0f;
        m_HeadObject->m_Transform.scale = {scale.x * headWidthScale, scale.y};
    }

    // Re-apply head placement so jump/fall visual offsets respond immediately
    // when the character turns in the air.
    this->SetPosition(this->m_Transform.translation);
}

void HeadBodyCharacter::SetHeadRotation(float radians) {
    m_HeadRotation = radians;
    if (m_HeadObject) {
        m_HeadObject->m_Transform.rotation = radians;
    }
}

void HeadBodyCharacter::SetAirborneRunHeadOffsetEnabled(bool enabled) {
    if (m_UseAirborneRunHeadOffset == enabled) {
        return;
    }

    m_UseAirborneRunHeadOffset = enabled;
    this->SetPosition(this->m_Transform.translation);
}

void HeadBodyCharacter::SetLifeState(LifeState state) {
    if (m_LifeState == state) {
        return;
    }

    m_LifeState = state;
    RefreshDrawables();
}

bool HeadBodyCharacter::IsWinAnimationFinished() const {
    if (!m_WinHeadDrawable) {
        return true;
    }

    if (auto anim = std::dynamic_pointer_cast<Util::Animation>(m_WinHeadDrawable)) {
        return anim->GetState() == Util::Animation::State::ENDED;
    }

    return true;
}

glm::vec2 HeadBodyCharacter::GetBodySize() const {
    auto currentBody = GetActiveBodyDrawable();
    if (!currentBody) {
        currentBody = m_BodyDrawable;
    }
    return GetDrawableSize(currentBody);
}

glm::vec2 HeadBodyCharacter::GetHeadSize() const {
    auto currentHead = GetActiveHeadDrawable();
    if (!currentHead) {
        currentHead = m_HeadDrawable;
    }
    return GetDrawableSize(currentHead);
}

glm::vec2 HeadBodyCharacter::GetHeadCenterOffset() const {
    const glm::vec2 bodySize = GetBodySize();
    const glm::vec2 headSize = GetHeadSize();
    return {
        m_HeadOffset.x,
        m_HeadOffset.y + (bodySize.y * 0.5f + headSize.y * 0.5f),
    };
}

// ----------------------------------------------------------------------------
// Internal
// ----------------------------------------------------------------------------

void HeadBodyCharacter::UpdateHeadTransform() {
    if (!m_BodyObject || !m_HeadObject) {
        return;
    }

    // 依目前狀態選擇正在顯示的 drawable
    auto currentBody = GetActiveBodyDrawable();
    auto currentHead = GetActiveHeadDrawable();

    if (!currentBody) {
        currentBody = m_BodyDrawable;
    }
    if (!currentHead) {
        currentHead = m_HeadDrawable;
    }

    if (m_MotionState == MotionState::Win) {
        auto referenceBody = m_IdleBodyDrawable ? m_IdleBodyDrawable : m_BodyDrawable;
        auto referenceHead = m_IdleHeadDrawable ? m_IdleHeadDrawable : m_HeadDrawable;
        if (!referenceBody) {
            referenceBody = currentBody;
        }
        if (!referenceHead) {
            referenceHead = currentHead;
        }

        const glm::vec2 bodySize = GetDrawableSize(referenceBody);
        const glm::vec2 headSize = GetDrawableSize(referenceHead);
        const glm::vec2 headCenter = m_HeadOffset + glm::vec2{
            0.0f,
            (bodySize.y * 0.5f + headSize.y * 0.5f),
        };

        const float left = std::min(-bodySize.x * 0.5f, headCenter.x - headSize.x * 0.5f);
        const float right = std::max(bodySize.x * 0.5f, headCenter.x + headSize.x * 0.5f);
        const float bottom = std::min(-bodySize.y * 0.5f, headCenter.y - headSize.y * 0.5f);
        const float top = std::max(bodySize.y * 0.5f, headCenter.y + headSize.y * 0.5f);
        const glm::vec2 winSize = {right - left, top - bottom};

        ApplyDrawableSize(currentHead, winSize);
        m_HeadObject->m_Transform.translation = {
            (left + right) * 0.5f,
            (bottom + top) * 0.5f,
        };
        m_HeadObject->m_Transform.scale = this->m_Transform.scale;
        m_HeadObject->m_Transform.rotation = 0.0f;
        return;
    }

    const glm::vec2 bodySize = GetDrawableSize(currentBody);
    const glm::vec2 headSize = GetDrawableSize(currentHead);

    // head 相對於 body 的自動偏移（置於 body 上方）
    const glm::vec2 autoOffset = {0.0f, (bodySize.y * 0.5f + headSize.y * 0.5f)};

    glm::vec2 stateOffset = {0.0f, 0.0f};
    if (m_MotionState == MotionState::Jump) {
        stateOffset = m_JumpHeadOffset;
    } else if (m_UseAirborneRunHeadOffset) {
        stateOffset = m_AirborneRunHeadOffset;
    }

    m_HeadObject->m_Transform.translation = m_HeadOffset + autoOffset + stateOffset;

    const float headWidthScale =
        (m_MotionState == MotionState::Move) ? m_MoveHeadWidthScale : 1.0f;
    m_HeadObject->m_Transform.scale = {
        this->m_Transform.scale.x * headWidthScale,
        this->m_Transform.scale.y,
    };
}

void HeadBodyCharacter::RefreshDrawables() {
    if (m_LifeState == LifeState::Dead) {
        if (m_BodyObject) {
            m_BodyObject->SetVisible(false);
        }

        if (m_HeadObject) {
            m_HeadObject->SetVisible(false);
        }

        return;
    }

    if (m_BodyObject) {
        m_BodyObject->SetDrawable(GetActiveBodyDrawable());
        m_BodyObject->SetVisible(m_MotionState != MotionState::Win);
    }

    if (m_HeadObject) {
        m_HeadObject->SetDrawable(GetActiveHeadDrawable());
        m_HeadObject->SetVisible(true);
    }
}

std::shared_ptr<Core::Drawable> HeadBodyCharacter::GetActiveBodyDrawable() const {
    if (m_MotionState != MotionState::Move && m_IdleBodyDrawable) {
        return m_IdleBodyDrawable;
    }
    return m_BodyDrawable;
}

std::shared_ptr<Core::Drawable> HeadBodyCharacter::GetActiveHeadDrawable() const {
    switch (m_MotionState) {
    case MotionState::Idle:
        if (m_IdleHeadDrawable) {
            return m_IdleHeadDrawable;
        }
        break;
    case MotionState::Jump:
        if (m_JumpHeadDrawable) {
            return m_JumpHeadDrawable;
        }
        break;
    case MotionState::Fall:
        if (m_FallHeadDrawable) {
            return m_FallHeadDrawable;
        }
        break;
    case MotionState::Win:
        if (m_WinHeadDrawable) {
            return m_WinHeadDrawable;
        }
        break;
    case MotionState::Move:
        break;
    }

    return m_HeadDrawable;
}
