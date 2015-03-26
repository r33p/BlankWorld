// GLEW STATIC LINK
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
//RtAudio
#include <RtAudio.h>
#include <RtError.h>
// calcul du temps
#include <time.h>
#include <sys/time.h>

// Shaders Sources
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
	"uniform float mydata[1024];"
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

// GLOBALES 3D
GLFWwindow* window; 										// GLFW
GLint 		modelTrans,	uniTime,	uniView,	uniProj;	// OGL
glm::mat4 	trans,		model,		view, 		proj;		// MATRICES
GLuint 		vbo, 		ebo, 		vao;					// VBO
GLuint 		vertexShader,	fragmentShader,	shaderProgram;	// SHADERS

// GLOBALES VBO -------------------------------------------------------<
const int samples=1024;
float vertices[samples*7];

// GLOBALES RTAUDIO
int channels = 2;
int sampleRate = 44100;
int bufferSize = 512;  // 256 sample frames
int nBuffers = 4;      // number of internal buffers used by device
int device = 0;        // 0 indicates default or first available device
double data[2];
char input;
RtAudio *audio = 0;

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
	
void setupVBO(){
	// ------------------------------------------------------------- VBO
	// création du VBO
	glGenBuffers(1, &vbo); // Generate 1 buffer
	// -------------------------------------------------------- VERTICES
 	for (int i=0;i<1024;i++){
		vertices[i*7] = ((float)i/1024) - 0.5; 
		vertices[i*7+1] = 0.0;
		vertices[i*7+2] = 1.0;
		vertices[i*7+3] = 1.0;
		vertices[i*7+4] = 1.0;
		vertices[i*7+5] = 1.0;
		vertices[i*7+6] = 1.0;
	}
		vertices[0*7+1]=0.3;
		vertices[1023*7+1]=0.3;
	for (int i=0;i<10;i++){
		printf("\n X:%f Y:%f R:%f G:%f B:%f U:%f V:%f", vertices[i*7],vertices[i*7+1],vertices[i*7+2],vertices[i*7+3],vertices[i*7+4],vertices[i*7+5],vertices[i*7+6]);
	}
	// rendre le buffer nouvellement créé, le buffer actif
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// uploader le VBO dans la RAM de la CG
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// ---------------------------------------- VERTEX ATTRIBUTE OBJECTS
	//                    stocke les liens entre un VBO et ses attributs
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
// ----------------------------------------------------------------- EBO
	// ça sert à gérer des index de points (comme dans les fichiers obj)
	glGenBuffers(1, &ebo);
	GLuint elements[2048];
	// remplissage					
	for(int i=0;i<2048;i++){elements[i]=i;}
	// rattachement
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(elements), elements, GL_STATIC_DRAW);
	
	}

void setupSHADERS(){
// ------------------------------------------------------------- SHADERS
	// Create and compile the vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    GLint statusVERT;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &statusVERT);
    printf("\n VERT : %u \n", statusVERT); // retour d'erreurs des shaders
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
}

void setupScene(){
	
	


    
// -------------------------------------------------------------- IMAGES 
    GLuint textures[2];
	glGenTextures(2, textures);
    
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

void setupAUDIO(){
	RtAudio *audio = 0;
	// Default RtAudio constructor
	try {
	audio = new RtAudio();
	}
	catch (RtError &error) {
	// Handle the exception here
	error.printMessage();
	}
	
	// Clean up
	delete audio;
  
  
	//try { audio = new RtAudio(device, channels, 0, 0, RTAUDIO_FLOAT64, sampleRate, &bufferSize, nBuffers);}
	//catch (RtError &error) { 
		//error.printMessage();
		//exit(EXIT_FAILURE);
	//}
}

void setupGlobal(){
	setupAUDIO();
	setupGLFW();
	setupVBO();
	setupScene();
	}

void updateVBO(){
	
	// ICI L'APPEL AU CALLBACK
	
	// -----------------------
	
	for (int i=0;i<1024;i++){
		vertices[i*7+1]= (float)(rand() % 100)/200;
//		vertices[i*7+1]= ((sin((float)i/64))/3)+((float)(rand() % 100)/1000);
		}
		
		
	}

void nettoyage(){
	glDeleteProgram(shaderProgram); // OpenGL
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
	glfwTerminate();     // GLFW	
	
	printf("\n Nettoyage fait, bye bye \n");
}

int main(){
	setupGlobal();
	uniTime = glGetUniformLocation(shaderProgram, "time");                  
	
// ----------------------------------------------------------- MAIN LOOP
	while(!glfwWindowShouldClose(window))
	
	
	
	{
		
		// mesure du temps d'execution
		struct timeval start, draw, swap, end;
		gettimeofday(&start, NULL);
		
		
		
		// update du VBO et re-upload des data
		updateVBO();
		// UPLOAD des nouvelles donées ! IMPORTANT
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		// clear screen au noir
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 
        
        glUniform1f(uniTime, (GLfloat)clock() / (GLfloat)CLOCKS_PER_SEC);
     
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        trans = glm::rotate(trans, glm::degrees(0.0001f), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelTrans, 1, GL_FALSE, glm::value_ptr(trans));
       
        gettimeofday(&draw, NULL);
        
        // dessine le triangle avec les 3 vertices
        glDrawElements(GL_LINES, 1024, GL_UNSIGNED_INT, 0);
       
        gettimeofday(&swap, NULL);
        
        glfwSwapBuffers(window);
		glfwPollEvents();
		
		gettimeofday(&end, NULL);

		//printf("\n %ld \n\n", ((end.tv_sec * 1000000 + end.tv_usec)
		//	  - (start.tv_sec * 1000000 + start.tv_usec)));
	
		//printf("  start.tv_sec    : %f\n",(double)start.tv_sec);
		//printf("  start.tv_usec   : %f\n",(double)start.tv_usec);
		printf("  init  = %10.0f\n",(double)draw.tv_usec-start.tv_usec);
		printf("  draw  + %10.0f\n",(double)swap.tv_usec-draw.tv_usec);
		printf("  swap  + %10.0f\n",(double)end.tv_usec-swap.tv_usec);
		printf("        ------------\n");
		printf("  total = %10.0f microseconds     > %10.0f FPS\n",(double)end.tv_usec-start.tv_usec,1000/(((double)end.tv_usec-start.tv_usec)/1000));
		printf("\n");
		
		
		
		}
	
	// ------------------------------------------------------- NETTOYAGE
	nettoyage();
}




