#include "cube.h"

Cube::Cube(const glm::vec3& center, float edgeLength, const Material& mat)
        : center(center), edgeLength(edgeLength), Object(mat) {}

Intersect Cube::rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const {
    float tMin = 0.0f, tMax = std::numeric_limits<float>::max();
    glm::vec3 bounds[2];
    bounds[0] = center - glm::vec3(edgeLength / 2.0f);
    bounds[1] = center + glm::vec3(edgeLength / 2.0f);
    glm::vec3 normal;

    for (int i = 0; i < 3; ++i) {
        float invD = 1.0f / rayDirection[i];
        float t0 = (bounds[0][i] - rayOrigin[i]) * invD;
        float t1 = (bounds[1][i] - rayOrigin[i]) * invD;
        if (invD < 0.0f) {
            std::swap(t0, t1);
        }
        tMin = t0 > tMin ? t0 : tMin;
        tMax = t1 < tMax ? t1 : tMax;
        if (tMax <= tMin) {
            return Intersect{false};
        }
    }

    float epsilon = 0.001f; // Un pequeño valor para evitar errores de precisión

    // Calcula la normal basada en la intersección más cercana
    if (std::abs(tMin - ((bounds[0].x - rayOrigin.x) / rayDirection.x)) < epsilon) {
        normal = glm::vec3(-1, 0, 0);
    } else if (std::abs(tMin - ((bounds[1].x - rayOrigin.x) / rayDirection.x)) < epsilon) {
        normal = glm::vec3(1, 0, 0);
    } else if (std::abs(tMin - ((bounds[0].y - rayOrigin.y) / rayDirection.y)) < epsilon) {
        normal = glm::vec3(0, -1, 0);
    } else if (std::abs(tMin - ((bounds[1].y - rayOrigin.y) / rayDirection.y)) < epsilon) {
        normal = glm::vec3(0, 1, 0);
    } else if (std::abs(tMin - ((bounds[0].z - rayOrigin.z) / rayDirection.z)) < epsilon) {
        normal = glm::vec3(0, 0, -1);
    } else if (std::abs(tMin - ((bounds[1].z - rayOrigin.z) / rayDirection.z)) < epsilon) {
        normal = glm::vec3(0, 0, 1);
    }

    glm::vec3 point = rayOrigin + tMin * rayDirection;
    return Intersect{true, tMin, point, normal};
}
