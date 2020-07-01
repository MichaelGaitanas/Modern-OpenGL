//A header that reads vertex, fragment and geometry shaders (provided we have
//already coded them). Then the header compiles, links and checks for errors.

#ifndef SHADER_H
#define SHADER_H

#include<GL/glew.h>
#include<glm/glm.hpp>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

class shader
{
public:
    unsigned int ID;
    //constructor
    shader(const char *vertexPath, const char* fragmentPath, const char *geometryPath = nullptr)
    {
        //1) read the vertex and fragment shaders from
        //   files and store them in const char *
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            if (geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "Error while reading shaders" << std::endl;
        }
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();

        //2) compile and link the shaders
        unsigned int vertex, fragment;
        //vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        CheckErrors(vertex,"VERTEX");
        //fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        CheckErrors(fragment,"FRAGMENT");
        //geometry shader
        unsigned int geometry;
        if (geometryPath != nullptr)
        {
            const char *gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            CheckErrors(geometry, "GEOMETRY");
        }
        //shader program
        ID = glCreateProgram();
        glAttachShader(ID,vertex);
        glAttachShader(ID,fragment);
        if (geometryPath != nullptr)
            glAttachShader(ID,geometry);
        glLinkProgram(ID);
        CheckErrors(ID,"PROGRAM");
        //free the resources since they are now linked in the program
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr)
            glDeleteShader(geometry);
    }

private:
    void CheckErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std:: cout << "Error while compiling " << type << " shader\n" << infoLog << "\n";
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std:: cout << "Error while linking " << type << "\n" << infoLog << "\n";
            }
        }
    }
};
#endif
