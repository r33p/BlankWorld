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

#include <stdio.h>
#include <iostream>
#include <cstdlib> // rand()

#include "Spectrum.GLShader.hpp"
#include "point.hpp"

// VARIABLES GLOBALES
GLuint		vbo, ebo, vao;								// VBO
const int 	points=64;									//
float		vertices[points*7];							//
GLuint		textures[2];								// TEXTURES
GLuint		vertexShader, fragmentShader, programGLSL;	// SHADERS
GLint 		uniModel,	uniView,	uniProj,	uniTime;// OGL
glm::mat4 	model,		view, 		proj;				// MATRICES


void setup_TEXTURES(){
		// ---------------------------------------------------------- IMAGES 
	glGenTextures(2, textures);
	}

void setup_VBO(){
	// ------------------------------------------------------------- VBO
	// création du VBO
	glGenBuffers(1, &vbo); // Generate 1 buffer
	// -------------------------------------------------------- VERTICES
 	for (int i=0;i<points;i++){
		vertices[i] = (float)1.0;
		printf("VERTICE[%i]=%f\n",i, vertices[i]);
 	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // rendre le buffer actif
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // uploader le VBO
	
	// ---------------------------------------- VERTEX ATTRIBUTE OBJECTS
	// stocke les liens entre un VBO et ses attributs
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	// ------------------------------------------------------------- EBO
	// ça sert à gérer des index de points (comme dans les fichiers obj)
	glGenBuffers(1, &ebo);
	GLuint elements[points];
	// rattachement
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(elements), elements, GL_STATIC_DRAW);
	
}

void setup_GLSL(){
	programGLSL = LoadShader("Spectrum.vert", "Spectrum.geom", "Spectrum.frag");
	printf("PROGRAM GLSL USE: %i \n",programGLSL);
	glUseProgram(programGLSL);

	// -------------------------------------------------- VERTEX ATTRIBS	
	GLint posAttrib = glGetAttribLocation(programGLSL, "position");
	glEnableVertexAttribArray(posAttrib);
	// comment sont formatées nos données de vertex :
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7* sizeof(GLfloat), 0);
	// ------------------------------------------------ FRAGMENT ATTRIBS	
	GLint colAttrib = glGetAttribLocation(programGLSL, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    // ------------------------------------------------- TEXTURE ATTRIBS
    /*
    GLint texAttrib = glGetAttribLocation(programGLSL, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));
    */
}

void setup_MATRICES(){
	uniModel	= glGetUniformLocation(programGLSL, "model");
	uniView		= glGetUniformLocation(programGLSL, "view");
	uniProj		= glGetUniformLocation(programGLSL, "proj");
	// ----------------------------------------------------------- VIEW
    view = glm::lookAt(
    glm::vec3(1.2f, 1.2f, 1.2f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));
	// ------------------------------------------------------------ PROJ
	proj = glm::perspective(glm::degrees(22.5f), 1024.0f / 512.0f, 1.0f, 20.0f);

	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
}


void updateVBO(){
	for (int i=0;i<points;i++){
		vertices[i*7+1] = ((rand() % 100)+ 1) / 100;
		printf("vertices[1] = %f\n",(float)vertices[1]);
	}
}

void point::initialize(int abs, int ord){
	x = abs;
	y = ord;
}
	
void point::update(int addabs, int addord){
	x += addabs;
	y += addord;
}
		
void point::affiche(){
	printf("X:%d, Y:%d \n", x,y);
}

