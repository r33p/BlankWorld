// LINKAGE STATIQUE DE GLEW
#define GLEW_STATIC
// opengl
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
// std
#include <iostream>
#include <cmath>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <cstdio>
//matrices
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shaders
const GLchar* vertexSource =
    "#version 150 core\n"
    "in vec2 texcoord;"
    "in vec2 position;"
    "in vec3 color;"
    "out vec3 Color;"
    "out vec2 Texcoord;"
    "uniform mat4 model;"
	"uniform mat4 view;"
	"uniform mat4 proj;"
    "void main() {"
    "   Color = color;"
    "   Texcoord = texcoord;"
    "   gl_Position = model * vec4(position, 0.0, 1.0);"
    "}";
const GLchar* fragmentSource =
    "#version 150 core\n"
    "in vec3 Color;"
    "in vec2 Texcoord;"
    "out vec4 outColor;"
    "uniform float time;"
    "void main() {"
    "   outColor = outColor = vec4(Color, 1.0);"
    "}";


// GLOBALE GLFW
GLFWwindow* window;

// OGL
GLint modelTrans;
GLint uniTime;
GLint uniView;
GLint uniProj;
glm::mat4 trans;               
glm::mat4 model;                
glm::mat4 view;                
glm::mat4 proj;
GLuint vbo;
GLuint ebo;
GLuint vao;
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;


void setupGLFW(){
	// ------------------------------------------------------- INIT GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// Windowed
	window = glfwCreateWindow(800, 600, "Blank World", nullptr, nullptr); 
	glfwMakeContextCurrent(window);
	// ------------------------------------------------------- INIT GLEW
	glewExperimental = GL_TRUE;
	glewInit();
}
	
void setupScene(){
	// ---------------------------------------- VERTEX ATTRIBUTE OBJECTS
	//                    stocke les liens entre un VBO et ses attributs
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// ------------------------------------------------------------- VBO
	// création du VBO
	
	glGenBuffers(1, &vbo); // Generate 1 buffer
	
	
	
	// -------------------------------------------------------- VERTICES
 	float vertices[1024*7];
 	//// X
 	for (int i=0;i<1024;i++){
		vertices[i*7] = ((float)i/1024) - 0.5; 
		vertices[i*7+1] = 0.0;
		vertices[i*7+2] = 1.0;
		vertices[i*7+3] = 1.0;
		vertices[i*7+4] = 1.0;
		vertices[i*7+5] = 1.0;
		vertices[i*7+6] = 1.0;
	}
		
		vertices[512*7+1]=0.3;
		
		
	for (int i=0;i<10;i++){
		printf("\n X:%f Y:%f R:%f G:%f B:%f U:%f V:%f", vertices[i*7],vertices[i*7+1],vertices[i*7+2],vertices[i*7+3],vertices[i*7+4],vertices[i*7+5],vertices[i*7+6]);}
 	
 	//vertices[] = {
    //-0.7f,  0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 
    //-0.4f,  0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 
     //0.0f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 
     //0.2f,  0.5f,   0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     //0.5f,  0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     //0.7f,  0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 1.0f};




	// rendre le buffer nouvellement créé, le buffer actif
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// uploader le VBO dans la RAM de la CG
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// ----------------------------------------------------------------- EBO
	// ça sert à gérer des index de points (comme dans les fichiers obj)
	// donc à faire des faces plus facilement
	
	glGenBuffers(1, &ebo);
	GLuint elements[2048];
	//GLuint elements[] = {	0, 1, 2,
							//1, 2, 3,
							//2, 3, 4,
							//3, 4, 5,
							//4, 5, 6,
							//5, 6, 7,
							//6, 7, 8,
							//7, 8, 9,
							//8, 9, 10,
							//9, 10, 11
							//};
							
	for(int i=0;i<2048;i++){elements[i]=i;}
	
	
//	for (int i=0;i<1024;i++) { elements[i*7] = (i/1024) - 0.5; }
							
							
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(elements), elements, GL_STATIC_DRAW);
	
// ------------------------------------------------------------- SHADERS
	// Create and compile the vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    // retour d'erreurs des shaders
    GLint statusVERT;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &statusVERT);
    printf("\n VERT : %u \n", statusVERT);
    //char buffer[512];
	//glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
	//printf("\n VERTbuf : %s", buffer);
    // Create and compile the fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    GLint statusFRAG;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &statusFRAG);
    printf("\n FRAG : %u \n", statusFRAG);
    // Link the vertex and fragment shader into a shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
	// -------------------------------------------------- VERTEX ATTRIBS	
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	// comment sont formatées nos données de vertex :
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
	// ------------------------------------------------ FRAGMENT ATTRIBS	
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    // ------------------------------------------------- TEXTURE ATTRIBS
    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));

    
// -------------------------------------------------------------- IMAGES 
    GLuint textures[2];
	glGenTextures(2, textures);
	//int width, height;
	//unsigned char* image;
	
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textures[0]);
	//image = SOIL_load_image("medias/hacheur1.png", &width, &height, 0, SOIL_LOAD_RGB);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //printf("\n dimensions image : %i x %i \n\n", width, height);
	//SOIL_free_image_data(image);
	//glUniform1i(glGetUniformLocation(shaderProgram, "tex1"), 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, textures[1]);
	//image = SOIL_load_image("medias/hacheur2.png", &width, &height, 0, SOIL_LOAD_RGB);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//printf("\n dimensions image : %i x %i \n\n", width, height);
	//SOIL_free_image_data(image);
	//glUniform1i(glGetUniformLocation(shaderProgram, "tex2"), 1);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    
    
// ------------------------------------------------------------ MATRICES
	modelTrans = glGetUniformLocation(shaderProgram, "model");
	uniProj = glGetUniformLocation(shaderProgram, "proj");
	uniView = glGetUniformLocation(shaderProgram, "view");
// ------------------------------------------------------------ MODEL
	glUniformMatrix4fv(modelTrans, 1, GL_FALSE, glm::value_ptr(model));
// ------------------------------------------------------------ VIEW
    view = glm::lookAt(
    glm::vec3(1.2f, 1.2f, 1.2f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));
// ------------------------------------------------------------ PROJ
	proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
	}




void setupGlobal(){
	setupGLFW();
	setupScene();
	}

int main()
{
	setupGlobal();
	uniTime = glGetUniformLocation(shaderProgram, "time");                  
	
// ----------------------------------------------------------- MAIN LOOP
	while(!glfwWindowShouldClose(window))
	{
		// clear screen au noir
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 
        
        glUniform1f(uniTime, (GLfloat)clock() / (GLfloat)CLOCKS_PER_SEC);
     
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        trans = glm::rotate(trans, glm::degrees(0.0001f), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelTrans, 1, GL_FALSE, glm::value_ptr(trans));
       
        // dessine le triangle avec les 3 vertices
        glDrawElements(GL_POINTS, 1024, GL_UNSIGNED_INT, 0);
       
        glfwSwapBuffers(window);
		glfwPollEvents();
		}
	
	// ------------------------------------------------------- NETTOYAGE
	glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    // cleanage glfw	
	glfwTerminate();
	
    // Clean up midi
	//cleanup:


}




