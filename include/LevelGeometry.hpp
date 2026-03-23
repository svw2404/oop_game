#ifndef LEVEL_GEOMETRY_HPP
#define LEVEL_GEOMETRY_HPP

#include "pch.hpp"

struct SolidRect {
    glm::vec2 center = {0.0f, 0.0f};
    glm::vec2 size = {0.0f, 0.0f};
    bool blockBottom = true;
};

struct SlopeSurface {
    glm::vec2 start = {0.0f, 0.0f};
    glm::vec2 end = {0.0f, 0.0f};
};

struct CeilingSlopeSurface {
    glm::vec2 start = {0.0f, 0.0f};
    glm::vec2 end = {0.0f, 0.0f};
};

struct HazardRect {
    enum class Type {
        Lava,
        Water,
    };

    glm::vec2 center = {0.0f, 0.0f};
    glm::vec2 size = {0.0f, 0.0f};
    Type type = Type::Lava;
};

#endif // LEVEL_GEOMETRY_HPP
