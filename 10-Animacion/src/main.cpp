#define _USE_MATH_DEFINES
#include <cmath>
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

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

GLFWwindow *window;

Shader shader;
//Shader de texturizado
Shader shaderTexture;
//Shader con iluminacion solo color
Shader shaderColorLighting;
//Shader con iluminacion y textura
Shader shaderTextureLighting;
// Descomentar
//Shader con materiales
Shader shaderMaterialLighting;
// Descomentar
//Shader con skybox
Shader shaderSkybox;
// Shader con multiples luces
Shader shaderMulLighting;

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere1(20, 20);
Sphere sphere2(20, 20);
Sphere sphere3(20, 20);
Sphere sphereLamp(20, 20);
// Descomentar
Sphere skyboxSphere(20, 20);
Cylinder cylinder1(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Cylinder cylinderMaterials(20, 20, 0.5, 0.5);
Box boxMaterials;
Box box1;
Box box2;
Box box3;
Box boxCesped;
Box boxWall;
// Models complex instances
Model modelRock;
Model modelRailRoad;
Model modelAircraft;
Model modelTable;
Model modelBuro;
Model modelCama;
Model modelCasaInterior;
Model modelEscritorio;
Model modelCarretera;
Model modelEclipseChasis;
Model modelHeliChasis;
Model modelHeliElises;
//Model modelMueble;


GLuint textureID1, textureID2, textureID3, textureID4, textureID5, textureID6, textureID7;
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

//texturas de cada cara del cubo
std::string fileNames[6] = {
		"../Textures/mp_rainforest/rainforest_ft.tga",
		"../Textures/mp_rainforest/rainforest_bk.tga",
		"../Textures/mp_rainforest/rainforest_up.tga",
		"../Textures/mp_rainforest/rainforest_dn.tga",
		"../Textures/mp_rainforest/rainforest_rt.tga",
		"../Textures/mp_rainforest/rainforest_lf.tga" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

float rot0 = 0.0, dz = 0.0;

float rot1 = 0.0, rot2 = 0.0, rot3 = 0.0, rot4 = 0.0;
bool sentido = true;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
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
	shaderTexture.initialize("../Shaders/texturizado_res.vs",
			"../Shaders/texturizado_res.fs");
	shaderColorLighting.initialize("../Shaders/iluminacion_color_res.vs",
			"../Shaders/iluminacion_color_res.fs");
	shaderTextureLighting.initialize("../Shaders/iluminacion_texture_res.vs",
			"../Shaders/iluminacion_texture_res.fs");
	shaderMaterialLighting.initialize("../Shaders/iluminacion_material.vs",
			"../Shaders/iluminacion_material_res.fs");
	shaderSkybox.initialize("../Shaders/cubeTexture.vs",
			"../Shaders/cubeTexture.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_texture_res.vs",
			"../Shaders/multipleLights.fs");

	// Inicializar los buffers VAO, VBO, EBO
	sphere1.init();
	// Método setter que colocar el apuntador al shader
	sphere1.setShader(&shaderColorLighting);
	//Setter para poner el color de la geometria
	sphere1.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	// Inicializar los buffers VAO, VBO, EBO
	sphere2.init();
	// Método setter que colocar el apuntador al shader
	sphere2.setShader(&shaderColorLighting);
	//Setter para poner el color de la geometria
	sphere2.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	// Inicializar los buffers VAO, VBO, EBO
	sphereLamp.init();
	// Método setter que colocar el apuntador al shader
	sphereLamp.setShader(&shader);
	//Setter para poner el color de la geometria
	sphereLamp.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	cylinder1.init();
	cylinder1.setShader(&shaderColorLighting);
	cylinder1.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	cylinder2.init();
	cylinder2.setShader(&shaderMulLighting);

	cylinderMaterials.init();
	cylinderMaterials.setShader(&shaderMaterialLighting);
	cylinderMaterials.setColor(glm::vec4(0.3, 0.3, 1.0, 1.0));

	boxMaterials.init();
	boxMaterials.setShader(&shaderMaterialLighting);

	// Inicializacion de la esfera del skybox
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	box1.init();
	// Settea el shader a utilizar
	box1.setShader(&shaderMulLighting);
	box1.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));

	box2.init();
	box2.setShader(&shaderMulLighting);

	sphere3.init();
	sphere3.setShader(&shaderMulLighting);

	box3.init();
	box3.setShader(&shaderMulLighting);

	boxCesped.init();
	boxCesped.setShader(&shaderMulLighting);

	boxWall.init();
	boxWall.setShader(&shaderMulLighting);

	modelRock.loadModel("../models/rock/rock.obj");
	modelRock.setShader(&shaderMulLighting);

	modelTable.loadModel("../models/CenterTable/CenterTable.obj");
	modelTable.setShader(&shaderMulLighting);

	modelRailRoad.loadModel("../models/railroad/railroad_track.obj");
	modelRailRoad.setShader(&shaderMulLighting);

	modelAircraft.loadModel("../models/Aircraft_obj/E 45 Aircraft_obj.obj");
	modelAircraft.setShader(&shaderMulLighting);

	modelBuro.loadModel("../models/nightstand_01/eb_nightstand_01.obj");
	modelBuro.setShader(&shaderMulLighting);

	modelCama.loadModel("../models/bed/Full_Size_Bed_with_White_Sheets_Black_V1.obj");
	modelCama.setShader(&shaderMulLighting);

	modelCasaInterior.loadModel("../models/House_interior/house interior.obj");
	modelCasaInterior.setShader(&shaderMulLighting);

	modelEscritorio.loadModel("../models/Escritorio/mesa_pc.obj");
	modelEscritorio.setShader(&shaderMulLighting);

	modelMueble.loadModel("../models/Wood_Table/Wood_Table.obj");
	modelMueble.setShader(&shaderMulLighting);

	modelCarretera.loadModel("../models/RoadV2/Road/roadV2.obj");
	modelCarretera.setShader(&shaderMulLighting);

	// Eclipse
	modelEclipseChasis.loadModel("../models/Eclipse/2003eclipse.obj");
	modelEclipseChasis.setShader(&shaderMulLighting);

	//Helicoptero
	modelHeliChasis.loadModel("../models/helicopter/Mi_24_chasis.obj");
	modelHeliChasis.setShader(&shaderMulLighting);
	modelHeliElises.loadModel("../models/helicopter/Mi_24_heli.obj");
	modelHeliElises.setShader(&shaderMulLighting);

	camera->setPosition(glm::vec3(0.0, 3.0, 4.0));

	// Descomentar
	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	// Definiendo la textura a utilizar
	Texture texture1("../Textures/sponge.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	FIBITMAP *bitmap = texture1.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	unsigned char *data = texture1.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID1);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture1.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture2("../Textures/water.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = texture2.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture2.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID2);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture2.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture3("../Textures/goku.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture3.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture3.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID3);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture3.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture4("../Textures/pared_exterior.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture4.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture4.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID4);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture4.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture5("../Textures/white.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture5.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture5.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID5);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID5);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture5.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture6("../Textures/piedra.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture6.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture6.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID6);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID6);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture6.freeImage(bitmap);

/*
	// Definiendo la textura a utilizar
	Texture texture4("../Textures/texturaLadrillos.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture4.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture4.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID4);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture4.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture texture5("../Textures/cesped.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	// Voltear la imagen
	bitmap = texture5.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture5.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureID5);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureID5);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	texture5.freeImage(bitmap);
	*/

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP *bitmap = skyboxTexture.loadImage(true);
		unsigned char *data = skyboxTexture.convertToData(bitmap, imageWidth,
				imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}

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

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
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

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rot1 += 0.001;
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rot1 -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rot2 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rot2 -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		rot3 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		rot3 -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		rot4 += 0.001;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		rot4 -= 0.001;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rot0 = 0.001;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rot0 = -0.001;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		dz = 0.001;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		dz = -0.001;

	sentido = true;

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0, 1.5, 0.0));
	float offX = 0.0;
	float angle = 0.0;
	float ratio = 30.0;
	glm::mat4 matrixModelAircraft = glm::mat4(1.0);
	matrixModelAircraft = glm::translate(matrixModelAircraft, glm::vec3(8.0, 2.0, -10.0));
	int state = 0;
	float offsetAircraftAdvance = 0.0;
	float offsetAircraftRot = 0.0;
	glm::mat4 modelMatrixEclipse = glm::mat4(1.0f);
	modelMatrixEclipse = glm::translate(modelMatrixEclipse, glm::vec3(20, 0, 10.0));
	float rotHeliElisesY = 0.0;
	float advanceCount = 0.0;
	float rotCount = 0.0;
	float rolWheelsX = 0.0;
	float rolWheelsY = 0.0;

	while (psi) {
		psi = processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / (float)screenHeight, 0.01f, 100.0f);
		glm::mat4 view = camera->getViewMatrix();

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader con solo textura
		shaderTexture.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con iluminacion solo color
		shaderColorLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderColorLighting.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con iluminacion con textura
		shaderTextureLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderTextureLighting.setMatrix4("view", 1, false,
			glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con iluminacion con material
		shaderMaterialLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderMaterialLighting.setMatrix4("view", 1, false,
			glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
			glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
			glm::value_ptr(view));

		// Propiedades de la luz para objetos con color
		shaderColorLighting.setVectorFloat3("viewPos",
			glm::value_ptr(camera->getPosition()));
		shaderColorLighting.setVectorFloat3("light.ambient",
			glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderColorLighting.setVectorFloat3("light.diffuse",
			glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderColorLighting.setVectorFloat3("light.specular",
			glm::value_ptr(glm::vec3(0.9, 0.0, 0.0)));

		// Propiedades de la luz para objetos con textura
		shaderTextureLighting.setVectorFloat3("viewPos",
			glm::value_ptr(camera->getPosition()));
		shaderTextureLighting.setVectorFloat3("light.ambient",
			glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTextureLighting.setVectorFloat3("light.diffuse",
			glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTextureLighting.setVectorFloat3("light.specular",
			glm::value_ptr(glm::vec3(0.9, 0.0, 0.0)));

		// Propiedades de la luz para objetos con textura
		shaderMaterialLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMaterialLighting.setVectorFloat3("light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMaterialLighting.setVectorFloat3("light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMaterialLighting.setVectorFloat3("light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));

		// Propiedades de la luz para objetos con multiples luces
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.4, 0.4)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));
		// Esto es para la luz spotlight
		shaderMulLighting.setInt("spotLightCount", 1);
		shaderMulLighting.setVectorFloat3("spotLights[0].position", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(camera->getFront()));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.01, 0.01, 0.01)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.4, 0.4)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.6, 0.6, 0.6)));
		shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5)));
		shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0)));
		shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		shaderMulLighting.setFloat("spotLights[0].linear", 0.1);
		shaderMulLighting.setFloat("spotLights[0].quadratic", 0.001);

		// Esto es para la luces pointlights
		//numeros de luces a utilizar pointlight= 3 y tambien se modifica el shader de multipleluces en la parte del maximo de luces
		shaderMulLighting.setInt("pointLightCount", 4);
		//posicion de la luz con indice 0
		shaderMulLighting.setVectorFloat3("pointLights[0].position", glm::value_ptr((glm::vec3(-5.1, 4.5, -3.5))));

		//propiedades de la luz verde
		shaderMulLighting.setVectorFloat3("pointLights[0].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.0, 0.01, 0.0))); //color verde
		shaderMulLighting.setVectorFloat3("pointLights[0].light.specular", glm::value_ptr(glm::vec3(0.0, 0.6, 0.0)));
		shaderMulLighting.setFloat("pointLights[0].constant", 1.0);
		shaderMulLighting.setFloat("pointLights[0].linear", 0.04);
		shaderMulLighting.setFloat("pointLights[0].quadratic", 0.004);

		//propiedades de la luz roja
		shaderMulLighting.setVectorFloat3("pointLights[1].position", glm::value_ptr((glm::vec3(-5.1, 4.8, -5.0))));
		shaderMulLighting.setVectorFloat3("pointLights[1].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[1].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.0, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[1].light.specular", glm::value_ptr(glm::vec3(0.6, 0.0, 0.0)));
		shaderMulLighting.setFloat("pointLights[1].constant", 1.0);
		shaderMulLighting.setFloat("pointLights[1].linear", 0.04);
		shaderMulLighting.setFloat("pointLights[1].quadratic", 0.004);

		//propiedades para la luz azul
		shaderMulLighting.setVectorFloat3("pointLights[2].position", glm::value_ptr((glm::vec3(-5.1, 4.5, -6.5))));
		shaderMulLighting.setVectorFloat3("pointLights[2].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[2].light.diffuse", glm::value_ptr(glm::vec3(0.0, 0.0, 0.01)));
		shaderMulLighting.setVectorFloat3("pointLights[2].light.specular", glm::value_ptr(glm::vec3(0.0, 0.0, 0.6)));
		shaderMulLighting.setFloat("pointLights[2].constant", 1.0);
		shaderMulLighting.setFloat("pointLights[2].linear", 0.04);
		shaderMulLighting.setFloat("pointLights[2].quadratic", 0.004);

		//propiedades de la luz amarilla
		shaderMulLighting.setVectorFloat3("pointLights[3].position", glm::value_ptr((glm::vec3(-14.9, 4.8, -5.0))));
		shaderMulLighting.setVectorFloat3("pointLights[3].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[3].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.01, 0.0)));
		shaderMulLighting.setVectorFloat3("pointLights[3].light.specular", glm::value_ptr(glm::vec3(0.6, 0.6, 0.0)));
		shaderMulLighting.setFloat("pointLights[3].constant", 1.0);
		shaderMulLighting.setFloat("pointLights[3].linear", 0.04);
		shaderMulLighting.setFloat("pointLights[3].quadratic", 0.004);

		//propiedades de la luz morada
		shaderMulLighting.setVectorFloat3("pointLights[4].position", glm::value_ptr((glm::vec3(-12.9, 4.8, -5.0))));
		shaderMulLighting.setVectorFloat3("pointLights[4].light.ambient", glm::value_ptr(glm::vec3(0.001, 0.001, 0.001)));
		shaderMulLighting.setVectorFloat3("pointLights[4].light.diffuse", glm::value_ptr(glm::vec3(0.01, 0.0, 0.01)));
		shaderMulLighting.setVectorFloat3("pointLights[4].light.specular", glm::value_ptr(glm::vec3(0.6, 0.6, 0.0)));
		shaderMulLighting.setFloat("pointLights[4].constant", 1.0);
		shaderMulLighting.setFloat("pointLights[4].linear", 0.04);
		shaderMulLighting.setFloat("pointLights[4].quadratic", 0.004);

		//colocar las esferas de las luces
		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.2));
		sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -3.5));
		sphereLamp.setColor(glm::vec4(0.0, 1.0, 0.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.2));
		sphereLamp.setPosition(glm::vec3(-5.1, 4.8, -5.0));
		sphereLamp.setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.2));
		sphereLamp.setPosition(glm::vec3(-5.1, 4.5, -6.5));
		sphereLamp.setColor(glm::vec4(0.0, 0.0, 1.0, 1.0));
		sphereLamp.render();

		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.2));
		sphereLamp.setPosition(glm::vec3(-14.9, 4.8, -5.0));
		sphereLamp.setColor(glm::vec4(0.8, 0.8, 0.0, 1.0));
		sphereLamp.render();
		//mejorar esta luz
		sphereLamp.setScale(glm::vec3(0.1, 0.1, 0.2));
		sphereLamp.setPosition(glm::vec3(-12.9, 4.8, -5.0));
		sphereLamp.setColor(glm::vec4(0.8, 0.0, 0.8, 1.0));
		sphereLamp.render();

		glm::mat4 lightModelmatrix = glm::rotate(glm::mat4(1.0f), angle,
			glm::vec3(1.0f, 0.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix,
			glm::vec3(0.0f, 0.0f, -ratio));
		// Posicion luz para objetos con color
		shaderColorLighting.setVectorFloat3("light.position",
			glm::value_ptr(
				glm::vec4(
					lightModelmatrix
					* glm::vec4(0.0, 0.0, 0.0, 1.0))));

		// Posicion luz para objetos con textura
		shaderTextureLighting.setVectorFloat3("light.position",
			glm::value_ptr(
				glm::vec4(
					lightModelmatrix
					* glm::vec4(0.0, 0.0, 0.0, 1.0))));

		// Posicion luz para objetos con materiales
		shaderMaterialLighting.setVectorFloat3("light.position",
			glm::value_ptr(
				glm::vec4(
					lightModelmatrix
					* glm::vec4(0.0, 0.0, 0.0, 1.0))));
		sphereLamp.setScale(glm::vec3(1, 1, 1));
		sphereLamp.setPosition(glm::vec3(0, 0, 0));
		sphereLamp.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
		sphereLamp.render(lightModelmatrix);

		model = glm::translate(model, glm::vec3(0, 0, dz));
		model = glm::rotate(model, rot0, glm::vec3(0, 1, 0));

		/*
				//box1.enableWireMode();
				// Usamos la textura ID 1
				glBindTexture(GL_TEXTURE_2D, textureID1);
				box1.render(glm::scale(model, glm::vec3(1.0, 1.0, 0.1)));
				// No utilizar ninguna textura
				glBindTexture(GL_TEXTURE_2D, 0);

				// Articulacion 1
				glm::mat4 j1 = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
				j1 = glm::rotate(j1, rot1, glm::vec3(0, 0, 1));
				j1 = glm::rotate(j1, rot2, glm::vec3(0, 1, 0));
				sphere1.enableWireMode();
				sphere1.render(glm::scale(j1, glm::vec3(0.1, 0.1, 0.1)));

				// Hueso 1
				glm::mat4 l1 = glm::translate(j1, glm::vec3(0.25f, 0.0, 0.0));
				l1 = glm::rotate(l1, glm::radians(90.0f), glm::vec3(0, 0, 1.0));
				//cylinder1.enableWireMode();
				cylinder1.render(glm::scale(l1, glm::vec3(0.1, 0.5, 0.1)));

				// Articulacion 2
				glm::mat4 j2 = glm::translate(j1, glm::vec3(0.5, 0.0f, 0.0f));
				j2 = glm::rotate(j2, rot3, glm::vec3(0.0, 0.0, 1.0));
				j2 = glm::rotate(j2, rot4, glm::vec3(1.0, 0.0, 0.0));
				sphere1.enableWireMode();
				sphere1.render(glm::scale(j2, glm::vec3(0.1, 0.1, 0.1)));

				// Hueso 2
				glm::mat4 l2 = glm::translate(j2, glm::vec3(0.25, 0.0, 0.0));
				l2 = glm::rotate(l2, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
				//cylinder1.enableWireMode();
				cylinder1.render(glm::scale(l2, glm::vec3(0.1, 0.5, 0.1)));

				// Ojo
				glm::mat4 ojo = glm::translate(model, glm::vec3(0.25, 0.25, 0.05));
				//sphere1.enableWireMode();
				sphere1.render(glm::scale(ojo, glm::vec3(0.2, 0.2, 0.1)));

				glm::mat4 ojo2 = glm::translate(model, glm::vec3(-0.25, 0.25, 0.05));
				//sphere2.enableWireMode();
				sphere2.render(glm::scale(ojo2, glm::vec3(0.2, 0.2, 0.1)));

				glm::mat4 modelAgua = glm::mat4(1.0);
				modelAgua = glm::translate(modelAgua, glm::vec3(5.0, 0.01, 5.0));
				modelAgua = glm::scale(modelAgua, glm::vec3(5.0, 0.01, 5.0));
				// Se activa la textura del agua
				glBindTexture(GL_TEXTURE_2D, textureID2);
				// Le cambiamos el shader con multiples luces NO OLVIDAR
				shaderMulLighting.setFloat("offsetX", offX);
				box2.render(modelAgua);
				glBindTexture(GL_TEXTURE_2D, 0);
				shaderMulLighting.setFloat("offsetX", 0);

				glm::mat4 modelCesped = glm::mat4(1.0);
				modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
				modelCesped = glm::scale(modelCesped, glm::vec3(40.0, 0.001, 40.0));
				// Se activa la textura del agua
				glBindTexture(GL_TEXTURE_2D, textureID5);
				boxCesped.render(modelCesped);
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 modelSphere = glm::mat4(1.0);
				modelSphere = glm::translate(modelSphere, glm::vec3(10.0, 0.5, 0.0));
				glBindTexture(GL_TEXTURE_2D, textureID3);
				sphere3.render(modelSphere);
				glBindTexture(GL_TEXTURE_2D, 0);

				glm::mat4 modelCylinder = glm::mat4(1.0);
				modelCylinder = glm::translate(modelCylinder,
						glm::vec3(-3.0, 0.5, 0.0));
				// Envolvente desde el indice 0, el tamanio es 20 * 20 * 6
				// Se usa la textura 1 ( Bon sponja)
				glBindTexture(GL_TEXTURE_2D, textureID1);
				cylinder2.render(0, cylinder2.getSlices() * cylinder2.getStacks() * 6,
						modelCylinder);
				// Tapa Superior desde el indice : 20 * 20 * 6, el tamanio de indices es 20 * 3
				// Se usa la textura 2 ( Agua )
				glBindTexture(GL_TEXTURE_2D, textureID2);
				cylinder2.render(cylinder2.getSlices() * cylinder2.getStacks() * 6,
						cylinder2.getSlices() * 3, modelCylinder);
				// Tapa inferior desde el indice : 20 * 20 * 6 + 20 * 3, el tamanio de indices es 20 * 3
				// Se usa la textura 3 ( Goku )
				glBindTexture(GL_TEXTURE_2D, textureID3);
				cylinder2.render(
						cylinder2.getSlices() * cylinder2.getStacks() * 6
								+ cylinder2.getSlices() * 3, cylinder2.getSlices() * 3,
						modelCylinder);
				glBindTexture(GL_TEXTURE_2D, 0);

				// Render del cubo con textura de ladrillos y con repeticion en x
				glm::mat4 cubeTextureModel = glm::mat4(1.0);
				cubeTextureModel = glm::translate(cubeTextureModel,
						glm::vec3(-5.0, 0.5, 3.0));
				glBindTexture(GL_TEXTURE_2D, textureID4);
				shaderMulLighting.setVectorFloat2("scaleUV",
						glm::value_ptr(glm::vec2(2.0, 1.0)));
				box3.render(cubeTextureModel);
				shaderMulLighting.setVectorFloat2("scaleUV",
						glm::value_ptr(glm::vec2(0.0, 0.0)));
				glBindTexture(GL_TEXTURE_2D, 0);

				// Render del cyindro con materiales
				glm::mat4 cylinderMaterialModel = glm::mat4(1.0);
				cylinderMaterialModel = glm::translate(cylinderMaterialModel,  glm::vec3(3.0, 0.5, -3.0));
				shaderMaterialLighting.setVectorFloat3("material.ambient", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
				shaderMaterialLighting.setVectorFloat3("material.diffuse", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
				shaderMaterialLighting.setVectorFloat3("material.specular", glm::value_ptr(glm::vec3(0.727811f, 0.626959f, 0.626959f)));
				shaderMaterialLighting.setFloat("material.shininess", 76.8f);
				cylinderMaterials.render(cylinderMaterialModel);

				glm::mat4 boxMaterialModel = glm::mat4(1.0f);
				boxMaterialModel = glm::translate(boxMaterialModel, glm::vec3(-3.0, 0.5, -3.0));
				shaderMaterialLighting.setVectorFloat3("material.ambient", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
				shaderMaterialLighting.setVectorFloat3("material.diffuse", glm::value_ptr(glm::vec3(0.61424f, 0.04136f, 0.04136f)));
				shaderMaterialLighting.setVectorFloat3("material.specular", glm::value_ptr(glm::vec3(0.727811f, 0.626959f, 0.626959f)));
				shaderMaterialLighting.setFloat("material.shininess", 76.8f);
				boxMaterials.render(boxMaterialModel);

				//Models complex render
				glm::mat4 matrixModelRock = glm::mat4(1.0);
				matrixModelRock = glm::translate(matrixModelRock, glm::vec3(-3.0, 0.0, 6.0));
				modelRock.render(matrixModelRock);
				// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
				glActiveTexture(GL_TEXTURE0);
				// Esto es para las vias del tren
				glm::mat4 matrixModelRailroad = glm::mat4(1.0);
				matrixModelRailroad = glm::translate(matrixModelRailroad, glm::vec3(3.0, 0.0, 10.0));
				modelRailRoad.render(matrixModelRailroad);
				// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
				glActiveTexture(GL_TEXTURE0);

				//se construye la casa
				glBindTexture(GL_TEXTURE_2D, textureID4);
				boxWall.setScale(glm::vec3(10.0, 5.0, 0.05));		//escalamiento
				boxWall.setPosition(glm::vec3(-10.0, 2.5, 0.0));	//posicion
				boxWall.setOrientation(glm::vec3(0.0, 0.0, 0.0));	//eje de giro
				boxWall.render();
				boxWall.setScale(glm::vec3(10.0, 5.0, 0.05));
				boxWall.setPosition(glm::vec3(-15.0, 2.5, -5.0));
				boxWall.setOrientation(glm::vec3(0.0, 90.0, 0.0));
				boxWall.render();
				boxWall.setScale(glm::vec3(10.0, 5.0, 0.05));
				boxWall.setPosition(glm::vec3(-5.0, 2.5, -5.0));
				boxWall.setOrientation(glm::vec3(0.0, 90.0, 0.0));
				boxWall.render();
				boxWall.setScale(glm::vec3(10.0, 5.0, 0.05));
				boxWall.setPosition(glm::vec3(-10.0, 2.5, -10.0));
				boxWall.setOrientation(glm::vec3(0.0, 0.0, 0.0));
				boxWall.render();
				boxWall.setScale(glm::vec3(12.0, 12.0, 0.05));
				boxWall.setPosition(glm::vec3(-10.0, 5, -5.0));
				boxWall.setOrientation(glm::vec3(90.0, 0.0, 0.0));
				boxWall.render();
				glBindTexture(GL_TEXTURE_2D, 0);

			*/

			//modelo de la casa//
		glm::mat4 cubeTex1 = glm::mat4(3.0);
		cubeTex1 = glm::translate(cubeTex1, glm::vec3(1.0, 2.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex1, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex2 = glm::mat4(1.0);
		cubeTex2 = glm::translate(cubeTex2, glm::vec3(2.0, 2.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex2, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex3 = glm::mat4(1.0);
		cubeTex3 = glm::translate(cubeTex3, glm::vec3(3.0, 2.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex3, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex4 = glm::mat4(1.0);
		cubeTex4 = glm::translate(cubeTex4, glm::vec3(4.0, 2.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex4, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex5 = glm::mat4(1.0);
		cubeTex5 = glm::translate(cubeTex5, glm::vec3(5.0, 2.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex5, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex6 = glm::mat4(1.0);
		cubeTex6 = glm::translate(cubeTex6, glm::vec3(6.0, 2.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex6, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex7 = glm::mat4(1.0);
		cubeTex7 = glm::translate(cubeTex7, glm::vec3(7.0, 2.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex7, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex8 = glm::mat4(1.0);
		cubeTex8 = glm::translate(cubeTex8, glm::vec3(8.0, 2.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex8, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex9 = glm::mat4(1.0);
		cubeTex9 = glm::translate(cubeTex9, glm::vec3(9.0, 2.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex9, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex10 = glm::mat4(1.0);
		cubeTex10 = glm::translate(cubeTex10, glm::vec3(10.0, 2.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex10, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex11 = glm::mat4(1.0);
		cubeTex11 = glm::translate(cubeTex11, glm::vec3(11.0, 2.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex11, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//Pared trasera//
		glm::mat4 cubeTex12 = glm::mat4(3.0);
		cubeTex12 = glm::translate(cubeTex12, glm::vec3(0.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex12, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex13 = glm::mat4(3.0);
		cubeTex13 = glm::translate(cubeTex13, glm::vec3(1.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex13, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex14 = glm::mat4(1.0);
		cubeTex14 = glm::translate(cubeTex14, glm::vec3(2.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex14, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex15 = glm::mat4(1.0);
		cubeTex15 = glm::translate(cubeTex15, glm::vec3(3.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex15, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex16 = glm::mat4(1.0);
		cubeTex16 = glm::translate(cubeTex16, glm::vec3(4.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex16, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex17 = glm::mat4(1.0);
		cubeTex17 = glm::translate(cubeTex17, glm::vec3(5.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex17, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex18 = glm::mat4(1.0);
		cubeTex18 = glm::translate(cubeTex18, glm::vec3(6.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex18, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex19 = glm::mat4(1.0);
		cubeTex19 = glm::translate(cubeTex19, glm::vec3(7.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex19, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex20 = glm::mat4(1.0);
		cubeTex20 = glm::translate(cubeTex20, glm::vec3(8.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex20, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex21 = glm::mat4(1.0);
		cubeTex21 = glm::translate(cubeTex21, glm::vec3(9.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex21, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex22 = glm::mat4(1.0);
		cubeTex22 = glm::translate(cubeTex22, glm::vec3(10.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex22, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex23 = glm::mat4(1.0);
		cubeTex23 = glm::translate(cubeTex23, glm::vec3(11.0, 3.0, 0.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex23, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);


		////Paredes Izquierdas 
		glm::mat4 cubeTex24 = glm::mat4(1.0);
		cubeTex24 = glm::translate(cubeTex24, glm::vec3(0.0, 2.0, 4.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex24, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex25 = glm::mat4(1.0);
		cubeTex25 = glm::translate(cubeTex25, glm::vec3(0.0, 2.0, 3.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex25, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex26 = glm::mat4(1.0);
		cubeTex26 = glm::translate(cubeTex26, glm::vec3(0.0, 2.0, 2.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex26, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex27 = glm::mat4(1.0);
		cubeTex27 = glm::translate(cubeTex27, glm::vec3(0.0, 2.0, 1.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex27, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex28 = glm::mat4(1.0);
		cubeTex28 = glm::translate(cubeTex28, glm::vec3(0.0, 2.0, 5.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex28, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex29 = glm::mat4(1.0);
		cubeTex29 = glm::translate(cubeTex29, glm::vec3(0.0, 2.0, 6.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex29, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex30 = glm::mat4(1.0);
		cubeTex30 = glm::translate(cubeTex30, glm::vec3(0.0, 2.0, 7.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex30, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex31 = glm::mat4(1.0);
		cubeTex31 = glm::translate(cubeTex31, glm::vec3(0.0, 2.0, 8.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex31, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex32 = glm::mat4(1.0);
		cubeTex32 = glm::translate(cubeTex32, glm::vec3(0.0, 2.0, 9.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex32, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex33 = glm::mat4(1.0);
		cubeTex33 = glm::translate(cubeTex33, glm::vec3(0.0, 2.0, 10.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex33, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex34 = glm::mat4(1.0);
		cubeTex34 = glm::translate(cubeTex34, glm::vec3(0.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex34, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//Paredes Derecha 0
		glm::mat4 cubeTex35 = glm::mat4(1.0);
		cubeTex35 = glm::translate(cubeTex35, glm::vec3(11.0, 2.0, 4.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex35, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex36 = glm::mat4(1.0);
		cubeTex36 = glm::translate(cubeTex36, glm::vec3(11.0, 2.0, 3.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex36, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex37 = glm::mat4(1.0);
		cubeTex37 = glm::translate(cubeTex37, glm::vec3(11.0, 2.0, 2.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex37, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex38 = glm::mat4(1.0);
		cubeTex38 = glm::translate(cubeTex38, glm::vec3(11.0, 2.0, 1.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex38, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex39 = glm::mat4(1.0);
		cubeTex39 = glm::translate(cubeTex39, glm::vec3(11.0, 2.0, 5.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex39, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex40 = glm::mat4(1.0);
		cubeTex40 = glm::translate(cubeTex40, glm::vec3(11.0, 2.0, 6.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex40, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex41 = glm::mat4(1.0);
		cubeTex41 = glm::translate(cubeTex41, glm::vec3(11.0, 2.0, 7.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex41, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex42 = glm::mat4(1.0);
		cubeTex42 = glm::translate(cubeTex42, glm::vec3(11.0, 2.0, 8.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex42, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex43 = glm::mat4(1.0);
		cubeTex43 = glm::translate(cubeTex43, glm::vec3(11.0, 2.0, 9.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex43, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex44 = glm::mat4(1.0);
		cubeTex44 = glm::translate(cubeTex44, glm::vec3(11.0, 2.0, 10.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex44, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex45 = glm::mat4(1.0);
		cubeTex45 = glm::translate(cubeTex45, glm::vec3(11.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex45, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		//Pared Frontales//
		glm::mat4 cubeTex46 = glm::mat4(3.0);
		cubeTex46 = glm::translate(cubeTex46, glm::vec3(0.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex46, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex47 = glm::mat4(3.0);
		cubeTex47 = glm::translate(cubeTex47, glm::vec3(1.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex47, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex48 = glm::mat4(1.0);
		cubeTex48 = glm::translate(cubeTex48, glm::vec3(2.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex48, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex49 = glm::mat4(1.0);
		cubeTex49 = glm::translate(cubeTex49, glm::vec3(3.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex49, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex50 = glm::mat4(1.0);
		cubeTex50 = glm::translate(cubeTex50, glm::vec3(4.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex50, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex51 = glm::mat4(1.0);
		cubeTex51 = glm::translate(cubeTex51, glm::vec3(5.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex51, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex52 = glm::mat4(1.0);
		cubeTex52 = glm::translate(cubeTex52, glm::vec3(6.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex52, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex53 = glm::mat4(1.0);
		cubeTex53 = glm::translate(cubeTex53, glm::vec3(7.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex53, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex54 = glm::mat4(1.0);
		cubeTex54 = glm::translate(cubeTex54, glm::vec3(8.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex54, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex55 = glm::mat4(1.0);
		cubeTex55 = glm::translate(cubeTex55, glm::vec3(9.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex55, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex56 = glm::mat4(1.0);
		cubeTex56 = glm::translate(cubeTex56, glm::vec3(10.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex56, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		glm::mat4 cubeTex57 = glm::mat4(1.0);
		cubeTex57 = glm::translate(cubeTex57, glm::vec3(11.0, 2.0, 11.0));
		glBindTexture(GL_TEXTURE_2D, textureID4);
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(2.0, 1.0)));
		box3.render(glm::scale(cubeTex57, glm::vec3(2.0, 2.0, 1.0)));
		glBindTexture(GL_TEXTURE_2D, 0);
		//Techo de la casa
		glm::mat4 modelTecho = glm::mat4(1.0);
		modelTecho = glm::translate(modelTecho, glm::vec3(5.5, 1.0, 5.5));
		glBindTexture(GL_TEXTURE_2D, textureID6);
		shaderTexture.setFloat("offsetX", offX);
		box2.render(glm::scale(modelTecho, glm::vec3(11.0, 0.01, 11.0)));
		shaderTexture.setFloat("offsetX", 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		///Piso 
		glm::mat4 modelPisoCasa = glm::mat4(1.0);
		modelPisoCasa = glm::translate(modelPisoCasa, glm::vec3(5.5, 3.0, 5.5));
		glBindTexture(GL_TEXTURE_2D, textureID5);
		shaderTexture.setFloat("offsetX", offX);
		box2.render(glm::scale(modelPisoCasa, glm::vec3(11.0, 0.01, 11.0)));
		shaderTexture.setFloat("offsetX", 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		//Piso afuera
		glm::mat4 modelPiso = glm::mat4(1.0);
		modelPiso = glm::translate(modelPiso, glm::vec3(5.5, 0.8, 5.5));
		glBindTexture(GL_TEXTURE_2D, textureID6);
		shaderTexture.setFloat("offsetX", offX);
		box2.render(glm::scale(modelPiso, glm::vec3(22.0, 0.01, 22.0)));
		shaderTexture.setFloat("offsetX", 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Models mesa de centro
		glm::mat4 matrixModelTable = glm::mat4(1.0);
		matrixModelTable = glm::translate(matrixModelTable, glm::vec3(3.0, 1.0, 3.0));
		modelTable.render(matrixModelTable);
		// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);


		//Models Buro
		glm::mat4 matrixModelBuro = glm::mat4(1.0);
		matrixModelBuro = glm::translate(matrixModelBuro, glm::vec3(1.0, 1.0, 3.0));
		matrixModelBuro = glm::scale(matrixModelBuro, glm::vec3(0.008, 0.008, 0.008));
		modelBuro.render(matrixModelBuro);
		// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);

		//Models Buro
		glm::mat4 matrixModelBuro2 = glm::mat4(1.0);
		matrixModelBuro2 = glm::translate(matrixModelBuro2, glm::vec3(1.0, 1.0, 6.0));
		matrixModelBuro2 = glm::scale(matrixModelBuro2, glm::vec3(0.008, 0.008, 0.008));
		modelBuro.render(matrixModelBuro2);
		// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);

		//Models Buro
		glm::mat4 matrixModelBuro3 = glm::mat4(1.0);
		matrixModelBuro3 = glm::translate(matrixModelBuro3, glm::vec3(1.0, 1.0, 10.0));
		matrixModelBuro3 = glm::scale(matrixModelBuro3, glm::vec3(0.008, 0.008, 0.008));
		modelBuro.render(matrixModelBuro3);
		// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);

		//Models Cama
		glm::mat4 matrixModelCama = glm::mat4(1.0);
		matrixModelCama = glm::translate(matrixModelCama, glm::vec3(3.5, 1.4, 9.0));
		matrixModelCama = glm::rotate(matrixModelCama, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
		matrixModelCama = glm::rotate(matrixModelCama, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
		matrixModelCama = glm::scale(matrixModelCama, glm::vec3(0.008, 0.008, 0.008));
		modelCama.render(matrixModelCama);
		// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);

		//Models house interior 
		glm::mat4 matrixModelCasa = glm::mat4(1.0);
		matrixModelCasa = glm::translate(matrixModelCasa, glm::vec3(10.0, 1.0, 10.0));
		matrixModelCasa = glm::rotate(matrixModelCasa, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
		matrixModelCasa = glm::scale(matrixModelCasa, glm::vec3(0.008, 0.008, 0.008));
		modelCasaInterior.render(matrixModelCasa);
		// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);

		//Models escritotio
		glm::mat4 matrixModelEs = glm::mat4(1.0);
		matrixModelEs = glm::translate(matrixModelEs, glm::vec3(9.0, 1.0, 5.0));
		matrixModelEs = glm::scale(matrixModelEs, glm::vec3(0.008, 0.008, 0.008));
		modelEscritorio.render(matrixModelEs);
		// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);
/*
		//Models mueble 
		glm::mat4 matrixModelWood_Table = glm::mat4(1.0);
		matrixModelWood_Table = glm::translate(matrixModelWood_Table, glm::vec3(9.0, 1.0, 7.0));
		//matrixModelWood_Table = glm::scale(matrixModelWood_Table, glm::vec3(0.008, 0.008, 0.008));
		modelMueble.render(matrixModelWood_Table);
		// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);
*/
		//Models Carretera
		glm::mat4 matrixModelCarretera = glm::mat4(1.0);
		matrixModelCarretera = glm::translate(matrixModelCarretera, glm::vec3(16.0, 0.0, 40.0));
		matrixModelCarretera = glm::scale(matrixModelCarretera, glm::vec3(10, 2, 10.0));
		modelCarretera.render(matrixModelCarretera);
		// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);


		// Render for the eclipse car
		glm::mat4 modelMatrixEclipseChasis = glm::mat4(modelMatrixEclipse);
		modelMatrixEclipseChasis = glm::scale(modelMatrixEclipse, glm::vec3(0.5, 0.5, 0.5));
		modelEclipseChasis.render(modelMatrixEclipseChasis);
		glActiveTexture(GL_TEXTURE0);

		//helicoptero
		glm::mat4 modelMatrixHeliChasis = glm::mat4(1.0);
		modelMatrixHeliChasis = glm::translate(modelMatrixHeliChasis, glm::vec3(17.0, 5.0, 5.0));
		modelHeliChasis.render(modelMatrixHeliChasis);
		glActiveTexture(GL_TEXTURE0);

		glm::mat4 modelMatrixHeliElises = glm::mat4(modelMatrixHeliChasis);
		modelMatrixHeliElises = glm::translate(modelMatrixHeliElises, glm::vec3(-0.003344, 1.88318, -0.254566));
		modelMatrixHeliElises = glm::rotate(modelMatrixHeliElises, rotHeliElisesY, glm::vec3(0.0, 1.0, 0.0));
		modelMatrixHeliElises = glm::translate(modelMatrixHeliElises, glm::vec3(0.003344, -1.88318, 0.254566));
		modelHeliElises.render(modelMatrixHeliElises);
		glActiveTexture(GL_TEXTURE0);

//		modelAircraft.render(matrixModelAircraft);
//		glActiveTexture(GL_TEXTURE0);

//		if (angle > 2 * M_PI)
//			angle = 0.0;
//		else
//			angle += 0.0001;

		// Se Dibuja el Skybox
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);

		dz = 0;
		rot0 = 0;
		offX += 0.001;
		rolWheelsX += 0.01;
		rotHeliElisesY += 0.01;

		/*******************************************
		 * State machines
		 *******************************************/
		 // State machine for eclipse car
		switch (state) {
		case 0:
			modelMatrixEclipse = glm::translate(modelMatrixEclipse, glm::vec3(0.0, 0.0, 0.1));
			advanceCount += 0.1;
			rolWheelsY -= 0.01;
			if (rolWheelsY < 0)
				rolWheelsY = 0;
			if (advanceCount > 10.0) {
				advanceCount = 0;
				state = 1;
			}
			break;
		case 1:
			modelMatrixEclipse = glm::translate(modelMatrixEclipse, glm::vec3(0.0, 0.0, 0.025));
			modelMatrixEclipse = glm::rotate(modelMatrixEclipse, glm::radians(0.5f), glm::vec3(0, 1, 0));
			rotCount += 0.5f;
			rolWheelsY += 0.01;
			if (rolWheelsY > glm::radians(15.0))
				rolWheelsY = glm::radians(15.0);
			if (rotCount >= 90.0) {
				rotCount = 0;
				state = 0;
			}
			break;
		}


		glfwSwapBuffers(window);
	}
}

int main(int argc, char **argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
