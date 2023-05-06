//
//	STEROWANIE: W, A, S, D, UP, DOWN - RUCH
//				L - AIRPLANE LIGHTS ON/OFF
//				R - RESET
//				C - DRAW COLLIDER ON/OFF
//				V - TOOGLE PRINT_DEBUG
//				B - CLEAR CONSOLE WINDOW
//				T - TOGGLE DAY/NIGHT CYCLE
//				ESC - EXIT
//				

#define GLM_FORCE_RADIANS
#define PI 3.14159265359

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <iostream>
#include <algorithm>

#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "Airplane.h"
#include "Skull.h"
#include "Grid.h"
#include "Bird.h"
#include "Skybox.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const float AIRPLANE_SPEED = 400.0f;
const bool FIRST_PERSON = false;	// FIRST PERSON AKTUALNIE NIE DZIALA
const bool FREE_CAM = false;
const bool FLIP_X_AXIS_ROTATION_MOVEMENT = false;
const float FP_CAM_Y_OFFSET = -0.0f;
const float AIRPLANE_SCALE = 0.001f; 
const float FOV = 45.0f;
const bool ENABLE_ANISOTROPY = true;
bool DAY_NIGHT_CYCLE = false;
const float DAY_NIGHT_CYCLE_SPEED = 0.05f;
bool ENABLE_AIRPLANE_FLASHLIGHT = true;
bool DRAW_COLLIDERS = true;

bool PRINT_DEBUG_INFO = true;


// terrain settings
const int WIDTH = 256;
const int HEIGHT = 256;
const float WORLD_SCALE = 4.0f;
const float HEIGHT_SCALE = 1.0f;
const float MIN_HEIGHT = -256.0f;
const float MAX_HEIGHT = 256.0f;
const int FAULT_FORMATION_ITER = 500;
const float SMOOTH_TERRAIN_FILTER = 0.5;

// camera
Camera* cam;


// time 
float currentFrame = 0.0f;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


// terrain generator
Grid* grid;
glm::vec3 worldPos; // center of the grid/world


// lighting
Light* light;	//universal light pointer
DirectionalLight dirLight;
glm::vec3 dirLightDirecrion(0.2f, -1.0f, -0.3f);	// with implementation of day night cycle direction should not be static
vector<PointerLight> pointLights;

// shaders
Shader* lightCubeShader; // shader for drawing point light position
Shader* modelShader;
Shader* terrainShader;
Shader* skyboxShader;


// Models
Model* skull;
Model* airplane_model;
Model* bird_model;


// objects
Airplane* airPlane;
glm::vec3 startingPoint;
glm::vec3 flashlightColor = glm::vec3(1.0f, 1.0f, 0.0f);
float flashlightOffset = .5f;

// skulls
const int numOfSkulls = 10;
Skull* skulls[numOfSkulls];

// birds
const int numOfBirds = 1;
Bird* birds[numOfBirds];


PointerLight *airPlaneLight;

// input
bool input[6] = { false, false, false, false, false, false}; // is pressed?: w a s d UP DOWN
// mouse input
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;	//last cursor positon 
bool firstMouseInput = true;



// function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path, int repeat);
unsigned int loadCubemap(vector<std::string> faces);
void loadTextures();
void processInput();
GLFWwindow* initOpenGL();
void drawSkulls(glm::mat4 projection, glm::mat4 view, Shader *shader);
void drawBirds(glm::mat4 projection, glm::mat4 view, Shader* shader);
void drawTerrain(glm::mat4 projection, glm::mat4 view, Shader* shader);
void drawAirplane(glm::mat4 projection, glm::mat4 view, Shader* shader);
void initLights();
void initCamera();
void initShaders();
void initModels();
void initTerrain();
void initSkulls();
void initBirds();
void initSkybox();
void update(float deltaTime);
void debugMessage(std::string msg);

unsigned int skyboxVAO, skyboxVBO;
unsigned int cubemapTexture;


/////////////////////////////////////////////////////////////////////////////////////////////////////

// Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

// handles window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// INITIALIZATION
void init(GLFWwindow* window)
{

	debugMessage("starting general initialization");

	// flips loaded textures vertically
	stbi_set_flip_vertically_on_load(true);
	

	if (ENABLE_ANISOTROPY)
	{
		debugMessage("Anisotropy enabled");
		float maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
		glTexParameterf(GL_TEXTURE_2D,
			GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
	}

	// make cursor attached to window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// set callbacks
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// init everything else

	debugMessage("initializing shaders");
	initShaders();
	debugMessage("shaders initialized");

	debugMessage("initializing models");
	initModels();
	debugMessage("models initialized");

	debugMessage("initializing terrain");
	initTerrain();
	debugMessage("terrain initialized");

	debugMessage("initializing skulls");
	initSkulls();
	debugMessage("skulls initialized");

	debugMessage("initializing birds");
	initBirds();
	debugMessage("birds initialized");
	
	debugMessage("initializing camera");
	initCamera();
	debugMessage("camera initialized");

	debugMessage("initializing skybox");
	initSkybox();
	debugMessage("skybox initialized");



	// create airPlane player object
	debugMessage("creating Airplane object");
	glm::vec3 scale = glm::vec3(AIRPLANE_SCALE, AIRPLANE_SCALE, AIRPLANE_SCALE);
	airPlane = new Airplane("models/airplane/11804_Airplane_v2_l2.obj", cam, startingPoint,grid, AIRPLANE_SPEED, FIRST_PERSON,FLIP_X_AXIS_ROTATION_MOVEMENT,
		scale, flashlightColor, flashlightOffset);

	debugMessage("Airplane created");
	//non player - to be tested
	//airPlane = new Airplane("models/airplane/11804_Airplane_v2_l2.obj", glm::vec3(0.0f, 0.0f, -5.0f), AIRPLANE_SPEED, glm::vec3(AIRPLANE_SCALE, AIRPLANE_SCALE, AIRPLANE_SCALE));


	// setup light for all models
	debugMessage("initializing lights");
	initLights();
	debugMessage("lights initialized");



	// to avoid wrong depth rendering
	glEnable(GL_DEPTH_TEST);

	debugMessage("general initialziation succeded");
	debugMessage("program ready");


}
void initCamera()
{
	// default camera settings
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	float cameraSpeed = 20.0f; // not used
	float pitch = 0.0f, yaw = -90.0f, roll = 0.0f;  //camera rotation angle
	float fov = FOV;
	const float sensitivity = 0.1f; //mouse sensitivity

	// create camera - camera creation should be moved to AirPlane object for optimization and cleaner code
	cam = new Camera(FREE_CAM, cameraPos, cameraUp, -90.0f, 0.0f);
	cam->MovementSpeed = cameraSpeed;
	cam->MouseSensitivity = sensitivity;
	cam->Zoom = fov;
}
void initShaders()
{
	// create shaders
	lightCubeShader = new Shader("shaders/lightCubeShader.vs", "shaders/lightCubeShader.fs");
	modelShader = new Shader("shaders/modelShader.vs", "shaders/modelShader.fs");
	terrainShader = new Shader("Shaders/terrain.vs", "Shaders/terrain.fs");
	skyboxShader = new Shader("skybox.vs", "skybox.fs");
}
void initModels()
{
	// create models
	skull = new Model("models/skull/12140_Skull_v3_L2.obj");
	bird_model = new Model("models/bird/V25H37990MMNHK8FQT9AHQ4B5.obj");
}
void initTerrain()
{
	// create terrain
	grid = new Grid(WIDTH, HEIGHT, WORLD_SCALE, HEIGHT_SCALE, MIN_HEIGHT, MAX_HEIGHT, FAULT_FORMATION_ITER, SMOOTH_TERRAIN_FILTER);

	std::vector<float> TEXTURE_HEIGHT_LEVELS;
	TEXTURE_HEIGHT_LEVELS.push_back(0.2f);
	TEXTURE_HEIGHT_LEVELS.push_back(0.5f);
	TEXTURE_HEIGHT_LEVELS.push_back(0.8f);
	TEXTURE_HEIGHT_LEVELS.push_back(0.9f);

	grid->tGenerator.LoadTextures("textures/red_laterite_soil_stones_diff_1k.jpg",
		"textures/aerial_rocks_04_diff_1k.jpg",
		"textures/forrest_sand_01_diff_1k.jpg",
		"textures/snow_02_diff_1k_2.jpg",
		TEXTURE_HEIGHT_LEVELS, *grid);

	worldPos = glm::vec3(grid->Width * grid->WorldScale / 2.0f, grid->GetMaxHeight() + 20.0f, grid->Depth * grid->WorldScale / 2.0f);
	startingPoint = worldPos;
}
void initSkulls()
{

	float lightOffset = 30.0f;

	// initialize skulls with random position and scale

	float minXZ = -grid->Width * grid->WorldScale;
	float maxXZ = grid->Width * grid->WorldScale;

	float minY = startingPoint.y;
	float maxY = minY + 10.0f;

	float minScale = 1.0f;
	float maxScale = 1.0f;

	float minRGB = 0.5f;
	float maxRGB = 1.0f;

	float x = 0;
	float y = 0;
	float z = 0;
	float s = 0;

	// random light colors
	float R = 0;
	float G = 0; 
	float B = 0;



	glm::vec3 pos = glm::vec3(0,0,0);
	glm::vec3 scale = glm::vec3(0,0,0);
	glm::vec3 rotation = glm::vec3(0,0,0);

	for (int i = 0; i < numOfSkulls; i++)
	{
		
		x = minXZ + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxXZ - minXZ)));
		y = minY + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxY - minY)));
		z = minXZ + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxXZ - minXZ)));
		s = minScale + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxScale - minScale)));

		R = minRGB + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxRGB - minRGB)));
		G = minRGB + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxRGB - minRGB)));
		B = minRGB + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxRGB - minRGB)));


		pos = glm::vec3(x, y, z) + startingPoint;
		scale = glm::vec3(s, s, s);

		skulls[i] = new Skull("models/skull/12140_Skull_v3_L2.obj", pos, lightCubeShader);


		skulls[i]->light.light.position = pos +glm::vec3(lightOffset,lightOffset,0.0f);
		skulls[i]->light.light.color = glm::vec3(R, G, B);
		skulls[i]->light.light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		skulls[i]->light.light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		skulls[i]->light.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
		skulls[i]->light.light.constant = 1.0f;
		skulls[i]->light.light.linear = 0.014f;
		skulls[i]->light.light.quadratic = 0.0007f;


		skulls[i]->transform.setScale(scale);

		
	}
}
void initBirds()
{

	float lightOffset = 30.0f;

	// initialize skulls with random position and scale

	float minXZ = -grid->Width * grid->WorldScale;
	float maxXZ = grid->Width * grid->WorldScale;

	float minY = startingPoint.y-250;
	float maxY = minY + 50.0f;

	float minScale = 8.0f;
	float maxScale = 9.0f;

	float minRGB = 0.5f;
	float maxRGB = 1.0f;

	float x = 0;
	float y = 0;
	float z = 0;
	float s = 0;

	// random light colors
	float R = 0;
	float G = 0;
	float B = 0;

	float r = 0;

	glm::vec3 pos = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(0, 0, 0);

	for (int i = 0; i < numOfBirds; i++)
	{

		x = minXZ + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxXZ - minXZ)));
		y = minY + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxY - minY)));
		z = minXZ + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxXZ - minXZ)));
		s = minScale + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxScale - minScale)));

		R = minRGB + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxRGB - minRGB)));
		G = minRGB + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxRGB - minRGB)));
		B = minRGB + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (maxRGB - minRGB)));

		r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360.0f));
		
		pos = glm::vec3(x, y, z) + startingPoint;
		rotation = glm::vec3(0,glm::radians(r), 0);
		scale = glm::vec3(s, s, s);
		stbi_set_flip_vertically_on_load(false);
		birds[i] = new Bird("models/bird/V25H37990MMNHK8FQT9AHQ4B5.obj", pos, lightCubeShader);
		stbi_set_flip_vertically_on_load(true);
		birds[i]->transform.SetRotation(rotation);


		birds[i]->light.light.position = pos + glm::vec3(lightOffset, lightOffset, 0.0f);
		birds[i]->light.light.color = glm::vec3(R, G, B);
		birds[i]->light.light.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
		birds[i]->light.light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		birds[i]->light.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
		birds[i]->light.light.constant = 1.0f;
		birds[i]->light.light.linear = 0.014f;
		birds[i]->light.light.quadratic = 0.0007f;


		birds[i]->transform.setScale(scale);


	}
}
void initLights()
{
	//temporary light setup
	dirLight.light.direction = dirLightDirecrion;
	dirLight.light.color = glm::vec3(1.0f, 1.0f, 1.0f);
	dirLight.light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	dirLight.light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	dirLight.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	dirLight.setSunSpeed(DAY_NIGHT_CYCLE_SPEED);

	light = &dirLight;		// universal light pointer is set to directional light

	// light for skulls
	for (int i = 0; i < numOfSkulls; i++)
	{

		PointerLight* p = &skulls[i]->light;
		light = p;
		skulls[i]->model->setLightData(light);

		pointLights.push_back(*p);

		light = &dirLight;
		skulls[i]->model->setLightData(light);
	}
	light = &dirLight;
	// for birds
	for (int i = 0; i < numOfBirds; i++)
	{

		PointerLight* p = &birds[i]->light;
		light = p;
		birds[i]->model->setLightData(light);

		pointLights.push_back(*p);

		light = &dirLight;
		birds[i]->model->setLightData(light);
	}

	light = &dirLight;
	airPlane->model->setLightData(light);
	airPlane->model->setLightData(pointLights, *lightCubeShader);

	

	// for testing only
	airPlaneLight = new PointerLight(airPlane->transform.Position, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0014f, 0.000007f);

}
void initSkybox() {
	
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load textures
	// -------------
	vector<std::string> faces
	{
		("skybox/right.jpg"),
		("skybox/left.jpg"),
		("skybox/top.jpg"),
		("skybox/bottom.jpg"),
		("skybox/front.jpg"),
		("skybox/back.jpg"),
	};
	cubemapTexture = loadCubemap(faces);
}
void drawSkybox(glm::mat4 projection, glm::mat4 view, Camera* camera) {
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader->use();
	view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
	skyboxShader->setMat4("view", view);
	skyboxShader->setMat4("projection", projection);
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default
}

// DRAWING
void drawTerrain(glm::mat4 projection, glm::mat4 view, Shader* shader)
{
	glm::mat4 model = glm::mat4(1.0f);


	// set shader variables / uniforms
	shader->use();
	shader->setFloat("minHeight", grid->MinHeight * grid->HeightScaleFactor);
	shader->setFloat("maxHeight", grid->MaxHeight * grid->HeightScaleFactor);
	shader->setMat4("model", model);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);
	shader->setVec3("lightDir", dirLight.light.direction);
	shader->setVec3("lightColor", dirLight.light.color);


	//airplane light


	shader->setBool("enableAirplaneFlashLight", ENABLE_AIRPLANE_FLASHLIGHT);
	shader->setVec3("light1.position", airPlane->leftFlashlight.position);
	shader->setVec3("light1.direction", airPlane->leftFlashlight.direction);
	shader->setFloat("light1.cutOff", airPlane->leftFlashlight.cutOff);
	shader->setFloat("light1.outerCutOff", airPlane->leftFlashlight.outerCutOff);
	shader->setVec3("light1.color", airPlane->leftFlashlight.color);
	shader->setFloat("light1.constant", airPlane->leftFlashlight.constant);
	shader->setFloat("light1.linear", airPlane->leftFlashlight.linear);
	shader->setFloat("light1.quadratic", airPlane->leftFlashlight.quadratic);

	shader->setVec3("light2.position", airPlane->rightFlashlight.position);
	shader->setVec3("light2.direction", airPlane->rightFlashlight.direction);
	shader->setFloat("light2.cutOff", airPlane->rightFlashlight.cutOff);
	shader->setFloat("light2.outerCutOff", airPlane->rightFlashlight.outerCutOff);
	shader->setVec3("light2.color", airPlane->rightFlashlight.color);
	shader->setFloat("light2.constant", airPlane->rightFlashlight.constant);
	shader->setFloat("light2.linear", airPlane->rightFlashlight.linear);
	shader->setFloat("light2.quadratic", airPlane->rightFlashlight.quadratic);


	grid->Draw(*shader);
}
void drawSkulls(glm::mat4 projection, glm::mat4 view, Shader* shader)
{
	// model matrix
	glm::mat4 model = glm::mat4(1.0f);

	// RENDER SKULL MODEL
	// start using model shader
	shader->use();
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);

	for (int i = 0; i < numOfSkulls; i++)
	{

		model = glm::mat4(1.0f);

		model = glm::translate(model, skulls[i]->transform.Position); // translate it down so it's at the center of the scene
		model = glm::rotate(model,skulls[i]->transform.Pitch,glm::vec3(1.0f,0.0f,0.0f));
		model = glm::rotate(model,skulls[i]->transform.Yaw,glm::vec3(0.0f,1.0f,0.0f));
		model = glm::rotate(model,skulls[i]->transform.Roll,glm::vec3(0.0f,0.0f,1.0f));
		model = glm::scale(model, skulls[i]->transform.scale);	// it's a bit too big for our scene, so scale it down
		model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		shader->use();
		shader->setMat4("model", model);
		skulls[i]->model->Draw(*shader);

		skulls[i]->drawLight(projection, view);

	}


}


float angle = 0.0f;
void drawBirds(glm::mat4 projection, glm::mat4 view, Shader* shader)
{
	// model matrix
	glm::mat4 model = glm::mat4(1.0f);

	// start using model shader
	shader->use();
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);

	for (int i = 0; i < numOfBirds; i++)
	{

		model = glm::mat4(1.0f);
		model = glm::translate(model, birds[i]->transform.Position); // translate it down so it's at the center of the scene
		
				// obróć model, żeby przód zgadzał się z transform.Front
		glm::vec3 x = glm::vec3(1.0, 0.0, 0.0);
		float dot = glm::dot(x, birds[i]->transform.Front);
		float l1 = glm::length(x);
		float l2 = glm::length(birds[i]->transform.Front);
		angle += glm::acos(dot / (l1 * l2));

		float p = PI * (2.0);
		angle = std::modf(angle, &p);

		model = glm::rotate(model, angle, glm::vec3(0.0, 1.0, 0.0));

		std::cout << angle << "\n";

		//model = glm::rotate(model, birds[i]->transform.Yaw, birds[i]->transform.Up);
		
		model = glm::scale(model, birds[i]->transform.scale);	// it's a bit too big for our scene, so scale it down

		shader->use();
		shader->setMat4("model", model);
		birds[i]->model->Draw(*shader);

		birds[i]->drawLight(projection, view);



	// draw helper cube
		lightCubeShader->use();
		lightCubeShader->setMat4("projection", projection);
		lightCubeShader->setMat4("view", view);

		birds[i]->light.light.position = birds[i]->transform.Position + birds[i]->transform.Front * 3.0f;
		birds[i]->light.draw(*lightCubeShader);

	}






}
void drawAirplane(glm::mat4 projection, glm::mat4 view, Shader* shader)
{

	//airPlane->checkTerrainCollision(grid);
	//printf("POZYCJA Y: %f\tWYSOKOSC TERENU: %f\n", airPlane->GetCollider(1).Position.y, airPlane->checkTerrainCollision(grid,airPlane->GetCollider(1)));
	//debugMessage("POZYCJA Y: " + std::to_string(airPlane->GetCollider(1).Position.y) + "\tWYSOKOSC TERENU: " + std::to_string(airPlane->checkTerrainCollision(grid, airPlane->GetCollider(1))));
	//debugMessage("POZYCJA X: " + std::to_string(airPlane->transform.Position.x) + "\tPOZYCJA Y: " + std::to_string(airPlane->transform.Position.y));

	// RENDER AIRPLANE MODEL
	glm::mat4 model = glm::mat4(1.0f);

	model = airPlane->calcModelMatrix(model);

	shader->use();
	shader->setMat4("model", model);
	airPlane->model->Draw(*shader);


	// draw colliders
	if (DRAW_COLLIDERS)
	{
		lightCubeShader->use();
		lightCubeShader->setMat4("projection", projection);
		lightCubeShader->setMat4("view", view);

		airPlaneLight->cubeScale = 0.02f;

		airPlaneLight->light.position = airPlane->GetCollider(1).Position;
		airPlaneLight->draw(*lightCubeShader);

		airPlaneLight->light.position = airPlane->GetCollider(2).Position;
		airPlaneLight->draw(*lightCubeShader);
	}

	// draw helper cube
	//lightCubeShader->use();
	//lightCubeShader->setMat4("projection", projection);
	//lightCubeShader->setMat4("view", view);

	//airPlaneLight->light.position = airPlane->transform.Position;
	//airPlaneLight->draw(*lightCubeShader);



}

// MAIN DRAW FUNCTION
void drawScene(GLFWwindow* window)
{
	// clear window
	glClearColor(0.03f, 0.03f, 0.03f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, (float)grid->Width * grid->Depth * grid->WorldScale);
	glm::mat4 view = cam->GetViewMatrix();

	// draw terrain
	drawTerrain(projection, view, terrainShader);

	// start using model shader
	modelShader->use();
	modelShader->setMat4("projection", projection);
	modelShader->setMat4("view", view);

	//drawSkulls(projection, view, modelShader);
	drawAirplane(projection, view, modelShader);
	drawBirds(projection, view, modelShader);
	drawSkybox(projection, view, cam);

	glfwSwapBuffers(window);
}


void update(float deltaTime)
{
	GameObject::UpdateGameObjects(deltaTime);
	
	if (DAY_NIGHT_CYCLE)
	{
		// day night cycle
		dirLight.dayNight(deltaTime);

		//update lights
			// light for skulls
		for (int i = 0; i < numOfSkulls; i++)
		{
			light = &dirLight;
			skulls[i]->model->setLightData(light);
		}
		

		for (int i = 0; i < numOfBirds; i++)
		{
			light = &dirLight;
			birds[i]->model->setLightData(light);
		}

		light = &dirLight;
		airPlane->model->setLightData(light);
	}


}



int main(void)
{
	srand(std::time(NULL));
	GLFWwindow* window = initOpenGL(); // initialize openGl and create window

	// set all necesarry settings / create all necessary objects
	init(window);

	// draw loop
	while (!glfwWindowShouldClose(window)) 
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		processInput();
		update(deltaTime);
		drawScene(window);

		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}



	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}



// everything that needs to be done to intialize openGL library // create window
GLFWwindow* initOpenGL()
{
	debugMessage("InitializingOpenGL");


	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	debugMessage("OpenGl initialized, window [SIZE " + std::to_string(SCR_HEIGHT) + "x" + std::to_string(SCR_WIDTH) + "] created");

	return window;
}


//INPUT
// 
// keyboard input callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// close window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// reset airplane position 
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		airPlane->reset();
	}

	// airplane lights switch
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		ENABLE_AIRPLANE_FLASHLIGHT = !ENABLE_AIRPLANE_FLASHLIGHT;
	}


	// switch draw colliders
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		DRAW_COLLIDERS = !DRAW_COLLIDERS;
	}

	// toggle debug
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		PRINT_DEBUG_INFO = !PRINT_DEBUG_INFO;
	}

	// toggle day/night cycle
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		DAY_NIGHT_CYCLE = !DAY_NIGHT_CYCLE;
	}

	// clear console
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
	{
		#ifdef _WIN32
				std::system("cls");
		#else
				// Assume POSIX
				std::system("clear");
		#endif
	}

	// AIRPLANE MOVEMENT
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
	{
		airPlane->onMovementRelease(Move_direction::M_LEFT);
	}
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
	{
		airPlane->onMovementRelease(Move_direction::M_RIGHT);
	}




	// SET / CLEAR INPUT BOOLEANS
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		input[0] = true;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		input[1] = true;
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		input[2] = true;
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		input[3] = true;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		input[4] = true;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		input[5] = true;

	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		input[0] = false;
	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		input[1] = false;
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		input[2] = false;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		input[3] = false;
	if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
		input[4] = false;
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
		input[5] = false;
}

// process keyboard input 
void processInput()
{
	if (input[0])
		airPlane->processMovement(Move_direction::M_FORWARD, deltaTime);
	if (input[1])
		airPlane->processMovement(Move_direction::M_BACKWARD, deltaTime);
	if (input[2])
		airPlane->processMovement(Move_direction::M_LEFT, deltaTime);
	if (input[3])
		airPlane->processMovement(Move_direction::M_RIGHT, deltaTime);
	if (input[4])
		airPlane->processMovement(Move_direction::M_RISE, deltaTime);
	if (input[5])
		airPlane->processMovement(Move_direction::M_DIVE, deltaTime);


	// camera
	if (input[0])
		cam->ProcessKeyboard(M_FORWARD, deltaTime);
	if (input[1])
		cam->ProcessKeyboard(M_BACKWARD, deltaTime);
	if (input[2])
		cam->ProcessKeyboard(M_LEFT, deltaTime);
	if (input[3])
		cam->ProcessKeyboard(M_RIGHT, deltaTime);


}

// mouse movement input callback for camera movement - may be used in first person camera
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouseInput) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseInput = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	cam->ProcessMouseMovement(xoffset, yoffset);

}

// mouse scroll input callback - camera zoom
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam->ProcessMouseScroll(yoffset);
}



// utility function for loading a 2D texture from file
// path - path to texture file
// repeat: should be one of GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
// ---------------------------------------------------
unsigned int loadTexture(char const* path, int repeat)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);							// przetestowac mirrored repeat na teksturze terenu
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
unsigned int loadCubemap(vector<std::string> faces)
{
	stbi_set_flip_vertically_on_load(false);
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	stbi_set_flip_vertically_on_load(true);

	return textureID;
}

void debugMessage(std::string msg)
{
	if (!PRINT_DEBUG_INFO)
		return;

	std::cout << " --" << msg << "\n";
}
