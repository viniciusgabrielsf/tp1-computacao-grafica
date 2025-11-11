#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * Enum para os 3 modos de câmera
 */
enum CameraMode {
    TOWER_VIEW,      // Modo 1: Do alto da torre
    BEHIND_FLOCK,    // Modo 2: Atrás do bando
    SIDE_VIEW        // Modo 3: Perpendicular ao vetor velocidade
};

/**
 * Classe para gerenciar a câmera e seus 3 modos de visualização
 * Todos os modos olham para o centro do bando com normal perpendicular ao chão
 */
class Camera {
public:
    // Posição da câmera
    glm::vec3 Position;

    // Vetor apontando para onde a câmera está olhando
    glm::vec3 Front;

    // Vetor "para cima" da câmera
    glm::vec3 Up;

    // Vetor direita (perpendicular a Front e Up)
    glm::vec3 Right;

    // Up do mundo (sempre (0,1,0) - perpendicular ao chão)
    glm::vec3 WorldUp;

    // Modo atual da câmera
    CameraMode mode;

    // Opções da câmera
    float Zoom;
    float distanceBehind;  // Distância atrás do bando (modo 2)
    float distanceSide;    // Distância lateral (modo 3)

    // Posição da torre (centro do mundo)
    glm::vec3 towerPosition;
    float towerHeight;

    /**
     * Construtor
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 50.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           glm::vec3 towerPos = glm::vec3(0.0f, 0.0f, 0.0f),
           float towerH = 50.0f);

    /**
     * Retorna a matriz de view calculada usando LookAt
     */
    glm::mat4 GetViewMatrix();

    /**
     * Atualiza a posição e orientação da câmera baseado no modo atual
     * @param flockCenter Centro do bando (ponto médio das posições dos boids)
     * @param flockVelocity Vetor velocidade média do bando
     */
    void UpdateCamera(const glm::vec3& flockCenter, const glm::vec3& flockVelocity);

    /**
     * Alterna entre os modos de câmera
     */
    void NextMode();

    /**
     * Define o modo da câmera
     */
    void SetMode(CameraMode newMode);

private:
    /**
     * Atualiza os vetores Front, Right e Up da câmera
     */
    void updateCameraVectors();
};

#endif
