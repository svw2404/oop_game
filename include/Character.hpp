#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <string>

#include "Util/GameObject.hpp"

class Character : public Util::GameObject {
public:
    explicit Character(const std::string& ImagePath, bool skipSetImage = false);

    Character(const Character&) = delete;

    Character(Character&&) = delete;

    Character& operator=(const Character&) = delete;

    Character& operator=(Character&&) = delete;

    [[nodiscard]] const std::string& GetImagePath() const { return m_ImagePath; }

    [[nodiscard]] const glm::vec2& GetPosition() const { return m_Transform.translation; }

    [[nodiscard]] bool GetVisibility() const { return m_Visible; }

    void SetImage(const std::string& ImagePath);

    void SetPosition(const glm::vec2& Position) { m_Transform.translation = Position; }

    [[nodiscard]] bool IfCollides(const std::shared_ptr<Character>& other) const {
        glm::vec2 diff = m_Transform.translation - other->GetPosition();
        float distance = glm::length(diff);
        return distance < 50.0f;  
    }

    [[nodiscard]] const glm::vec2& GetSize() const { return m_Size; }
    void SetSize(const glm::vec2& size);

private:
    void ResetPosition() { m_Transform.translation = {0, 0}; }
    glm::vec2 m_Size = { 100.0f, 100.0f };
    std::string m_ImagePath;

};


#endif //CHARACTER_HPP
