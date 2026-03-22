#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <string>

#include "Util/GameObject.hpp"

class Character : public Util::GameObject {
public:
    // 建立角色，可選擇先不載入圖片（給衍生類別延後設定）
    explicit Character(const std::string& imagePath, bool skipSetImage = false);

    Character(const Character&) = delete;
    Character(Character&&) = delete;
    Character& operator=(const Character&) = delete;
    Character& operator=(Character&&) = delete;

    // ------------------------------------------------------------------------
    // Query
    // ------------------------------------------------------------------------
    [[nodiscard]] const std::string& GetImagePath() const { return m_ImagePath; }
    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }
    [[nodiscard]] bool GetVisibility() const { return m_Visible; }
    [[nodiscard]] const glm::vec2& GetSize() const { return m_Size; }

    // 簡易距離碰撞判定（半徑固定）
    [[nodiscard]] bool IfCollides(const std::shared_ptr<Character>& other) const {
        const glm::vec2 diff = m_Transform.translation - other->GetPosition();
        const float distance = glm::length(diff);
        return distance < 50.0f;
    }

    // ------------------------------------------------------------------------
    // Update
    // ------------------------------------------------------------------------
    void SetImage(const std::string& imagePath);
    void SetPosition(const glm::vec2& position) { m_Transform.translation = position; }
    void SetSize(const glm::vec2& size);

private:
    void ResetPosition() { m_Transform.translation = {0.0f, 0.0f}; }

private:
    glm::vec2 m_Size = {100.0f, 100.0f};
    std::string m_ImagePath;
};

#endif // CHARACTER_HPP
