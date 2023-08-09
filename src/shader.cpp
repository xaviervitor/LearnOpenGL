#include "shader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    // store paths for debugging
    this->vertexSourcePath = vertexPath;
    this->fragmentSourcePath = fragmentPath;
    
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;

    // make sure the ifstreams can throw exceptions
    vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        // open files
        vertexShaderFile.open(vertexPath);
        fragmentShaderFile.open(fragmentPath);
        std::stringstream vertexShaderStream, fragmentShaderStream;

        // read files as streams
        vertexShaderStream << vertexShaderFile.rdbuf();
        fragmentShaderStream << fragmentShaderFile.rdbuf();

        // close files
        vertexShaderFile.close();
        fragmentShaderFile.close();
        
        // copy strings from streams
        vertexCode = vertexShaderStream.str();
        fragmentCode = fragmentShaderStream.str();

    } catch(std::ifstream::failure& e) {
        std::cout << "SHADER SOURCE FILE(S) COULD NOT BE READ" << std::endl << \
        "vertex: " << vertexPath << std::endl << \
        "fragment: " << fragmentPath << std::endl;
    }

    // copy c++ strings to c strings 
    const char* vertexShaderSource = vertexCode.c_str();
    const char* fragmentShaderSource = fragmentCode.c_str();

    unsigned int vertexShader, fragmentShader;

    // vertex shader compilation and error checking
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkVertexShaderCompileErrors(vertexShader);

    // fragment shader compilation and error checking
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkFragmentShaderCompileErrors(fragmentShader);

    // shader program linking and error checking
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    checkProgramLinkErrors(ID);

    // the already compiled and linked shaders can be deleted
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat4(const std::string &name, float value1, float value2, float value3, float value4) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3, value4);
}

void Shader::checkVertexShaderCompileErrors(unsigned int shader) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "VERTEX SHADER COMPILATION ERROR" << std::endl;
        std::cout << "path: " << this->vertexSourcePath << std::endl;
        std::cout << infoLog;
    }
}

void Shader::checkFragmentShaderCompileErrors(unsigned int shader) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "FRAGMENT SHADER COMPILATION ERROR" << std::endl;
        std::cout << "path: " << this->fragmentSourcePath << std::endl;
        std::cout << infoLog;
    }
}

void Shader::checkProgramLinkErrors(unsigned int program) {
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(ID, 1024, NULL, infoLog);
        std::cout << "SHADER PROGRAM LINKING ERROR" << std::endl;
        std::cout << infoLog << std::endl;
    }
}