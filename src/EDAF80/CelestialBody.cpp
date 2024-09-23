#include "CelestialBody.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include "core/Log.h"
#include "core/helpers.hpp"

CelestialBody::CelestialBody(bonobo::mesh_data const& shape,
                             GLuint const* program,
                             GLuint diffuse_texture_id) {
    _body.node.set_geometry(shape);
    _body.node.add_texture("diffuse_texture", diffuse_texture_id, GL_TEXTURE_2D);
    _body.node.set_program(program);
}

glm::mat4 CelestialBody::render(std::chrono::microseconds elapsed_time,
                                glm::mat4 const& view_projection,
                                glm::mat4 const& parent_transform,
                                bool show_basis) {
    // Convert the duration from microseconds to seconds.
    auto const elapsed_time_s = std::chrono::duration<float>(elapsed_time).count();
    // If a different ratio was needed, for example a duration in
    // milliseconds, the following would have been used:
    // auto const elapsed_time_ms = std::chrono::duration<float, std::milli>(elapsed_time).count();

    _body.spin.rotation_angle += _body.spin.speed * elapsed_time_s;

    _body.orbit.rotation_angle += _body.orbit.speed * elapsed_time_s;

    glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), _body.scale);

    glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), _body.spin.rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 axial_tilt_matrix = glm::rotate(glm::mat4(1.0f), _body.spin.axial_tilt, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 r_orbit_movement_matrix = glm::rotate(glm::mat4(1.0f), -_body.orbit.rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));


    glm::mat4 orbit_translation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(_body.orbit.radius, 0.0f, 0.0f));

    glm::mat4 orbit_movement_matrix = glm::rotate(glm::mat4(1.0f), _body.orbit.rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 orbit_tilt_matrix = glm::rotate(glm::mat4(1.0f), _body.orbit.inclination, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 children_transform = parent_transform * orbit_tilt_matrix * orbit_movement_matrix * orbit_translation_matrix * axial_tilt_matrix;

    glm::mat4 world = parent_transform * orbit_tilt_matrix * orbit_movement_matrix * orbit_translation_matrix * scaling_matrix * r_orbit_movement_matrix * axial_tilt_matrix * rotation_matrix;

    if (show_basis) {
        bonobo::renderBasis(1.0f, 2.0f, view_projection, world);
    }

    // Note: The second argument of `node::render()` is supposed to be the
    // parent transform of the node, not the whole world matrix, as the
    // node internally manages its local transforms. However in our case we
    // manage all the local transforms ourselves, so the internal transform
    // of the node is just the identity matrix and we can forward the whole
    // world matrix.
    _body.node.render(view_projection, world);

    return children_transform;
}

void CelestialBody::renderAll(std::chrono::microseconds elapsed_time,
                                glm::mat4 const& view_projection,
                                glm::mat4 const& parent_transform,
                                bool show_basis) {

    glm::mat4 new_parent = render(elapsed_time, view_projection, parent_transform, show_basis);
    for (const auto& c : _children) {
        c->renderAll(elapsed_time, view_projection, new_parent, show_basis);
    }
}



void CelestialBody::add_child(CelestialBody* child) {
    _children.push_back(child);
}

std::vector<CelestialBody*> const& CelestialBody::get_children() const {
    return _children;
}

void CelestialBody::set_orbit(OrbitConfiguration const& configuration) {
    _body.orbit.radius = configuration.radius;
    _body.orbit.inclination = configuration.inclination;
    _body.orbit.speed = configuration.speed;
    _body.orbit.rotation_angle = 0.0f;
}

void CelestialBody::set_scale(glm::vec3 const& scale) {
    _body.scale = scale;
}

void CelestialBody::set_spin(SpinConfiguration const& configuration) {
    _body.spin.axial_tilt = configuration.axial_tilt;
    _body.spin.speed = configuration.speed;
    _body.spin.rotation_angle = 0.0f;
}

void CelestialBody::set_ring(bonobo::mesh_data const& shape,
                             GLuint const* program,
                             GLuint diffuse_texture_id,
                             glm::vec2 const& scale) {
    _ring.node.set_geometry(shape);
    _ring.node.add_texture("diffuse_texture", diffuse_texture_id, GL_TEXTURE_2D);
    _ring.node.set_program(program);

    _ring.scale = scale;

    _ring.is_set = true;
}
