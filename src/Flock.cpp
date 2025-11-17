#include "Flock.h"
#include <random>
#include <iostream>
#include <algorithm>

Flock::Flock(int initialBoidCount, float bounds)
    : targetBoid(nullptr),
      worldBounds(bounds),
      paused(false),
      stepMode(false),
      enableShadows(true),
      enableFog(false) {

    // Cria o boid-objetivo no centro do mundo
    targetBoid = new Boid(glm::vec3(0.0f, 10.0f, 0.0f),
                          glm::vec3(2.0f, 0.0f, 0.0f),
                          true);  // isTarget = true

    // Inicializa o bando
    initialize(initialBoidCount);


    // TODO adicionar os obstaculos se a colisão der certo, por enquanto desisti de implementar pq não ta funfando
    // Adiciona alguns obstáculos (requisito extra)
    // Esferas
    // obstacles.push_back({glm::vec3(15.0f, 5.0f, 15.0f), 3.0f, 0.0f, Obstacle::SPHERE});
    // obstacles.push_back({glm::vec3(-15.0f, 8.0f, -15.0f), 2.5f, 0.0f, Obstacle::SPHERE});

    // Cones (além da torre central)
    // obstacles.push_back({glm::vec3(20.0f, 0.0f, -20.0f), 3.0f, 10.0f, Obstacle::CONE});

    std::cout << "Flock initialized with " << boids.size() << " boids" << std::endl;
}

Flock::~Flock() {
    // Libera memória dos boids
    for (auto* boid : boids) {
        delete boid;
    }
    delete targetBoid;
}

void Flock::initialize(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> posDistr(-10.0f, 10.0f);
    std::uniform_real_distribution<> velDistr(-2.0f, 2.0f);

    for (int i = 0; i < count; ++i) {
        glm::vec3 pos(posDistr(gen), posDistr(gen) + 10.0f, posDistr(gen));
        glm::vec3 vel(velDistr(gen), velDistr(gen), velDistr(gen));
        boids.push_back(new Boid(pos, vel, false));
    }
}

void Flock::addBoid() {
    // Adiciona um boid próximo ao centro do bando
    glm::vec3 center = getFlockCenter();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> offsetDistr(-5.0f, 5.0f);

    glm::vec3 pos = center + glm::vec3(offsetDistr(gen), offsetDistr(gen), offsetDistr(gen));
    glm::vec3 vel(offsetDistr(gen) * 0.5f, offsetDistr(gen) * 0.5f, offsetDistr(gen) * 0.5f);

    boids.push_back(new Boid(pos, vel, false));
    std::cout << "Boid added. Total: " << boids.size() << std::endl;
}

void Flock::removeBoid() {
    if (boids.empty()) {
        std::cout << "No boids to remove!" << std::endl;
        return;
    }

    // Remove um boid aleatório
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, boids.size() - 1);

    int index = dis(gen);
    delete boids[index];
    boids.erase(boids.begin() + index);

    std::cout << "Boid removed. Total: " << boids.size() << std::endl;
}

void Flock::update(float deltaTime) {
    if (paused && !stepMode) return;

    // Atualiza o boid-objetivo
    if (targetBoid) {
        targetBoid->update(deltaTime);
        targetBoid->stayInBounds(worldBounds);
    }

    // Atualiza cada boid
    for (auto* boid : boids) {
        // Aplica comportamento de bando
        boid->applyBehavior(boids);

        // Segue o boid-objetivo
        if (targetBoid) {
            boid->seekTarget(targetBoid->position);
        }

        // Evita obstáculos
        boid->avoidObstacles(obstacles);

        // Mantém dentro dos limites
        boid->stayInBounds(worldBounds);

        // Atualiza física
        boid->update(deltaTime);
    }

    // Desativa modo step após um passo
    if (stepMode) {
        stepMode = false;
        paused = true;
    }
}

void Flock::render(const glm::mat4& view, const glm::mat4& projection) {
    // Renderização será feita no main com shaders apropriados
    // Este método serve para iterar sobre os boids
    for (auto* boid : boids) {
        boid->render(view, projection);
    }

    // Renderiza o boid-objetivo de forma diferente (outra cor)
    if (targetBoid) {
        targetBoid->render(view, projection);
    }
}

void Flock::renderShadows(const glm::mat4& view, const glm::mat4& projection, float groundY) {
    if (!enableShadows) return;

    for (auto* boid : boids) {
        boid->renderShadow(view, projection, groundY);
    }

    if (targetBoid) {
        targetBoid->renderShadow(view, projection, groundY);
    }
}

glm::vec3 Flock::getFlockCenter() const {
    if (boids.empty()) return glm::vec3(0.0f);

    glm::vec3 center(0.0f);
    for (const auto* boid : boids) {
        center += boid->position;
    }
    center /= (float)boids.size();
    return center;
}

glm::vec3 Flock::getFlockVelocity() const {
    if (boids.empty()) return glm::vec3(0.0f, 0.0f, 1.0f);

    glm::vec3 avgVelocity(0.0f);
    for (const auto* boid : boids) {
        avgVelocity += boid->velocity;
    }
    avgVelocity /= (float)boids.size();
    return avgVelocity;
}

void Flock::controlTarget(glm::vec3 direction, float deltaTime) {
    if (!targetBoid) return;

    // Aplica força de controle na direção desejada
    // O deltaTime será aplicado no update() do boid, não aqui
    float controlForce = 2.0f;
    targetBoid->acceleration += direction * controlForce;
}

void Flock::addObstacle(const Obstacle& obstacle) {
    obstacles.push_back(obstacle);
    std::cout << "Obstacle added. Total: " << obstacles.size() << std::endl;
}

void Flock::togglePause() {
    paused = !paused;
    stepMode = false;
    std::cout << (paused ? "PAUSED" : "RUNNING") << std::endl;
}

void Flock::step() {
    if (paused) {
        stepMode = true;
        std::cout << "Step forward" << std::endl;
    }
}

void Flock::toggleShadows() {
    enableShadows = !enableShadows;
    std::cout << "Shadows: " << (enableShadows ? "ON" : "OFF") << std::endl;
}

void Flock::toggleFog() {
    enableFog = !enableFog;
    std::cout << "Fog: " << (enableFog ? "ON" : "OFF") << std::endl;
}
