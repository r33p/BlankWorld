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
#include <cstdlib>		// rand()
#include "GLShader.hpp"	// glsl file loader

#include "Cube.class.hpp"

cube::cube(){};
cube::~cube(){}

void cube::setup(){
	// --------------------------------------------------------- TEXTURE 
	glGenTextures(2, textures);
	
	// ------------------------------------------------------------- VBO
	// création du VBO
	glGenBuffers(1, &vbo); // Generate 1 buffer
	// VERTICES
 	// reset 0
 	for (int i=0;i<points*7;i++){cube::vertices[i] = (float)0.0;}
 	
 	// X
 	float offset = 1;
 	for (int i=0;i<points;i++)	
 	{ 
		float truc = (float)i;
		cube::vertices[i*7] = (truc / 512.0) - offset ;
	}
	
	// COLOR
 	for (int i=0;i<points;i++)	
 	{
		cube::vertices[i*7+2] = (float)1.0;
 	}
 	
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // rendre le buffer actif
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // uploader le VBO
	
	// ------------------------------------------------------------- VAO
	// stocke les liens entre un VBO et ses attributs
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	// ------------------------------------------------------------- EBO
	// points indexes (.obj)
	glGenBuffers(1, &ebo);
	GLuint elements[points];
	// rattachement
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(elements), elements, GL_STATIC_DRAW);
	
	
	// ------------------------------------------------------------ GLSL
	programGLSL = LoadShader("Cube.vert", "Cube.geom", "Cube.frag");
	printf(" PROGRAM GLSL USE: %i \n",programGLSL);
	glUseProgram(programGLSL);

	//  VERTEX ATTRIBS	
	GLint posAttrib = glGetAttribLocation(programGLSL, "position");
	glEnableVertexAttribArray(posAttrib);
	// comment sont formatées nos données de vertex :
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7* sizeof(GLfloat), 0);
	// FRAGMENT ATTRIBS	
	GLint colAttrib = glGetAttribLocation(programGLSL, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    // TEXTURE ATTRIBS
    GLint texAttrib = glGetAttribLocation(programGLSL, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));
    
    
    // -------------------------------------------------------- MATRICES
    // MODEL
    uniModel	= glGetUniformLocation(programGLSL, "model");
	uniView		= glGetUniformLocation(programGLSL, "view");
	uniProj		= glGetUniformLocation(programGLSL, "proj");
	// VIEW
    view = glm::lookAt(
    glm::vec3(1.2f, 1.2f, 1.2f), 	// position
    glm::vec3(0.0f, 0.0f, 0.0f),	// focus point
    glm::vec3(0.0f, 1.0f, 0.0f));	// up vector
	// PROJ
	proj = glm::perspective(glm::degrees(22.5f), 1024.0f / 256.0f, 1.0f, 20.0f);
	
	// BIND
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
}

void cube::update(signed short* ptr_buuff){
	for (int i=0;i<points;i++){
		
		//vertices[i*7+1] = randoom();
		//vertices[i*7+1] = (float)(rand() % 100 + 1) / 300;
		
		// affectation au Y des données du buffer audio
		vertices[i*7+1] = (float)ptr_buuff[i] / 50000.0;
		
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
	
	// ------------------------------------------------------------ DRAW
	// FAIRE BIEN ATTENTION ICI
	// DRAW ARRAY OU DRAW ELEMENT : C'EST PAS LA MEME CHOSE...
	//glDrawElements(GL_POINTS, 4096, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_LINES, 0 , points);
}

void cube::nettoyage(){
	glDeleteProgram(programGLSL); // OpenGL
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}
