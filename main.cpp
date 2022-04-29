/*****************************************************************//**
 * \file   main.cpp
 * \brief  Main of the Assignment 1 of the Fundamentals
 *         of Computer Graphics course
 * 
 * \author Kevin Pruvost (pruvostkevin0@gmail.com)
 * \date   March, 26 2022
 *********************************************************************/

// C++ includes
#include <format>

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Project includes
#include "OGL_Implementation\Window.hpp"
#include "OGL_Implementation\Shader\Shader.hpp"
#include "OGL_Implementation\Obj.hpp"
#include "OGL_Implementation\Camera.hpp"
#include "OGL_Implementation\Mesh\Mesh.hpp"
#include "OGL_Implementation\GUI.hpp"
#include "OGL_Implementation\Entity\Entity.hpp"
#include "OGL_Implementation\OpenGL_Timer.hpp"
#include "OGL_Implementation\DebugInfo\FpsCounter.hpp"
#include "OGL_Implementation\Texture.hpp"
#include "OGL_Implementation\Rendering\Rendering.hpp"
#include "OGL_Implementation\Text\Text.hpp"
#include "OGL_Implementation\Light\Light.hpp"

#include "OGL_Implementation\Entity\ParticleSystem\Snow\ParticleSystem_Snow.hpp"
#include "OGL_Implementation\Image\Image2D.hpp"

#include "Constants.hpp"

int main()
{
	Window * window = Window::Init(Constants::Window::windowName, Constants::Paths::windowIcon);
	if (!window)
		return EXIT_FAILURE;

	Rendering::Init();

	Texture snowTexture;
	if (!GenerateTexture(Constants::Paths::snowflake, snowTexture))
	{
		LOG_PRINT(stderr, "Couldn't generate texture '%s'\n", Constants::Paths::snowflake);
		return EXIT_FAILURE;
	}

	Texture backgroundTexture;
	if (!GenerateTexture(Constants::Paths::snowyBackground, backgroundTexture))
	{
		LOG_PRINT(stderr, "Couldn't generate texture '%s'\n", Constants::Paths::snowyBackground);
		return EXIT_FAILURE;
	}

	ParticleSystem_Snow snowSystem(
		*Rendering::shaders.at(Constants::Paths::pointShaderVertex),
		*Rendering::shaders.at(Constants::Paths::wireframeShaderVertex),
		*Rendering::shaders.at(Constants::Paths::snowShaderVertex)
	);
	snowSystem.texture = snowTexture;
	snowSystem.radius = 11.0f;
	snowSystem.frequency = 30.0f;
	snowSystem.maxParticles = 300;

	Mesh meshImage = GenerateMeshImage();
	Image2D background(meshImage,
		*Rendering::shaders.at(Constants::Paths::pointShaderVertex),
		*Rendering::shaders.at(Constants::Paths::wireframeShaderVertex),
		*Rendering::shaders.at(Constants::Paths::face2DShaderVertex),
		{0.5f, 0.5f}, 1.0f
	);
	background.SetTexture(backgroundTexture);

	Camera camera(window->windowWidth(), window->windowHeight(), 0.0f, 0.0f, -20.0f);
	camera.MovementSpeed *= 5.0f;
	mainCamera = &camera;

	bool cameraLock = false;
	// GUI
	GUI gui(window->window);
	// Creating Second Window
	bool enableGui = true;
	gui.AddCallback([&]() {
		const float width = 320.0f;
		const float height = 430.0f;
		ImGui::SetNextWindowSize({ width, height }, ImGuiCond_::ImGuiCond_Always);
		ImGui::SetNextWindowPos(
			{ImGui::GetIO().DisplaySize.x - 20.0f - width, 20.0f},
			ImGuiCond_::ImGuiCond_Always);
		ImGui::Begin("Object Properties:");

		ImGui::Text(std::format("FPS: {}", GetFpsCount(window->DeltaTimeNoMultiplier(), 0.5f)).c_str());
		ImGui::SliderInt("FPS cap", (int *)&window->fpsCap, 0, 60);
		ImGui::SliderFloat("Time Multiplier", const_cast<float *>(&window->GetTimeMultiplier()), 0.0f, 5.0f);
		ImGui::Checkbox("Enable/Disable GUI (Press T)", &enableGui);

		int displayMode = 0;
		if (DisplayMode & RenderingMode::WireframeMode) displayMode += 1;
		if (DisplayMode & RenderingMode::FacesMode) displayMode += 2;
		const char * const displayModeItems[4] = { "Vertices", "Wireframes", "Faces", "Faces + Wireframes" };
		if (ImGui::Combo("Display Mode", &displayMode, displayModeItems, IM_ARRAYSIZE(displayModeItems)))
		{
			switch (displayMode)
			{
				case 0:
					DisplayMode = RenderingMode::VerticesMode; break;
				case 1:
					DisplayMode = RenderingMode::WireframeMode; break;
				case 2:
					DisplayMode = RenderingMode::FacesMode; break;
				case 3:
					DisplayMode = RenderingMode::FacesAndWireframeMode; break;
			}
		}

		ImGui::SliderFloat("Spawn Radius", &snowSystem.radius, 0.0f, 15.0f);
		ImGui::SliderFloat("Gravity (m/s^2)", &snowSystem.gravity, 0.1f, 9.81f);
		ImGui::SliderFloat("Particle Speed", &snowSystem.particleSpeed, 0.0f, 20.0f);
		ImGui::SliderFloat("Min Scale", &snowSystem.minScale, 0.1f, 2.0f);
		ImGui::SliderFloat("Max Scale", &snowSystem.maxScale, 2.0f, 5.0f);
		ImGui::SliderInt("Scale Steps", (int *)&snowSystem.stepScale, 1, 50);

		ImGui::SliderFloat("Frequency", &snowSystem.frequency, 0.0f, 50.0f);
		ImGui::SliderFloat("Life Span", &snowSystem.lifeSpan, 0.0f, 60.0f);
		ImGui::SliderInt("Max Particles", (int *)&snowSystem.maxParticles, 0, 5000);

		ImGui::LabelText("Entities Count", "%d\n", snowSystem.GetParticles().size());

		if (ImGui::Button(snowSystem.isStopped() ? "Start" : "Stop", { width / 2.5f, 50.0f}))
		{
			snowSystem.isStopped() ? snowSystem.Start() : snowSystem.Stop();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset", { width / 2.5f, 50.0f }))
		{
			snowSystem.Reset();
		}

		ImGui::End();
		return true;
	});

	camera.LookAt(snowSystem.pos);
	snowSystem.pos.y = 3.0f;

	float backgroundColor[4] = { 0.15f, 0.3f, 0.4f, 1.0f };

	window->Loop([&]() {
		// Render
		// Clear the colorbuffer
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Switch mesh mode
		if (window->key(GLFW_KEY_C) == InputKey::JustPressed)
			DisplayMode = (RenderingMode)(((int)DisplayMode + 1) % 4);

		// Camera Lock
		if (window->key(GLFW_KEY_L) == InputKey::JustPressed)
		{
			cameraLock = !cameraLock;
			glfwSetInputMode(window->window, GLFW_CURSOR,
				cameraLock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		}

		// Font change
		if (window->key(GLFW_KEY_F) == InputKey::JustPressed)
		{
			Rendering::RotateFonts();
		}

		// Enable/Disable GUI
		if (window->key(GLFW_KEY_T) == InputKey::JustPressed) enableGui = !enableGui;

		if (cameraLock)
		{
			// Camera movement
			if (window->key(GLFW_KEY_W) == InputKey::Pressed)
				camera.ProcessKeyboard(Camera_Movement::Forward, window->DeltaTime());
			if (window->key(GLFW_KEY_S) == InputKey::Pressed)
				camera.ProcessKeyboard(Camera_Movement::Backward, window->DeltaTime());
			if (window->key(GLFW_KEY_A) == InputKey::Pressed)
				camera.ProcessKeyboard(Camera_Movement::Left, window->DeltaTime());
			if (window->key(GLFW_KEY_D) == InputKey::Pressed)
				camera.ProcessKeyboard(Camera_Movement::Right, window->DeltaTime());
			if (window->key(GLFW_KEY_SPACE) == InputKey::Pressed)
				camera.ProcessKeyboard(Camera_Movement::Up, window->DeltaTime());
			if (window->key(GLFW_KEY_LEFT_CONTROL) == InputKey::Pressed)
				camera.ProcessKeyboard(Camera_Movement::Down, window->DeltaTime());

			// Camera Rotation
			if (window->mouseHasMoved())
				camera.ProcessMouseMovement(window->mousePositionOffset().x, window->mousePositionOffset().y);
			if (window->mouseScrollHasMoved())
				camera.ProcessMouseScroll(window->mouseWheelOffset().y);
		}

		// Window Dimensions changed
		if (window->windowDimensionsHasChanged())
			camera.SetWindowDimensions(window->windowWidth(), window->windowHeight());

		// Camera/View transformation
		camera.GetProjViewMatrixUbo();

		// Wireframe Color change
		if (window->key(GLFW_KEY_P) == InputKey::JustPressed)
			Rendering::RotateWireframeColor();

		Rendering::Refresh();

		snowSystem.Update();
		snowSystem.displayMode = DisplayMode;

		Rendering::DrawImage(background);

		// display mode & activate shader
		Rendering::DrawParticleSystem(&snowSystem);

		/*for (auto e : { })
		{
			if (DisplayMode & RenderingMode::VerticesMode)  Rendering::DrawVertices(*e);
			if (DisplayMode & RenderingMode::WireframeMode) Rendering::DrawWireframe(*e);
			if (DisplayMode & RenderingMode::FacesMode)     Rendering::DrawFaces(*e);
		}*/
		

		if (enableGui)
		{
			//// Drawing 3D Text
			//for (auto t : { &textEarth, &textJupiter, &textMars, &textMercury, &textMoon, &textNeptune, &textSaturn, &textSun, &textUranus, &textVenus })
			//{
			//	Rendering::DrawText(*t);
			//}

			//// Drawing 2D Text
			//Rendering::DrawText(text);

			// Drawing ImGui GUI
			if (!gui.DrawGUI()) return false;
		}

		return true;
	});

	return EXIT_SUCCESS;
}