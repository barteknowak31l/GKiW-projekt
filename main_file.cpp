//
//	STEROWANIE: W A S D / UP DOWN - MOVEMENT
//				R - RESET
//				ESC - KONIEC
//

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>

#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "Airplane.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



// settings
const unsigned int SCR_WIDTH = 1920; //800;
const unsigned int SCR_HEIGHT = 1080; // 600;
const float AIRPLANE_SPEED = 2.0f;
const bool FIRST_PERSON = false;
const bool FLIP_X_AXIS_ROTATION_MOVEMENT = false;
const float FP_CAM_Y_OFFSET = -0.0f;
const float AIRPLANE_SACLE = 0.001f;
const float FOV = 45.0f;



// function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path);
void loadTextures();
void processInput();
GLFWwindow* initOpenGL();
void drawSkulls(glm::mat4 projection, glm::mat4 view);


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


// handles window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	 glViewport(0, 0, width, height);
}



//CAMERA
Camera* cam;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
const float cameraSpeed = 10.0f; // adjust accordingly

float pitch = 0.0f, yaw = -90.0f, roll = 0.0f;  //camera rotation angles
float lastX = SCR_WIDTH/2.0f, lastY = SCR_HEIGHT/2.0f;	//last cursor positon 
float fov = FOV;
const float sensitivity = 0.1f; //mouse sensitivity
bool firstMouseInput = true;

//time
float currentFrame = 0.0f;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


// lighting
Light* light;	//universal light pointer
DirectionalLight dirLight;
glm::vec3 dirLightDirecrion(0.2f, -1.0f, -0.3f);
vector<PointerLight> pointLights;
PointerLight* pointLight;
PointerLight* pointLight2;
PointerLight* pointLight3;
PointerLight* pointLight4;
glm::vec3 pointLightPos[] = 
{	glm::vec3(0.0f, 1.0f, -5.0f), 
	glm::vec3(0.0f, 1.0f, 5.0f),
	glm::vec3(5.0f, 1.0f, 0.0f),
	glm::vec3(-5.0f, 1.0f, 0.0f)};

//shaders
Shader* lightCubeShader;
Shader* modelShader;


//Models
Model* backpack;
Model* skull;
Model* airplane_model;


//objects
Airplane* airPlane;
glm::vec3 startingPoint = glm::vec3(0.0f, 0.0f, -2.5f);

//temporary input - to do: implement Input class
bool input[6] = { false, false, false, false, false, false}; // is pressed?: w a s d UP DOWN


void init(GLFWwindow* window)
{
	//temporary light setup - for testing purposes only
	dirLight.light.direction = dirLightDirecrion;
	dirLight.light.color = glm::vec3(1.0f,1.0f,1.0f);
	dirLight.light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	dirLight.light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	dirLight.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);


	pointLight = new PointerLight();
	pointLight2 = new PointerLight();
	pointLight3 = new PointerLight();
	pointLight4 = new PointerLight();

	pointLight->light.position = pointLightPos[0];
	pointLight->light.color = glm::vec3(171.0f/255.0f, 11.0f/255.0f,0.0f/255.0f);
	pointLight->light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	pointLight->light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	pointLight->light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLight->light.constant = 1.0f;
	pointLight->light.linear = 0.07f;
	pointLight->light.quadratic = 0.017f;

	pointLight2->light.position = pointLightPos[1];
	pointLight2->light.color = glm::vec3(0.0f, 0.0f, 1.0f);
	pointLight2->light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	pointLight2->light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	pointLight2->light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLight2->light.constant = 1.0f;
	pointLight2->light.linear = 0.07f;
	pointLight2->light.quadratic = 0.017f;

	pointLight3->light.position = pointLightPos[2];
	pointLight3->light.color = glm::vec3(1.0f, 1.0f, 0.0f);
	pointLight3->light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	pointLight3->light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	pointLight3->light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLight3->light.constant = 1.0f;
	pointLight3->light.linear = 0.07f;
	pointLight3->light.quadratic = 0.017f;

	pointLight4->light.position = pointLightPos[3];
	pointLight4->light.color = glm::vec3(0.0f, 1.0f, 0.0f);
	pointLight4->light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	pointLight4->light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	pointLight4->light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLight4->light.constant = 1.0f;
	pointLight4->light.linear = 0.07f;
	pointLight4->light.quadratic = 0.017f;

	pointLights.push_back(*pointLight);
	pointLights.push_back(*pointLight2);
	pointLights.push_back(*pointLight3);
	pointLights.push_back(*pointLight4);

	// flips loaded textures vertically
	stbi_set_flip_vertically_on_load(true);
	
	// make cursor attached to window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// set callbacks
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// create shaders
	lightCubeShader = new Shader("shaders/lightCubeShader.vs", "shaders/lightCubeShader.fs");
	modelShader = new Shader("shaders/modelShader.vs", "shaders/modelShader.fs");
	
	// create models
	backpack = new Model("models/backpack/backpack.obj");
	skull = new Model("models/skull/12140_Skull_v3_L2.obj");


	// create camera - camera creation should be moved to AirPlane object for optimization
	cam = new Camera(false,cameraPos, cameraUp, -90.0f, 0.0f);
	cam->MovementSpeed = cameraSpeed;
	cam->MouseSensitivity = sensitivity;
	cam->Zoom = fov;

	// to avoid wrong depth rendering
	glEnable(GL_DEPTH_TEST);



	//create airPlane player object
	airPlane = new Airplane("models/airplane/11804_Airplane_v2_l2.obj", cam, startingPoint, AIRPLANE_SPEED, FIRST_PERSON,FLIP_X_AXIS_ROTATION_MOVEMENT);

	//non player - to be tested
	//airPlane = new Airplane("models/airplane/11804_Airplane_v2_l2.obj", glm::vec3(0.0f, 0.0f, -5.0f), AIRPLANE_SPEED);


	// setup light for all models
	// universal light pointer is set to directional light
	light = &dirLight;
	skull->setLightData(light);
	skull->setLightData(pointLights, *lightCubeShader, false);
	backpack->setLightData(light);
	backpack->setLightData(pointLights, *lightCubeShader, false);
	airPlane->model->setLightData(light);
	airPlane->model->setLightData(pointLights, *lightCubeShader, false);


}


void drawSkulls(glm::mat4 projection, glm::mat4 view)
{

	// start using model shader
	modelShader->use();
	modelShader->setMat4("projection", projection);
	modelShader->setMat4("view", view);

	// model matrix
	glm::mat4 model = glm::mat4(1.0f);


	// RENDER SKULL MODEL
	// calculate its model matrix
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, -glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.2f, .2f, .2f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, -5.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, -5.0f, -5.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, -glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(5.0f, 0.0f, -5.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-5.0f, 0.0f, -5.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.2f, .2f, .2f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 5.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, -5.0f, 5.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, -glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(5.0f, 0.0f, 5.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 5.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.2f, .2f, .2f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->Draw(*modelShader);


}

void drawScene(GLFWwindow* window)
{
	// clear window
	glClearColor(0.03f, 0.03f, 0.03f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = cam->GetViewMatrix();

	// draw point lights
	for (int i = 0; i < pointLights.size(); i++)
	{
		lightCubeShader->use();
		lightCubeShader->setMat4("projection", projection);
		lightCubeShader->setMat4("view", view);
		pointLights[i].draw(*lightCubeShader);
	}

	// start using model shader
	modelShader->use();
	modelShader->setMat4("projection", projection);
	modelShader->setMat4("view", view);

	// model matrix
	glm::mat4 model = glm::mat4(1.0f);


	drawSkulls(projection, view);


	// RENDER AIRPLANE MODEL
	float rollRot = std::clamp(-glm::radians(YAW + airPlane->yawAnimation + 90.0f), glm::radians(-30.0f), glm::radians(30.0f));
	float pitchRot = airPlane->transform.Pitch;

	airPlane->update(deltaTime);
	model = glm::mat4(1.0f);
	
	model = glm::translate(model, airPlane->transform.Position);
	model = glm::translate(model, glm::vec3(0.0f,FP_CAM_Y_OFFSET,0.0f));
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));


	model = glm::rotate(model, -glm::radians(airPlane->transform.Yaw + 90.0f), glm::vec3(.0f, .0f, 1.0f));
	model = glm::rotate(model, glm::radians(pitchRot), glm::vec3(.0f, 1.0f, 0.0f));
	
	model = glm::rotate(model, rollRot, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(AIRPLANE_SACLE, AIRPLANE_SACLE, AIRPLANE_SACLE));
	modelShader->setMat4("model", model);
	airPlane->model->Draw(*modelShader);

	// debug
	//airPlane->transform.DrawLines(projection, view, model);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
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
		drawScene(window);

		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}



	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}


// everything that needs to be done to intialize openGL library
GLFWwindow* initOpenGL()
{
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

	return window;
}

// keyboard input callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		airPlane->reset(startingPoint);
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


	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
		airPlane->camera->flipY = !airPlane->camera->flipY;

}

// process keyboard input - useful when something needs to be called when key is PRESSED - in the future this will be integrated with Input class
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
}

// mouse movement input callback
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

// mouse scroll input callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cam->ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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