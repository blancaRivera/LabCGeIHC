//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Descomentar
#include "Headers/Texture.h"

int screenWidth;
int screenHeight;

GLFWwindow * window;

Shader shader;
//shader del texturizado
Shader shaderTexture;

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

// Descomentar buffer del texturizado
GLuint textureID1, textureID2, textureID3, textureID4;

Sphere sphere1(20, 20);
Sphere sphere2(20, 20);
Sphere sphere3(10, 10);
Sphere sphere4(10, 10);
Sphere sphere5(10, 10);
Sphere sphere7(10, 10);
Cylinder cylinder1(20, 20, 0.5, 0.5);
Box box1;
Box box2;
Box box3;

Cylinder cylinder2(10, 10, 0.5, 0.5);
Cylinder cylinder3(10, 10, 0.5, 0.5);

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

float rot0 = 0, dz = 0, rotacion=0;

float rot1 = 0.0, rot2 = 0.0, rot3=0.0, rot4=0.0, rot5 = 0.0, rot6 = 0.0, rot7 = 0.0, rot8 = 0.0;
float ro1 = 0.0, ro2 = 0.0, ro3 = 0.0, ro4 = 0.0, ro5 = 0.0, ro6 = 0.0, ro7 = 0.0, ro8 = 0.0;


bool sentido = true;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
		int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
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
		destroy();
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
	glEnable(GL_CULL_FACE);

	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	//Descomentar
	shaderTexture.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado.fs");

	// Inicializar los buffers VAO, VBO, EBO
	sphere1.init();
	// Método setter que colocar el apuntador al shader
	sphere1.setShader(&shader);
	//Setter para poner el color de la geometria
	sphere1.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));

	// Inicializar los buffers VAO, VBO, EBO
	sphere2.init();
	// Método setter que colocar el apuntador al shader
	sphere2.setShader(&shader);
	//Setter para poner el color de la geometria
	sphere2.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	// Inicializar los buffers VAO, VBO, EBO
	sphere7.init();
	// Método setter que colocar el apuntador al shader
	sphere7.setShader(&shaderTexture);
	//Setter para poner el color de la geometria
	sphere7.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
	camera->setPosition(glm::vec3(0.0, 0.0, 4.0));

	cylinder1.init();
	cylinder1.setShader(&shaderTexture);
	cylinder1.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
	camera->setPosition(glm::vec3(0.0, 0.0, 4.0));

	cylinder2.init();
	cylinder2.setShader(&shaderTexture);

	box1.init();
	//settea el shader a utilizar
	box1.setShader(&shaderTexture);
	box1.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
	camera->setPosition(glm::vec3(0.0, 0.0, 4.0));

	box2.init();
	box2.setShader(&shader);
	box2.setColor(glm::vec4(0.60, 0.40, 0.24, 1.0));
	box2.init();

	box3.init();
	box3.setShader(&shaderTexture);
	box3.setColor(glm::vec4(0.60, 0.40, 0.24, 1.0));
	camera->setPosition(glm::vec3(0.0, 0.0, 4.0));

	sphere3.init();
	sphere3.setShader(&shader);

	// Descomentar
	//definimos el tamanio de la imagen 
	int imageWidth, imageHeight;
	//definimos la textura a utilizar
	Texture texture1("../Textures/sponge.jpg");
	//carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	FIBITMAP *bitmap = texture1.loadImage();
	//convertimos el mapa de bits en una arreglo unidiemnsional de tipo unsigned
	unsigned char *data = texture1.convertToData(bitmap, imageWidth,
			imageHeight);
	//creando la textura con id 1
	glGenTextures(1, &textureID1);
	//enlazar esa textura a un tipo de textura de 2d
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//verifica si se pudo abrir la textura
	if (data) {
		//tranferir los dtos de la imagen a memoria
		//pararametros; tipos de textura, niveles de bit maps, formato interno de opengl, ancho, alto, bitmap, 
		//formato interno de la imagen, típo de dtao, apuntador  a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		//genera los niveles del mimap opengl es el encargado de realizarlo
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	//libera la memoria de la textura
	texture1.freeImage(bitmap);

	//definimos la textura 2 a utilizar
	Texture texture2("../Textures/water.jpg");
	//carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = texture2.loadImage();
	//convertimos el mapa de bits en una arreglo unidiemnsional de tipo unsigned
	data = texture2.convertToData(bitmap, imageWidth,
		imageHeight);
	//creando la textura con id 1
	glGenTextures(1, &textureID2);
	//enlazar esa textura a un tipo de textura de 2d
	glBindTexture(GL_TEXTURE_2D, textureID2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//verifica si se pudo abrir la textura
	if (data) {
		//tranferir los dtos de la imagen a memoria
		//pararametros; tipos de textura, niveles de bit maps, formato interno de opengl, ancho, alto, bitmap, 
		//formato interno de la imagen, típo de dtao, apuntador  a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		//genera los niveles del mimap opengl es el encargado de realizarlo
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	//libera la memoria de la textura
	texture2.freeImage(bitmap);


	//definimos la textura 3 a utilizar
	Texture texture3("../Textures/goku.png");
	//carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = texture3.loadImage(true); //voltear la imagen 
	//convertimos el mapa de bits en una arreglo unidiemnsional de tipo unsigned
	data = texture3.convertToData(bitmap, imageWidth,
		imageHeight);
	//creando la textura con id 1
	glGenTextures(1, &textureID3);
	//enlazar esa textura a un tipo de textura de 2d
	glBindTexture(GL_TEXTURE_2D, textureID3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//verifica si se pudo abrir la textura
	if (data) {
		//tranferir los dtos de la imagen a memoria
		//pararametros; tipos de textura, niveles de bit maps, formato interno de opengl, ancho, alto, bitmap, 
		//formato interno de la imagen, típo de dtao, apuntador  a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		//genera los niveles del mimap opengl es el encargado de realizarlo
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	//libera la memoria de la textura
	texture3.freeImage(bitmap);

	//definimos la textura 4 a utilizar
	Texture texture4("../Textures/coca_cola.jpg");
	//carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = texture4.loadImage(true); //voltear la imagen 
	//convertimos el mapa de bits en una arreglo unidiemnsional de tipo unsigned
	data = texture4.convertToData(bitmap, imageWidth,
		imageHeight);
	//creando la textura con id 1
	glGenTextures(1, &textureID4);
	//enlazar esa textura a un tipo de textura de 2d
	glBindTexture(GL_TEXTURE_2D, textureID4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//verifica si se pudo abrir la textura
	if (data) {
		//tranferir los dtos de la imagen a memoria
		//pararametros; tipos de textura, niveles de bit maps, formato interno de opengl, ancho, alto, bitmap, 
		//formato interno de la imagen, típo de dtao, apuntador  a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		//genera los niveles del mimap opengl es el encargado de realizarlo
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	//libera la memoria de la textura
	texture3.freeImage(bitmap);


}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Destrucción de los VAO, EBO, VBO
	sphere1.destroy();
	cylinder1.destroy();
	box1.destroy();

	shader.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
		int mode) {
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
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, 0.01);
	offsetX = 0;
	offsetY = 0;

	//movimiento en brazos
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		sentido = true;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
		rot1 += 0.001;
	else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
		rot1 -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sentido)
		rot2 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && sentido)
		rot3 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && sentido)
		rot4 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && sentido)
		rot5 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && sentido)
		rot6 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && sentido)
		rot7 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && sentido)
		rot8 += 0.001;

	sentido = false;

	//movimiento en piernas
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		sentido = true;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
		ro1 += 0.001;
	else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && sentido)
		ro1 -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && sentido)
		ro2 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && sentido)
		ro3 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && sentido)
		ro4 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && sentido)
		ro5 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && sentido)
		ro6 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && sentido)
		ro7 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && sentido)
		ro8 += 0.001;

	sentido = false;

	//movimiento de avance y rotacion
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rot0 = 0.0001;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rot0 = -0.0001;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		dz = 0.0001;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		dz = -0.0001;


	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::mat4 model = glm::mat4(1.0f);
	float offX = 0.0;
	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float) screenWidth / (float) screenHeight, 0.01f, 100.0f);
		glm::mat4 view = camera->getViewMatrix();

		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));
		//Descomentar
		//settea la matriz vista y proyeciion al nuevo shader
		shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(view));

		model = glm::translate(model, glm::vec3(0, 0, dz));
		model = glm::rotate(model, rot0, glm::vec3(0, 1, 0));


		//modelo bob esponja
		//box1.enableWireMode();
		//usamos la textura id 1
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		box1.render(glm::scale(model, glm::vec3(1.0, 1.0, 0.1)));
		//Descomentar
		//sirve para no utilizar ninguna textura
		glBindTexture(GL_TEXTURE_2D, 0);

		//pantalon
		glm::mat4 p1 = glm::translate(model, glm::vec3(0.0, -0.4, 0.0));
		box2.render(glm::scale(p1, glm::vec3(1.0, 0.2, 0.5)));

		// Articulacion 1
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 j1 = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		sphere1.render(glm::scale(j1, glm::vec3(0.1, 0.1, 0.1)));
		j1 = glm::rotate(j1, rot1, glm::vec3(0, 0, 1));			//hacemos una rotacion en el eje z
		j1 = glm::rotate(j1, rot2, glm::vec3(0, 1, 0));			//hacemos una rotacion en el eje y
		glBindTexture(GL_TEXTURE_2D, 0);

		// Hueso 1
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 l1 = glm::translate(j1, glm::vec3(0.25f, 0.0, 0.0));
		l1 = glm::rotate(l1, glm::radians(90.0f), glm::vec3(0, 0, 1.0));
		cylinder1.render(glm::scale(l1, glm::vec3(0.1, 0.5, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		// Articulacion 2
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 j2 = glm::translate(j1, glm::vec3(0.5, 0.0f, 0.0f));
		j2 = glm::rotate(j2, rot3, glm::vec3(0, 0, 1));			//hacemos una rotacion en el eje z
		j2 = glm::rotate(j2, rot4, glm::vec3(1, 0, 0));			//hacemos una rotacion en el eje x
		sphere1.render(glm::scale(j2, glm::vec3(0.1, 0.1, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		// Hueso 2
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 l2 = glm::translate(j2, glm::vec3(0.15, 0.0, 0.0));
		l2 = glm::rotate(l2, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.1));
		//cylinder1.enableWireMode();
		cylinder1.render(glm::scale(l2, glm::vec3(0.1, 0.25, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		// Ojo
		glm::mat4 ojo = glm::translate(model, glm::vec3(0.25, 0.25, 0.05));
		//sphere1.enableWireMode();
		sphere2.render(glm::scale(ojo, glm::vec3(0.2, 0.2, 0.1)));

		glm::mat4 ojo2 = glm::translate(model, glm::vec3(-0.25, 0.25, 0.05));
		//sphere2.enableWireMode();
		sphere2.render(glm::scale(ojo2, glm::vec3(0.2, 0.2, 0.1)));

		glm::mat4 ojoi = glm::translate(model, glm::vec3(0.25, 0.25, 0.5));
		sphere3.render(glm::scale(ojo, glm::vec3(0.1, 0.1, 0.2)));

		glm::mat4 ojo2i = glm::translate(model, glm::vec3(-0.25, 0.25, 0.5));
		sphere3.render(glm::scale(ojo2, glm::vec3(0.1, 0.1, 0.2)));

		//brazo derecho
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 j3 = glm::translate(model, glm::vec3(-0.5f, 0.0, 0.0));
		sphere1.render(glm::scale(j3, glm::vec3(0.1, 0.1, 0.1)));
		j3 = glm::rotate(j3, rot5, glm::vec3(0, 0, 1));			//hacemos una rotacion en el eje z
		j3 = glm::rotate(j3, rot6, glm::vec3(0, 1, 0));			//hacemos una rotacion en el eje y
		glBindTexture(GL_TEXTURE_2D, 0);

		//hueso tres
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 l3 = glm::translate(j3, glm::vec3(-0.25, 0.0, 0.0));
		l3 = glm::rotate(l3, glm::radians(90.0f), glm::vec3(0, 0, 0.1));
		cylinder1.render(glm::scale(l3, glm::vec3(0.1, 0.5, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//segunda articulacion
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 j4 = glm::translate(j3, glm::vec3(-0.5, 0.0, 0.0));
		j4 = glm::rotate(j4, rot7, glm::vec3(0, 0, 1));			//hacemos una rotacion en el eje z
		j4 = glm::rotate(j4, rot8, glm::vec3(1, 0, 0));			//hacemos una rotacion en el eje x
		sphere1.render(glm::scale(j4, glm::vec3(0.1, 0.1, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//hueso cuatro
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 l4 = glm::translate(j4, glm::vec3(-0.15, 0.0, 0.0));
		l4 = glm::rotate(l4, glm::radians(90.0f), glm::vec3(0.0, 0.0, -1.1));
		cylinder1.render(glm::scale(l4, glm::vec3(0.1, 0.25, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//pierna derecha
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 j5 = glm::translate(model, glm::vec3(-0.25, -0.5, 0.0));
		sphere1.render(glm::scale(j5, glm::vec3(0.1, 0.1, 0.1)));
		j5 = glm::rotate(j5, ro1, glm::vec3(0, 0, 1));			//hacemos una rotacion en el eje z
		j5 = glm::rotate(j5, ro2, glm::vec3(0, 1, 0));			//hacemos una rotacion en el eje y
		glBindTexture(GL_TEXTURE_2D, 0);

		//hueso cinco
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 l5 = glm::translate(j5, glm::vec3(0.0, -0.15, 0.0));
		l5 = glm::rotate(l5, glm::radians(00.0f), glm::vec3(0, 0, 0.1));
		cylinder1.render(glm::scale(l5, glm::vec3(0.1, 0.25, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//segunda articulacion
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 j6 = glm::translate(j5, glm::vec3(0.0, -0.30, 0.0));
		j6 = glm::rotate(j6, ro3, glm::vec3(0, 1, 0));			//hacemos una rotacion en el eje z
		sphere1.render(glm::scale(j6, glm::vec3(0.1, 0.1, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//hueso seis
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 l6 = glm::translate(j5, glm::vec3(0.0, -0.4, 0.0));
		l6 = glm::rotate(l6, glm::radians(00.0f), glm::vec3(0.0, 0.0, -1.1));
		cylinder1.render(glm::scale(l6, glm::vec3(0.1, 0.25, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//zapato
		glm::mat4 j11 = glm::translate(j5, glm::vec3(0.0, -0.58, 0.0));
		sphere3.render(glm::scale(j11, glm::vec3(0.1, 0.1, 0.1)));


		//pierna izquierda
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 j7 = glm::translate(model, glm::vec3(0.25, -0.5, 0.0));
		sphere1.render(glm::scale(j7, glm::vec3(0.1, 0.1, 0.1)));
		j7 = glm::rotate(j7, ro5, glm::vec3(0, 0, 1));			//hacemos una rotacion en el eje z
		j7 = glm::rotate(j7, ro6, glm::vec3(0, 1, 0));			//hacemos una rotacion en el eje y
		glBindTexture(GL_TEXTURE_2D, 0);

		//hueso seis
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 l7 = glm::translate(j7, glm::vec3(0.0, -0.15, 0.0));
		l7 = glm::rotate(l7, glm::radians(00.0f), glm::vec3(0, 0, 0.1));
		cylinder1.render(glm::scale(l7, glm::vec3(0.1, 0.25, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//segunda articulacion
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 j8 = glm::translate(j7, glm::vec3(0.0, -0.25, 0.0));
		j8 = glm::rotate(j8, ro7, glm::vec3(0, 0, 1));			//hacemos una rotacion en el eje z
		sphere1.render(glm::scale(j8, glm::vec3(0.1, 0.1, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//hueso siete
		glBindTexture(GL_TEXTURE_2D, textureID1);
		shaderTexture.setFloat("offsetX", 0);
		glm::mat4 l8 = glm::translate(j7, glm::vec3(0.0, -0.4, 0.0));
		l8 = glm::rotate(l8, glm::radians(00.0f), glm::vec3(0.0, 0.0, -1.1));
		cylinder1.render(glm::scale(l8, glm::vec3(0.1, 0.25, 0.1)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//zapato
		glm::mat4 j10 = glm::translate(j7, glm::vec3(0.0, -0.58, 0.0));
		sphere3.render(glm::scale(j10, glm::vec3(0.1, 0.1, 0.1)));



		glm::mat4 modelAgua = glm::mat4(1.0);
		modelAgua = glm::translate(modelAgua, glm::vec3(0.0, -3.0, 0.0));
		modelAgua = glm::scale(modelAgua, glm::vec3(5.0, 0.0, 5.0));
		//se activa la textura del agua
		glBindTexture(GL_TEXTURE_2D, textureID2);
		shaderTexture.setFloat("offsetX", offX);
		box3.render(modelAgua);
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 modelSphere = glm::mat4(1.0);
		modelSphere = glm::translate(modelSphere, glm::vec3(3.0, 0.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID3);
		shaderTexture.setFloat("offsetX", 0);
		sphere7.render(modelSphere);
		glBindTexture(GL_TEXTURE_2D, 0);

		//lata de coca cola
		glm::mat4 modelCylinder = glm::mat4(1.0);
		modelCylinder = glm::translate(modelCylinder, glm::vec3(-3.0, 0.0, 0.0));
		shaderTexture.setFloat("offsetX", 0);
		// Envolvente desde el indice 0, el tamanio es 20 * 20 * 6
		// Se usa la textura 1 ( Bon sponja)
		glBindTexture(GL_TEXTURE_2D, textureID4);
		cylinder2.render(0, cylinder2.getSlices() * cylinder2.getStacks() * 6,
				modelCylinder);
		// Tapa Superior desde el indice : 20 * 20 * 6, el tamanio de indices es 20 * 3
		// Se usa la textura 2 ( Agua )
		glBindTexture(GL_TEXTURE_2D, textureID4);
		cylinder2.render(cylinder2.getSlices() * cylinder2.getStacks() * 6,
				cylinder2.getSlices() * 3,
				modelCylinder);
		// Tapa inferior desde el indice : 20 * 20 * 6 + 20 * 3, el tamanio de indices es 20 * 3
		// Se usa la textura 3 ( Goku )
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder2.render(cylinder2.getSlices() * cylinder2.getStacks() * 6 + cylinder2.getSlices() * 3,
				cylinder2.getSlices() * 3,
				modelCylinder);

		glBindTexture(GL_TEXTURE_2D, 0);
		

		shader.turnOff();

		dz = 0;
		rot0 = 0;
		offX += 0.001;

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}

//tarea mapear todas las caras del cubo