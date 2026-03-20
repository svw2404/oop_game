#include "HeadBodyCharacter.hpp"

HeadBodyCharacter::HeadBodyCharacter(const std::string &bodyPath,
                                     const std::string &headPath,
                                     float zIndex)
    : Character(std::string(), true)
{
    // 建立 body drawable 與 body game object（body 為 parent）
    m_BodyDrawable = std::make_shared<Util::Image>(bodyPath);
    m_BodyObject = std::make_shared<Util::GameObject>(m_BodyDrawable, zIndex);
    m_BodyObject->m_Transform.translation = {0.0f, 0.0f};

    // 建立 head drawable 與 head game object（head 為 body 的 child）
    m_HeadDrawable = std::make_shared<Util::Image>(headPath);
    m_HeadObject = std::make_shared<Util::GameObject>(m_HeadDrawable, zIndex + 0.1f);
    m_HeadObject->m_Transform.translation = {0.0f, 0.0f};

    this->AddChild(m_BodyObject);
    this->AddChild(m_HeadObject);

    UpdateHeadTransform();
}

HeadBodyCharacter::HeadBodyCharacter(const std::string &bodyPath,
                                     const std::vector<std::string> &headPaths,
                                     float zIndex,
                                     std::size_t interval,
                                     bool looping)
    : Character(std::string(), true)
{
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

HeadBodyCharacter::HeadBodyCharacter(const std::vector<std::string> &bodyPaths,
                                     const std::vector<std::string> &headPaths,
                                     float zIndex,
                                     std::size_t bodyInterval,
                                     bool bodyLooping,
                                     std::size_t headInterval,
                                     bool headLooping)
    : Character(std::string(), true)
{
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

void HeadBodyCharacter::SetSize(const glm::vec2& size) {
    // 定義一個內部的小工具來設定尺寸
    auto syncSize = [&](std::shared_ptr<Core::Drawable> drawable, glm::vec2 target) {
        if (!drawable) return;
        // 必須確保 drawable 是 std::shared_ptr<Core::Drawable>
        if (auto img = std::dynamic_pointer_cast<Util::Image>(drawable)) {
            img->SetSize(target);
        }
        else if (auto anim = std::dynamic_pointer_cast<Util::Animation>(drawable)) {
            anim->SetSize(target);
        }
        };

    // 設定身體
    syncSize(m_BodyDrawable, size);
    syncSize(m_IdleBodyDrawable, size);

    // 設定頭部 (例如身體的 0.8 倍)
    glm::vec2 headSize = size * 0.8f;
    syncSize(m_HeadDrawable, headSize);
    syncSize(m_IdleHeadDrawable, headSize);

    UpdateHeadTransform();
}

void HeadBodyCharacter::SetHeadOffset(const glm::vec2 &offset) {
    m_HeadOffset = offset;
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetBodyImage(const std::string &path) {
    m_BodyDrawable = std::make_shared<Util::Image>(path);
    if (m_BodyObject) {
        m_BodyObject->SetDrawable(m_BodyDrawable);
        m_BodyObject->m_Transform.translation = {0.0f, 0.0f};
    } else {
        m_BodyObject = std::make_shared<Util::GameObject>(m_BodyDrawable, this->GetZIndex());
        m_BodyObject->m_Transform.translation = {0.0f, 0.0f};
        this->AddChild(m_BodyObject);
    }

    // 若 head 已存在，確保 head 為 body 的 child
    if (m_HeadObject) {
        this->AddChild(m_HeadObject);
    }

    UpdateHeadTransform();
}

void HeadBodyCharacter::SetHeadImage(const std::string &path) {
    m_HeadDrawable = std::make_shared<Util::Image>(path);
    if (m_HeadObject) {
        m_HeadObject->SetDrawable(m_HeadDrawable);
        m_HeadObject->m_Transform.translation = {0.0f, 0.0f};
    } else {
        m_HeadObject = std::make_shared<Util::GameObject>(m_HeadDrawable, this->GetZIndex() + 0.1f);
        m_HeadObject->m_Transform.translation = {0.0f, 0.0f};
        if (m_BodyObject)
            this->AddChild(m_HeadObject);
        else
            this->AddChild(m_HeadObject);
    }
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetHeadImage(const std::vector<std::string> &paths,
                                     std::size_t interval,
                                     bool looping)
{
    m_HeadDrawable = std::make_shared<Util::Animation>(paths, true, interval, looping, 0);
    if (m_HeadObject) {
        m_HeadObject->SetDrawable(m_HeadDrawable);
        m_HeadObject->m_Transform.translation = {0.0f, 0.0f};
    } else {
        m_HeadObject = std::make_shared<Util::GameObject>(m_HeadDrawable, this->GetZIndex() + 0.1f);
        m_HeadObject->m_Transform.translation = {0.0f, 0.0f};
        if (m_BodyObject)
            this->AddChild(m_HeadObject);
        else
            this->AddChild(m_HeadObject);
    }
    UpdateHeadTransform();
}

void HeadBodyCharacter::SetBodyImage(const std::vector<std::string> &paths,
                                     std::size_t interval,
                                     bool looping)
{
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

void HeadBodyCharacter::SetHeadScale(float scale) {
    m_HeadScale = scale;
    if (m_BodyDrawable) {
        // 重新套用目前 body 大小以更新 head scale
        glm::vec2 bodySize{0.0f, 0.0f};
        if (auto img = std::dynamic_pointer_cast<Util::Image>(m_BodyDrawable)) {
            bodySize = img->GetSize();
        } else if (auto anim = std::dynamic_pointer_cast<Util::Animation>(m_BodyDrawable)) {
            bodySize = anim->GetSize();
        }
        if (bodySize.x > 0.0f) {
            SetSize(bodySize);
        }
    }
}

// head 的 local transform（head 為 body 的 child，因此此處只設定 local translation）
void HeadBodyCharacter::UpdateHeadTransform() {
    if (!m_BodyObject || !m_HeadObject) return;

    // 取得當前 Body 和 Head 的 Drawable (根據 Idle 狀態)
    auto currentBody = m_IsIdle ? m_IdleBodyDrawable : m_BodyDrawable;
    auto currentHead = m_IsIdle ? m_IdleHeadDrawable : m_HeadDrawable;
    if (!currentBody) currentBody = m_BodyDrawable;
    if (!currentHead) currentHead = m_HeadDrawable;

    glm::vec2 bSize{ 0, 0 }, hSize{ 0, 0 };

    if (auto img = std::dynamic_pointer_cast<Util::Image>(currentBody)) bSize = img->GetSize();
    else if (auto anim = std::dynamic_pointer_cast<Util::Animation>(currentBody)) bSize = anim->GetSize();

    if (auto img = std::dynamic_pointer_cast<Util::Image>(currentHead)) hSize = img->GetSize();
    else if (auto anim = std::dynamic_pointer_cast<Util::Animation>(currentHead)) hSize = anim->GetSize();

    // 計算偏移量 (相對位移)
    // 這裡我們暫時把結果存在 m_HeadOffset 以外的局部變數
    glm::vec2 autoOffset = { 0.0f, (bSize.y * 0.5f + hSize.y * 0.5f) };

    // 重要：我們在 SetPosition 裡才會真正用到這個數值
    // 這裡先把相對位移存進 m_HeadObject 的 translation 備用
    m_HeadObject->m_Transform.translation = m_HeadOffset + autoOffset;
}

void HeadBodyCharacter::SetHeadAbsoluteSize(const glm::vec2 &size) {
    if (!m_HeadDrawable)
        return;

    if (auto img = std::dynamic_pointer_cast<Util::Image>(m_HeadDrawable)) {
        img->SetSize(size);
    } else if (auto anim = std::dynamic_pointer_cast<Util::Animation>(m_HeadDrawable)) {
        anim->SetSize(size);
    }
    UpdateHeadTransform();
}

// 設定靜止時要顯示的 body 圖（idle）
void HeadBodyCharacter::SetIdleBodyImage(const std::string &path) {
    m_IdleBodyDrawable = std::make_shared<Util::Image>(path);

    // 以目前 body 大小調整 idle drawable，避免顯示過大或過小
    glm::vec2 bodySize{0.0f, 0.0f};
    if (m_BodyDrawable) {
        if (auto img = std::dynamic_pointer_cast<Util::Image>(m_BodyDrawable)) {
            bodySize = img->GetSize();
        } else if (auto anim = std::dynamic_pointer_cast<Util::Animation>(m_BodyDrawable)) {
            bodySize = anim->GetSize();
        }
    }
    if (bodySize.x <= 0.0f || bodySize.y <= 0.0f) {
        bodySize = this->GetSize();
    }

    if (auto imgIdle = std::dynamic_pointer_cast<Util::Image>(m_IdleBodyDrawable)) {
        imgIdle->SetSize(bodySize);
    } else if (auto animIdle = std::dynamic_pointer_cast<Util::Animation>(m_IdleBodyDrawable)) {
        animIdle->SetSize(bodySize);
    }

    m_UseIdleWhenIdle = true;

    if (m_IsIdle && m_BodyObject) {
        m_BodyObject->SetDrawable(m_IdleBodyDrawable);
        UpdateHeadTransform();
    }
}

void HeadBodyCharacter::SetPosition(const glm::vec2& pos) {
    // 1. 更新容器座標
    this->m_Transform.translation = pos;

    // 2. 更新身體座標
    if (m_BodyObject) {
        m_BodyObject->m_Transform.translation = pos;
    }

    // 3. 更新偏移量計算 (會更新 m_HeadObject->m_Transform.translation 存入相對位移)
    UpdateHeadTransform();

    // 4. 強制對位：頭部位置 = 身體位置 + 相對位移
    if (m_HeadObject) {
        glm::vec2 relativeOffset = m_HeadObject->m_Transform.translation;
        m_HeadObject->m_Transform.translation = pos + relativeOffset;
    }
}

void HeadBodyCharacter::SetScale(const glm::vec2& scale) {
    this->m_Transform.scale = scale;
    if (m_BodyObject) {
        m_BodyObject->m_Transform.scale = scale;
    }
}

// 切換靜止 / 移動（由外部呼叫）
void HeadBodyCharacter::SetIdleState(bool idle) {
    if (!m_UseIdleWhenIdle || m_IsIdle == idle) return;
    m_IsIdle = idle;

    if (idle) {
        // --- 處理 Body ---
        m_BodyDrawableBackup = m_BodyDrawable;
        if (m_IdleBodyDrawable && m_BodyObject) {
            m_BodyObject->SetDrawable(m_IdleBodyDrawable);
        }

        // --- 處理 Head (預留未來切換) ---
        m_HeadDrawableBackup = m_HeadDrawable;
        if (m_IdleHeadDrawable && m_HeadObject) {
            m_HeadObject->SetDrawable(m_IdleHeadDrawable);
        }
    }
    else {
        // --- 還原 Body ---
        if (m_BodyDrawableBackup && m_BodyObject) {
            m_BodyObject->SetDrawable(m_BodyDrawableBackup);
        }

        // --- 還原 Head ---
        if (m_HeadDrawableBackup && m_HeadObject) {
            m_HeadObject->SetDrawable(m_HeadDrawableBackup);
        }
    }

    // 換完圖片後，座標和縮放可能會跑掉，立刻重新對齊
    UpdateHeadTransform();
    this->SetPosition(this->m_Transform.translation);
}
