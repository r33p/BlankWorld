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

//RtAudio
#include <RtAudio.h>

GLFWwindow* window;

// GLOBALES RTAUDIO
int channels = 2;
int sampleRate = 44100;
int nBuffers = 4;      // number of internal buffers used by device
int device = 0;        // 0 indicates default or first available device
double data[2];
char input;
RtAudio *audio = 0;
RtAudio adc;
// STOCKE une copie du BUFFER AUDIO avec memcpy depuis le callback audio
signed short buuff[1024];


// --------------------------------------------------------------- AUDIO
int SoundInput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData ){
	signed short *buffer = (signed short *) inputBuffer;
	if ( status ){
		std::cout << "Stream overflow detected!" << std::endl;
	}
	// Copie brute des données du buffer (uint short)
	memcpy(&buuff,buffer,nBufferFrames * 2 * sizeof(signed short));
	return 0;
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


// ---------------------------------------------------------------- GLFW    
int setup_GLFW(){
    /* Initialize the library */
    if (!glfwInit())	return -1;
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1024, 256, "YUYUYU", NULL, NULL);
    if (!window){	glfwTerminate(); return -1;}
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	// ------------------------------------------------------------ GLEW
	glewExperimental = GL_TRUE;
	glewInit();
	return 0;
	}

void rendu(cube* c){
	
	// for MOUSE POS
	double xpos, ypos;
	
    // ----------------------------------------------------- RENDER LOOP
    while (!glfwWindowShouldClose(window))
    //for(int i=0; i<1; i++)
    {		
		glfwGetCursorPos(window, &xpos, &ypos);
		printf("MOUSE POS : X:%f Y:%f\n", xpos, ypos);
		
		//usleep(1000000);	// -------------- FPS CONTROL (µ_seconds)
		// ------------------------------------------------------- CLEAR
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);         
        // -------------------------------------------------- UPDATE VBO
        //spectrum_updateVBO(); // Spectrum.points.cpp

        //signed short buuff[1024];
        c->update(buuff);
        
        // ------------------------------------------------- SWAP BUFFER
        glfwSwapBuffers(window);
        // ------------------------------------------------- POLL EVENTS
        glfwPollEvents();
		
    }
}

void nettoyage(cube* c) {
	// OGL
	c->nettoyage();
	
  	// GLFW
  	glfwTerminate();	
	printf("\n Nettoyage fait, bye bye \n\n");
}

int main(void){
	//SETUP
	cube kub;
	setup_GLFW();
	setup_AUDIO();
	
	//RENDER
	kub.setup();
	rendu(&kub);
	
	// CLEAN
	nettoyage(&kub);
	
    return 0;
}

