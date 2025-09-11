#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "Scripts/Shader.h"
#include "Scripts/Camera.h"
#include "Scripts/MeshRenderer.h"
#include "Scripts/Cloth.h"
#include "Scripts/LightObject.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Scripts/Debug.cpp"

using namespace Engine;

// Default camera
Camera* cam{ Camera::MainCamera };

// DELTA TIME
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// For first mouse movement
bool firstMouse{ true };

#pragma region Setup
// Callback to resize window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// INPUT
void processInput(GLFWwindow* window)
{
	// TODO: change camera behavior (rotate on mouse drag)
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Camera Movement
	const float camSpeed = 20 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam->translate(camSpeed * cam->forward);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam->translate(camSpeed * -cam->forward);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam->translate(camSpeed * -cam->right);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam->translate(camSpeed * cam->right);
}
float lastX = 400, lastY = 300;
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos, lastY = ypos;
		firstMouse = false;
	}

	float dx = xpos - lastX, dy = ypos - lastY;
	lastX = xpos, lastY = ypos;

	const float sensitivity = 0.001f;
	float yaw = sensitivity * dx;
	float pitch = sensitivity * dy;

	//// Constrain angles
	//if (cam.pitch > 89.9f) cam.pitch = 89.9f;
	//if (cam.pitch < -89.9f) cam.pitch = -89.9f;

	// Adjust camera rotation
	cam->rotate(glm::vec3(pitch, yaw, 0));
}
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	const float sensitivity = 5;
	cam->fov -= sensitivity * (float)yOffset;
	if (cam->fov < 1.0f) cam->fov = 1.0f;
	else if (cam->fov > 135.0f) cam->fov = 135.0f;
}
#pragma endregion

GLFWwindow* window;

void physicsLoop()
{
	float time;
	while (!glfwWindowShouldClose(window))
	{
		time = glfwGetTime();
		Physics::FixedUpdateAll();

		float elapsedTime = glfwGetTime() - time;
		float waitMS{ 1000 * (Physics::FIXEDDELTATIME - elapsedTime) };
		std::cout << elapsedTime * 1000 << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(waitMS)));
	}
}

int main()
{
	// Initialize Window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create Window
	window = glfwCreateWindow(800, 600, "GooGoo", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); // Disable VSYNC

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Set up viewport
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set mouse input
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// Lighting
	LightObject light{ {}, {-1, -1, 0} };

	// Cloth stuff
	Cloth cloth{ {}, 2, 20 };
	Shader visualizeShader{ "ClothVisualShaderVert.glsl", "ClothVisualShaderFrag.glsl", "ClothVisualShaderGeom.glsl" };
	//Shader clothShader{ "ClothShaderVert.glsl", "ClothShaderFrag.glsl" };
	//Shader normalShader{ "DefaultShaderVert.glsl", "NormalShaderFrag.glsl", "NormalShaderGeom.glsl"};
	Shader texShader{ "TextureShaderVert.glsl", "TextureShaderFrag.glsl" };
	Texture clothTex{ "ClothTexture.jpg", GL_REPEAT, GL_REPEAT, {}, GL_NEAREST, GL_LINEAR, GL_RGB };
	Material clothMat{ {0.2, 0.2, 0.8}, {0, 0, 9}, 1.0F, true };
	MeshRenderer clothRend{ cloth.mesh, texShader, clothMat, { clothTex } };
	clothRend.addShader(visualizeShader);
	cloth.setRenderer(&clothRend);

	
	// Dispatch Physics thread for fixed updates
	std::thread physicsThread(physicsLoop);
	physicsThread.detach();


	// Make camera target cloth
	cam->beginTargeting(cloth.position);


	// Render Loop
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		// Update Delta Time
		float time = glfwGetTime();
		deltaTime = time - lastFrame;
		lastFrame = time;
		float fps{ 1.0F / deltaTime };
		//std::cout << "FPS: " << fps << std::endl;

		// Input
		processInput(window);

		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Rendering Commands
		MeshRenderer::UpdateAll();

		// Check/Call Events and Swap Buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up
	glfwTerminate();
	return 0;
}