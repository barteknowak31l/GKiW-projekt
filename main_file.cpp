/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float AIRPLANE_SPEED = 1.0f;
const bool FIRST_PERSON = false;

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






void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path);
void loadTextures();

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


//////////LEARNOPENGL TUTORIAL  SECTION/////////////////////////////////


//CAM
Camera* cam;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
const float cameraSpeed = 10.0f; // adjust accordingly

float pitch = 0.0f, yaw = -90.0f, roll = 0.0f;  //camera rotation angles
float lastX = 400, lastY = 300;	//last cursor positon 
float fov = 45.0f;
const float sensitivity = 0.1f; //mouse sensitivity
bool firstMouseInput = true;

//time
float currentFrame;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


// lighting
glm::vec3 lightPos(0.2f, -1.0f, -0.3f);

Light* light;	//light pointer
DirectionalLight dirLight;
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
Shader* lightingShader;
Shader* lightCubeShader;
Shader* modelShader;


//Models
Model* backpack;
Model* skull;
Model* airplane_model;


//objects
Airplane* airPlane;
glm::vec3 startingPoint = glm::vec3(0.0f, 0.0f, -2.5f);

//textures
unsigned int texture;
unsigned int texture2;
unsigned int diffuseMap, specularMap, emmisionMap;


//temporary input - to do: implement Input class
bool input[4] = { false, false, false, false }; // is pressed: w a s d

void processInput();

void init(GLFWwindow* window)
{
	//temporary light setup - for testing purposes only
	dirLight.light.direction = lightPos;
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


	stbi_set_flip_vertically_on_load(true);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	lightingShader = new Shader("shaders/colorShader.vs", "shaders/colorShader.fs");
	lightCubeShader = new Shader("shaders/lightCubeShader.vs", "shaders/lightCubeShader.fs");
	modelShader = new Shader("shaders/modelShader.vs", "shaders/modelShader.fs");
	
	backpack = new Model("models/backpack/backpack.obj");
	skull = new Model("models/skull/12140_Skull_v3_L2.obj");
	airplane_model = new Model("models/airplane/11804_Airplane_v2_l2.obj");

	loadTextures();

	cam = new Camera(false,cameraPos, cameraUp, yaw, pitch);
	cam->MovementSpeed = cameraSpeed;
	cam->MouseSensitivity = sensitivity;
	cam->Zoom = fov;


	glEnable(GL_DEPTH_TEST);



	//create airPlane object
	airPlane = new Airplane(airplane_model, cam, cam->Position, AIRPLANE_SPEED, FIRST_PERSON);
	airPlane->reset(startingPoint);
	//airPlane = new Airplane(airplane_model, glm::vec3(0.0f, 0.0f, -5.0f), AIRPLANE_SPEED);


}


void loadTextures()
{
	diffuseMap = loadTexture("container2.png");
	specularMap = loadTexture("container2spec.png");
	emmisionMap = loadTexture("matrix.jpg");
}



void drawModel(GLFWwindow* window)
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// don't forget to enable shader before setting uniforms
	modelShader->use();

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = cam->GetViewMatrix();
	modelShader->setMat4("projection", projection);
	modelShader->setMat4("view", view);


	glm::mat4 model = glm::mat4(1.0f);
	light = &dirLight;

	// RENDER SKULL MODEL
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->setLightData(light);
	lightCubeShader->use();
	lightCubeShader->setMat4("projection", projection);
	lightCubeShader->setMat4("view", view);
	skull->setLightData(pointLights,*lightCubeShader,true);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, -glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->setLightData(light);
	lightCubeShader->use();
	lightCubeShader->setMat4("projection", projection);
	lightCubeShader->setMat4("view", view);
	skull->setLightData(pointLights, *lightCubeShader, true);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(5.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.02f, .02f, .02f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->setLightData(light);
	lightCubeShader->use();
	lightCubeShader->setMat4("projection", projection);
	lightCubeShader->setMat4("view", view);
	skull->setLightData(pointLights, *lightCubeShader, true);
	skull->Draw(*modelShader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(.2f, .2f, .2f));	// it's a bit too big for our scene, so scale it down
	modelShader->setMat4("model", model);
	skull->setLightData(light);
	lightCubeShader->use();
	lightCubeShader->setMat4("projection", projection);
	lightCubeShader->setMat4("view", view);
	skull->setLightData(pointLights, *lightCubeShader, true);
	skull->Draw(*modelShader);


	// RENDER AIRPLANE MODEL
	float rollRot = std::clamp(-glm::radians(YAW + airPlane->yawAnimation + 90.0f), glm::radians(-30.0f), glm::radians(30.0f));
	airPlane->update(deltaTime);
	model = glm::mat4(1.0f);
	
	model = glm::translate(model, airPlane->transform.Position);
	
	
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, -glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	
	model = glm::rotate(model, -glm::radians(airPlane->transform.Yaw + 90.0f), glm::vec3(.0f, .0f, 1.0f));

	model = glm::rotate(model, rollRot, glm::vec3(1.0f, 0.0f, 0.0f));

	model = glm::scale(model, glm::vec3(.002f, .002f, .002f));
	modelShader->setMat4("model", model);

	airPlane->model.setLightData(light);
	lightCubeShader->use();
	lightCubeShader->setMat4("projection", projection);
	lightCubeShader->setMat4("view", view);
	airPlane->model.setLightData(pointLights, *lightCubeShader, false);
	airPlane->model.Draw(*modelShader);



	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
}


int main(void)
{
	srand(std::time(NULL));

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

	init(window);
	//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput();
		drawModel(window);

		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}



	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}


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

	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		input[0] = false;
	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		input[1] = false;
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		input[2] = false;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		input[3] = false;




}

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
}

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