// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

//#pragma comment(lib,"legacy_stdio_definitions.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32.lib")

#define GLFW_DLL

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw/glfw3.h>
GLFWwindow* window;

// Include GLM
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "controls.hpp"

#include <iostream>
#include "OpenGLDriver.h"
#include "SceneNode.h"
#include "FFXIMesh.h"
#include "FPS_Counter.h"
#include "SceneManager.h"

int main( int argc, char** argv )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 768, 512, "Tutorial 08 - Basic Shading", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 600,100);
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 768/2, 512/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	GLenum err = glGetError();
	err = glGetError();

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
//	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardTransparentShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
//	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
//	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
//	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	//create driver and sceneManager
	COpenGLDriver *pDriver = new COpenGLDriver;
	pDriver->setProgramID(programID);
	pDriver->createMatrixHandler();

	CSceneManager *pSceneMgr = new CSceneManager;
	pSceneMgr->addDriver(pDriver);
	//onLoad mesh, it generate all the animation vertices/normals
	CFFXIMesh *pFFXImesh = (CFFXIMesh*)pSceneMgr->loadMesh(argc >= 2 ? argv[1] : "3044");
	if(!pFFXImesh)
		return 2;

	CSceneNode *pnode = (CSceneNode*)pSceneMgr->createSceneNode(pFFXImesh);

	int frameStart, frameEnd;
	float speed;
	pFFXImesh->setCurrentAnimation(0, frameStart, frameEnd, speed);

	pnode->setPosition(glm::vec3(0,0,0));
	pnode->setCurrentFrame(0);
	pnode->setFrameLoop(frameStart, frameEnd);
	pnode->setAnimationSpeed(speed);

	CSceneNode *pTargetNode=pnode;

	pDriver->assignGLBufferID();

	GLuint shaderTextureID  = glGetUniformLocation(programID, "myTextureSampler");
	pDriver->setShaderTextureID(shaderTextureID);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

//	glFrontFace(GL_CW);
	// Enable blending
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_ONE, GL_ZERO);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	char title[100];
	int lastFPS=0, count=0;
	CFPS_Counter fpsCounter;
	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		if (glfwGetKey( window, GLFW_KEY_N ) == GLFW_PRESS){
			if(glfwGetKey(window,GLFW_KEY_N) == GLFW_RELEASE) {
				pFFXImesh->nextAnimation(speed, frameStart, frameEnd);
				pTargetNode->setFrameLoop(frameStart, frameEnd);
				pTargetNode->setAnimationSpeed(speed);
				std::cout << "frameStart: " << frameStart << "  frameEnd: " << frameEnd << "  Speed: " << speed << std::endl;
			}
		}
		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		glm::vec3 lightPos = glm::vec3(4,4,4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		pSceneMgr->drawAll(ProjectionMatrix, ViewMatrix);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		fpsCounter.registerFrame(pSceneMgr->getTime());
		count=fpsCounter.getFPS();
		if(lastFPS!=count) {
			sprintf_s(title,100,"FPS: %d", count);
			glfwSetWindowTitle(window, title);
			lastFPS=count;
		}

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	pDriver->cleanUp();
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

