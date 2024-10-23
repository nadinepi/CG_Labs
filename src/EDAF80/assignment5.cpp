#include "assignment5.hpp"

#include <imgui.h>
#include <tinyfiledialogs.h>

#include <clocale>
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <list>
#include <stdexcept>

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/ShaderProgramManager.hpp"
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "parametric_shapes.hpp"

struct WormSegment {
    glm::vec3 pos;
    glm::vec3 velocity;

    WormSegment(glm::vec3 pos) {
        this->pos = pos;
        this->velocity = glm::vec3(0.0, 0.0, 0.0);
    }
};
struct Planet {
    Node node;
    glm::vec3 position;
    float radius;
};

edaf80::Assignment5::Assignment5(WindowManager& windowManager) : mCamera(0.5f * glm::half_pi<float>(),
                                                                         static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
                                                                         0.01f, 1000.0f),
                                                                 inputHandler(),
                                                                 mWindowManager(windowManager),
                                                                 window(nullptr) {
    WindowManager::WindowDatum window_datum{inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};

    window = mWindowManager.CreateGLFWWindow("EDAF80: Assignment 5", window_datum, config::msaa_rate);
    if (window == nullptr) {
        throw std::runtime_error("Failed to get a window: aborting!");
    }

    bonobo::init();
}

edaf80::Assignment5::~Assignment5() {
    bonobo::deinit();
}

void edaf80::Assignment5::run() {
    // Set up the camera
    mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 6.0f));
    mCamera.mMouseSensitivity = glm::vec2(0.003f);
    mCamera.mMovementSpeed = glm::vec3(3.0f);  // 3 m/s => 10.8 km/h
    auto camera_position = mCamera.mWorld.GetTranslation();
    auto light_position = glm::vec3(0.0f, 6.0f, 5.0f);
    float elapsed_time_s = 0.0f;
    int ate = 0;

    // Create the shader programs
    ShaderProgramManager program_manager;
    GLuint fallback_shader = 0u;
    program_manager.CreateAndRegisterProgram("Fallback",
                                             {{ShaderType::vertex, "common/fallback.vert"},
                                              {ShaderType::fragment, "common/fallback.frag"}},
                                             fallback_shader);
    if (fallback_shader == 0u) {
        LogError("Failed to load fallback shader");
        return;
    }

    GLuint planet_shader = 0u;
    program_manager.CreateAndRegisterProgram("Celestial Body",
                                             {{ShaderType::vertex, "EDAF80/planet.vert"},
                                              {ShaderType::fragment, "EDAF80/planet.frag"}},
                                             planet_shader);
    if (planet_shader == 0u) {
        LogError("Failed to generate the “Celestial Body” shader program: exiting.");
        return;
    }
    //
    // Todo: Insert the creation of other shader programs.
    //       (Check how it was done in assignment 3.)
    //

    GLuint phong_shader = 0u;
    program_manager.CreateAndRegisterProgram("Phong",
                                             {{ShaderType::vertex, "EDAF80/phong.vert"},
                                              {ShaderType::fragment, "EDAF80/phong.frag"}},
                                             phong_shader);

    if (phong_shader == 0u)
        LogError("Failed to load phong shader");

    GLuint tube_shader = 0u;
    program_manager.CreateAndRegisterProgram("Tube",
                                             {{ShaderType::vertex, "EDAF80/tube.vert"},
                                              {ShaderType::fragment, "EDAF80/tube.frag"}},
                                             tube_shader);
    if (fallback_shader == 0u) {
        LogError("Failed to load fallback shader");
        return;
    }

    auto const tube_set_uniforms = [&elapsed_time_s, &ate](GLuint program) {
        glUniform1f(glGetUniformLocation(program, "elapsed_time_s"), elapsed_time_s);
        glUniform1i(glGetUniformLocation(program, "ate"), ate);
    };

    //
    // Todo: Load your geometry
    //
    glm::vec3 ambient = glm::vec3(222.0f / 255.0f, 4.0f / 255.0f, 131.0f / 255.0f) * .25f;
    float shininess = 10.0f;
    auto const phong_set_uniforms = [&light_position, &camera_position, &ambient, &shininess](GLuint program) {
        glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
        glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
        glUniform3fv(glGetUniformLocation(program, "ambient"), 1, glm::value_ptr(ambient));
        glUniform1f(glGetUniformLocation(program, "shininess"), shininess);
    };

    GLuint leather_texture = bonobo::loadTexture2D(config::resources_path("textures/leather_red_02_coll1_2k.jpg"));
    GLuint leather_normal_map = bonobo::loadTexture2D(config::resources_path("textures/leather_red_02_nor_2k.jpg"));
    GLuint leather_rough_map = bonobo::loadTexture2D(config::resources_path("textures/leather_red_02_rough_2k.jpg"));

    auto sphere = parametric_shapes::createSphere(0.25f, 30u, 30u);
    glm::vec3 player_position = glm::vec3(0.0f, -0.75f, 2.0f);
    glm::vec3 player_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    float segment_distance = .4f;
    float player_speed = 5.0f;
    float player_radius = 0.25f;
    std::vector<Node> worm;
    std::vector<WormSegment> segments;

    Node s;
    for (int i = 0; i < 10; i++) {
        Node s = Node();
        s.set_geometry(sphere);
        s.set_program(&phong_shader, phong_set_uniforms);
        worm.push_back(s);
        WormSegment ws = WormSegment(player_position + static_cast<float>(i) * glm::vec3(0.0f, 0.0f, segment_distance));
        segments.push_back(ws);
    }

    for (int i = 0; i < worm.size(); i++) {
        worm[i].add_texture("texture_map", leather_texture, GL_TEXTURE_2D);
        worm[i].add_texture("normal_map", leather_normal_map, GL_TEXTURE_2D);
        worm[i].add_texture("roughness_map", leather_rough_map, GL_TEXTURE_2D);
    }

    auto cylinder = parametric_shapes::createCylinder(5.0f, 15000.0f, 100u, 100u);
    auto hole = Node();
    hole.set_geometry(cylinder);
    hole.set_program(&tube_shader, tube_set_uniforms);

    glClearDepthf(1.0f);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    auto lastTime = std::chrono::high_resolution_clock::now();

    bool show_logs = true;
    bool show_gui = true;
    bool shader_reload_failed = false;
    bool show_basis = false;
    float basis_thickness_scale = 1.0f;
    float basis_length_scale = 1.0f;

    std::vector<Planet> planets;
    float radius = 1.0f;

    // Initialize planets
    auto earth_sphere = parametric_shapes::createSphere(0.25f, 30u, 30u);
    auto mercury_sphere = parametric_shapes::createSphere(0.1f, 30u, 30u);
    auto venus_sphere = parametric_shapes::createSphere(0.15f, 30u, 30u);
    auto mars_sphere = parametric_shapes::createSphere(0.2f, 30u, 30u);
    auto jupiter_sphere = parametric_shapes::createSphere(0.5f, 30u, 30u);
    auto saturn_sphere = parametric_shapes::createSphere(0.4f, 30u, 30u);
    auto uranus_sphere = parametric_shapes::createSphere(0.3f, 30u, 30u);
    auto neptune_sphere = parametric_shapes::createSphere(0.3f, 30u, 30u);

    GLuint earth_texture = bonobo::loadTexture2D(config::resources_path("planets/2k_earth_daymap.jpg"));
    GLuint mercury_texture = bonobo::loadTexture2D(config::resources_path("planets/2k_mercury.jpg"));
    GLuint venus_texture = bonobo::loadTexture2D(config::resources_path("planets/2k_venus_atmosphere.jpg"));
    GLuint mars_texture = bonobo::loadTexture2D(config::resources_path("planets/2k_mars.jpg"));
    GLuint jupiter_texture = bonobo::loadTexture2D(config::resources_path("planets/2k_jupiter.jpg"));
    GLuint saturn_texture = bonobo::loadTexture2D(config::resources_path("planets/2k_saturn.jpg"));
    GLuint uranus_texture = bonobo::loadTexture2D(config::resources_path("planets/2k_uranus.jpg"));
    GLuint neptune_texture = bonobo::loadTexture2D(config::resources_path("planets/2k_neptune.jpg"));

    std::vector<float> planet_radius = {0.25f, 0.1f, 0.15f, 0.2f, 0.5f, 0.4f, 0.3f, 0.3f};

    std::vector<std::pair<GLuint, std::shared_ptr<bonobo::mesh_data>>> planet_data =
        {{earth_texture, std::make_shared<bonobo::mesh_data>(earth_sphere)},
         {mercury_texture, std::make_shared<bonobo::mesh_data>(mercury_sphere)},
         {venus_texture, std::make_shared<bonobo::mesh_data>(venus_sphere)},
         {mars_texture, std::make_shared<bonobo::mesh_data>(mars_sphere)},
         {jupiter_texture, std::make_shared<bonobo::mesh_data>(jupiter_sphere)},
         {saturn_texture, std::make_shared<bonobo::mesh_data>(saturn_sphere)},
         {uranus_texture, std::make_shared<bonobo::mesh_data>(uranus_sphere)},
         {neptune_texture, std::make_shared<bonobo::mesh_data>(neptune_sphere)}};

    // Randomly select a planet
    auto random_index = glm::linearRand(0.0f, 8.0f);
    auto selected_planet_data = planet_data[random_index];

    auto planet = Node();
    planet.set_geometry(*(selected_planet_data.second));
    planet.set_program(&planet_shader);
    planet.add_texture("diffuse_texture", selected_planet_data.first, GL_TEXTURE_2D);

    // Set initial position
    float angle = glm::linearRand(0.0f, 2.0f * glm::pi<float>());
    auto position = glm::vec3(radius * cos(angle), radius * sin(angle), -20.0f);
    Planet np = {planet, position, planet_radius[random_index]};
    planets.push_back(np);  // Add a new planet

    while (!glfwWindowShouldClose(window)) {
        auto const nowTime = std::chrono::high_resolution_clock::now();
        auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
        float dt = std::chrono::duration<float>(deltaTimeUs).count();

        lastTime = nowTime;
        elapsed_time_s += std::chrono::duration<float>(deltaTimeUs).count();

        auto& io = ImGui::GetIO();
        inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

        glfwPollEvents();
        inputHandler.Advance();
        // mCamera.Update(deltaTimeUs, inputHandler);

        if (inputHandler.GetKeycodeState(GLFW_KEY_R) & JUST_PRESSED) {
            shader_reload_failed = !program_manager.ReloadAllPrograms();
            if (shader_reload_failed)
                tinyfd_notifyPopup("Shader Program Reload Error",
                                   "An error occurred while reloading shader programs; see the logs for details.\n"
                                   "Rendering is suspended until the issue is solved. Once fixed, just reload the shaders again.",
                                   "error");
        }
        if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
            show_logs = !show_logs;
        if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
            show_gui = !show_gui;
        if (inputHandler.GetKeycodeState(GLFW_KEY_F11) & JUST_RELEASED)
            mWindowManager.ToggleFullscreenStatusForWindow(window);

        // Retrieve the actual framebuffer size: for HiDPI monitors,
        // you might end up with a framebuffer larger than what you
        // actually asked for. For example, if you ask for a 1920x1080
        // framebuffer, you might get a 3840x2160 one instead.
        // Also it might change as the user drags the window between
        // monitors with different DPIs, or if the fullscreen status is
        // being toggled.
        int framebuffer_width, framebuffer_height;
        glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
        glViewport(0, 0, framebuffer_width, framebuffer_height);

        //
        // Todo: If you need to handle inputs, you can do it here
        //
        // std::cout << player_velocity.x << std::endl;
        if (inputHandler.GetKeycodeState(GLFW_KEY_A) & JUST_PRESSED) {
            player_velocity.x = -player_speed;
        } else if ((inputHandler.GetKeycodeState(GLFW_KEY_A) & JUST_RELEASED) && player_velocity.x < 0.0f) {
            player_velocity.x = 0.0f;
        }

        if (inputHandler.GetKeycodeState(GLFW_KEY_D) & JUST_PRESSED) {
            player_velocity.x = player_speed;
        } else if ((inputHandler.GetKeycodeState(GLFW_KEY_D) & JUST_RELEASED) && player_velocity.x > 0.0f) {
            player_velocity.x = 0.0f;
        }

        if (inputHandler.GetKeycodeState(GLFW_KEY_W) & JUST_PRESSED) {
            player_velocity.y = player_speed;
        } else if ((inputHandler.GetKeycodeState(GLFW_KEY_W) & JUST_RELEASED) && player_velocity.y > 0.0f) {
            player_velocity.y = 0.0f;
        }

        if (inputHandler.GetKeycodeState(GLFW_KEY_S) & JUST_PRESSED) {
            player_velocity.y = -player_speed;
        } else if ((inputHandler.GetKeycodeState(GLFW_KEY_S) & JUST_RELEASED) && player_velocity.y < 0.0f) {
            player_velocity.y = 0.0f;
        }

        mWindowManager.NewImGuiFrame();

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        if (!shader_reload_failed) {
            //
            // Todo: Render all your geometry here.
            //
            glm::vec3 player_final_velocity = player_velocity;
            if (player_velocity.x != 0.0f && player_velocity.y != 0.0f) {
                player_final_velocity *= .707f;
            }
            segments[0].pos += player_final_velocity * dt;
            int seg = 0;
            for (int i = 0; i < worm.size(); i++) {
                if (seg > 0) {
                    segments[seg].pos += segments[seg].velocity * dt;
                    float x = segments[seg - 1].pos.x;

                    float y = segments[seg - 1].pos.y;

                    float z = segments[seg].pos.z;

                    glm::vec3 new_position = glm::vec3(x, y, z);

                    float distance = glm::distance(segments[seg].pos, new_position);
                    glm::vec3 diff = new_position - segments[seg].pos;
                    float magnitude = std::sqrt(std::pow(diff.x, 2) + std::pow(diff.y, 2) + std::pow(diff.z, 2));
                    glm::vec3 direction = glm::vec3(0.0, 0.0, 0.0);
                    if (magnitude != 0) {
                        direction = (1.0f / magnitude) * diff;
                    }
                    // std::cout << direction << std::endl;
                    segments[seg].velocity = 15.0f * distance * direction;
                }
                glm::mat4 player_transformation_matrix = glm::translate(glm::mat4(1.0f), segments[seg].pos);
                worm[i].render(mCamera.GetWorldToClipMatrix(), player_transformation_matrix);
                seg++;
            }

            if (glm::linearRand(0.0f, 10.0f) > 9.9f) {
                // Randomly select a planet
                auto random_index = glm::linearRand(0.0f, 8.0f);
                auto selected_planet_data = planet_data[random_index];

                auto planet = Node();
                planet.set_geometry(*(selected_planet_data.second));
                planet.set_program(&planet_shader);
                planet.add_texture("diffuse_texture", selected_planet_data.first, GL_TEXTURE_2D);
                // Set initial position
                float angle = glm::linearRand(0.0f, 2.0f * glm::pi<float>());
                auto position = glm::vec3(radius * cos(angle), radius * sin(angle), -20.0f);
                Planet np = {planet, position, planet_radius[random_index]};
                planets.push_back(np);  // Add a new planet
            }

            for (auto& planet : planets) {
                auto curr_planet = planet;
                auto curr_planet_pos = curr_planet.position;

                glm::mat4 planet_transformation_matrix = glm::translate(glm::mat4(1.0f), curr_planet_pos);
                curr_planet.node.render(mCamera.GetWorldToClipMatrix(), planet_transformation_matrix);
            }

            for (size_t i = 0; i < planets.size(); ++i) {
                auto curr_planet = planets[i];
                auto curr_planet_pos = curr_planet.position;

                planets[i].position.z += dt * (6.0f + (ate >> 2));  // Move towards the camera

                for (auto& worm_segment : segments) {
                    auto distance_to_planet = glm::distance(worm_segment.pos, curr_planet_pos);

                    if (distance_to_planet < player_radius + curr_planet.radius) {
                        planets.erase(planets.begin() + i);
                        ++ate;
                    }
                }

                if (curr_planet.position.z >= 6.0f) {
                    planets.erase(planets.begin() + i);
                }
            }
            glm::mat4 tube_transformation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -9000.0f));
            hole.render(mCamera.GetWorldToClipMatrix(), tube_transformation_matrix);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //
        // Todo: If you want a custom ImGUI window, you can set it up
        //       here
        //
        ImGui::SetNextWindowSize(ImVec2(100, 60), ImGuiCond_Always);  // Adjust the size as needed
        ImGui::Begin("Ate Count");
        ImGui::Text("Ate: %d", ate);
        ImGui::End();

        bool const opened = ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_None);
        if (opened) {
            ImGui::Checkbox("Show basis", &show_basis);
            ImGui::SliderFloat("Basis thickness scale", &basis_thickness_scale, 0.0f, 100.0f);
            ImGui::SliderFloat("Basis length scale", &basis_length_scale, 0.0f, 100.0f);
        }
        ImGui::End();

        if (show_basis)
            bonobo::renderBasis(basis_thickness_scale, basis_length_scale, mCamera.GetWorldToClipMatrix());
        if (show_logs)
            Log::View::Render();
        mWindowManager.RenderImGuiFrame(show_gui);

        glfwSwapBuffers(window);
    }
}

int main() {
    std::setlocale(LC_ALL, "");

    Bonobo framework;

    try {
        edaf80::Assignment5 assignment5(framework.GetWindowManager());
        assignment5.run();
    } catch (std::runtime_error const& e) {
        LogError(e.what());
    }
}
