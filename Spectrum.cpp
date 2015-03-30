////////////////////////////////////////////////////////////////////////
// BUILD ALSA
// g++ -Wall -c "%f" -std=c++11 -D__LINUX_ALSA__ -lglfw -lGLEW -lGL -lSOIL -lrtmidi -lrtaudio -lpthread -lasound -lm
// g++ -Wall -o "%e" "%f" -std=c++11 -D__LINUX_ALSA__  -lglfw -lGLEW -lGL -lSOIL -lrtmidi -lrtaudio -lpthread -lasound -lm
// BUILD JACK
// g++ -Wall -D__UNIX_JACK__ -c  "%f" $(pkg-config –cflags –libs jack) -lpthread -std=c++11 -lglfw -lGLEW -lGL -lSOIL -lrtmidi -lrtaudio -lpthread -lasound -lm
// g++ -Wall -D__UNIX_JACK__ -o  "%e" "%f" $(pkg-config –cflags –libs jack) -lpthread -std=c++11 -lglfw -lGLEW -lGL -lSOIL -lrtmidi -lrtaudio -lpthread -lasound -lm
////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////
#include "GLShader.hpp"
////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////

// Shaders Sources
//std::string vertex2;
const GLchar* vertex2;
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
GLuint 		vertexShader,	fragmentShader,	program;	// SHADERS
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

int record( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData ){
	signed short *buffer = (signed short *) inputBuffer;
	if ( status ){
		std::cout << "Stream overflow detected!" << std::endl;
	}
	// Copie brute des données du buffer (uint short)
	memcpy(&buuff,buffer,nBufferFrames * 2 * sizeof(signed short));
	return 0;
}

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
		vertices[i*7] = ((float)i/512) - 1.0; 
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


//bool loadShaderSource(const std::string& fileName, std::string& vertexsourcecontent){
bool loadShaderSource(const std::string& fileName, const GLchar* vertexsourcecontent){
	std::ifstream fichier;
	fichier.open(fileName.c_str());
	
	if (! fichier) {return false;}
	return true;
	
	std::stringstream stream;
	stream <<  fichier.rdbuf();
	fichier.close();
//	vertexsourcecontent = stream.str();
	return true;
	}

void setupGLSL(){
	
	
	GLuint program = LoadShader("BlankWorld.vert", "BlankWorld.frag");
	glUseProgram(program);
	
	/*
	loadShaderSource("simple.vert", vertex2);
	*/
	
	// LOAD FROM FILES :
	// http://www.gamedev.net/topic/568188-loading-glsl-shader-from-file-c/
	// http://www.nexcius.net/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c/
	// http://lazyfoo.net/tutorials/OpenGL/index.php#Loading%20Text%20File%20Shaders
	// --------------------------------------------------------- SHADERS
	// Create and compile the vertex shader
    
    
    /*
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
    */
	// -------------------------------------------------- VERTEX ATTRIBS	
	GLint posAttrib = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(posAttrib);
	// comment sont formatées nos données de vertex :
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
	// ------------------------------------------------ FRAGMENT ATTRIBS	
	GLint colAttrib = glGetAttribLocation(program, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    // ------------------------------------------------- TEXTURE ATTRIBS
    GLint texAttrib = glGetAttribLocation(program, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));
}

void setupScene(){
	// ---------------------------------------------------------- IMAGES 
    GLuint textures[2];
	glGenTextures(2, textures);
    
	// -------------------------------------------------------- MATRICES
	modelTrans = glGetUniformLocation(program, "model");
	uniProj = glGetUniformLocation(program, "proj");
	uniView = glGetUniformLocation(program, "view");
	// ----------------------------------------------------------- MODEL
	glUniformMatrix4fv(modelTrans, 1, GL_FALSE, glm::value_ptr(model));
	// ----------------------------------------------------------- VIEW
    view = glm::lookAt(
    glm::vec3(1.2f, 1.2f, 1.2f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));
	// ------------------------------------------------------------ PROJ
	proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
	}

void setupAUDIO(){
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
		sampleRate, &bufferFrames, &record );
		adc.startStream();
	}
	catch ( RtAudioError& e ) {
		e.printMessage();
	}
}

void setup(){
	setupAUDIO();
	setupGLFW();
	setupVBO();
	setupGLSL();
	setupScene();
	}

void updateVBO(){
	for (int i=0;i<1024;i++){
		
		//~ vertices[i*7+1] = (float)buuff[i] / 4096;
		vertices[i*7+1] = (float)buuff[i] / 4096;
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
	glDeleteProgram(program); // OpenGL
//    glDeleteShader(fragmentShader);
//    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
	glfwTerminate();     // GLFW	
	
	printf("\n Nettoyage fait, bye bye \n");
}

void rendu(){

	uniTime = glGetUniformLocation(program, "time");                  
	
// ----------------------------------------------------------- MAIN LOOP
	while(!glfwWindowShouldClose(window))
	{	
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
        glUniformMatrix4fv(modelTrans, 1, GL_FALSE, glm::value_ptr(trans));
        gettimeofday(&draw, NULL);
        // dessine le triangle avec les 3 vertices
        glDrawElements(GL_LINE_LOOP, 1024, GL_UNSIGNED_INT, 0);
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



