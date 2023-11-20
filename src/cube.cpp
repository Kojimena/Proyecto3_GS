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

    // Calcula la normal basada en la dirección desde el centro del cubo al punto de intersección
    glm::vec3 delta = rayOrigin + tMin * rayDirection - center;
    float maxComponent = glm::max(glm::abs(delta.x), glm::max(glm::abs(delta.y), glm::abs(delta.z)));
    normal = glm::normalize(delta / maxComponent);

    // Calcula las coordenadas de textura para cualquier cubo en el espacio
    float tx, ty;

// Proyecta las coordenadas del punto de intersección en cada cara del cubo sobre un plano 2D
    glm::vec3 hitPoint = rayOrigin + tMin * rayDirection;
    glm::vec3 localHitPoint = hitPoint - center; // Punto de intersección en coordenadas locales

// Normaliza el punto de intersección local a un rango de [0, 1]
    if (std::abs(normal.x) > 0) {
        tx = (localHitPoint.z / edgeLength + 0.5f);
        ty = (localHitPoint.y / edgeLength + 0.5f);
    } else if (std::abs(normal.y) > 0) {
        tx = (localHitPoint.x / edgeLength + 0.5f);
        ty = (localHitPoint.z / edgeLength + 0.5f);
    } else { // normal.z != 0
        tx = (localHitPoint.x / edgeLength + 0.5f);
        ty = (localHitPoint.y / edgeLength + 0.5f);
    }

// Asegúrate de que las coordenadas UV estén en el rango [0, 1]
    tx = glm::clamp(tx, 0.0f, 1.0f);
    ty = glm::clamp(ty, 0.0f, 1.0f);

    glm::vec3 point = rayOrigin + tMin * rayDirection;
    return Intersect{true, tMin, point, normal, tx, ty};
}
