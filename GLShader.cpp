#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "GLShader.hpp"

std::string readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "\n\nNe peut pas lire : " << filePath << ". Le fichier n'existe pas.\n\n" << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}


GLuint LoadShader(const char *vertex_path, const char *geometry_path, const char *fragment_path) {
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint geomShader = glCreateShader(GL_GEOMETRY_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read shaders
    std::string vertShaderStr = readFile(vertex_path);
    std::string geomShaderStr = readFile(geometry_path);
    std::string fragShaderStr = readFile(fragment_path);
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *geomShaderSrc = geomShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    GLint result = 0;
    int logLength;

    // VERTEX  ---------------------------------------------------------
    std::cout << " COMPILE VERT." << std::endl;
    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);
    // CHECK ERREUR
    GLint statusVERT, lenVERT;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &statusVERT);
    std::cout << " STATUS VERT : "<< statusVERT << std::endl; // retour d'erreurs des shaders
        glGetShaderiv(vertShader, GL_SHADER_SOURCE_LENGTH, &lenVERT);
    std::cout << " LENGHT VERT : "<< lenVERT << std::endl;
    
    // GEOMETRY  -------------------------------------------------------
    std::cout << " COMPILE GEOM." << std::endl;
    glShaderSource(geomShader, 1, &geomShaderSrc, NULL);
    glCompileShader(geomShader);
    // CHECK ERREUR
    GLint statusGEOM, lenGEOM;
    glGetShaderiv(geomShader, GL_COMPILE_STATUS, &statusGEOM);
    std::cout << " STATUS GEOM : "<< statusGEOM << std::endl; // retour d'erreurs des shaders
        glGetShaderiv(geomShader, GL_SHADER_SOURCE_LENGTH, &lenGEOM);
    std::cout << " LENGHT GEOM : "<< lenGEOM << std::endl;
    
    // FRAGMENT  -------------------------------------------------------
    std::cout << " COMPILE FRAG" << std::endl;
    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);
    // CHECK ERREUR    
	GLint statusFRAG, lenFRAG;
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &statusFRAG);
    std::cout << " STATUS FRAG : "<< statusFRAG << std::endl;
    glGetShaderiv(fragShader, GL_SHADER_SOURCE_LENGTH, &lenFRAG);
    std::cout << " LENGHT FRAG : "<< lenFRAG << std::endl;
        
    std::cout << " LINKING PROGRAM" << std::endl;
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, geomShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    
    

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    std::cout << " PROGRAM LINK STATUS : " << result << std::endl;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	std::cout << " LOG LENGTH : " << logLength << std::endl;

    std::vector<char> programError( (logLength > 1) ? logLength : 1 );
    glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
    std::cout << " ERREUR PROGRAM : " << &programError[0] << std::endl;

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}
