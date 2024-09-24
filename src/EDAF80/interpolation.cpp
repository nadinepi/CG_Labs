#include "interpolation.hpp"

glm::vec3
interpolation::evalLERP(glm::vec3 const& p0, glm::vec3 const& p1, float const x) {
    glm::vec3 result = (1 - x) * p0 + x * p1;
    return result;
}

glm::vec3
interpolation::evalCatmullRom(glm::vec3 const& p0, glm::vec3 const& p1,
                              glm::vec3 const& p2, glm::vec3 const& p3,
                              float const t, float const x) {
    glm::vec4 rowMatrix = glm::vec4(1.0f, x, x * x, x * x * x);
    glm::mat4x4 catmullRomMatrix = glm::mat4x4(0.0f, -t, 2 * t, -t,
                                               1.0f, 0.0f, t - 3, 2 - t,
                                               0.0f, t, 3 - 2 * t, t - 2,
                                               0.0f, 0.0f, -t, t);

    glm::mat4x3 controlPointsMatrix = glm::mat4x3(p0, p1, p2, p3);
    glm::mat3x4 controlPointsMatrixTranspose = glm::transpose(controlPointsMatrix);

    glm::vec3 result = rowMatrix * catmullRomMatrix * controlPointsMatrixTranspose;

    return result;
}
