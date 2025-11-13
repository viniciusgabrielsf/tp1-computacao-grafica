# Documentação de Implementação - Simulação de Boids

## Índice
1. [Visão Geral](#visão-geral)
2. [Estrutura do Projeto](#estrutura-do-projeto)
3. [Features Implementadas](#features-implementadas)
4. [Arquivos e Suas Funções](#arquivos-e-suas-funções)
5. [Parâmetros Ajustáveis](#parâmetros-ajustáveis)
6. [Guia de Modificação](#guia-de-modificação)

---

## Visão Geral

Este projeto implementa uma simulação de boids (pássaros virtuais) em 3D usando OpenGL, seguindo o algoritmo de Craig Reynolds. O sistema simula comportamento de bando emergente através de regras locais simples.

### Tecnologias Utilizadas
- **OpenGL 3.3+**: Renderização 3D
- **GLFW**: Gerenciamento de janela e entrada
- **GLM**: Matemática vetorial e matricial
- **GLAD**: Carregamento de funções OpenGL
- **C++17**: Linguagem de programação

---

## Estrutura do Projeto

```
tp1-computacao-grafica/
├── include/                    # Headers (.h)
│   ├── Boid.h                 # Classe Boid individual
│   ├── Flock.h                # Gerenciador do bando
│   ├── Camera.h               # Sistema de câmera
│   └── Shader.h               # Gerenciador de shaders
├── src/                       # Implementações (.cpp)
│   ├── main.cpp               # Ponto de entrada e loop principal
│   ├── Boid.cpp               # Lógica dos boids
│   ├── Flock.cpp              # Gerenciamento do bando
│   ├── Camera.cpp             # Lógica da câmera
│   └── Shader.cpp             # Carregamento de shaders
├── shaders/                   # Shaders GLSL
│   ├── vertex_shader.glsl     # Vertex shader principal
│   ├── fragment_shader.glsl   # Fragment shader principal
│   ├── shadow_vertex.glsl     # Vertex shader de sombras
│   └── shadow_fragment.glsl   # Fragment shader de sombras
├── external/                  # Bibliotecas externas
│   └── glad/                  # GLAD (OpenGL loader)
└── build/                     # Arquivos de compilação
```

---

## Features Implementadas

### ✅ Features Obrigatórias (100%)

#### 1. **Simulação de Boids (40%)**
- **Localização**: `src/Boid.cpp` (linhas 63-276)
- **Implementação**: Três regras fundamentais

##### 1.1 Separação
```cpp
// src/Boid.cpp:192-221
glm::vec3 Boid::separation(const std::vector<Boid*>& boids)
```
- Mantém distância mínima de 2.0 unidades entre boids
- Força inversamente proporcional à distância
- **Peso**: 1.5f (linha 75)

##### 1.2 Coesão
```cpp
// src/Boid.cpp:223-250
glm::vec3 Boid::cohesion(const std::vector<Boid*>& boids)
```
- Move boids em direção ao centro do grupo
- Raio de percepção: 15.0 unidades
- **Peso**: 1.1f (linha 77)

##### 1.3 Alinhamento
```cpp
// src/Boid.cpp:252-276
glm::vec3 Boid::alignment(const std::vector<Boid*>& boids)
```
- Sincroniza velocidade com vizinhos próximos
- Raio de percepção: 15.0 unidades
- **Peso**: 1.0f (linha 76)

#### 2. **Boid-Objetivo (20%)**
- **Localização**: `src/Boid.cpp:83-97`, `src/Flock.cpp:14-17`
- **Controle**: WASD/Setas + Q/E (cima/baixo)
- **Força de Controle**: 2.0f (`src/Flock.cpp:175`)
- **Peso do Seguimento**: 3.5f (`src/Boid.cpp:95`)

```cpp
// Criação do boid-objetivo
targetBoid = new Boid(glm::vec3(0.0f, 10.0f, 0.0f),
                      glm::vec3(2.0f, 0.0f, 0.0f),
                      true);  // isTarget = true
```

#### 3. **Modelo de Boid (10%)**
- **Localização**: `src/Boid.cpp:288-338`
- **Geometria**: 5 pirâmides formando um pássaro
  - 1 pirâmide central (corpo)
  - 2 pirâmides laterais (asas)
  - 1 pirâmide traseira (cauda)
  - Total: 17 vértices, cada um com posição e normal

```cpp
// Estrutura de cada vértice (6 floats)
// x, y, z (posição) + nx, ny, nz (normal)
```

#### 4. **Mundo e Torre (10%)**

##### 4.1 Chão
- **Localização**: `src/main.cpp:318-328, 320-355`
- **Tamanho**: 100x100 unidades
- **Cor**: Verde escuro (0.2, 0.5, 0.2)

##### 4.2 Torre (Cone)
- **Localização**: `src/main.cpp:330-341, 363-448`
- **Posição**: Centro do mundo (0, 0, 0)
- **Altura**: 50 unidades
- **Raio da base**: 5 unidades
- **Segmentos**: 32 (para suavidade)
- **Cor**: Cinza (0.6, 0.6, 0.6)

#### 5. **Modos de Câmera (20%)**
- **Localização**: `src/Camera.cpp:22-72`
- **Controle**: Tecla C para alternar

##### Modo 1: Torre (TOWER_VIEW)
```cpp
// src/Camera.cpp:29-35
Position = towerPosition + glm::vec3(0.0f, towerHeight, 0.0f);
```
- Olho no topo da torre (0, 50, 0)
- Olha para o centro do bando
- Up perpendicular ao chão (0, 1, 0)

##### Modo 2: Atrás do Bando (BEHIND_FLOCK)
```cpp
// src/Camera.cpp:37-45
Position = flockCenter - flockDirection * distanceBehind + glm::vec3(0.0f, 5.0f, 0.0f);
```
- Distância fixa: 30.0 unidades (`Camera.h:43`)
- Offset vertical: 5.0 unidades
- Acompanha o movimento do bando

##### Modo 3: Lateral (SIDE_VIEW)
```cpp
// src/Camera.cpp:47-68
glm::vec3 sideDirection = glm::normalize(glm::cross(flockDirectionHorizontal, WorldUp));
Position = flockCenter + sideDirection * distanceSide + glm::vec3(0.0f, 2.0f, 0.0f);
```
- Perpendicular à velocidade do bando
- Paralelo ao plano do chão
- Distância lateral: 40.0 unidades (`Camera.h:44`)

### ✅ Features Opcionais Implementadas

#### 1. **Animação de Asas (10%)**
- **Localização**: `src/Boid.cpp:130-191`
- **Estado por Boid**: `wingState` (0.0 a 1.0)
- **Velocidade**: `wingSpeed = 5.0f` (5 ciclos/segundo)
- **Inicialização**: Aleatória para cada boid (linha 25)

```cpp
// Cálculo da animação
float wingAngle = sin(wingState * 2.0f * π) * 0.5f; // ±0.5 radianos (~28°)

// Atualização em tempo real
void Boid::updateWingGeometry() {
    // Rotaciona pontas das asas
    // Atualiza VBO com glBufferSubData()
}
```

#### 2. **Banking (Rotação Dinâmica) (5%)**
- **Localização**: `src/Boid.cpp:193-212`
- **Cálculo**: Baseado na aceleração lateral

```cpp
// Ângulo proporcional à aceleração lateral
float lateralAccel = glm::dot(currentAcceleration, right);
float targetBankAngle = lateralAccel * 2.0f;
targetBankAngle = clamp(targetBankAngle, -45°, +45°);

// Suavização
bankAngle = mix(bankAngle, targetBankAngle, 0.1f);
```

#### 3. **Sombras (5%)**
- **Localização**: `src/main.cpp:185-239`
- **Tipo**: Projeção paralela simples no chão
- **Controle**: Tecla H

```cpp
// Matriz de sombra
shadowModel = translate(shadowModel, vec3(boid.x, 0.01, boid.z));
shadowModel = rotate(shadowModel, yaw, vec3(0, 1, 0));
shadowModel = scale(shadowModel, vec3(0.5, 0.0, 0.5));  // Achata em Y
```

**Técnicas anti-z-fighting**:
- `glDepthMask(GL_FALSE)` durante renderização
- Offset de 0.01f acima do chão
- Cor semi-transparente (0.0, 0.0, 0.0, 0.3)

#### 4. **Fog (Névoa) (5%)**
- **Localização**: `src/main.cpp:149-152`
- **Controle**: Tecla F
- **Implementação**: No fragment shader

```cpp
// Configuração
boidShader.setBool("enableFog", flock->enableFog);
boidShader.setFloat("fogDensity", 0.02f);
boidShader.setVec3("fogColor", 0.1f, 0.2f, 0.3f);
```

#### 5. **Iluminação (10%)**
- **Localização**: `src/main.cpp:144-147`
- **Modelo**: Phong simplificado (no shader)

```cpp
// Luz posicionada acima da torre
boidShader.setVec3("lightPos", 0.0f, 100.0f, 0.0f);
boidShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
boidShader.setVec3("viewPos", camera->Position);
```

#### 6. **Obstáculos (5%)**
- **Localização**: `src/Boid.cpp:94-110`, `src/Flock.cpp:23-28`
- **Tipos**: Esferas e cones
- **Comportamento**: Força de repulsão inversamente proporcional

```cpp
// Obstáculos iniciais
obstacles.push_back({vec3(15, 5, 15), 3.0f, 0.0f, SPHERE});
obstacles.push_back({vec3(-15, 8, -15), 2.5f, 0.0f, SPHERE});
obstacles.push_back({vec3(20, 0, -20), 3.0f, 10.0f, CONE});
```

#### 7. **Controles Extras**
- **P**: Pausar/Retomar simulação
- **N**: Avançar um passo (quando pausado)
- **+**: Adicionar boid
- **-**: Remover boid
- **ESC**: Sair

---

## Arquivos e Suas Funções

### 1. `src/main.cpp`
**Propósito**: Ponto de entrada, loop principal de renderização, gerenciamento de entrada.

#### Funções Principais:

##### `main()` (linhas 47-221)
- Inicializa GLFW e OpenGL
- Cria janela e contexto
- Carrega shaders
- Inicializa câmera e flock
- Loop de renderização principal

##### `renderGround()` (linhas 318-328)
- Renderiza o plano do chão
- Escala: 100x100 unidades
- Usa `g_groundVAO`

##### `renderTower()` (linhas 330-341)
- Renderiza torre cônica no centro
- Escala: 5 (raio) x 50 (altura) x 5 (raio)
- Usa `g_coneVAO`

##### `createGroundVAO()` (linhas 360-395)
- Cria geometria do plano (2 triângulos)
- 4 vértices, 6 índices
- Cada vértice: posição + normal

##### `createConeVAO()` (linhas 397-481)
- Gera cone proceduralmente
- Parâmetro: número de segmentos (32)
- Faces laterais + base circular

##### Input Handling:
- `key_callback()`: Eventos de tecla única
- `processInput()`: Controle contínuo (WASD)

### 2. `src/Boid.cpp` + `include/Boid.h`
**Propósito**: Implementação de um boid individual.

#### Estrutura da Classe Boid:

```cpp
class Boid {
public:
    // Estado físico
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    // Animação
    float wingState;        // 0.0 a 1.0
    float wingSpeed;        // 5.0 Hz
    float bankAngle;        // Rotação roll

    // Parâmetros
    bool isTarget;          // Se é o boid-objetivo
    float maxSpeed;         // 10.0 (normal) ou 15.0 (objetivo)
    float maxForce;         // 0.8
    float perceptionRadius; // 15.0

    // Renderização
    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};
```

#### Funções Principais:

##### `update(float deltaTime)` (linhas 37-61)
- Atualiza velocidade: `velocity += acceleration * deltaTime`
- Limita velocidade máxima
- Atualiza posição: `position += velocity * deltaTime`
- Reseta aceleração
- Atualiza animação de asas e banking

##### `applyBehavior()` (linhas 63-82)
- Calcula e aplica as 3 regras:
  - Separação × 1.5
  - Alinhamento × 1.0
  - Coesão × 1.1

##### `seekTarget()` (linhas 83-97)
- Calcula força para seguir o boid-objetivo
- Peso: 3.5f
- Steering behavior: desired - velocity

##### `avoidObstacles()` (linhas 94-110)
- Repulsão de obstáculos
- Força inversamente proporcional à distância

##### `stayInBounds()` (linhas 112-124)
- Mantém boids dentro do mundo
- Limites: ±worldBounds (50.0)

##### `updateWingAnimation()` (linhas 130-138)
- Incrementa wingState
- Chama updateWingGeometry()

##### `updateWingGeometry()` (linhas 140-191)
- Calcula ângulo: `sin(wingState * 2π) * 0.5`
- Rotaciona vértices das asas
- Atualiza VBO com `glBufferSubData()`

##### `calculateBanking()` (linhas 193-212)
- Calcula aceleração lateral
- Suaviza transição do ângulo
- Limita a ±45°

##### `getModelMatrix()` (linhas 214-244)
- Constrói matriz de transformação
- Translação → Rotação (yaw, pitch, roll) → Escala

##### Regras dos Boids:

###### `separation()` (linhas 246-277)
```cpp
// Parâmetros ajustáveis:
float desiredSeparation = 2.0f;  // Linha 250

// Para cada vizinho dentro da distância:
glm::vec3 diff = position - other->position;
diff = normalize(diff) / distance;  // Peso inversamente proporcional
```

###### `cohesion()` (linhas 279-306)
```cpp
// Calcula centro dos vizinhos próximos
if (distance < perceptionRadius) {  // 15.0
    center += other->position;
}
// Retorna steering para o centro
```

###### `alignment()` (linhas 308-332)
```cpp
// Calcula velocidade média dos vizinhos
if (distance < perceptionRadius) {  // 15.0
    avgVelocity += other->velocity;
}
// Retorna steering para alinhar
```

##### `createGeometry()` (linhas 344-393)
- Define os 17 vértices do modelo
- 5 pirâmides: corpo + 2 asas + cauda
- Cada vértice: 6 floats (pos + normal)

### 3. `src/Flock.cpp` + `include/Flock.h`
**Propósito**: Gerencia o bando completo de boids.

#### Estrutura da Classe Flock:

```cpp
class Flock {
public:
    std::vector<Boid*> boids;
    Boid* targetBoid;
    std::vector<Obstacle> obstacles;

    float worldBounds;      // 50.0
    bool paused;
    bool stepMode;
    bool enableShadows;
    bool enableFog;
};
```

#### Funções Principais:

##### `Flock()` (linhas 6-31)
- Cria boid-objetivo
- Inicializa bando (20 boids)
- Adiciona obstáculos

##### `initialize()` (linhas 41-52)
- Cria boids com posições e velocidades aleatórias
- Distribuição: ±10 unidades do centro

##### `update(float deltaTime)` (linhas 87-121)
```cpp
// Para cada boid:
1. boid->applyBehavior(boids);      // Regras locais
2. boid->seekTarget(targetBoid);    // Segue objetivo
3. boid->avoidObstacles(obstacles); // Evita obstáculos
4. boid->stayInBounds(worldBounds); // Limites do mundo
5. boid->update(deltaTime);         // Física
```

##### `controlTarget()` (linhas 170-177)
- Aplica força no boid-objetivo baseado em input
- Força: 2.0f

##### `getFlockCenter()` (linhas 148-157)
- Calcula posição média de todos os boids

##### `getFlockVelocity()` (linhas 159-168)
- Calcula velocidade média do bando

##### `addBoid()` / `removeBoid()` (linhas 54-85)
- Adiciona/remove boids dinamicamente

### 4. `src/Camera.cpp` + `include/Camera.h`
**Propósito**: Sistema de câmera com 3 modos.

#### Estrutura da Classe Camera:

```cpp
class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;  // Sempre (0, 1, 0)

    CameraMode mode;    // TOWER_VIEW, BEHIND_FLOCK, SIDE_VIEW
    float Zoom;         // 45°
    float distanceBehind;  // 30.0
    float distanceSide;    // 40.0

    glm::vec3 towerPosition;  // (0, 0, 0)
    float towerHeight;        // 50.0
};
```

#### Funções Principais:

##### `UpdateCamera()` (linhas 22-72)
- Atualiza posição baseado no modo
- **IMPORTANTE**: Up sempre = WorldUp (perpendicular ao chão)

##### `updateCameraVectors()` (linhas 91-99)
```cpp
// CRÍTICO: Mantém Up perpendicular ao chão
Up = WorldUp;  // Não recalcula!
Right = normalize(cross(Front, Up));
```

##### `NextMode()` (linhas 68-85)
- Alterna entre os 3 modos
- Imprime modo atual no console

### 5. `src/Shader.cpp` + `include/Shader.h`
**Propósito**: Carregamento e gerenciamento de shaders GLSL.

#### Funções:
- `use()`: Ativa o shader program
- `setMat4()`, `setVec3()`, `setBool()`, etc.: Define uniforms

### 6. Shaders GLSL

#### `shaders/vertex_shader.glsl`
- Transforma vértices: `gl_Position = projection * view * model * aPos`
- Passa normais e posições para fragment shader
- Calcula iluminação (se implementado)

#### `shaders/fragment_shader.glsl`
- Calcula cor final do pixel
- Iluminação Phong (ambiente + difusa + especular)
- Aplica fog (se habilitado)

#### `shaders/shadow_vertex.glsl` e `shadow_fragment.glsl`
- Renderiza sombras semi-transparentes
- Usa `shadowColor` uniform (0, 0, 0, 0.3)

---

## Parâmetros Ajustáveis

### 1. Comportamento dos Boids

#### Velocidades e Forças (`src/Boid.cpp:6-19`)
```cpp
maxSpeed(target ? 15.0f : 10.0f)    // Velocidade máxima
maxForce(0.8f)                       // Força máxima de steering
perceptionRadius(15.0f)              // Raio de visão dos vizinhos
```

#### Pesos das Regras (`src/Boid.cpp:75-77`)
```cpp
sep *= 1.5f;   // Separação (evita colisões)
ali *= 1.0f;   // Alinhamento (sincroniza velocidade)
coh *= 1.1f;   // Coesão (mantém grupo junto)
```

#### Seguir Objetivo (`src/Boid.cpp:95`)
```cpp
acceleration += steer * 3.5f;  // Peso do seekTarget
```

#### Controle do Objetivo (`src/Flock.cpp:175`)
```cpp
float controlForce = 2.0f;  // Responsividade ao input
```

#### Separação (`src/Boid.cpp:250`)
```cpp
float desiredSeparation = 2.0f;  // Distância mínima entre boids
```

### 2. Câmera (`include/Camera.h:43-44`)
```cpp
float distanceBehind;  // 30.0 - Distância no modo BEHIND_FLOCK
float distanceSide;    // 40.0 - Distância no modo SIDE_VIEW
```

### 3. Animação de Asas (`src/Boid.cpp:13`)
```cpp
wingSpeed(5.0f)  // Frequência de batimento (Hz)
```

Ângulo de rotação (`src/Boid.cpp:143`):
```cpp
float wingAngle = sin(wingState * 2.0f * π) * 0.5f;  // ±0.5 radianos
// Modificar 0.5f para ajustar amplitude
```

### 4. Banking (`src/Boid.cpp:206-207`)
```cpp
float targetBankAngle = lateralAccel * 2.0f;  // Sensibilidade
targetBankAngle = clamp(targetBankAngle, -45°, +45°);  // Limites
```

### 5. Mundo (`src/Flock.cpp:8`, `src/main.cpp:83-87`)
```cpp
float worldBounds = 50.0f;      // Tamanho do mundo
float towerHeight = 50.0f;      // Altura da torre
int initialBoidCount = 20;      // Número inicial de boids
```

### 6. Iluminação (`src/main.cpp:145-147`)
```cpp
lightPos = vec3(0.0f, 100.0f, 0.0f);  // Posição da luz
lightColor = vec3(1.0f, 1.0f, 1.0f);  // Cor da luz (branco)
```

### 7. Fog (`src/main.cpp:151`)
```cpp
float fogDensity = 0.02f;  // Densidade da névoa
```

### 8. Sombras (`src/main.cpp:137, 195, 213`)
```cpp
vec4 shadowColor = vec4(0.0f, 0.0f, 0.0f, 0.3f);  // Cor e transparência
float groundY = 0.01f;  // Altura da sombra (anti z-fighting)
scale(0.5f, 0.0f, 0.5f) // Tamanho da sombra (0.5 = 50%)
```

---

## Guia de Modificação

### Cenário 1: Boids Muito Agrupados
**Problema**: Boids colidem frequentemente.

**Solução**: Aumentar separação
```cpp
// src/Boid.cpp:250
float desiredSeparation = 3.0f;  // Era 2.0f

// src/Boid.cpp:75
sep *= 2.0f;  // Era 1.5f
```

### Cenário 2: Bando Muito Disperso
**Problema**: Boids se espalham demais.

**Solução**: Aumentar coesão
```cpp
// src/Boid.cpp:77
coh *= 2.0f;  // Era 1.1f

// src/Boid.cpp:18
perceptionRadius(20.0f)  // Era 15.0f
```

### Cenário 3: Boids Não Seguem Objetivo
**Problema**: Bando ignora o boid-objetivo.

**Solução**: Aumentar peso do seekTarget
```cpp
// src/Boid.cpp:95
acceleration += steer * 5.0f;  // Era 3.5f

// src/Flock.cpp:175
float controlForce = 4.0f;  // Era 2.0f

// src/Boid.cpp:17
maxForce(1.2f)  // Era 0.8f (permite mais aceleração)
```

### Cenário 4: Movimento Muito Rápido/Lento
**Problema**: Velocidade inadequada.

**Solução**: Ajustar maxSpeed
```cpp
// src/Boid.cpp:16
maxSpeed(target ? 20.0f : 15.0f)  // Era 15.0/10.0
// Ou para mais lento:
maxSpeed(target ? 10.0f : 7.0f)
```

### Cenário 5: Animação de Asas Muito Rápida/Lenta
**Problema**: Batimento não natural.

**Solução**: Ajustar wingSpeed
```cpp
// src/Boid.cpp:13
wingSpeed(3.0f)  // Mais lento (era 5.0f)
wingSpeed(8.0f)  // Mais rápido
```

### Cenário 6: Banking Muito Sutil/Exagerado
**Problema**: Rotação nas curvas inadequada.

**Solução**: Ajustar sensibilidade
```cpp
// src/Boid.cpp:206
float targetBankAngle = lateralAccel * 4.0f;  // Mais banking (era 2.0f)
// E/ou ajustar limites:
clamp(targetBankAngle, -60°, +60°)  // Era ±45°
```

### Cenário 7: Câmera Muito Perto/Longe
**Problema**: Distância da câmera inadequada.

**Solução**: Ajustar distâncias
```cpp
// src/Camera.cpp:7-8
distanceBehind(50.0f)  // Era 30.0f
distanceSide(60.0f)    // Era 40.0f
```

### Cenário 8: Adicionar Mais Obstáculos
**Como**: Adicionar no construtor do Flock

```cpp
// src/Flock.cpp:23-28
// Adicionar nova esfera
obstacles.push_back({
    glm::vec3(25.0f, 10.0f, 25.0f),  // Posição
    4.0f,                             // Raio
    0.0f,                             // Altura (0 para esfera)
    Obstacle::SPHERE                  // Tipo
});

// Adicionar novo cone
obstacles.push_back({
    glm::vec3(-25.0f, 0.0f, -25.0f),  // Posição
    5.0f,                              // Raio da base
    15.0f,                             // Altura
    Obstacle::CONE                     // Tipo
});
```

### Cenário 9: Mudar Cores
**Como**: Modificar em src/main.cpp

```cpp
// Cor dos boids normais (linha 167)
boidShader.setVec3("objectColor", 1.0f, 0.5f, 0.0f);  // Laranja

// Cor do boid-objetivo (linha 181)
boidShader.setVec3("objectColor", 0.0f, 1.0f, 0.5f);  // Verde-água

// Cor do chão (linha 322)
shader.setVec3("objectColor", 0.3f, 0.6f, 0.3f);  // Verde mais claro

// Cor da torre (linha 335)
shader.setVec3("objectColor", 0.5f, 0.5f, 0.8f);  // Azul acinzentado
```

### Cenário 10: Ajustar Número Inicial de Boids
**Como**: Modificar na criação do Flock

```cpp
// src/main.cpp:93
flock = new Flock(50, 50.0f);  // Era 20 boids
//                ^^ número de boids
//                    ^^ tamanho do mundo
```

---

## Fluxo de Execução

### Loop Principal (`src/main.cpp:109-210`)

```
1. Timing
   ├─> Calcula deltaTime

2. Input
   ├─> processInput(window)
   │   └─> Controla boid-objetivo (WASD/setas)

3. Atualização da Física
   ├─> flock->update(deltaTime)
   │   └─> Para cada boid:
   │       ├─> applyBehavior()      [Separação, Coesão, Alinhamento]
   │       ├─> seekTarget()         [Seguir objetivo]
   │       ├─> avoidObstacles()     [Evitar obstáculos]
   │       ├─> stayInBounds()       [Limites do mundo]
   │       └─> update(deltaTime)    [Física + Animação]

4. Atualização da Câmera
   ├─> camera->UpdateCamera(flockCenter, flockVelocity)

5. Renderização
   ├─> Limpa buffers
   ├─> Configura shader principal
   ├─> renderGround()               [Chão]
   ├─> renderTower()                [Torre]
   ├─> renderObstacles()            [Obstáculos]
   ├─> Para cada boid:
   │   └─> boid->render()
   ├─> Se sombras habilitadas:
   │   ├─> Configura shader de sombras
   │   └─> Para cada boid:
   │       └─> boid->renderShadow()
   └─> Troca buffers
```

---

## Compilação

### Linux/macOS
```bash
cd build
cmake ..
make
./BoidsSimulation
```

### Windows (Visual Studio)
```bash
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
Release\BoidsSimulation.exe
```

---

## Controles do Teclado

| Tecla | Ação |
|-------|------|
| **WASD** ou **Setas** | Controlar boid-objetivo (horizontal) |
| **Q** | Boid-objetivo sobe |
| **E** | Boid-objetivo desce |
| **C** | Alternar modo de câmera |
| **+** | Adicionar boid |
| **-** | Remover boid |
| **P** | Pausar/Retomar |
| **N** | Avançar um passo (pausado) |
| **F** | Ligar/Desligar fog |
| **H** | Ligar/Desligar sombras |
| **ESC** | Sair |

---

## Troubleshooting

### Boids Voam para Fora do Mundo
- Verificar `worldBounds` em `src/Flock.cpp:8`
- Aumentar força em `stayInBounds()` (`src/Boid.cpp:113`)

### Performance Baixa
- Reduzir número de boids (tecla **-**)
- Desabilitar sombras (tecla **H**)
- Desabilitar fog (tecla **F**)
- Reduzir segmentos do cone (`src/main.cpp:97`)

### Boids Atravessam Obstáculos
- Aumentar força em `avoidObstacles()` (`src/Boid.cpp:106`)
- Aumentar `avoidDistance` (`src/Boid.cpp:101`)

### Compilação Falha
- Verificar GLAD instalado em `external/glad/`
- Verificar GLM e GLFW instalados
- Consultar `SETUP.md` para instruções detalhadas

---

## Créditos

**Algoritmo**: Craig Reynolds (1987) - "Flocks, Herds, and Schools: A Distributed Behavioral Model"

**Implementação**: Desenvolvido para o TP1 de Computação Gráfica

**Bibliotecas**:
- OpenGL
- GLFW
- GLM
- GLAD
