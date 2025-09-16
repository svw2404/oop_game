#include "Character.hpp"
#include "Util/Image.hpp"


Character::Character(const std::string& ImagePath, bool skipSetImage) {
    if (!skipSetImage && !ImagePath.empty()) {
        this->SetImage(ImagePath);       
    }
    this->ResetPosition();               
}


void Character::SetImage(const std::string& ImagePath) {
    m_ImagePath = ImagePath;

    m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
}
