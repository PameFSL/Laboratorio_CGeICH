//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// program include
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"
// Geometry primitives
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
//Texture includes
//Descomentar
#include "Headers/Texture.h"

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere(20, 20);
Sphere sphere2(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Box box;

Shader shader;
//Descomentar
Shader shaderTexture;

GLuint textureID1, textureID2;

GLuint textureID3, textureID4, textureID5;

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX, offsetX;
int lastMousePosY, offsetY;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	shader.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");
	//Descomentar
	shaderTexture.initialize("../../Shaders/texturizado.vs", "../../Shaders/texturizado.fs");

	sphere.init();
	sphere.setShader(&shader);
	sphere.setColor(glm::vec3(0.3, 0.3, 1.0));

	sphere2.init();
	//Cambiar el objetos shader
	sphere2.setShader(&shaderTexture);
	sphere2.setColor(glm::vec3(0.3, 0.3, 1.0));
	sphere2.scaleUVS(glm::vec2(3.0f, 1.0f));

	cylinder.init();
	cylinder.setShader(&shaderTexture);
	cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));

	cylinder2.init();
	cylinder2.setShader(&shader);
	cylinder2.setColor(glm::vec3(0.2, 0.7, 0.3));

	box.init();
	box.setShader(&shaderTexture);
	box.setColor(glm::vec3(0.2, 0.8, 0.4));

	camera->setPosition(glm::vec3(0.0f, 0.0f, 0.4f));

	// Descomentar
	int imageWidth, imageHeight;
	Texture texture1("../../Textures/texturaLadrillos.jpg");
	FIBITMAP* bitmap = texture1.loadImage();
	unsigned char * data = texture1.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID1); //Se genera la textura, el n�mero de texturas y se le pasa la referencia para que la pueda modificar
	//Se enlace el tipo de textura al ID textureID1 (Texture 2D)
	glBindTexture(GL_TEXTURE_2D, textureID1); //Se indica el tipo de textura que se trata
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data){
		//Param: TipoTextura, 0 (MIPMAPS), FORMATO INTERNO BUFFER, W, H, BORDE , FORMATO DE LIBRERIA, 
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GL_RGBA, 
			imageWidth, 
			imageHeight, 
			0, 
			GL_BGRA, 
			GL_UNSIGNED_BYTE, // Tipo de dato
			data); // Datos de la imagen
		// Le indica a openGL que se encargue de generar los mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture1.freeImage(bitmap);


	//Segunda textura
	Texture texture2("../../Textures/h.jpg");
	FIBITMAP* bitmap2 = texture2.loadImage(false);
	unsigned char * data2 = texture2.convertToData(bitmap2, imageWidth, imageHeight);
	glGenTextures(1, &textureID2); //Se genera la textura, el n�mero de texturas y se le pasa la referencia para que la pueda modificar
								   //Se enlace el tipo de textura al ID textureID1 (Texture 2D)
	glBindTexture(GL_TEXTURE_2D, textureID2); //Se indica el tipo de textura que se trata
											  // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data2) {
		//Param: TipoTextura, 0 (MIPMAPS), FORMATO INTERNO BUFFER, W, H, BORDE , FORMATO DE LIBRERIA, 
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			imageWidth,
			imageHeight,
			0,
			GL_BGRA,
			GL_UNSIGNED_BYTE, // Tipo de dato
			data2); // Datos de la imagen
				   // Le indica a openGL que se encargue de generar los mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture2.freeImage(bitmap2);

	//Tercera textura
	Texture texture3("../../Textures/coca-cola.jpg");
	FIBITMAP* bitmap3 = texture3.loadImage(false);
	unsigned char * data3 = texture3.convertToData(bitmap3, imageWidth, imageHeight);
	glGenTextures(1, &textureID3); //Se genera la textura, el n�mero de texturas y se le pasa la referencia para que la pueda modificar
								   //Se enlace el tipo de textura al ID textureID1 (Texture 2D)
	glBindTexture(GL_TEXTURE_2D, textureID3); //Se indica el tipo de textura que se trata
											  // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data3) {
		//Param: TipoTextura, 0 (MIPMAPS), FORMATO INTERNO BUFFER, W, H, BORDE , FORMATO DE LIBRERIA, 
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			imageWidth,
			imageHeight,
			0,
			GL_BGRA,
			GL_UNSIGNED_BYTE, // Tipo de dato
			data3); // Datos de la imagen
				   // Le indica a openGL que se encargue de generar los mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture3.freeImage(bitmap3);

	//Cuarta textura
	Texture texture4("../../Textures/tapa_sup.jpg");
	FIBITMAP* bitmap4 = texture4.loadImage(false);
	unsigned char * data4 = texture4.convertToData(bitmap4, imageWidth, imageHeight);
	glGenTextures(1, &textureID4); //Se genera la textura, el n�mero de texturas y se le pasa la referencia para que la pueda modificar
								   //Se enlace el tipo de textura al ID textureID1 (Texture 2D)
	glBindTexture(GL_TEXTURE_2D, textureID4); //Se indica el tipo de textura que se trata
											  // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data4) {
		//Param: TipoTextura, 0 (MIPMAPS), FORMATO INTERNO BUFFER, W, H, BORDE , FORMATO DE LIBRERIA, 
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			imageWidth,
			imageHeight,
			0,
			GL_BGRA,
			GL_UNSIGNED_BYTE, // Tipo de dato
			data4); // Datos de la imagen
				   // Le indica a openGL que se encargue de generar los mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture4.freeImage(bitmap4);

	//Quinta textura
	Texture texture5("../../Textures/tapa_inf.png");
	FIBITMAP* bitmap5 = texture5.loadImage(false);
	unsigned char * data5 = texture5.convertToData(bitmap5, imageWidth, imageHeight);
	glGenTextures(1, &textureID5); //Se genera la textura, el n�mero de texturas y se le pasa la referencia para que la pueda modificar
								   //Se enlace el tipo de textura al ID textureID1 (Texture 2D)
	glBindTexture(GL_TEXTURE_2D, textureID5); //Se indica el tipo de textura que se trata
											  // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data5) {
		//Param: TipoTextura, 0 (MIPMAPS), FORMATO INTERNO BUFFER, W, H, BORDE , FORMATO DE LIBRERIA, 
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			imageWidth,
			imageHeight,
			0,
			GL_BGRA,
			GL_UNSIGNED_BYTE, // Tipo de dato
			data5); // Datos de la imagen
				   // Le indica a openGL que se encargue de generar los mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture5.freeImage(bitmap5);
}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shader.destroy();
	sphere.destroy();
	cylinder.destroy();
	cylinder2.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista
		glm::mat4 view = camera->getViewMatrix();

		//Descomentar
		// Enlazamos la textura que deseamos utilizar
		glBindTexture(GL_TEXTURE_2D, textureID1);
		sphere2.setProjectionMatrix(projection);
		sphere2.setViewMatrix(view);
		//sphere2.enableWireMode();
		sphere2.setPosition(glm::vec3(0.0, 0.0, -4.0));
		sphere2.render();

		glBindTexture(GL_TEXTURE_2D, textureID2);
		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		box.setPosition(glm::vec3(2.0f, 0.0f, -3.0f));
		box.render();

		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.setPosition(glm::vec3(-2.0f, 0.0f, -3.0f));
		cylinder.render(0, cylinder.getSlices() * cylinder.getStacks() * 2  * 3);

		glBindTexture(GL_TEXTURE_2D, textureID4);
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cylinder.getSlices() * 3);
		glBindTexture(GL_TEXTURE_2D, textureID5);
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3 + cylinder.getSlices() * 3, cylinder.getSlices() * 3);

		//Descomentar
		// No utilizamos ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}