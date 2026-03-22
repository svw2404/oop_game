#ifndef HEAD_BODY_CHARACTER_HPP
#define HEAD_BODY_CHARACTER_HPP

#include <memory>
#include <string>
#include <vector>

#include "Character.hpp"
#include "Util/Animation.hpp"
#include "Util/Image.hpp"

class HeadBodyCharacter : public Character {
public:
    // ------------------------------------------------------------------------
    // Constructors
    // ------------------------------------------------------------------------

    // bodyPath: 身體單張圖片, headPath: 頭部單張圖片
    explicit HeadBodyCharacter(const std::string& bodyPath,
                               const std::string& headPath,
                               float zIndex = 10.0f);

    // bodyPath: 身體單張圖片, headPaths: 頭部動畫影格
    explicit HeadBodyCharacter(const std::string& bodyPath,
                               const std::vector<std::string>& headPaths,
                               float zIndex = 10.0f,
                               std::size_t interval = 100,
                               bool looping = true);

    // bodyPaths/headPaths: 身體與頭部動畫影格
    explicit HeadBodyCharacter(const std::vector<std::string>& bodyPaths,
                               const std::vector<std::string>& headPaths,
                               float zIndex = 10.0f,
                               std::size_t bodyInterval = 100,
                               bool bodyLooping = true,
                               std::size_t headInterval = 100,
                               bool headLooping = true);

    // ------------------------------------------------------------------------
    // Runtime configuration
    // ------------------------------------------------------------------------

    // 設定整體尺寸（body 為基準，head 依比例同步）
    void SetSize(const glm::vec2& size);

    // 設定頭部相對 body 的偏移
    void SetHeadOffset(const glm::vec2& offset);

    // 設定頭部縮放倍率（相對於自動尺寸）
    void SetHeadScale(float scale);

    // 直接設定頭部絕對尺寸
    void SetHeadAbsoluteSize(const glm::vec2& size);

    // ------------------------------------------------------------------------
    // Body / Head drawable swap
    // ------------------------------------------------------------------------

    void SetBodyImage(const std::string& path);
    void SetBodyImage(const std::vector<std::string>& paths,
                      std::size_t interval = 100,
                      bool looping = true);

    void SetHeadImage(const std::string& path);
    void SetHeadImage(const std::vector<std::string>& paths,
                      std::size_t interval = 100,
                      bool looping = true);

    // ------------------------------------------------------------------------
    // Idle drawable configuration
    // ------------------------------------------------------------------------

    void SetIdleBodyImage(const std::string& path);

    void SetIdleHeadImage(const std::string& path);
    void SetIdleHeadImage(const std::vector<std::string>& paths,
                          std::size_t interval = 100,
                          bool looping = true);

    // true = idle, false = moving
    void SetIdleState(bool idle);

    // ------------------------------------------------------------------------
    // Transform
    // ------------------------------------------------------------------------

    void SetPosition(const glm::vec2& pos);
    void SetScale(const glm::vec2& scale);

private:
    // 重新計算 head 的相對位置
    void UpdateHeadTransform();

private:
    // Body
    std::shared_ptr<Core::Drawable> m_BodyDrawable;
    std::shared_ptr<Util::GameObject> m_BodyObject;
    std::shared_ptr<Core::Drawable> m_IdleBodyDrawable;
    std::shared_ptr<Core::Drawable> m_BodyDrawableBackup;

    // Head
    std::shared_ptr<Core::Drawable> m_HeadDrawable;
    std::shared_ptr<Util::GameObject> m_HeadObject;
    std::shared_ptr<Core::Drawable> m_IdleHeadDrawable;
    std::shared_ptr<Core::Drawable> m_HeadDrawableBackup;

    // Runtime flags / parameters
    glm::vec2 m_HeadOffset{0.0f, 0.0f};
    float m_HeadScale{1.0f};
    bool m_UseIdleWhenIdle{false};
    bool m_IsIdle{false};
};

#endif // HEAD_BODY_CHARACTER_HPP
