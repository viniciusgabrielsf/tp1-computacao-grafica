#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, glm::vec3 towerPos, float towerH)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      mode(TOWER_VIEW),
      Zoom(45.0f),
      distanceBehind(30.0f),
      distanceSide(40.0f),
      towerPosition(towerPos),
      towerHeight(towerH) {
    Position = position;
    WorldUp = up;
    Up = up;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::UpdateCamera(const glm::vec3& flockCenter, const glm::vec3& flockVelocity) {
    // Normaliza o vetor velocidade do bando
    glm::vec3 flockDirection = glm::length(flockVelocity) > 0.001f ?
                               glm::normalize(flockVelocity) :
                               glm::vec3(0.0f, 0.0f, 1.0f);

    switch(mode) {
        case TOWER_VIEW: {
            // Modo 1: Olho no alto da torre, olhando para o bando
            Position = towerPosition + glm::vec3(0.0f, towerHeight + 5.0f, 0.0f);
            Front = glm::normalize(flockCenter - Position);
            Up = WorldUp;
            break;
        }

        case BEHIND_FLOCK: {
            // Modo 2: Atrás do bando a uma distância fixa
            // Posiciona a câmera atrás do bando (oposto à direção de movimento)
            // Adiciona pequeno offset vertical para melhor visualização
            Position = flockCenter - flockDirection * distanceBehind + glm::vec3(0.0f, 5.0f, 0.0f);
            Front = glm::normalize(flockCenter - Position);
            Up = WorldUp;
            break;
        }

        case SIDE_VIEW: {
            // Modo 3: Perpendicular ao vetor velocidade, paralelo ao chão
            // Primeiro, projeta o vetor velocidade no plano do chão (remove componente Y)
            glm::vec3 flockDirectionHorizontal = glm::vec3(flockDirection.x, 0.0f, flockDirection.z);

            // Se a projeção for muito pequena (bando voando quase verticalmente), usa direção padrão
            if (glm::length(flockDirectionHorizontal) < 0.001f) {
                flockDirectionHorizontal = glm::vec3(0.0f, 0.0f, 1.0f);
            } else {
                flockDirectionHorizontal = glm::normalize(flockDirectionHorizontal);
            }

            // Calcula vetor perpendicular ao movimento no plano horizontal
            // cross(flockDirection, WorldUp) nos dá um vetor perpendicular à direção e paralelo ao chão
            glm::vec3 sideDirection = glm::normalize(glm::cross(flockDirectionHorizontal, WorldUp));

            // Posiciona a câmera ao lado do bando, no mesmo plano horizontal (ou próximo)
            Position = flockCenter + sideDirection * distanceSide + glm::vec3(0.0f, 2.0f, 0.0f);
            Front = glm::normalize(flockCenter - Position);
            Up = WorldUp;
            break;
        }
    }

    updateCameraVectors();
}

void Camera::NextMode() {
    int currentMode = static_cast<int>(mode);
    currentMode = (currentMode + 1) % 3;
    mode = static_cast<CameraMode>(currentMode);

    std::cout << "Camera Mode: ";
    switch(mode) {
        case TOWER_VIEW:
            std::cout << "TOWER VIEW" << std::endl;
            break;
        case BEHIND_FLOCK:
            std::cout << "BEHIND FLOCK" << std::endl;
            break;
        case SIDE_VIEW:
            std::cout << "SIDE VIEW" << std::endl;
            break;
    }
}

void Camera::SetMode(CameraMode newMode) {
    mode = newMode;
}

void Camera::updateCameraVectors() {
    // O Up deve sempre ser perpendicular ao plano do chão (WorldUp)
    Up = WorldUp;

    // Recalcula o vetor Right (perpendicular a WorldUp e Front)
    // Usando cross(Front, Up) para obter a direita correta
    Right = glm::normalize(glm::cross(Front, Up));
}
