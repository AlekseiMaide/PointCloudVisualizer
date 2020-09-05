#include <iostream>
#include <sstream>
#include <vector>

#define GLEW_STATIC
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include "ShaderProgram.h"
#include "Camera.h"
#include "PointLoader.h"

const char* TITLE = "Point Cloud Visualizer";

int gWindowWidth = 1024;
int gWindowHeight = 768;
bool isFullScreen = false;

float gYaw = 0.0f;
float gPitch = 0.0f;
float gRadius = 10.0f;

FPSCamera fpsCamera(glm::vec3(0.0, 0.0, 5.0));
const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 5.0f; // units per second.
const float MOUSE_SENSITIVITY = 0.1f; // 0.25 for OrbitCamera.


void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height); // Todo: actually use it.
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY);
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
void update(GLFWwindow* window, double elapsedTime); // Todo: The fuck is this?
void showFPS(GLFWwindow* window);

struct Vertex {
	glm::vec3 pos;
};

const std::vector<glm::vec3> vertices = {
	{-0.5f, -0.5f, -0.5f},
	{-0.5f,-0.5f, 0.5f},
	{-0.5f, 0.5f, 0.5f},
	{0.5f, 0.5f,-0.5f},
	{-0.5f,-0.5f,-0.5f},
	{-0.5f, 0.5f,-0.5f},
	{0.5f,-0.5f, 0.5f},
	{-0.5f,-0.5f,-0.5f},
	{0.5f,-0.5f,-0.5f},
	{0.5f, 0.5f,-0.5f},
	{0.5f,-0.5f,-0.5f},
	{-0.5f,-0.5f,-0.5f},
	{-0.5f,-0.5f,-0.5f},
	{-0.5f, 0.5f, 0.5f},
	{-0.5f, 0.5f,-0.5f},
	{0.5f,-0.5f, 0.5f},
	{-0.5f,-0.5f, 0.5f},
	{-0.5f,-0.5f,-0.5f},
	{-0.5f, 0.5f, 0.5f},
	{-0.5f,-0.5f, 0.5f},
	{0.5f,-0.5f, 0.5f},
	{0.5f, 0.5f, 0.5f},
	{0.5f,-0.5f,-0.5f},
	{0.5f, 0.5f,-0.5f},
	{0.5f,-0.5f,-0.5f},
	{0.5f, 0.5f, 0.5f},
	{0.5f,-0.5f, 0.5f},
	{0.5f, 0.5f, 0.5f},
	{0.5f, 0.5f,-0.5f},
	{-0.5f, 0.5f,-0.5f},
	{0.5f, 0.5f, 0.5f},
	{-0.5f, 0.5f,-0.5f},
	{-0.5f, 0.5f, 0.5f},
	{0.5f, 0.5f, 0.5f},
	{-0.5f, 0.5f, 0.5f},
	{0.5f,-0.5f, 0.5f}
};

class PointCloudVisualizer {
public:
	void run() {
		initWindow();
		initOpenGL();
		mainLoop();
		cleanup();
	}

private:
	GLFWwindow* window;
	ShaderProgram basicShaderProgram;
	PointLoader3D pointLoader3D;
	//OrbitCamera orbitCamera;

	std::vector<glm::vec3> verts;

	GLuint vbo, vao;

	float cubeAngle = 0.0f;
	double lastTime = glfwGetTime();

	void initWindow() {
		// Refactor like this:
		// https://stackoverflow.com/questions/47402766/switching-between-windowed-and-full-screen-in-opengl-glfw-3-2

		if (!glfwInit()) {
			throw std::runtime_error("GLFW initialization failed.");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		if (isFullScreen) {
			// Todo: Make F11 button go full screen.
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* primaryVideoMode = glfwGetVideoMode(primaryMonitor);

			if (primaryVideoMode != NULL) {
				gWindowWidth = primaryVideoMode->width;
				gWindowHeight = primaryVideoMode->height;
				
				window = glfwCreateWindow(gWindowWidth, gWindowHeight, TITLE, primaryMonitor, nullptr);
			}
		} else {
			window = glfwCreateWindow(gWindowWidth, gWindowHeight, TITLE, nullptr, nullptr);
		}

		if (window == NULL) {
			throw std::runtime_error("GLFW window creation failed.");
		}

		glfwMakeContextCurrent(window);

		glClearColor(0.24f, 0.38f, 0.24f, 1.0f);
		glViewport(0, 0, gWindowWidth, gWindowHeight);

		// Hide mouse cursor and center it.
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(window, gWindowWidth / 2.0f, gWindowHeight / 2.0f);

		setWindowEvents();
	}

	void setWindowEvents() {
		glfwSetKeyCallback(window, glfw_onKey);
		glfwSetCursorPosCallback(window, glfw_onMouseMove);
		//glfwSetScrollCallback(window, glfw_onMouseScroll);
	}

	void initOpenGL() {
		initGLEW();
		enableCapabilities();
		initShaders();
		loadVertexData();
		generateBuffers();
	}

	void mainLoop() {

		basicShaderProgram.use();

		while (!glfwWindowShouldClose(window)) {
			showFPS(window);

			glfwPollEvents();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			drawFrame();

			glfwSwapBuffers(window);
		}

	}

	void cleanup() {
		glfwTerminate();
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	void initGLEW() {
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			throw std::runtime_error("GLEW initialization failed.");
		}
	}

	void enableCapabilities() {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_PROGRAM_POINT_SIZE);
	}

	void initShaders() {
		basicShaderProgram.loaderShaders("basic.vert", "basic.frag");
	}

	void loadVertexData() {
		pointLoader3D.loadFromFile("test_scan.3d");
	}

	void generateBuffers() {
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glVertexAttribPointer(
			0,                                // attribute. Must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			nullptr							  // array buffer offset
		);
		glEnableVertexAttribArray(0);
	}

	void drawFrame() {
		glm::vec3 lookAtPos = glm::vec3(0.0f, 0.0f, -5.0f);

		// NB: LookAtPosition doesnt currently change.
		//orbitCamera.setLookAt(lookAtPos);
		//orbitCamera.rotate(gYaw, gPitch);
		//orbitCamera.setRadius(gRadius);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		update(window, deltaTime);

		cubeAngle += (float)(deltaTime * 50.0f);
		if (cubeAngle >= 360) cubeAngle = 0.0f;

		glm::mat4 model(1.0f), view, projection;

		model = glm::translate(model, lookAtPos) * glm::rotate(model, glm::radians(cubeAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		view = fpsCamera.getViewMatrix();
		projection = glm::perspective(fpsCamera.getFOV(), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);

		basicShaderProgram.setUniform("model", model);
		basicShaderProgram.setUniform("view", view);
		basicShaderProgram.setUniform("projection", projection);

		glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, 36);
		glBindVertexArray(0);

		lastTime = currentTime;
	}
};

int main() {
	PointCloudVisualizer app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height) {
	gWindowWidth = width;
	gWindowHeight = height;

	glViewport(0, 0, gWindowWidth, gWindowHeight);
}

void glfw_onMouseMove(GLFWwindow* window, double posX, double posY) {
	static glm::vec2 lastMousePos = glm::vec2(0.0f, 0.0f);

	// Update angles based on Left Mouse Button input to orbit around target origin.
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == 1) {
		// Each pixel represents 1 / MOUSE_SENSITIVITY degree rotation.
		gYaw -= ((float)posX - lastMousePos.x) * MOUSE_SENSITIVITY;
		gPitch += ((float)posY - lastMousePos.y) * MOUSE_SENSITIVITY;
	}

	// Change OrbitCamera radius with Right Mouse Button.
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == 1) {
		// Each pixel represents 1 / MOUSE_SENSITIVITY degree rotation.
		float dx = 0.01f * ((float)posX - lastMousePos.x);
		float dy = 0.01f * ((float)posY - lastMousePos.y);

		gRadius += dx - dy;
	}

	lastMousePos.x = (float)posX;
	lastMousePos.y = (float)posY;
}

void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY) {
	// Todo: THIS ACTS VERY WIERD, CURRENTLY DISABLED.
	double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;
	fov = glm::clamp(fov, 1.0, 120.0);

	fpsCamera.setFOV((float)fov);
}

void update(GLFWwindow* window, double elapsedTime) {
	// Todo: RENAME THIS.
	// Todo: MOVE INTO CAMERA IMPL ?
	double mouseX, mouseY;

	glfwGetCursorPos(window, &mouseX, &mouseY);

	// Rotate the camera by the distance from the screen center. Multiply by speed scaler.
	fpsCamera.rotate(
		(float)(gWindowWidth / 2.0 - mouseX) * MOUSE_SENSITIVITY, 
		(float)(gWindowHeight / 2.0 - mouseY) * MOUSE_SENSITIVITY);

	// Clamp mouse cursor to center of screen.
	glfwSetCursorPos(window, gWindowWidth / 2.0, gWindowHeight / 2.0);

	// Forward/Backward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
	} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook());
	}

	// Strafe Left/Right
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getRight());
	} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getRight());
	}

	// Up/Down
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getUp());
	} else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getUp());
	}
}

void showFPS(GLFWwindow* window) {
	static double prevTime = 0.0;
	static int frameCount = 0;
	double elapsedTime;
	double currentTime = glfwGetTime(); // Returns number of seconds since GLFW started.

	elapsedTime = currentTime - prevTime;

	// Limit update to 4 times a second.
	if (elapsedTime > 0.25) {
		prevTime = currentTime;
		double fps = (double)frameCount / (double)elapsedTime;
		double msPerFrame = 1000.0 / fps;

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed
			<< TITLE << "   "
			<< "FPS: " << fps << "   "
			<< "(" << msPerFrame << " ms)";

		glfwSetWindowTitle(window, outs.str().c_str());

		frameCount = 0;
	}

	frameCount++;
}
