
class cube
{
	public :
	
	// variables publique
	GLuint		vbo, ebo, vao;								// VBO
	static const int 	points=1024;						//
	float		vertices[points*7];							//
	GLuint		textures[2];								// TEXTURES
	GLuint		vertexShader, fragmentShader, programGLSL;	// SHADERS
	GLint 		uniModel,	uniView,	uniProj,	uniTime;// OGL
	glm::mat4 	model,		view, 		proj;				// MATRICES
	
	// construct destruct
	cube();
	~cube();
	
	// méthodes
	void setup();
	void update(signed short*);
	void nettoyage();
};



