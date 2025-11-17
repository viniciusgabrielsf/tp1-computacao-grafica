#ifndef BOID_H
#define BOID_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Estrutura para representar obstáculos na cena
struct Obstacle {
    glm::vec3 position;
    float radius;  // Para esferas
    float height;  // Para cones
    enum Type { SPHERE, CONE, CILINDER } type;
};

/**
 * Classe que representa um único Boid (pássaro virtual)
 * Implementa as 3 regras principais: Separação, Coesão e Alinhamento
 */
class Boid {
public:
    // Posição e velocidade
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    // Histórico de posições para calcular banking (derivada segunda)
    glm::vec3 previousVelocity;
    glm::vec3 previousPosition;

    // Estado da animação de bater asas (0.0 a 1.0)
    float wingState;
    float wingSpeed;

    // Ângulo de banking (roll - rotação no eixo X do boid)
    float bankAngle;

    // Se é o boid-objetivo (fantasma que passa através de obstáculos)
    bool isTarget;

    // Parâmetros de comportamento
    float maxSpeed;
    float maxForce;
    float perceptionRadius;  // Raio para detectar vizinhos

    // Buffers OpenGL para renderização
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    /**
     * Construtor
     * @param pos Posição inicial
     * @param vel Velocidade inicial
     * @param target Se é o boid-objetivo
     */
    Boid(glm::vec3 pos = glm::vec3(0.0f),
         glm::vec3 vel = glm::vec3(0.0f),
         bool target = false);

    /**
     * Destrutor
     */
    ~Boid();

    /**
     * Atualiza a posição e velocidade do boid
     * @param deltaTime Tempo decorrido desde o último frame
     */
    void update(float deltaTime);

    /**
     * Aplica as 3 regras de comportamento dos boids
     * @param boids Lista de todos os boids
     */
    void applyBehavior(const std::vector<Boid*>& boids);

    /**
     * Adiciona comportamento de seguir o objetivo
     * @param target Posição do boid-objetivo
     */
    void seekTarget(const glm::vec3& target);

    /**
     * Evita obstáculos na cena
     * @param obstacles Lista de obstáculos
     */
    void avoidObstacles(const std::vector<Obstacle>& obstacles);

    /**
     * Mantém o boid dentro dos limites do mundo
     * @param bounds Tamanho do mundo
     */
    void stayInBounds(float bounds);

    /**
     * Renderiza o boid
     * @param view Matriz de view
     * @param projection Matriz de projeção
     */
    void render(const glm::mat4& view, const glm::mat4& projection);

    /**
     * Renderiza a sombra do boid no chão (projeção paralela)
     * @param view Matriz de view
     * @param projection Matriz de projeção
     * @param groundY Altura do chão
     */
    void renderShadow(const glm::mat4& view, const glm::mat4& projection, float groundY);

    /**
     * Atualiza a animação de bater asas
     * @param deltaTime Tempo decorrido
     */
    void updateWingAnimation(float deltaTime);

    /**
     * Atualiza a geometria das asas baseado no estado de animação
     * Aplica rotação nas asas para simular batimento
     */
    void updateWingGeometry();

    /**
     * Calcula o ângulo de banking baseado na curvatura da trajetória
     * Banking é a rotação no eixo X (roll) relacionada à derivada segunda
     */
    void calculateBanking(float deltaTime);

    /**
     * Retorna a matriz de transformação do boid (posição, rotação, escala)
     */
    glm::mat4 getModelMatrix();

private:
    /**
     * Regra 1: Separação - mantém distância dos vizinhos
     */
    glm::vec3 separation(const std::vector<Boid*>& boids);

    /**
     * Regra 2: Coesão - move em direção ao centro do bando
     */
    glm::vec3 cohesion(const std::vector<Boid*>& boids);

    /**
     * Regra 3: Alinhamento - alinha velocidade com vizinhos
     */
    glm::vec3 alignment(const std::vector<Boid*>& boids);

    /**
     * Limita um vetor a um valor máximo
     */
    glm::vec3 limit(glm::vec3 vec, float max);

    /**
     * Cria a geometria do boid (5 pirâmides conforme enunciado)
     */
    void createGeometry();

    /**
     * Configura os buffers OpenGL
     */
    void setupBuffers();
};

#endif
