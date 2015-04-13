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
// calcul du temps
#include <time.h>
#include <sys/time.h>
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
	midiin->openPort( 1 ); // index du port que l'on ouvre
	// Don't ignore sysex, timing, or active sensing messages.
	midiin->ignoreTypes( false, false, false );
	done = false;// Install an interrupt handler function.
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
	image = SOIL_load_image("medias/hacheur1.png", &width, &height, 0, SOIL_LOAD_RGB);
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
	image = SOIL_load_image("medias/hacheur2.png", &width, &height, 0, SOIL_LOAD_RGB);
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
	//setupMidi();
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
		// mesure du temps d'execution
		struct timeval start, draw, swap, end;
		gettimeofday(&start, NULL);
  
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
        
        gettimeofday(&draw, NULL);
        
        // dessine le triangle avec les 3 vertices
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        gettimeofday(&swap, NULL);
        // IMPORTANT : DOUBLE BUFFER - TIMING - REFRESH RATE
        // regarder la fonction :
        // glfwSwapInterval(1);
        
        // double buffer si = 1
        glfwSwapInterval(1);
        
        glfwSwapBuffers(window);
		//glfwPollEvents();
		
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


/*
int main(int argc, char **argv)
{
  if (argc < 1)
    {
      printf("USAGE: %s loop-iterations\n", argv[0]);
      return 1;
    }

  int iterations = atoi(argv[1]);

  struct timeval start, end;
  
  gettimeofday(&start, NULL);
  
  for (int i = 0; i < iterations; i++) { ;}
  
  gettimeofday(&end, NULL);

  //printf("\n %ld \n\n", ((end.tv_sec * 1000000 + end.tv_usec)
  //	  - (start.tv_sec * 1000000 + start.tv_usec)));
  
  printf("\n");
  printf(" start.tv_sec : %f\n",(double)start.tv_sec);
  printf("start.tv_usec : %f\n",(double)start.tv_usec);
  printf("\n");
  printf("   end.tv_sec : %f \n",(double)end.tv_sec);
  printf("  end.tv_usec : %f\n",(double)end.tv_usec);
  printf("\n");
  double tutu = (double)end.tv_usec-start.tv_usec;
  printf("  end_usec - start_usec : %f\n",tutu);

  return 0;
}
*/


