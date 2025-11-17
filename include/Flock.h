#ifndef FLOCK_H
#define FLOCK_H

#include "Boid.h"
#include <vector>
#include <glm/glm.hpp>

/**
 * Classe que gerencia todo o bando de boids
 * Controla a criação, remoção e atualização de todos os boids
 */
class Flock {
public:
    std::vector<Boid*> boids;
    Boid* targetBoid;  // Boid-objetivo especial
    std::vector<Obstacle> obstacles;

    float worldBounds;
    bool paused;
    bool stepMode;
    bool enableShadows;
    bool enableFog;

    /**
     * Construtor
     * @param initialBoidCount Número inicial de boids
     * @param bounds Tamanho do mundo
     */
    Flock(int initialBoidCount = 20, float bounds = 50.0f);

    /**
     * Destrutor
     */
    ~Flock();

    /**
     * Adiciona um novo boid ao bando
     */
    void addBoid();

    /**
     * Remove um boid aleatório do bando
     */
    void removeBoid();

    /**
     * Atualiza todos os boids
     * @param deltaTime Tempo decorrido
     */
    void update(float deltaTime);

    /**
     * Renderiza todos os boids
     * @param view Matriz de view
     * @param projection Matriz de projeção
     */
    void render(const glm::mat4& view, const glm::mat4& projection);

    /**
     * Renderiza sombras de todos os boids
     * @param view Matriz de view
     * @param projection Matriz de projeção
     * @param groundY Altura do chão
     */
    void renderShadows(const glm::mat4& view, const glm::mat4& projection, float groundY);

    /**
     * Calcula o centro do bando (média das posições)
     */
    glm::vec3 getFlockCenter() const;

    /**
     * Calcula a velocidade média do bando
     */
    glm::vec3 getFlockVelocity() const;

    /**
     * Controla a velocidade do boid-objetivo via teclado
     * @param direction Direção do movimento (WASD, etc)
     * @param deltaTime Tempo decorrido
     */
    void controlTarget(glm::vec3 direction, float deltaTime);

    /**
     * Adiciona um obstáculo à cena
     */
    void addObstacle(const Obstacle& obstacle);

    /**
     * Alterna modo de pausa
     */
    void togglePause();

    /**
     * Avança um passo na simulação (modo passo-a-passo)
     */
    void step();

    /**
     * Alterna sombras
     */
    void toggleShadows();

    /**
     * Alterna névoa (fog)
     */
    void toggleFog();

private:
    /**
     * Inicializa o bando com boids em posições aleatórias
     */
    void initialize(int count);
};

#endif
