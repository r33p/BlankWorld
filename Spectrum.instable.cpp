/*
 * APITRACE
 * LD_PRELOAD=~/Apps/apitrace/apitrace/wrappers/glxtrace.so ./Spectrum.instable
 * ~/Apps/apitrace/apitrace/build/apitrace replay ./Spectrum.instable.trace  
 * 
 * BUILD ALSA
 * g++ -Wall -c "%f" -std=c++11 -D__LINUX_ALSA__ -lglfw -lGLEW -lGL -lSOIL -lrtmidi -lrtaudio -lpthread -lasound -lm
 * g++ -Wall -o "%e" "%f" -std=c++11 -D__LINUX_ALSA__  -lglfw -lGLEW -lGL -lSOIL -lrtmidi -lrtaudio -lpthread -lasound -lm
 * 
 * BUILD JACK
 * g++ -Wall -D__UNIX_JACK__ -c  "%f" $(pkg-config –cflags –libs jack) -lpthread -std=c++11 -lglfw -lGLEW -lGL -lSOIL -lrtmidi -lrtaudio -lpthread -lasound -lm
 * g++ -Wall -D__UNIX_JACK__ -o  "%e" "%f" GLShader.cpp $(pkg-config –cflags –libs jack) -lpthread -std=c++11 -lglfw -lGLEW -lGL -lSOIL -lrtmidi -lrtaudio -lpthread -lasound -lm
 * 
 */

#include "GLShader.hpp"
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
//RtAudio
#include <RtAudio.h>
//std
#include <fstream> //ifstream pour le loader glsl
#include <math.h> // matrices
// calcul du temps
#include <sys/time.h>

// GLOBALES 3D
GLFWwindow* window; 										// GLFW
GLint 		uniModel,	uniView,	uniProj,	uniTime;	// OGL
GLuint textures[2];											// TEXTURES
glm::mat4 	trans,		model,		view, 		proj;		// MATRICES
GLuint 		vbo, 		ebo, 		vao;					// VBO
GLuint 		vertexShader,	fragmentShader,	programGLSL;	// SHADERS
// GLOBALES VBO (DATA)
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
RtAudio adc;
// STOCKE une copie du BUFFER AUDIO avec memcpy depuis le callback audio
signed short buuff[1024];
// TIMESTAMPS (DEBUG)
struct timeval start, draw, swap, end;

int SoundInput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData ){
	signed short *buffer = (signed short *) inputBuffer;
	if ( status ){
		std::cout << "Stream overflow detected!" << std::endl;
	}
	// Copie brute des données du buffer (uint short)
	memcpy(&buuff,buffer,nBufferFrames * 2 * sizeof(signed short));
	return 0;
}

void setup_GLFW(){
	// ------------------------------------------------------- INIT GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// Windowed
	window = glfwCreateWindow(1024, 512, "Blank World", nullptr, nullptr); 
	glfwMakeContextCurrent(window);
	// ------------------------------------------------------- INIT GLEW
	glewExperimental = GL_TRUE;
	glewInit();
}

void setup_VBO(){
	// ------------------------------------------------------------- VBO
	// création du VBO
	glGenBuffers(1, &vbo); // Generate 1 buffer
	// -------------------------------------------------------- VERTICES
 	for (int i=0;i<1024;i++){
		vertices[i*7] = ((float)i/512) - 1.0; 	// X	
		vertices[i*7+1] = 0.0;					// Y
		// -----------------------------------------
		vertices[i*7+2] = 1.0;					// R
		vertices[i*7+3] = 1.0;					// G
		vertices[i*7+4] = 1.0;					// B
		// -----------------------------------------
		vertices[i*7+5] = 1.0;					// U
		vertices[i*7+6] = 1.0;					// V
	}
		//vertices[0*7+1]=0.3;
		//vertices[1023*7+1]=0.3;
		
	for (int i=0;i<10;i++){
		//printf("\n X:%f Y:%f R:%f G:%f B:%f U:%f V:%f", vertices[i*7],vertices[i*7+1],vertices[i*7+2],vertices[i*7+3],vertices[i*7+4],vertices[i*7+5],vertices[i*7+6]);
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // rendre le buffer actif
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // uploader le VBO
	
	// ---------------------------------------- VERTEX ATTRIBUTE OBJECTS
	//                    stocke les liens entre un VBO et ses attributs
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// ------------------------------------------------------------- EBO
	// ça sert à gérer des index de points (comme dans les fichiers obj)
	glGenBuffers(1, &ebo);
	GLuint elements[2048];
	// remplissage					
	for(int i=0;i<2048;i++){elements[i]=i;}
	// rattachement
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(elements), elements, GL_STATIC_DRAW);
	
}

void setup_GLSL(){
	programGLSL = LoadShader("BlankWorld.vert", "BlankWorld.frag");
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
    GLint texAttrib = glGetAttribLocation(programGLSL, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));
}

void setup_MATRICES(){
	// ---------------------------------------------------------- IMAGES 
	glGenTextures(2, textures);
	// -------------------------------------------------------- MATRICES
	uniModel	= glGetUniformLocation(programGLSL, "model");
	uniView		= glGetUniformLocation(programGLSL, "view");
	uniProj		= glGetUniformLocation(programGLSL, "proj");
	// ----------------------------------------------------------- MODEL
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
	// ----------------------------------------------------------- VIEW
    view = glm::lookAt(
    glm::vec3(1.2f, 1.2f, 1.2f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));
	// ------------------------------------------------------------ PROJ
	proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
	}

void setup_AUDIO(){
	if ( adc.getDeviceCount() < 1 ) {
		std::cout << "\nNo audio devices found!\n";	
		exit( 0 );
	}
	RtAudio::StreamParameters parameters;
	parameters.deviceId = adc.getDefaultInputDevice();
	unsigned int sampleRate = 44100;
	unsigned int bufferFrames = 512; // 512 sample frames
	parameters.nChannels = 2;
	parameters.firstChannel = 0;
	try {
		adc.openStream( NULL, &parameters, RTAUDIO_SINT16,
		sampleRate, &bufferFrames, &SoundInput );
		adc.startStream();
	}
	catch ( RtAudioError& e ) {
		e.printMessage();
	}
}

void setup(){
	setup_AUDIO();
	setup_GLFW();
	setup_VBO();
	setup_GLSL();
	setup_MATRICES();
	}

void updateVBO(){
	for (int i=0;i<1024;i++){
		//~ vertices[i*7+1] = (float)buuff[i] / 4096;
		vertices[i*7+1] = (float)buuff[i] / 50000;
	}
}

void timing(){
	printf("  init  = %10.0f\n",(double)draw.tv_usec-start.tv_usec);
	printf("  draw  + %10.0f\n",(double)swap.tv_usec-draw.tv_usec);
	printf("  swap  + %10.0f\n",(double)end.tv_usec-swap.tv_usec);
	printf("        ------------\n");
	printf("  total = %10.0f microseconds     > %10.0f FPS\n",(double)end.tv_usec-start.tv_usec,1000/(((double)end.tv_usec-start.tv_usec)/1000));
	printf("\n");
}

void nettoyage(){
	glDeleteProgram(programGLSL); // OpenGL
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
	glfwTerminate();     // GLFW	
	
	printf("\n Nettoyage fait, bye bye \n");
}

void rendu(){

	uniTime = glGetUniformLocation(programGLSL, "time");    
	
	glEnable(GL_PROGRAM_POINT_SIZE);
	
// ----------------------------------------------------------- MAIN LOOP
	while(!glfwWindowShouldClose(window))
	{	
		glPointSize(10.0);
		// TIMESTAMP : "START"
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
        //trans = glm::rotate(trans, glm::degrees(0.0001f), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(trans));
        gettimeofday(&draw, NULL);
        // dessine le triangle avec les 3 vertices
        glDrawElements(GL_POINTS, 1024, GL_UNSIGNED_INT, 0);
        gettimeofday(&swap, NULL);
        glfwSwapBuffers(window);
		glfwPollEvents();
		gettimeofday(&end, NULL);
//		timing();
		}
}

int main(){
	setup();
	rendu();	
	// ------------------------------------------------------- NETTOYAGE
	nettoyage();
	// CLEAN RTAUDIO
	
	try {
		// Stop the stream
		adc.stopStream();
	}
	catch (RtAudioError& e) {
		e.printMessage();
	}
	if ( adc.isStreamOpen() ) adc.closeStream();
	return 0;
}



