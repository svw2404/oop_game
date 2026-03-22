#include "HeadBodyCharacter.hpp"

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

void HeadBodyCharacter::SetHeadAbsoluteSize(const glm::vec2& size) {
    ApplyDrawableSize(m_HeadDrawable, size);
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

    if (m_IsIdle && m_BodyObject) {
        m_BodyObject->SetDrawable(m_IdleBodyDrawable);
        UpdateHeadTransform();
    }
}

void HeadBodyCharacter::SetIdleHeadImage(const std::string& path) {
    m_IdleHeadDrawable = std::make_shared<Util::Image>(path);

    const glm::vec2 headSize = GetDrawableSize(m_HeadDrawable);
    if (headSize.x > 0.0f && headSize.y > 0.0f) {
        ApplyDrawableSize(m_IdleHeadDrawable, headSize);
    }

    m_UseIdleWhenIdle = true;

    if (m_IsIdle && m_HeadObject) {
        m_HeadObject->SetDrawable(m_IdleHeadDrawable);
        UpdateHeadTransform();
    }
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

    if (m_IsIdle && m_HeadObject) {
        m_HeadObject->SetDrawable(m_IdleHeadDrawable);
        UpdateHeadTransform();
    }
}

void HeadBodyCharacter::SetIdleState(bool idle) {
    if (!m_UseIdleWhenIdle || m_IsIdle == idle) {
        return;
    }

    m_IsIdle = idle;

    if (idle) {
        // 切到 idle drawable
        m_BodyDrawableBackup = m_BodyDrawable;
        m_HeadDrawableBackup = m_HeadDrawable;

        if (m_IdleBodyDrawable && m_BodyObject) {
            m_BodyObject->SetDrawable(m_IdleBodyDrawable);
        }
        if (m_IdleHeadDrawable && m_HeadObject) {
            m_HeadObject->SetDrawable(m_IdleHeadDrawable);
        }
    } else {
        // 還原移動 drawable
        if (m_BodyDrawableBackup && m_BodyObject) {
            m_BodyObject->SetDrawable(m_BodyDrawableBackup);
        }
        if (m_HeadDrawableBackup && m_HeadObject) {
            m_HeadObject->SetDrawable(m_HeadDrawableBackup);
        }
    }

    // 切換後重新對齊
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
        m_HeadObject->m_Transform.scale = scale;
    }
}

// ----------------------------------------------------------------------------
// Internal
// ----------------------------------------------------------------------------

void HeadBodyCharacter::UpdateHeadTransform() {
    if (!m_BodyObject || !m_HeadObject) {
        return;
    }

    // 依目前狀態選擇正在顯示的 drawable
    auto currentBody = m_IsIdle ? m_IdleBodyDrawable : m_BodyDrawable;
    auto currentHead = m_IsIdle ? m_IdleHeadDrawable : m_HeadDrawable;

    if (!currentBody) {
        currentBody = m_BodyDrawable;
    }
    if (!currentHead) {
        currentHead = m_HeadDrawable;
    }

    const glm::vec2 bodySize = GetDrawableSize(currentBody);
    const glm::vec2 headSize = GetDrawableSize(currentHead);

    // head 相對於 body 的自動偏移（置於 body 上方）
    const glm::vec2 autoOffset = {0.0f, (bodySize.y * 0.5f + headSize.y * 0.5f)};
    m_HeadObject->m_Transform.translation = m_HeadOffset + autoOffset;
}
