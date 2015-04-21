// BlankWorld.cpp point.cpp point.hpp Spectrum.GLShader.hpp	[Spectrum.vert, etc]
// g++ -Wall -c ./BlankWorld.objets.cpp -lrtaudio -std=c++11 -g
// g++ -Wall -o ./BlankWorld.objets ./BlankWorld.objets.cpp point.cpp Spectrum.GLShader.cpp -lrtaudio -lglfw -lGLEW -lGL -std=c++11 -g

#include <fstream> // printf
#include <unistd.h> // usleep

// glew statique
#define GLEW_STATIC
// opengl
#include <GL/glew.h> // wrapper fonctions opengl
#include <GLFW/glfw3.h> // fenetrage
// matrices
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// modele 3d
//#include "Spectrum.points.hpp"
#include "Cube.class.hpp"

GLFWwindow* window;

int setup_GLFW(){
	// ------------------------------------------------------- INIT GLFW    
    /* Initialize the library */
    if (!glfwInit())	return -1;
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "YUYUYU", NULL, NULL);
    if (!window){	glfwTerminate(); return -1;}
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	// ------------------------------------------------------- INIT GLEW
	glewExperimental = GL_TRUE;
	glewInit();
	return 0;
	}

int main(void){
	//structure();	
	setup_GLFW();
	// ----------------------------------------------- INIT VBO & OPENGL
	/*
	setup_TEXTURES();
	setup_VBO();
	setup_GLSL();
	setup_MATRICES();
	*/
	// --------------------------------------------------- CUBE INSTANCE
	
	cube cube1;
	cube1.setup_TEXTURES();
	cube1.setup_VBO();
	cube1.setup_GLSL();
	cube1.setup_MATRICES();
	
	
    // ----------------------------------------------------- RENDER LOOP
    while (!glfwWindowShouldClose(window))
    //for(int i=0; i<1; i++)
    {		
		// ---------------------------------- FPS CONTROL (microseconds)
		//usleep(1000000);
		
		// ------------------------------------------------------- CLEAR
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 
        
        // -------------------------------------------------- UPDATE VBO
        //spectrum_updateVBO(); // Spectrum.points.cpp
        cube1.update_VBO();
    
        // ------------------------------------------------- SWAP BUFFER
        glfwSwapBuffers(window);
        // ------------------------------------------------- POLL EVENTS
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
    
	//~ printf("\n");
	//~ printf("   char : %ld octets\n", sizeof(char));
	//~ printf("    int : %ld octets\n", sizeof(int));
	//~ printf("  float : %ld octets\n", sizeof(float));
	//~ printf("   long : %ld octets\n", sizeof(long));
	//~ printf(" double : %ld octets\n", sizeof(double)); 
	//~ printf("\n");
	
	
	
}

