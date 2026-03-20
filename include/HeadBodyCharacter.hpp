#ifndef HEAD_BODY_CHARACTER_HPP
#define HEAD_BODY_CHARACTER_HPP

#include <string>
#include <memory>
#include <vector>

#include "Character.hpp"
#include "Util/Image.hpp"
#include "Util/Animation.hpp"

class HeadBodyCharacter : public Character {
public:
    // bodyPath: 主體影像路徑, headPath: 單張頭部影像路徑, zIndex: 繪製層級
    explicit HeadBodyCharacter(const std::string &bodyPath,
                               const std::string &headPath,
                               float zIndex = 10.0f);

    // headPaths: 多張頭部影格路徑（例如 idle 資料夾內的所有幀）
    explicit HeadBodyCharacter(const std::string &bodyPath,
                               const std::vector<std::string> &headPaths,
                               float zIndex = 10.0f,
                               std::size_t interval = 100,
                               bool looping = true);

    // body 與 head 都為多張幀（bodyPaths 與 headPaths）
    explicit HeadBodyCharacter(const std::vector<std::string> &bodyPaths,
                               const std::vector<std::string> &headPaths,
                               float zIndex = 10.0f,
                               std::size_t bodyInterval = 100,
                               bool bodyLooping = true,
                               std::size_t headInterval = 100,
                               bool headLooping = true);

    // 設定整體大小（body 與 head 會根據 body 調整）
    void SetSize(const glm::vec2 &size);

    // 設定頭部位移（用來微調位置）
    void SetHeadOffset(const glm::vec2 &offset);

    // 以單張路徑設定 body/head
    void SetBodyImage(const std::string &path);
    void SetHeadImage(const std::string &path);

    // 以多張路徑設定頭部或身體動畫
    void SetHeadImage(const std::vector<std::string> &paths, std::size_t interval = 100, bool looping = true);
    void SetBodyImage(const std::vector<std::string> &paths, std::size_t interval = 100, bool looping = true);

    // 設定頭部縮放（相對於 auto-sizing）
    void SetHeadScale(float scale);

    void SetHeadAbsoluteSize(const glm::vec2 &size);

    // --- 新增：靜止時使用的 body 靜態影像，與切換靜止/移動狀態 ---
    // 設定靜止時要使用的 body 靜態影像（例如 fireboy_body_00）
    void SetIdleBodyImage(const std::string &path);
    // 切換靜止狀態（true = 靜止，false = 移動）
    void SetIdleState(bool idle);

    void SetPosition(const glm::vec2& pos);
    void SetScale(const glm::vec2& scale);

private:
    void UpdateHeadTransform();

private:
    // --- Body 相關 ---
    std::shared_ptr<Core::Drawable> m_BodyDrawable;         // 移動時的 Body (動畫/圖片)
    std::shared_ptr<Util::GameObject> m_BodyObject;
    std::shared_ptr<Core::Drawable> m_IdleBodyDrawable;     // 靜止時的 Body
    std::shared_ptr<Core::Drawable> m_BodyDrawableBackup;   // 切換備份

    // --- Head 相關 ---
    std::shared_ptr<Core::Drawable> m_HeadDrawable;         // 移動時的 Head (動畫/圖片)
    std::shared_ptr<Util::GameObject> m_HeadObject;
    std::shared_ptr<Core::Drawable> m_IdleHeadDrawable;     // 靜止時的 Head (新增)
    std::shared_ptr<Core::Drawable> m_HeadDrawableBackup;   // 切換備份 (新增)

    // --- 參數 ---
    glm::vec2 m_HeadOffset{ 0.0f, 0.0f };
    float m_HeadScale{ 1.0f };
    bool m_UseIdleWhenIdle{ false };
    bool m_IsIdle{ false };
};

#endif // HEAD_BODY_CHARACTER_HPP
