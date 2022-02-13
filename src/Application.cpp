#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Camera.h"
#include "vendor\glm/gtc/type_ptr.hpp"
#include "vendor\stb_image\stb_image.h"

/* GLEW is OpenGL Extension Wrangler Library */
#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

struct  ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};
// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Camera
GLfloat lastX = 400;
GLfloat lastY = 300;
bool keys[1024];
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Light attributes
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
static void GLClearError();
static ShaderProgramSource ParseShader(std::string& filepath);
static unsigned int CompileShader(unsigned int type, const std::string& source);
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

int main(void)
{
	GLFWwindow* window;

	/* 初始化 GLFW 库 */
	if (!glfwInit())
		return -1;

	/* 设置窗口的模式（核心模式/立即渲染模式）和版本 */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WIDTH, HEIGHT, "Bohou's demo", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* 设置窗口的上下文 */
	glfwMakeContextCurrent(window);

	/* 初始化 GLEW 来管理OpenGL的函数指针 */
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cout << "glewInit error!" << std::endl;
	}

	/* 设置键盘输入事件回调 */
	glfwSetKeyCallback(window, key_callback);
	/* 设置鼠标输入事件回调 */
	glfwSetCursorPosCallback(window, mouse_callback);
	/* 隐藏鼠标 */
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/* 设置视口 即相对于窗口的渲染范围 */
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	// 开启深度测试
	glEnable(GL_DEPTH_TEST);

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};
	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	// Positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	// 物体
	unsigned int vao_cube;
	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);
	// 发光物
	GLuint vao_light;
	glGenVertexArrays(1, &vao_light);
	glBindVertexArray(vao_light);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	std::string filepath = "res/shader/basic.shader";
	std::string lightshaderpath = "res/shader/light.shader";
	ShaderProgramSource cubesource = ParseShader(filepath);
	ShaderProgramSource lightsource = ParseShader(lightshaderpath);
	std::cout << "VertexSource" << std::endl;
	std::cout << cubesource.VertexSource << std::endl;
	std::cout << "FragmentSource" << std::endl;
	std::cout << cubesource.FragmentSource << std::endl;
	std::cout << "VertexSource" << std::endl;
	std::cout << lightsource.VertexSource << std::endl;
	std::cout << "FragmentSource" << std::endl;
	std::cout << lightsource.FragmentSource << std::endl;
	// 物体shader程序对象
	unsigned int shader_cube = CreateShader(cubesource.VertexSource, cubesource.FragmentSource);
	// 灯shader程序对象
	unsigned int shader_light = CreateShader(lightsource.VertexSource, lightsource.FragmentSource);

	int texture_width = 0, texture_height = 0, channels_in_file = 0;
	unsigned char* image = stbi_load("res/textures/container2.png", &texture_width, &texture_height, &channels_in_file, 3);
	GLuint diffuseMap, specularMap;
	glGenTextures(1, &diffuseMap);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	image = stbi_load("res/textures/container2_specular.png", &texture_width, &texture_height, &channels_in_file, 3);
	glGenTextures(1, &specularMap);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		/* Render here 当调用glClear()清空颜色缓冲后，窗口的颜色将设置为glClearColor()中的颜色 */
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// 清理深度缓冲
		do_movement();
		glUseProgram(shader_cube);
		//////使用物体的着色器程序对象///////
		/****             设置物体的材质属性                ***/
		glUniform1i(glGetUniformLocation(shader_cube, "material.diffuse"), 0);
		glUniform1i(glGetUniformLocation(shader_cube, "material.specular"), 1);
		glUniform1f(glGetUniformLocation(shader_cube, "material.shininess"), 64.0f);
		/****             设置光的属性                      ***/
		GLint viewPosLoc = glGetUniformLocation(shader_cube, "viewPos");
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
		// Directional light
		glUniform3f(glGetUniformLocation(shader_cube, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(shader_cube, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader_cube, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(shader_cube, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
		// Point light 1
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[0].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[0].quadratic"), 0.032);
		// Point light 2
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[1].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[1].quadratic"), 0.032);
		// Point light 3
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[2].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[2].quadratic"), 0.032);
		// Point light 4
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(shader_cube, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[3].linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader_cube, "pointLights[3].quadratic"), 0.032);
		// SpotLight
		/*glUniform3f(glGetUniformLocation(shader_cube, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform3f(glGetUniformLocation(shader_cube, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
		glUniform3f(glGetUniformLocation(shader_cube, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(shader_cube, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shader_cube, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(shader_cube, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(shader_cube, "spotLight.linear"), 0.09);
		glUniform1f(glGetUniformLocation(shader_cube, "spotLight.quadratic"), 0.032);
		glUniform1f(glGetUniformLocation(shader_cube, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(shader_cube, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));*/
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 model;
		view = view = camera.GetViewMatrix();
		projection = glm::perspective(camera.Zoom, (float)width / (float)height, 0.1f, 100.0f);
		GLint modelLoc = glGetUniformLocation(shader_cube, "model");
		GLint viewLoc = glGetUniformLocation(shader_cube, "view");
		GLint projLoc = glGetUniformLocation(shader_cube, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		// Bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glUniform1i(glGetUniformLocation(shader_cube, "material.diffuse"), 0);
		// Bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glUniform1i(glGetUniformLocation(shader_cube, "material.specular"), 1);
		// 使用物体的vao
		glBindVertexArray(vao_cube);
		for (GLuint i = 0; i < 10; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, cubePositions[i]);
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		////////////使用灯的着色器程序对象///////////////////
		glUseProgram(shader_light);
		modelLoc = glGetUniformLocation(shader_light, "model");
		viewLoc = glGetUniformLocation(shader_light, "view");
		projLoc = glGetUniformLocation(shader_light, "projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		// 使用灯的vao
		glBindVertexArray(vao_light);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	/* 删除创建的 缓冲对象 和 着色器程序对象 */
	glDeleteProgram(shader_cube);
	glDeleteProgram(shader_light);

	glDeleteVertexArrays(1, &vao_cube);
	glDeleteVertexArrays(1, &vao_light);

	glDeleteBuffers(1, &vbo);
	glfwTerminate();
	return 0;
}

/* 事件回调函数 */
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	//cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void do_movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): " <<
			function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

static ShaderProgramSource ParseShader(std::string & filepath)
{
	std::ifstream stream(filepath);

	enum  class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[3];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				//set mode to vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				//set mode to fragment
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string & source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	//TODO:error handing
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static unsigned int CreateShader(const std::string & vertexShader, const std::string & fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}