// BlankWorld.cpp point.cpp point.hpp Spectrum.GLShader.hpp

#include <fstream> // printf

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
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
    {
		
        // ------------------------------------------------- DRAW OPENGL
        updateVBO();
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        printf("RENDER\n");
        
        
        // -------------------------------------------- GLFW SWAP BUFFER
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

