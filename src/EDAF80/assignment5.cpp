#include "assignment5.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/ShaderProgramManager.hpp"
#include "core/node.hpp"
#include "parametric_shapes.hpp"

#include <imgui.h>
#include <tinyfiledialogs.h>

#include <clocale>
#include <stdexcept>
#include "core/node.hpp"
#include <glm/gtc/type_ptr.hpp>

edaf80::Assignment5::Assignment5(WindowManager& windowManager) :
	mCamera(0.5f * glm::half_pi<float>(),
	        static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
	        0.01f, 1000.0f),
	inputHandler(), mWindowManager(windowManager), window(nullptr)
{
	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};

	window = mWindowManager.CreateGLFWWindow("EDAF80: Assignment 5", window_datum, config::msaa_rate);
	if (window == nullptr) {
		throw std::runtime_error("Failed to get a window: aborting!");
	}

	bonobo::init();
}

edaf80::Assignment5::~Assignment5()
{
	bonobo::deinit();
}

void
edaf80::Assignment5::run()
{
	// Set up the camera
	mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 6.0f));
	mCamera.mMouseSensitivity = glm::vec2(0.003f);
	mCamera.mMovementSpeed = glm::vec3(3.0f); // 3 m/s => 10.8 km/h
	auto camera_position = mCamera.mWorld.GetTranslation();
	auto light_position = glm::vec3(0.0f, 2.0f, 0.0f);

	// Create the shader programs
	ShaderProgramManager program_manager;
	GLuint fallback_shader = 0u;
	program_manager.CreateAndRegisterProgram("Fallback",
	                                         { { ShaderType::vertex, "common/fallback.vert" },
	                                           { ShaderType::fragment, "common/fallback.frag" } },
	                                         fallback_shader);
	if (fallback_shader == 0u) {
		LogError("Failed to load fallback shader");
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

	//
	// Todo: Load your geometry
	//
	glm::vec3 ambient = glm::vec3(74.0f/255.0f, 48.0f/255.0f, 18.0f/255.0f) * .25f;
	float shininess = 10.0f;
	auto const phong_set_uniforms = [&light_position, &camera_position, &ambient, &shininess](GLuint program) 
	{
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
		glUniform3fv(glGetUniformLocation(program, "ambient"), 1, glm::value_ptr(ambient));
		glUniform1f(glGetUniformLocation(program, "shininess"), shininess);
	};

	auto sphere = parametric_shapes::createSphere(0.4f, 30u, 30u);
	auto player = Node();
	player.set_geometry(sphere);
	player.set_program(&phong_shader, phong_set_uniforms);
	glm::vec3 player_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 player_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	float player_speed = 5.0f;

	GLuint leather_texture = bonobo::loadTexture2D(config::resources_path("textures/leather_red_02_coll1_2k.jpg"));
    player.add_texture("texture_map", leather_texture, GL_TEXTURE_2D);

    GLuint leather_normal_map = bonobo::loadTexture2D(config::resources_path("textures/leather_red_02_nor_2k.jpg"));
    player.add_texture("normal_map", leather_normal_map, GL_TEXTURE_2D);

    GLuint leather_rough_map = bonobo::loadTexture2D(config::resources_path("textures/leather_red_02_rough_2k.jpg"));
    player.add_texture("roughness_map", leather_rough_map, GL_TEXTURE_2D);

	

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

	while (!glfwWindowShouldClose(window)) {
		auto const nowTime = std::chrono::high_resolution_clock::now();
		auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
		float dt = std::chrono::duration<float>(deltaTimeUs).count();

		lastTime = nowTime;

		auto& io = ImGui::GetIO();
		inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

		glfwPollEvents();
		inputHandler.Advance();
		//mCamera.Update(deltaTimeUs, inputHandler);

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
		std::cout << player_velocity.x << std::endl;
		if(inputHandler.GetKeycodeState(GLFW_KEY_A) & JUST_PRESSED) {
			player_velocity.x = -player_speed;
		} else if((inputHandler.GetKeycodeState(GLFW_KEY_A) & JUST_RELEASED) && player_velocity.x < 0.0f) {
			player_velocity.x = 0.0f;
		}
		
		if(inputHandler.GetKeycodeState(GLFW_KEY_D) & JUST_PRESSED) {
			player_velocity.x = player_speed;
		} else if((inputHandler.GetKeycodeState(GLFW_KEY_D) & JUST_RELEASED) && player_velocity.x > 0.0f) {
			player_velocity.x = 0.0f;
		}

		if(inputHandler.GetKeycodeState(GLFW_KEY_W) & JUST_PRESSED) {
			player_velocity.y = player_speed;
		} else if((inputHandler.GetKeycodeState(GLFW_KEY_W) & JUST_RELEASED) && player_velocity.y > 0.0f) {
			player_velocity.y = 0.0f;
		}

		if(inputHandler.GetKeycodeState(GLFW_KEY_S) & JUST_PRESSED) {
			player_velocity.y = -player_speed;
		} else if((inputHandler.GetKeycodeState(GLFW_KEY_S) & JUST_RELEASED) && player_velocity.y < 0.0f) {
			player_velocity.y = 0.0f;
		}
 

		mWindowManager.NewImGuiFrame();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


		if (!shader_reload_failed) {
			//
			// Todo: Render all your geometry here.
			//
			glm::vec3 player_final_velocity = player_velocity;
			if(player_velocity.x != 0.0f && player_velocity.y != 0.0f) {
				player_final_velocity *= .7070f;
			}
			player_position += player_final_velocity * dt;
			glm::mat4 player_transformation_matrix = glm::translate(glm::mat4(1.0f), player_position);
			player.render(mCamera.GetWorldToClipMatrix(), player_transformation_matrix);
		}


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//
		// Todo: If you want a custom ImGUI window, you can set it up
		//       here
		//
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

int main()
{
	std::setlocale(LC_ALL, "");

	Bonobo framework;

	try {
		edaf80::Assignment5 assignment5(framework.GetWindowManager());
		assignment5.run();
	} catch (std::runtime_error const& e) {
		LogError(e.what());
	}
}
