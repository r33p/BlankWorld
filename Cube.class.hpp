
class cube
{
	public :
	
	// variables publique
	GLuint		vbo, ebo, vao;								// VBO
	int 		points=8;										//
	float		vertices[8*7];								//
	GLuint		textures[2];								// TEXTURES
	GLuint		vertexShader, fragmentShader, programGLSL;	// SHADERS
	GLint 		uniModel,	uniView,	uniProj,	uniTime;// OGL
	glm::mat4 	model,		view, 		proj;				// MATRICES
	
	// contructeur
	cube();
	
	// méthodes
	void setup_VBO();
	void setup_GLSL();
	void setup_TEXTURES();
	void setup_MATRICES();
	void update_VBO();
};



