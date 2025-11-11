#include "Boid.h"
#include <glad/glad.h>
#include <cmath>
#include <random>

Boid::Boid(glm::vec3 pos, glm::vec3 vel, bool target)
    : position(pos),
      velocity(vel),
      acceleration(0.0f),
      previousVelocity(vel),
      previousPosition(pos),
      wingState(0.0f),
      wingSpeed(5.0f),
      bankAngle(0.0f),
      isTarget(target),
      maxSpeed(target ? 15.0f : 10.0f),
      maxForce(0.5f),
      perceptionRadius(5.0f),
      VAO(0), VBO(0), EBO(0) {

    // Randomiza o estado inicial das asas para cada boid
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    wingState = dis(gen);

    createGeometry();
    setupBuffers();
}

Boid::~Boid() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Boid::update(float deltaTime) {
    // Salva estado anterior para cálculo de banking
    previousPosition = position;
    previousVelocity = velocity;

    // Atualiza velocidade e posição
    velocity += acceleration * deltaTime;

    // Limita velocidade máxima
    float speed = glm::length(velocity);
    if (speed > maxSpeed) {
        velocity = glm::normalize(velocity) * maxSpeed;
    }

    position += velocity * deltaTime;

    // Reseta aceleração
    acceleration = glm::vec3(0.0f);

    // Atualiza animação das asas
    updateWingAnimation(deltaTime);

    // Calcula banking
    calculateBanking(deltaTime);
}

void Boid::applyBehavior(const std::vector<Boid*>& boids) {
    if (isTarget) return;  // Boid-objetivo não segue regras do bando

    glm::vec3 sep = separation(boids);
    glm::vec3 ali = alignment(boids);
    glm::vec3 coh = cohesion(boids);

    // Pesos para cada comportamento (ajustáveis)
    sep *= 1.5f;
    ali *= 1.0f;
    coh *= 1.0f;

    acceleration += sep;
    acceleration += ali;
    acceleration += coh;
}

void Boid::seekTarget(const glm::vec3& target) {
    if (isTarget) return;  // Boid-objetivo não segue nada

    glm::vec3 desired = target - position;
    float distance = glm::length(desired);

    if (distance > 0.001f) {
        desired = glm::normalize(desired) * maxSpeed;
        glm::vec3 steer = desired - velocity;
        steer = limit(steer, maxForce);
        acceleration += steer * 0.5f;  // Peso do comportamento de seguir
    }
}

void Boid::avoidObstacles(const std::vector<Obstacle>& obstacles) {
    if (isTarget) return;  // Boid-objetivo é fantasma

    for (const auto& obstacle : obstacles) {
        glm::vec3 diff = position - obstacle.position;
        float distance = glm::length(diff);

        float avoidDistance = obstacle.radius + 3.0f;  // Margem de segurança

        if (distance < avoidDistance && distance > 0.001f) {
            // Força de repulsão inversamente proporcional à distância
            glm::vec3 avoidForce = glm::normalize(diff) / distance;
            avoidForce *= maxForce * 2.0f;  // Força forte de evasão
            acceleration += avoidForce;
        }
    }
}

void Boid::stayInBounds(float bounds) {
    float turnForce = 1.0f;

    // Repele das bordas
    if (position.x > bounds) acceleration.x -= turnForce;
    if (position.x < -bounds) acceleration.x += turnForce;
    if (position.z > bounds) acceleration.z -= turnForce;
    if (position.z < -bounds) acceleration.z += turnForce;

    // Mantém acima do chão e abaixo de um teto
    if (position.y < 2.0f) acceleration.y += turnForce;
    if (position.y > bounds * 0.8f) acceleration.y -= turnForce;
}

void Boid::updateWingAnimation(float deltaTime) {
    wingState += wingSpeed * deltaTime;
    if (wingState > 1.0f) {
        wingState = 0.0f;
    }
}

void Boid::calculateBanking(float deltaTime) {
    if (deltaTime < 0.001f) return;

    // Calcula a aceleração (derivada da velocidade)
    glm::vec3 currentAcceleration = (velocity - previousVelocity) / deltaTime;

    // Componente lateral da aceleração (perpendicular à direção de movimento)
    if (glm::length(velocity) > 0.001f) {
        glm::vec3 forward = glm::normalize(velocity);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));

        // Projeção da aceleração no eixo lateral
        float lateralAccel = glm::dot(currentAcceleration, right);

        // Ângulo de banking proporcional à aceleração lateral
        // Ajuste o fator multiplicador para controlar a intensidade
        float targetBankAngle = lateralAccel * 2.0f;
        targetBankAngle = glm::clamp(targetBankAngle, -glm::radians(45.0f), glm::radians(45.0f));

        // Suaviza a transição do ângulo de banking
        bankAngle = glm::mix(bankAngle, targetBankAngle, 0.1f);
    }
}

glm::mat4 Boid::getModelMatrix() {
    glm::mat4 model = glm::mat4(1.0f);

    // Translação para a posição do boid
    model = glm::translate(model, position);

    // Rotação para apontar na direção da velocidade
    if (glm::length(velocity) > 0.001f) {
        glm::vec3 forward = glm::normalize(velocity);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Yaw: rotação em torno do eixo Y (horizontal)
        float yaw = atan2(forward.x, forward.z);
        model = glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));

        // Pitch: rotação para cima/baixo
        float pitch = asin(forward.y);
        model = glm::rotate(model, -pitch, glm::vec3(1.0f, 0.0f, 0.0f));

        // Banking (Roll): rotação no eixo de movimento
        model = glm::rotate(model, bankAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // Animação das asas (rotação das pirâmides laterais)
    // Isso será aplicado na geometria

    // Escala
    model = glm::scale(model, glm::vec3(0.5f));

    return model;
}

// ===== REGRAS DOS BOIDS =====

glm::vec3 Boid::separation(const std::vector<Boid*>& boids) {
    glm::vec3 steering = glm::vec3(0.0f);
    int count = 0;
    float desiredSeparation = 2.0f;

    for (const auto* other : boids) {
        if (other == this || other->isTarget) continue;

        float distance = glm::distance(position, other->position);

        if (distance > 0 && distance < desiredSeparation) {
            glm::vec3 diff = position - other->position;
            diff = glm::normalize(diff);
            diff /= distance;  // Peso inversamente proporcional à distância
            steering += diff;
            count++;
        }
    }

    if (count > 0) {
        steering /= (float)count;
        if (glm::length(steering) > 0) {
            steering = glm::normalize(steering) * maxSpeed;
            steering -= velocity;
            steering = limit(steering, maxForce);
        }
    }

    return steering;
}

glm::vec3 Boid::cohesion(const std::vector<Boid*>& boids) {
    glm::vec3 center = glm::vec3(0.0f);
    int count = 0;

    for (const auto* other : boids) {
        if (other == this || other->isTarget) continue;

        float distance = glm::distance(position, other->position);

        if (distance > 0 && distance < perceptionRadius) {
            center += other->position;
            count++;
        }
    }

    if (count > 0) {
        center /= (float)count;
        glm::vec3 desired = center - position;
        if (glm::length(desired) > 0) {
            desired = glm::normalize(desired) * maxSpeed;
            glm::vec3 steer = desired - velocity;
            steer = limit(steer, maxForce);
            return steer;
        }
    }

    return glm::vec3(0.0f);
}

glm::vec3 Boid::alignment(const std::vector<Boid*>& boids) {
    glm::vec3 avgVelocity = glm::vec3(0.0f);
    int count = 0;

    for (const auto* other : boids) {
        if (other == this || other->isTarget) continue;

        float distance = glm::distance(position, other->position);

        if (distance > 0 && distance < perceptionRadius) {
            avgVelocity += other->velocity;
            count++;
        }
    }

    if (count > 0) {
        avgVelocity /= (float)count;
        avgVelocity = glm::normalize(avgVelocity) * maxSpeed;
        glm::vec3 steer = avgVelocity - velocity;
        steer = limit(steer, maxForce);
        return steer;
    }

    return glm::vec3(0.0f);
}

glm::vec3 Boid::limit(glm::vec3 vec, float max) {
    float lengthSq = glm::dot(vec, vec);
    if (lengthSq > max * max) {
        vec = glm::normalize(vec) * max;
    }
    return vec;
}

// ===== GEOMETRIA E RENDERIZAÇÃO =====

void Boid::createGeometry() {
    // Modelo de 5 pirâmides conforme enunciado
    // Centro + 4 pirâmides nas laterais formando um corpo de pássaro

    // Vértices: posição (x, y, z) e normal (nx, ny, nz)
    vertices = {
        // Pirâmide Central (corpo)
        0.0f, 0.0f, 2.0f,   0.0f, 0.0f, 1.0f,  // 0: Ponta frontal
        -0.5f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // 1: Base esquerda
        0.5f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,  // 2: Base direita
        0.0f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // 3: Base topo
        0.0f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // 4: Base baixo

        // Asa Esquerda (pirâmide)
        -2.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 5: Ponta asa esquerda
        -0.5f, 0.0f, 0.5f,  0.0f, 1.0f, 0.0f,  // 6: Base asa esquerda frente
        -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f,  // 7: Base asa esquerda trás
        -0.5f, 0.3f, 0.0f,  0.0f, 1.0f, 0.0f,  // 8: Base asa esquerda topo

        // Asa Direita (pirâmide)
        2.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,  // 9: Ponta asa direita
        0.5f, 0.0f, 0.5f,   0.0f, 1.0f, 0.0f,  // 10: Base asa direita frente
        0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  // 11: Base asa direita trás
        0.5f, 0.3f, 0.0f,   0.0f, 1.0f, 0.0f,  // 12: Base asa direita topo

        // Cauda (pirâmide menor)
        0.0f, 0.0f, -1.5f,  0.0f, 0.0f, -1.0f, // 13: Ponta cauda
        -0.3f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f,  // 14: Base cauda esquerda
        0.3f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  // 15: Base cauda direita
        0.0f, 0.3f, -0.5f,  0.0f, 1.0f, 0.0f,  // 16: Base cauda topo
    };

    // Índices para formar as 5 pirâmides
    indices = {
        // Pirâmide Central
        0, 1, 3,  0, 3, 2,  0, 2, 4,  0, 4, 1,
        1, 2, 3,  2, 4, 3,

        // Asa Esquerda
        5, 6, 8,  5, 8, 7,  5, 7, 6,
        6, 7, 8,

        // Asa Direita
        9, 10, 12,  9, 12, 11,  9, 11, 10,
        10, 11, 12,

        // Cauda
        13, 14, 16,  13, 16, 15,  13, 15, 14,
        14, 15, 16,
    };
}

void Boid::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Atributo de posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributo de normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Boid::render(const glm::mat4& view, const glm::mat4& projection) {
    // A renderização será feita no main com o shader ativo
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Boid::renderShadow(const glm::mat4& view, const glm::mat4& projection, float groundY) {
    // Sombra será renderizada no main com shader apropriado
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
