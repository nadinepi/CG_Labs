#include "parametric_shapes.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "core/Log.h"

bonobo::mesh_data createCylinder(float const radius, float const length, unsigned int const radial_split_count, unsigned int const length_split_count) {
    bonobo::mesh_data data;

    auto const radial_edge_count = radial_split_count + 1u;
    auto const radial_vertex_count = radial_edge_count + 1u;

    auto const length_edge_count = length_split_count + 1u;
    auto const length_vertex_count = length_edge_count + 1u;

    auto const total_vertices = radial_edge_count * length_edge_count;

    auto vertices = std::vector<glm::vec3>(total_vertices);
    auto index_sets = std::vector<glm::uvec3>(radial_edge_count * length_edge_count * 2u);
    auto normals = std::vector<glm::vec3>(total_vertices);
    auto binormals = std::vector<glm::vec3>(total_vertices);
    auto tangents = std::vector<glm::vec3>(total_vertices);
    auto texcoords = std::vector<glm::vec3>(total_vertices);

    auto const dL = length / static_cast<float>(length_edge_count);
    auto const dt =  glm::two_pi<float>() / static_cast<float>(radial_edge_count);
    size_t index = 0u;
    for(unsigned int t = 0; t < radial_vertex_count; ++t) {
        float cos_theta = std::cos(t * dt);
        float sin_theta = std::sin(t * dt);
        for(unsigned int l = 0; l < length_vertex_count; ++l) {
            vertices[index] = glm::vec3(radius * cos_theta, radius * sin_theta, l * dL);
            normals[index] = glm::vec3(cos_theta, sin_theta, 0.0f);
            tangents[index] = glm::vec3(0.0f, 0.0f, 1.0f);
            binormals[index] = glm::cross(vertices[index], tangents[index]);

            texcoords[index] = glm::vec3(static_cast<float>(t) / static_cast<float>(radial_edge_count), static_cast<float>(l) / static_cast<float>(length_edge_count),0.0f);
            ++index;
        }
    }

    index = 0;
    for(unsigned int t = 0; t < radial_edge_count; t++) {
        for(unsigned int l = 0; l < length_edge_count; l++) {
            index_sets[index] = glm::uvec3(radial_vertex_count * t + l,
                                            radial_vertex_count * (t + 1) + (l + 1),
                                            radial_vertex_count * t + (l + 1));
                                
            index++;

            index_sets[index] = glm::uvec3(radial_vertex_count * t + l,
                                            radial_vertex_count * (t + 1) + l,
                                            radial_vertex_count * (t + 1) + (l + 1));
            
            index++;
        }
    }

    return data;
}


bonobo::mesh_data
parametric_shapes::createQuad(float const width, float const height,
                              unsigned int const horizontal_split_count,
                              unsigned int const vertical_split_count) {
    auto const vertices = std::array<glm::vec3, 4>{
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(width, 0.0f, 0.0f),
        glm::vec3(width, height, 0.0f),
        glm::vec3(0.0f, height, 0.0f)};

    auto const index_sets = std::array<glm::uvec3, 2>{
        glm::uvec3(0u, 1u, 2u),
        glm::uvec3(0u, 2u, 3u)};

    bonobo::mesh_data data;
    // horizontal split count is the # of vertical slices, left to right
    // vertical split count is the # of horizontal slices, top to bottom
   if (horizontal_split_count > 0u || vertical_split_count > 0u) {
        auto const horizontal_edge_count = horizontal_split_count + 1u; 
        auto const vertical_edge_count = vertical_split_count + 1u;
        auto const horizontal_vertex_count = horizontal_edge_count + 1u;
        auto const vertical_vertex_count = vertical_edge_count + 1u;
        auto const total_vertex_count = horizontal_vertex_count * vertical_vertex_count;

        auto vertices_t = std::vector<glm::vec3>(total_vertex_count);
        auto index_sets_t = std::vector<glm::uvec3>(horizontal_edge_count * vertical_edge_count * 2u);
        auto normals = std::vector<glm::vec3>(total_vertex_count);
        auto binormals = std::vector<glm::vec3>(total_vertex_count);
        auto tangents = std::vector<glm::vec3>(total_vertex_count);
        auto texcoords = std::vector<glm::vec3>(total_vertex_count);

        auto const dx = width / horizontal_edge_count;
        auto const dy = height / vertical_edge_count;
        size_t index = 0;   
        for (unsigned int x = 0; x < horizontal_vertex_count; x++) {        
            for (unsigned int y = 0; y < vertical_vertex_count; y++) {
                vertices_t[index] = glm::vec3(x * dx, 0.0f, y * dy);
                normals[index] = glm::vec3(0.0f, 1.0f, 0.0f);
                tangents[index] = glm::vec3(1.0f, 0.0f, 0.0f);
                binormals[index] = glm::cross(normals[index], tangents[index]);
                texcoords[index] = glm::vec3(static_cast<float>(x) / static_cast<float>(horizontal_vertex_count), static_cast<float>(y) / static_cast<float>(vertical_vertex_count), 0.0f);
                ++index;
            }
        }


        index = 0;
        for (unsigned int x = 0; x < horizontal_edge_count; x++) {
            for(unsigned int y = 0; y < vertical_edge_count; y++) {
                index_sets_t[index] = glm::uvec3(vertical_vertex_count * x  +  y, 
                                                vertical_vertex_count * x + y + 1u,
                                                vertical_vertex_count * (x + 1u) + y + 1u);

                ++index;

                index_sets_t[index] = glm::uvec3(vertical_vertex_count * x + y,
                                                vertical_vertex_count * (x + 1u) + (y + 1u),
                                                vertical_vertex_count * (x + 1u) + y);

                ++index;
            }
        }


        glGenVertexArrays(1, &data.vao);
        assert(data.vao != 0u);
        glBindVertexArray(data.vao);

        auto const vertices_offset = 0u;
        auto const vertices_size = static_cast<GLsizeiptr>(vertices_t.size() * sizeof(glm::vec3));
        auto const normals_offset = vertices_size;
        auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
        auto const texcoords_offset = normals_offset + normals_size;
        auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
        auto const tangents_offset = texcoords_offset + texcoords_size;
        auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
        auto const binormals_offset = tangents_offset + tangents_size;
        auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
        auto const bo_size = static_cast<GLsizeiptr>(vertices_size + normals_size + texcoords_size + tangents_size + binormals_size);
        glGenBuffers(1, &data.bo);
        assert(data.bo != 0u);
        glBindBuffer(GL_ARRAY_BUFFER, data.bo);
        glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

        glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices_t.data()));
        glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
        glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

        glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
        glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
        glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

        glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
        glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
        glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

        glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
        glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
        glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));

        glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
        glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
        glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));

        glBindBuffer(GL_ARRAY_BUFFER, 0u);

        data.indices_nb = static_cast<GLsizei>(index_sets_t.size() * 3u);
        glGenBuffers(1, &data.ibo);
        assert(data.ibo != 0u);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets_t.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets_t.data()), GL_STATIC_DRAW);

        glBindVertexArray(0u);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
        return data;
    }

    //
    // NOTE:
    //
    // Only the values preceeded by a `\todo` tag should be changed, the
    // other ones are correct!
    //

    // Create a Vertex Array Object: it will remember where we stored the
    // data on the GPU, and  which part corresponds to the vertices, which
    // one for the normals, etc.
    //
    // The following function will create new Vertex Arrays, and pass their
    // name in the given array (second argument). Since we only need one,
    // pass a pointer to `data.vao`.
    glGenVertexArrays(1, &data.vao);

    // To be able to store information, the Vertex Array has to be bound
    // first.
    glBindVertexArray(data.vao);

    // To store the data, we need to allocate buffers on the GPU. Let's
    // allocate a first one for the vertices.
    //
    // The following function's syntax is similar to `glGenVertexArray()`:
    // it will create multiple OpenGL objects, in this case buffers, and
    // return their names in an array. Have the buffer's name stored into
    // `data.bo`.
    glGenBuffers(1, &data.bo);

    // Similar to the Vertex Array, we need to bind it first before storing
    // anything in it. The data stored in it can be interpreted in
    // different ways. Here, we will say that it is just a simple 1D-array
    // and therefore bind the buffer to the corresponding target.
    glBindBuffer(GL_ARRAY_BUFFER, data.bo);

    auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));

    glBufferData(GL_ARRAY_BUFFER, vertices_size,
                 /* where is the data stored on the CPU? */ vertices.data(),
                 /* inform OpenGL that the data is modified once, but used often */ GL_STATIC_DRAW);

    // Vertices have been just stored into a buffer, but we still need to
    // tell Vertex Array where to find them, and how to interpret the data
    // within that buffer.
    //
    // You will see shaders in more detail in lab 3, but for now they are
    // just pieces of code running on the GPU and responsible for moving
    // all the vertices to clip space, and assigning a colour to each pixel
    // covered by geometry.
    // Those shaders have inputs, some of them are the data we just stored
    // in a buffer object. We need to tell the Vertex Array which inputs
    // are enabled, and this is done by the following line of code, which
    // enables the input for vertices:
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));

    // Once an input is enabled, we need to explain where the data comes
    // from, and how it interpret it. When calling the following function,
    // the Vertex Array will automatically use the current buffer bound to
    // GL_ARRAY_BUFFER as its source for the data. How to interpret it is
    // specified below:
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices),
                          3,
                          /* what is the type of each component? */ GL_FLOAT,
                          /* should it automatically normalise the values stored */ GL_FALSE,
                          /* once all components of a vertex have been read, how far away (in bytes) is the next vertex? */ 0,
                          /* how far away (in bytes) from the start of the buffer is the first vertex? */ reinterpret_cast<GLvoid const*>(0x0));

    // Now, let's allocate a second one for the indices.
    //
    // Have the buffer's name stored into `data.ibo`.
    glGenBuffers(1, &data.ibo);

    // We still want a 1D-array, but this time it should be a 1D-array of
    // elements, aka. indices!
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);

    auto const indices_size = static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size,
                 /* where is the data stored on the CPU? */ index_sets.data(),
                 /* inform OpenGL that the data is modified once, but used often */ GL_STATIC_DRAW);

    data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u);

    // All the data has been recorded, we can unbind them.
    glBindVertexArray(0u);
    glBindBuffer(GL_ARRAY_BUFFER, 0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

    return data;
}

bonobo::mesh_data
parametric_shapes::createSphere(float const radius,
                                unsigned int const longitude_split_count,
                                unsigned int const latitude_split_count) {
    auto const longitude_edges_count = longitude_split_count + 1u;
    auto const latitude_edges_count = latitude_split_count + 1u;
    auto const longitude_vertices_count = longitude_edges_count + 1u;
    auto const latitude_vertices_count = latitude_edges_count + 1u;
    auto const vertices_nb = longitude_vertices_count * latitude_vertices_count;

    auto vertices = std::vector<glm::vec3>(vertices_nb);
    auto normals = std::vector<glm::vec3>(vertices_nb);
    auto texcoords = std::vector<glm::vec3>(vertices_nb);
    auto tangents = std::vector<glm::vec3>(vertices_nb);
    auto binormals = std::vector<glm::vec3>(vertices_nb);

    float const d_theta = glm::two_pi<float>() / (static_cast<float>(longitude_edges_count));
    float const d_phi = glm::pi<float>() / (static_cast<float>(latitude_edges_count));

    size_t index = 0u;
    float phi = 0.0f;
    for (unsigned int i = 0u; i < latitude_vertices_count; ++i) {
        float const cos_phi = std::cos(phi);
        float const sin_phi = std::sin(phi);

        float theta = 0.0f;
        for (unsigned int j = 0u; j < longitude_vertices_count; ++j) {
            float const cos_theta = std::cos(theta);
            float const sin_theta = std::sin(theta);

            // vertex
            vertices[index] = glm::vec3(radius * sin_theta * sin_phi,
                                        -radius * cos_phi,
                                        radius * cos_theta * sin_phi);

            // tangent
            auto const t = glm::vec3(radius * cos_theta * sin_phi, 0.0f, -radius * sin_theta * sin_phi);
            tangents[index] = t;

            // binormal
            auto const b = glm::vec3(radius * sin_theta * cos_phi, radius * sin_phi, radius * cos_theta * cos_phi);
            binormals[index] = b;

            // normal
            auto const n = glm::cross(t, b);
            normals[index] = n;

            // texture coordinates
            texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(longitude_vertices_count)),
                                         static_cast<float>(i) / (static_cast<float>(latitude_vertices_count)),
                                         0.0f);
            theta += d_theta;
            ++index;
        }

        phi += d_phi;
    }

    // create index array
    auto index_sets = std::vector<glm::uvec3>(2u * longitude_edges_count * latitude_edges_count);

    // generate indices it32eratively
    index = 0u;
    for (unsigned int i = 0u; i < latitude_edges_count; ++i) {
        for (unsigned int j = 0u; j < longitude_edges_count; ++j) {
            index_sets[index] = glm::uvec3(longitude_vertices_count * (i) + (j),
                                           longitude_vertices_count * (i) + (j + 1u),
                                           longitude_vertices_count * (i + 1u) + (j + 1u));
            ++index;

            index_sets[index] = glm::uvec3(longitude_vertices_count * (i + 0u) + (j + 0u),
                                           longitude_vertices_count * (i + 1u) + (j + 1u),
                                           longitude_vertices_count * (i + 1u) + (j + 0u));
            ++index;
        }
    }

    bonobo::mesh_data data;
    glGenVertexArrays(1, &data.vao);
    assert(data.vao != 0u);
    glBindVertexArray(data.vao);

    auto const vertices_offset = 0u;
    auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
    auto const normals_offset = vertices_size;
    auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
    auto const texcoords_offset = normals_offset + normals_size;
    auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
    auto const tangents_offset = texcoords_offset + texcoords_size;
    auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
    auto const binormals_offset = tangents_offset + tangents_size;
    auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
    auto const bo_size = static_cast<GLsizeiptr>(vertices_size + normals_size + texcoords_size + tangents_size + binormals_size);
    glGenBuffers(1, &data.bo);
    assert(data.bo != 0u);
    glBindBuffer(GL_ARRAY_BUFFER, data.bo);
    glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

    glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

    glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

    glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));

    glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));

    glBindBuffer(GL_ARRAY_BUFFER, 0u);

    data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u);
    glGenBuffers(1, &data.ibo);
    assert(data.ibo != 0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

    glBindVertexArray(0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

    return data;
}

bonobo::mesh_data
parametric_shapes::createTorus(float const major_radius,
                               float const minor_radius,
                               unsigned int const major_split_count,
                               unsigned int const minor_split_count) {
    //! \todo (Optional) Implement this function
    unsigned int const major_edge_count = major_split_count + 1u;
    unsigned int const major_vertices_count = major_edge_count + 1u;
    unsigned int const minor_edge_count = minor_split_count + 1u;
    unsigned int const minor_vertices_count = minor_edge_count + 1u;
    unsigned int const total_vertices = major_vertices_count * minor_vertices_count;

    auto vertices = std::vector<glm::vec3>(total_vertices);
    auto normals = std::vector<glm::vec3>(total_vertices);
    auto texcoords = std::vector<glm::vec3>(total_vertices);
    auto tangents = std::vector<glm::vec3>(total_vertices);
    auto binormals = std::vector<glm::vec3>(total_vertices);

    float const d_phi = glm::two_pi<float>() / static_cast<float>(major_edge_count);    // change in major angle
    float const d_theta = glm::two_pi<float>() / static_cast<float>(minor_edge_count);  // change in minor angle

    size_t index = 0u;
    float phi = 0.0f;
    for (unsigned int i = 0u; i < major_vertices_count; ++i) {
        float const cos_phi = std::cos(phi);
        float const sin_phi = std::sin(phi);
        float theta = 0.0f;
        for (unsigned int j = 0u; j < minor_vertices_count; ++j) {
            float const cos_theta = std::cos(theta);
            float const sin_theta = std::sin(theta);

            vertices[index] = glm::vec3((major_radius + minor_radius * cos_theta) * cos_phi,
                                        (major_radius + minor_radius * cos_theta) * sin_phi,
                                        -minor_radius * sin_theta);

            tangents[index] = glm::vec3(-minor_radius * sin_theta * cos_phi,
                                        -minor_radius * sin_theta * sin_phi,
                                        -minor_radius * cos_theta);

            binormals[index] = glm::vec3(-(major_radius + minor_radius * cos_theta) * sin_phi,
                                         0,
                                         (major_radius + minor_radius * cos_theta) * cos_phi);

            normals[index] = glm::cross(tangents[index], binormals[index]);

            theta += d_theta;
            ++index;
        }
        phi += d_phi;
    }
    auto index_set = std::vector<glm::uvec3>(2u * major_edge_count * minor_edge_count);
    index = 0u;
    for (unsigned int i = 0; i < major_edge_count; i++) {
        for (unsigned int j = 0; j < minor_edge_count; j++) {
            index_set[index] = glm::uvec3(minor_vertices_count * i + j,
                                          minor_vertices_count * i + j + 1,
                                          minor_vertices_count * (i + 1) + j + 1);
            index++;

            index_set[index] = glm::uvec3(minor_vertices_count * i + j,
                                          minor_vertices_count * (i + 1) + j + 1,
                                          minor_vertices_count * (i + 1) + j);

            index++;
        }
    }

    bonobo::mesh_data data;
    glGenVertexArrays(1, &data.vao);
    assert(data.vao != 0u);
    glBindVertexArray(data.vao);

    auto const vertices_offset = 0u;
    auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
    auto const normals_offset = vertices_size;
    auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
    auto const texcoords_offset = normals_offset + normals_size;
    auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
    auto const tangents_offset = texcoords_offset + texcoords_size;
    auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
    auto const binormals_offset = tangents_offset + tangents_size;
    auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
    auto const bo_size = static_cast<GLsizeiptr>(vertices_size + normals_size + texcoords_size + tangents_size + binormals_size);
    glGenBuffers(1, &data.bo);
    assert(data.bo != 0u);
    glBindBuffer(GL_ARRAY_BUFFER, data.bo);
    glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

    glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

    glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

    glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));

    glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));

    glBindBuffer(GL_ARRAY_BUFFER, 0u);

    data.indices_nb = static_cast<GLsizei>(index_set.size() * 3u);
    glGenBuffers(1, &data.ibo);
    assert(data.ibo != 0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_set.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_set.data()), GL_STATIC_DRAW);

    glBindVertexArray(0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

    return data;
}

bonobo::mesh_data
parametric_shapes::createCircleRing(float const radius,
                                    float const spread_length,
                                    unsigned int const circle_split_count,
                                    unsigned int const spread_split_count) {
    auto const circle_slice_edges_count = circle_split_count + 1u;
    auto const spread_slice_edges_count = spread_split_count + 1u;
    auto const circle_slice_vertices_count = circle_slice_edges_count + 1u;
    auto const spread_slice_vertices_count = spread_slice_edges_count + 1u;
    auto const vertices_nb = circle_slice_vertices_count * spread_slice_vertices_count;

    auto vertices = std::vector<glm::vec3>(vertices_nb);
    auto normals = std::vector<glm::vec3>(vertices_nb);
    auto texcoords = std::vector<glm::vec3>(vertices_nb);
    auto tangents = std::vector<glm::vec3>(vertices_nb);
    auto binormals = std::vector<glm::vec3>(vertices_nb);

    float const spread_start = radius - 0.5f * spread_length;
    float const d_theta = glm::two_pi<float>() / (static_cast<float>(circle_slice_edges_count));
    float const d_spread = spread_length / (static_cast<float>(spread_slice_edges_count));

    // generate vertices iteratively
    size_t index = 0u;
    float theta = 0.0f;
    for (unsigned int i = 0u; i < circle_slice_vertices_count; ++i) {
        float const cos_theta = std::cos(theta);
        float const sin_theta = std::sin(theta);

        float distance_to_centre = spread_start;
        for (unsigned int j = 0u; j < spread_slice_vertices_count; ++j) {
            // vertex
            vertices[index] = glm::vec3(distance_to_centre * cos_theta,
                                        distance_to_centre * sin_theta,
                                        0.0f);

            // texture coordinates
            texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(spread_slice_vertices_count)),
                                         static_cast<float>(i) / (static_cast<float>(circle_slice_vertices_count)),
                                         0.0f);

            // tangent
            auto const t = glm::vec3(cos_theta, sin_theta, 0.0f);
            tangents[index] = t;

            // binormal
            auto const b = glm::vec3(-sin_theta, cos_theta, 0.0f);
            binormals[index] = b;

            // normal
            auto const n = glm::cross(t, b);
            normals[index] = n;

            distance_to_centre += d_spread;
            ++index;
        }

        theta += d_theta;
    }

    // create index array
    auto index_sets = std::vector<glm::uvec3>(2u * circle_slice_edges_count * spread_slice_edges_count);

    // generate indices iteratively
    index = 0u;
    for (unsigned int i = 0u; i < circle_slice_edges_count; ++i) {
        for (unsigned int j = 0u; j < spread_slice_edges_count; ++j) {
            index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
                                           spread_slice_vertices_count * (i + 0u) + (j + 1u),
                                           spread_slice_vertices_count * (i + 1u) + (j + 1u));
            ++index;

            index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
                                           spread_slice_vertices_count * (i + 1u) + (j + 1u),
                                           spread_slice_vertices_count * (i + 1u) + (j + 0u));
            ++index;
        }
    }

    bonobo::mesh_data data;
    glGenVertexArrays(1, &data.vao);
    assert(data.vao != 0u);
    glBindVertexArray(data.vao);

    auto const vertices_offset = 0u;
    auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
    auto const normals_offset = vertices_size;
    auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
    auto const texcoords_offset = normals_offset + normals_size;
    auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
    auto const tangents_offset = texcoords_offset + texcoords_size;
    auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
    auto const binormals_offset = tangents_offset + tangents_size;
    auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
    auto const bo_size = static_cast<GLsizeiptr>(vertices_size + normals_size + texcoords_size + tangents_size + binormals_size);
    glGenBuffers(1, &data.bo);
    assert(data.bo != 0u);
    glBindBuffer(GL_ARRAY_BUFFER, data.bo);
    glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

    glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

    glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

    glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));

    glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
    glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
    glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));

    glBindBuffer(GL_ARRAY_BUFFER, 0u);

    data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u);
    glGenBuffers(1, &data.ibo);
    assert(data.ibo != 0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

    glBindVertexArray(0u);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

    return data;
}
