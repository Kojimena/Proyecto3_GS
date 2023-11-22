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

    glm::vec3 delta = rayOrigin + tMin * rayDirection - center;
    glm::vec3 absDelta = glm::abs(delta);

    if (absDelta.x > absDelta.y && absDelta.x > absDelta.z) {
        normal = glm::vec3(delta.x > 0 ? 1 : -1, 0, 0);
    } else if (absDelta.y > absDelta.z) {
        normal = glm::vec3(0, delta.y > 0 ? 1 : -1, 0);
    } else {
        normal = glm::vec3(0, 0, delta.z > 0 ? 1 : -1);
    }

    float tx, ty;
    if (absDelta.x > absDelta.y && absDelta.x > absDelta.z) {
        normal = glm::vec3(delta.x > 0 ? 1 : -1, 0, 0);
        tx = (delta.y + edgeLength / 2) / edgeLength;
        ty = (delta.z + edgeLength / 2) / edgeLength;
    } else if (absDelta.y > absDelta.z) {
        normal = glm::vec3(0, delta.y > 0 ? 1 : -1, 0);
        tx = (delta.x + edgeLength / 2) / edgeLength;
        ty = (delta.z + edgeLength / 2) / edgeLength;
    } else {
        normal = glm::vec3(0, 0, delta.z > 0 ? 1 : -1);
        tx = (delta.x + edgeLength / 2) / edgeLength;
        ty = (delta.y + edgeLength / 2) / edgeLength;
    }

    tx = glm::clamp(tx, 0.0f, 1.0f);
    ty = glm::clamp(ty, 0.0f, 1.0f);

    glm::vec3 point = rayOrigin + tMin * rayDirection;
    return Intersect{true, tMin, point, normal, tx, ty};
}
