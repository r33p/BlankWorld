// BlankWorld.cpp point.cpp point.hpp Spectrum.GLShader.hpp

#include <fstream> // printf
#include <unistd.h> // usleep

// GLEW STATIC LINK
#define GLEW_STATIC
// opengl
#include <GL/glew.h> // wrapper fonctions opengl
#include <GLFW/glfw3.h> // fenetrage
//matrices
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "point.hpp"

void structure(){	
	point a;
	a.initialize(4,3);
	a.affiche();
	a.update(1,5);
	a.affiche();
}


int main(void){
	structure();
	
	// ------------------------------------------------------- INIT GLFW
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "YUYUYU", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

	
	// ------------------------------------------------------- INIT GLEW
	glewExperimental = GL_TRUE;
	glewInit();
	
	
	// ------------------------------------------------- INIT VBO OPENGL
	setup_TEXTURES();
	setup_VBO();
	setup_GLSL();
	setup_MATRICES();
    
    // ----------------------------------------------------- RENDER LOOP
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    //for(int i=0; i<1; i++)
    {		
		
		// --------------------------------------------------------- FPS
		usleep(500000);
		
		
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 
        
        // ------------------------------------------------------ OPENGL
        updateVBO(); // point.cpp
        
        printf("SWAP BUFFER \n");
        
        // ------------------------------------------------- SWAP BUFFER
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

