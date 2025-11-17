#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Flock.h"

#include <iostream>
#include <vector>

// Configurações da janela
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Objetos globais
Camera* camera = nullptr;
Flock* flock = nullptr;

// VAOs globais
unsigned int g_groundVAO = 0;
unsigned int g_coneVAO = 0;
unsigned int g_sphereVAO = 0;
int g_coneIndexCount = 0;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Controle de input
bool keys[1024] = {false};

// Protótipos de funções
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void renderGround(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
void renderTower(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
void renderObstacles(Shader& shader, const glm::mat4& view, const glm::mat4& projection,
                     const std::vector<Obstacle>& obstacles);
unsigned int createGroundVAO();
unsigned int createConeVAO(int segments, int* indexCount);
unsigned int createSphereVAO(int segments = 32);

int main() {
    // Inicializa GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Cria janela GLFW
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Boids Simulation - TP1", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Carrega GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Configurações OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Iluminação global (OpenGL fixo simulado no shader)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Carrega shaders
    Shader boidShader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    Shader shadowShader("shaders/shadow_vertex.glsl", "shaders/shadow_fragment.glsl");

    // Inicializa objetos
    glm::vec3 towerPos(0.0f, 0.0f, 0.0f);
    float towerHeight = 50.0f;
    camera = new Camera(glm::vec3(0.0f, towerHeight, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                        towerPos, towerHeight);
    flock = new Flock(20, 50.0f);

    // Cria VAOs para mundo
    g_groundVAO = createGroundVAO();
    g_coneVAO = createConeVAO(32, &g_coneIndexCount);
    g_sphereVAO = createSphereVAO();

    std::cout << "\n=== BOIDS SIMULATION ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD/Arrows - Control target boid" << std::endl;
    std::cout << "  Q/E         - Target boid up/down" << std::endl;
    std::cout << "  C           - Change camera mode" << std::endl;
    std::cout << "  +           - Add boid" << std::endl;
    std::cout << "  -           - Remove boid" << std::endl;
    std::cout << "  P           - Pause/Resume" << std::endl;
    std::cout << "  N           - Step (when paused)" << std::endl;
    std::cout << "  F           - Toggle fog" << std::endl;
    std::cout << "  H           - Toggle shadows" << std::endl;
    std::cout << "  ESC         - Exit" << std::endl;
    std::cout << "========================\n" << std::endl;

    // Loop de renderização
    while (!glfwWindowShouldClose(window)) {
        // Timing
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window);

        // Atualiza simulação
        flock->update(deltaTime);

        // Atualiza câmera
        glm::vec3 flockCenter = flock->getFlockCenter();
        glm::vec3 flockVelocity = flock->getFlockVelocity();
        camera->UpdateCamera(flockCenter, flockVelocity);

        // Renderização
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrizes de view e projeção
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom),
                                                 (float)SCR_WIDTH / (float)SCR_HEIGHT,
                                                 0.1f, 200.0f);

        // shadowcolor
        glm::vec4 shadowColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.3f);

        // Ativa shader principal
        boidShader.use();
        boidShader.setMat4("view", view);
        boidShader.setMat4("projection", projection);

        // Configuração de iluminação
        boidShader.setVec3("lightPos", 0.0f, 100.0f, 0.0f);
        boidShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        boidShader.setVec3("viewPos", camera->Position);

        // Fog
        boidShader.setBool("enableFog", flock->enableFog);
        boidShader.setFloat("fogDensity", 0.02f);
        boidShader.setVec3("fogColor", 0.1f, 0.2f, 0.3f);

        // Renderiza chão
        renderGround(boidShader, view, projection);

        // Renderiza torre central
        renderTower(boidShader, view, projection);

        // Renderiza obstáculos
        renderObstacles(boidShader, view, projection, flock->obstacles);

        // Renderiza boids
        for (auto* boid : flock->boids) {
            glm::mat4 model = boid->getModelMatrix();
            boidShader.setMat4("model", model);
            boidShader.setVec3("objectColor", 0.8f, 0.3f, 0.2f);  // Cor laranja
            boid->render(view, projection);
        }

        // Renderiza boid-objetivo (cor diferente)
        if (flock->targetBoid) {
            glm::mat4 model = flock->targetBoid->getModelMatrix();
            boidShader.setMat4("model", model);
            boidShader.setVec3("objectColor", 0.8f, 0.8f, 0.2f);  // Cor amarela
            flock->targetBoid->render(view, projection);
        }

        // TODO passar as renderizacoes para as respectivas classes no futuro
        // Renderiza sombras (projeção paralela simples no chão)
        if (flock->enableShadows) {
            shadowShader.use();
            shadowShader.setMat4("view", view);
            shadowShader.setMat4("projection", projection);
            shadowShader.setVec4("shadowColor", shadowColor);

            // Desabilita escrita no depth buffer para evitar z-fighting
            glDepthMask(GL_FALSE);

            float groundY = 0.01f;  // Pequeno offset para evitar z-fighting com o chão

            // Renderiza sombras dos boids normais
            for (auto* boid : flock->boids) {
                // Cria matriz de projeção paralela no chão
                glm::mat4 shadowModel = glm::mat4(1.0f);

                // Posiciona a sombra no chão, mantendo x e z do boid
                shadowModel = glm::translate(shadowModel, glm::vec3(boid->position.x, groundY, boid->position.z));

                // Aplica rotação apenas em Y (yaw) para orientar a sombra
                if (glm::length(boid->velocity) > 0.001f) {
                    glm::vec3 forward = glm::normalize(boid->velocity);
                    float yaw = atan2(forward.x, forward.z);
                    shadowModel = glm::rotate(shadowModel, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
                }

                // Achata completamente no eixo Y (projeção paralela)
                shadowModel = glm::scale(shadowModel, glm::vec3(0.5f, 0.0f, 0.5f));

                shadowShader.setMat4("model", shadowModel);
                boid->renderShadow(view, projection, groundY);
            }

            // Renderiza sombra do boid-objetivo
            if (flock->targetBoid) {
                glm::mat4 shadowModel = glm::mat4(1.0f);

                shadowModel = glm::translate(shadowModel, glm::vec3(flock->targetBoid->position.x, groundY, flock->targetBoid->position.z));

                if (glm::length(flock->targetBoid->velocity) > 0.001f) {
                    glm::vec3 forward = glm::normalize(flock->targetBoid->velocity);
                    float yaw = atan2(forward.x, forward.z);
                    shadowModel = glm::rotate(shadowModel, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
                }

                shadowModel = glm::scale(shadowModel, glm::vec3(0.5f, 0.0f, 0.5f));

                shadowShader.setMat4("model", shadowModel);
                flock->targetBoid->renderShadow(view, projection, groundY);
            }

            // Re-habilita escrita no depth buffer
            glDepthMask(GL_TRUE);
        }

        // Troca buffers e processa eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpeza
    delete camera;
    delete flock;
    glDeleteVertexArrays(1, &g_groundVAO);
    glDeleteVertexArrays(1, &g_coneVAO);
    glDeleteVertexArrays(1, &g_sphereVAO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        keys[key] = true;

        // Comandos de tecla única
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }
        else if (key == GLFW_KEY_C) {
            camera->NextMode();
        }
        else if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) {  // Tecla +
            flock->addBoid();
        }
        else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) {  // Tecla -
            flock->removeBoid();
        }
        else if (key == GLFW_KEY_P) {
            flock->togglePause();
        }
        else if (key == GLFW_KEY_N) {
            flock->step();
        }
        else if (key == GLFW_KEY_F) {
            flock->toggleFog();
        }
        else if (key == GLFW_KEY_H) {
            flock->toggleShadows();
        }
    }
    else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }
}

void processInput(GLFWwindow* window) {
    // Controle contínuo do boid-objetivo
    glm::vec3 targetControl(0.0f);

    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
        targetControl.z += 1.0f;
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
        targetControl.z -= 1.0f;
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
        targetControl.x += 1.0f;
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
        targetControl.x -= 1.0f;
    if (keys[GLFW_KEY_Q])
        targetControl.y += 1.0f;
    if (keys[GLFW_KEY_E])
        targetControl.y -= 1.0f;

    if (glm::length(targetControl) > 0.001f) {
        flock->controlTarget(glm::normalize(targetControl), deltaTime);
    }
}

void renderGround(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(100.0f, 1.0f, 100.0f));
    shader.setMat4("model", model);
    shader.setVec3("objectColor", 0.2f, 0.5f, 0.2f);  // Verde escuro

    // Renderiza plano do chão
    glBindVertexArray(g_groundVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void renderTower(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f, 50.0f, 5.0f));
    shader.setMat4("model", model);
    shader.setVec3("objectColor", 0.6f, 0.6f, 0.6f);  // Cinza

    // Renderiza cone
    glBindVertexArray(g_coneVAO);
    glDrawElements(GL_TRIANGLES, g_coneIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void renderObstacles(Shader& shader, const glm::mat4& view, const glm::mat4& projection,
                     const std::vector<Obstacle>& obstacles) {
    for (const auto& obstacle : obstacles) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, obstacle.position);

        if (obstacle.type == Obstacle::SPHERE) {
            model = glm::scale(model, glm::vec3(obstacle.radius));
            shader.setMat4("model", model);
            shader.setVec3("objectColor", 0.8f, 0.2f, 0.2f);  // Vermelho
            // Renderiza esfera
        }
        else if (obstacle.type == Obstacle::CONE) {
            model = glm::scale(model, glm::vec3(obstacle.radius, obstacle.height, obstacle.radius));
            shader.setMat4("model", model);
            shader.setVec3("objectColor", 0.7f, 0.5f, 0.3f);  // Marrom
            // Renderiza cone
        }
    }
}

unsigned int createGroundVAO() {
    // Cria um plano simples para o chão
    float vertices[] = {
        // Posições              // Normais
        -1.0f, 0.0f, -1.0f,      0.0f, 1.0f, 0.0f,
         1.0f, 0.0f, -1.0f,      0.0f, 1.0f, 0.0f,
         1.0f, 0.0f,  1.0f,      0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f,  1.0f,      0.0f, 1.0f, 0.0f,
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    return VAO;
}

unsigned int createConeVAO(int segments, int* indexCount) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Vértice do topo do cone (apex)
    vertices.push_back(0.0f);  // x
    vertices.push_back(1.0f);  // y (altura normalizada)
    vertices.push_back(0.0f);  // z
    vertices.push_back(0.0f);  // normal x
    vertices.push_back(1.0f);  // normal y
    vertices.push_back(0.0f);  // normal z

    // Vértices da base circular
    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / (float)segments * 2.0f * 3.14159265359f;
        float x = cos(angle);
        float z = sin(angle);

        // Posição
        vertices.push_back(x);
        vertices.push_back(0.0f);  // Base no y=0
        vertices.push_back(z);

        // Normal (aproximada para as faces laterais)
        float nx = x;
        float ny = 0.5f;  // Inclinação do cone
        float nz = z;
        float len = sqrt(nx*nx + ny*ny + nz*nz);
        vertices.push_back(nx / len);
        vertices.push_back(ny / len);
        vertices.push_back(nz / len);
    }

    // Centro da base (para fechar a base)
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(-1.0f);  // Normal apontando para baixo
    vertices.push_back(0.0f);

    // Índices para faces laterais
    for (int i = 0; i < segments; i++) {
        indices.push_back(0);           // Vértice do topo
        indices.push_back(i + 1);       // Vértice atual da base
        indices.push_back(i + 2);       // Próximo vértice da base
    }

    // Índices para a base (triângulos do centro para a borda)
    int centerIndex = segments + 2;
    for (int i = 0; i < segments; i++) {
        indices.push_back(centerIndex);
        indices.push_back(i + 2);
        indices.push_back(i + 1);
    }

    unsigned int VAO, VBO, EBO;
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

    // Armazena o número de índices
    if (indexCount) {
        *indexCount = indices.size();
    }

    return VAO;
}

unsigned int createSphereVAO(int segments) {
    // Implementação simplificada de esfera
    // Retorna VAO da esfera
    return 0;  // Placeholder
}
