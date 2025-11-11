#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * Classe para gerenciar shaders OpenGL
 * Permite carregar, compilar e usar vertex e fragment shaders
 */
class Shader {
public:
    // ID do programa shader
    unsigned int ID;

    /**
     * Construtor: lê e compila os shaders
     * @param vertexPath Caminho para o vertex shader
     * @param fragmentPath Caminho para o fragment shader
     */
    Shader(const char* vertexPath, const char* fragmentPath);

    /**
     * Ativa o shader
     */
    void use() const;

    /**
     * Funções utilitárias para setar uniforms
     */
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    /**
     * Verifica erros de compilação/linkagem
     */
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
