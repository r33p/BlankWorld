// LINKAGE STATIQUE DE GLEW
#define GLEW_STATIC
// opengl
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// images
#include <SOIL/SOIL.h>
//midi
#include "RtMidi.h"
// PortAUdio
#include "portaudio.h"
#include <math.h>
#define NUM_SECONDS   (4)
#define SAMPLE_RATE   (44100)

typedef struct
{
    float left_phase;
    float right_phase;
}
paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    /* Cast data passed through stream to our structure. */
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;
    unsigned int i;
    (void) inputBuffer; /* Prevent unused variable warning. */

    for( i=0; i<framesPerBuffer; i++ )
    {
        *out++ = data->left_phase;  /* left */
        *out++ = data->right_phase;  /* right */
        /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
        data->left_phase += 0.01f;
        /* When signal reaches top, drop back down. */
        if( data->left_phase >= 1.0f ) data->left_phase -= 2.0f;
        /* higher pitch so we can distinguish left and right. */
        data->right_phase += 0.03f;
        if( data->right_phase >= 1.0f ) data->right_phase -= 2.0f;
    }
    return 0;
}

/*******************************************************************/
static paTestData data;
// fin du tru degueu

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
    "uniform sampler2D tex1;"
    "uniform sampler2D tex2;"
    "void main() {"
    "   float factor = (sin(time * 300.0) + 1.0) / 2.0;"
    "   outColor = mix(texture(tex1, Texcoord), texture(tex2, Texcoord), factor);"
    "}";

bool done;
static void finish(int ignore){ done = true; }

// GLOBALE GLFW
GLFWwindow* window;

//GLOBALES MIDI
int nBytes, i;
double stamp;
RtMidiIn *midiin;
//std::vector<unsigned char> message;
float valc = 0;
float valp = 0;

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



void mycallback( double deltatime, std::vector< unsigned char > *message, void *userData )
{
	unsigned int nBytes = message->size();
	//stamp = midiin->getMessage( &message );	
	for ( unsigned int i=0; i<nBytes; i++ )
	std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
	if ( nBytes > 0 )
	std::cout << "stamp = " << deltatime << std::endl;
	if ( nBytes > 0 ) valc = ((64-(float)message->at(2))/10000);
	}

void setupMidi(){
	midiin = new RtMidiIn();
	midiin->setCallback(&mycallback);
	
	// Check available ports.
	unsigned int nPorts = midiin->getPortCount();
	if ( nPorts == 0 ) {
		std::cout << "No ports available!\n";
		delete midiin;
	}
	// index du port que l'on ouvre
	midiin->openPort( 1 );
	// Don't ignore sysex, timing, or active sensing messages.
	midiin->ignoreTypes( false, false, false );
	// Install an interrupt handler function.
	done = false;
	(void) signal(SIGINT, finish);
	// Periodically check input queue.
	std::cout << "Reading MIDI from port ... quit with Ctrl-C.\n";
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
	
void setupScene(){
	// ---------------------------------------- VERTEX ATTRIBUTE OBJECTS
	//                    stocke les liens entre un VBO et ses attributs
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// ------------------------------------------------------------- VBO
	// création du VBO
	
	glGenBuffers(1, &vbo); // Generate 1 buffer
	// création des données des vertices
	float vertices[] = {
//  Position      | Color             | Texcoords
    -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // Top-left
     0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top-right
     0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // Bottom-right
    -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom-left
};

	// rendre le buffer nouvellement créé, le buffer actif
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// uploader le VBO dans la RAM de la CG
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// ----------------------------------------------------------------- EBO
	// ça sert à gérer des index de points (comme dans les fichiers obj)
	// donc à faire des faces plus facilement
	
	glGenBuffers(1, &ebo);
	GLuint elements[] = {	0, 1, 2,
							2, 3, 0   };
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
    printf("\n VERT : %u", statusVERT);
    //char buffer[512];
	//glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
	//printf("\n VERTbuf : %s", buffer);
    // Create and compile the fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    GLint statusFRAG;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &statusFRAG);
    printf("\n FRAG : %u", statusFRAG);
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
	int width, height;
	unsigned char* image;
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	image = SOIL_load_image("hacheur1.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    printf("\n dimensions image : %i x %i \n\n", width, height);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex1"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	image = SOIL_load_image("hacheur2.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	printf("\n dimensions image : %i x %i \n\n", width, height);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex2"), 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    
    
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
	setupMidi();
	setupGLFW();
	setupScene();
	}

int main()
{
	setupGlobal();
	uniTime = glGetUniformLocation(shaderProgram, "time");                  
			// truc degueu portaudio

    PaStream *stream;
    PaError err;
    
    printf("PortAudio Test: output sawtooth wave.\n");
    /* Initialize our data for use by callback. */
    data.left_phase = data.right_phase = 0.0;

    // fin truc port audio


// ----------------------------------------------------------- MAIN LOOP
	while(!glfwWindowShouldClose(window))
	{
			// truc degueu portaudio

    
    /* Open an audio I/O stream. */
    Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                2,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                SAMPLE_RATE,
                                256,        /* frames per buffer */
                                patestCallback,
                                &data );
 

// fin du truc degueu	

		
		
		
		
		// clear screen au noir
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 
        
        glUniform1f(uniTime, (GLfloat)clock() / (GLfloat)CLOCKS_PER_SEC);
     
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        
        trans = glm::rotate(trans, glm::degrees(valc-valp), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(modelTrans, 1, GL_FALSE, glm::value_ptr(trans));
        //std::cout << "valc :" << valc << " valp :" << valp << std::endl;
        valp = valc;
        
        // dessine le triangle avec les 3 vertices
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
       
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




