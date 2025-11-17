# Detalhes de Implementação - Simulação de Boids

## Visão Geral

Este documento descreve detalhes técnicos da implementação do projeto de Boids.

## Arquitetura do Sistema

### Classes Principais

#### 1. Boid (Boid.h / Boid.cpp)

Representa um único pássaro virtual com:
- **Posição e Velocidade**: Vetores 3D (glm::vec3)
- **Aceleração**: Acumulada a cada frame pelas regras de comportamento
- **Estado de Animação**: `wingState` para batimento de asas
- **Ângulo de Banking**: Rotação roll calculada pela curvatura

**Métodos Principais:**
- `applyBehavior()`: Aplica as 3 regras (separação, coesão, alinhamento)
- `seekTarget()`: Comportamento de seguir o boid-objetivo
- `avoidObstacles()`: Evita colisão com obstáculos
- `calculateBanking()`: Calcula rotação roll baseada na derivada segunda
- `update()`: Integração física (Euler)

**Parâmetros Ajustáveis:**
```cpp
float maxSpeed = 10.0f;        // Velocidade máxima
float maxForce = 0.5f;         // Força máxima de steering
float perceptionRadius = 5.0f; // Raio de percepção de vizinhos
```

#### 2. Flock (Flock.h / Flock.cpp)

Gerencia o bando completo:
- Vetor de ponteiros para Boids
- Boid-objetivo especial
- Lista de obstáculos
- Estados globais (pausa, fog, sombras)

**Responsabilidades:**
- Criar/remover boids dinamicamente
- Atualizar todos os boids
- Gerenciar obstáculos
- Controlar estados de renderização

#### 3. Camera (Camera.h / Camera.cpp)

Implementa 3 modos de visualização:

**Modo 1 - Torre (TOWER_VIEW):**
- Posição fixa no topo da torre
- Sempre olha para o centro do bando
- Melhor para visão geral da simulação

**Modo 2 - Atrás (BEHIND_FLOCK):**
- Segue o bando a uma distância fixa
- Posicionada atrás da direção de movimento
- Perspectiva de "perseguição"

**Modo 3 - Lateral (SIDE_VIEW):**
- Perpendicular ao vetor velocidade
- Paralelo ao chão
- Útil para visualizar comportamento lateral

#### 4. Shader (Shader.h / Shader.cpp)

Classe utilitária para:
- Carregar shaders de arquivos
- Compilar e linkar programas
- Setar uniforms facilmente
- Verificar erros de compilação

## Algoritmo de Boids

### 1. Separação

Evita colisões mantendo distância mínima:

```cpp
glm::vec3 Boid::separation(const std::vector<Boid*>& boids) {
    glm::vec3 steering = glm::vec3(0.0f);
    float desiredSeparation = 2.0f;

    for (cada vizinho próximo) {
        diff = posição - vizinho.posição
        diff = normalize(diff) / distância  // Força inversamente proporcional
        steering += diff
    }

    return steering (limitado a maxForce)
}
```

**Características:**
- Força inversamente proporcional à distância
- Apenas vizinhos dentro de `desiredSeparation`
- Peso ajustável (padrão: 1.5x)

### 2. Coesão

Atrai boids para o centro do grupo:

```cpp
glm::vec3 Boid::cohesion(const std::vector<Boid*>& boids) {
    glm::vec3 center = média(posições dos vizinhos)
    glm::vec3 desired = center - posição
    desired = normalize(desired) * maxSpeed

    return desired - velocidade_atual (steering)
}
```

**Características:**
- Calcula centro de massa local
- Considera apenas vizinhos dentro de `perceptionRadius`
- Steering suave em direção ao centro

### 3. Alinhamento

Ajusta velocidade para corresponder aos vizinhos:

```cpp
glm::vec3 Boid::alignment(const std::vector<Boid*>& boids) {
    glm::vec3 avgVelocity = média(velocidades dos vizinhos)
    avgVelocity = normalize(avgVelocity) * maxSpeed

    return avgVelocity - velocidade_atual (steering)
}
```

**Características:**
- Média vetorial das velocidades
- Normalizada para velocidade máxima
- Produz movimento coordenado

## Banking (Rotação Roll)

Uma das implementações mais complexas do projeto.

### Conceito

Banking é a inclinação lateral que pássaros/aviões fazem ao fazer curvas. É relacionado à **derivada segunda da trajetória** (taxa de mudança da aceleração).

### Implementação

```cpp
void Boid::calculateBanking(float deltaTime) {
    // 1. Calcula aceleração atual
    glm::vec3 accel = (velocidade - velocidade_anterior) / deltaTime;

    // 2. Define sistema de coordenadas local
    glm::vec3 forward = normalize(velocidade);
    glm::vec3 up = vec3(0, 1, 0);
    glm::vec3 right = normalize(cross(forward, up));

    // 3. Projeta aceleração no eixo lateral
    float lateralAccel = dot(accel, right);

    // 4. Calcula ângulo proporcional
    float targetBankAngle = lateralAccel * 2.0f;
    targetBankAngle = clamp(targetBankAngle, -45°, +45°);

    // 5. Suaviza transição
    bankAngle = mix(bankAngle, targetBankAngle, 0.1f);
}
```

**Detalhes Importantes:**
- Sistema de coordenadas: X=frente, Y=cima, Z=lado
- Banking é rotação em torno do eixo X (forward)
- Suavização previne mudanças bruscas
- Limitado a ±45° para realismo

### Rotações do Boid

```cpp
glm::mat4 Boid::getModelMatrix() {
    model = translate(position);

    // Yaw: rotação horizontal (segue direção)
    float yaw = atan2(forward.x, forward.z);
    model = rotate(model, yaw, vec3(0, 1, 0));

    // Pitch: rotação vertical (sobe/desce)
    float pitch = asin(forward.y);
    model = rotate(model, -pitch, vec3(1, 0, 0));

    // Roll (Banking): inclinação lateral
    model = rotate(model, bankAngle, vec3(0, 0, 1));

    return model;
}
```

## Sistema de Câmera

### Implementação dos Modos

```cpp
void Camera::UpdateCamera(vec3 center, vec3 velocity) {
    switch(mode) {
    case TOWER_VIEW:
        Position = towerPos + vec3(0, towerHeight, 0);
        Front = normalize(center - Position);
        break;

    case BEHIND_FLOCK:
        vec3 direction = normalize(velocity);
        Position = center - direction * distanceBehind
                 + vec3(0, 10, 0);
        Front = normalize(center - Position);
        break;

    case SIDE_VIEW:
        vec3 side = normalize(cross(velocity, vec3(0,1,0)));
        Position = center + side * distanceSide
                 + vec3(0, 5, 0);
        Front = normalize(center - Position);
        break;
    }

    Up = vec3(0, 1, 0);  // Sempre perpendicular ao chão
}
```

### Características

- **Todas** olham para o centro do bando
- **Todas** mantêm normal vertical (perpendicular ao chão)
- Transição suave entre modos
- Distâncias ajustáveis

## Iluminação (Modelo Phong)

Implementado no fragment shader:

```glsl
// 1. Luz Ambiente (30%)
vec3 ambient = 0.3 * lightColor;

// 2. Luz Difusa (Lambert)
vec3 norm = normalize(Normal);
vec3 lightDir = normalize(lightPos - FragPos);
float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diff * lightColor;

// 3. Luz Especular (Blinn-Phong)
vec3 viewDir = normalize(viewPos - FragPos);
vec3 reflectDir = reflect(-lightDir, norm);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
vec3 specular = 0.5 * spec * lightColor;

// Cor final
vec3 result = (ambient + diffuse + specular) * objectColor;
```

### Parâmetros

- **Luz Ambiente**: 30% (garante visibilidade mínima)
- **Shininess**: 32 (reflexão moderada)
- **Especular Strength**: 0.5
- **Posição da Luz**: (0, 100, 0) - acima do mundo

## Névoa (Fog)

Fog exponencial para atmosfera:

```glsl
if (enableFog) {
    float distance = length(viewPos - FragPos);
    float fogFactor = exp(-fogDensity * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    result = mix(fogColor, result, fogFactor);
}
```

### Características

- Densidade ajustável (padrão: 0.02)
- Cor da névoa: cinza-azulado
- Ativável/desativável em tempo real (tecla F)

## Sombras

Projeção paralela simples no chão:

```cpp
// Para cada boid
glm::mat4 shadowMatrix = glm::mat4(1.0f);
shadowMatrix = translate(shadowMatrix,
    vec3(0, groundY - boid.position.y + 0.1f, 0));
shadowMatrix = scale(shadowMatrix, vec3(1, 0, 1));  // Achata Y

model = boid.getModelMatrix() * shadowMatrix;
```

### Características

- Projeção ortogonal no plano Y=0
- Cor semi-transparente (alpha = 0.3)
- Shader separado para performance
- Offset pequeno (0.1) para evitar z-fighting

## Obstáculos

Sistema de evasão implementado:

```cpp
void Boid::avoidObstacles(const vector<Obstacle>& obstacles) {
    for (cada obstáculo) {
        float distance = length(position - obstacle.position);
        float avoidDistance = obstacle.radius + margem;

        if (distance < avoidDistance) {
            // Força repulsiva inversamente proporcional
            vec3 avoidForce = normalize(position - obstacle.position);
            avoidForce /= distance;
            avoidForce *= maxForce * 2.0f;  // Força alta

            acceleration += avoidForce;
        }
    }
}
```

### Tipos de Obstáculos

1. **Esferas**: Raio simples
2. **Cones**: Aproximação por cilindro

**Nota**: Boid-objetivo é fantasma (atravessa obstáculos)

## Geometria do Boid

Modelo de 5 pirâmides conforme especificação:

```
     Asa Esq.    Corpo Central    Asa Dir.
        /|\         /|\            /|\
       / | \       / | \          / | \
      /  |  \     /  |  \        /  |  \
     ----------- ----------- -----------
                    |
                   \|/  Cauda
```

### Vértices Principais

- **Corpo**: Pirâmide central apontando para frente
- **Asas**: Pirâmides laterais (podem animar)
- **Cauda**: Pirâmide menor atrás

**Total**: ~17 vértices, ~60 índices

## Animação de Asas

```cpp
void Boid::updateWingAnimation(float deltaTime) {
    wingState += wingSpeed * deltaTime;
    if (wingState > 1.0f) wingState = 0.0f;

    // wingState pode modular geometria das asas
    // Por exemplo: rotação das pirâmides laterais
}
```

Cada boid tem estado independente (inicializado aleatoriamente).

## Parâmetros Ajustáveis

### Comportamento de Boids

```cpp
// Em Boid.cpp
float separationWeight = 1.5f;
float alignmentWeight = 1.0f;
float cohesionWeight = 1.0f;
float targetWeight = 0.5f;

float desiredSeparation = 2.0f;
float perceptionRadius = 5.0f;
```

### Física

```cpp
float maxSpeed = 10.0f;
float maxForce = 0.5f;
float worldBounds = 50.0f;
```

### Câmera

```cpp
float distanceBehind = 30.0f;  // Modo 2
float distanceSide = 40.0f;    // Modo 3
float towerHeight = 50.0f;     // Modo 1
```

### Visual

```cpp
float fogDensity = 0.02f;
vec4 shadowColor = vec4(0, 0, 0, 0.3);
float shininess = 32.0f;
```

## Conclusão

Este projeto implementa todos os requisitos básicos (80%) e extras (20%) especificados:

**Básicos:**
- ✅ Lógica de Boids (separação, coesão, alinhamento)
- ✅ Boid-objetivo controlável
- ✅ Mundo com chão e torre
- ✅ Modelo 3D (5 pirâmides)
- ✅ Animação de asas
- ✅ Controles +/-
- ✅ Iluminação Phong
- ✅ 3 modos de câmera

**Extras:**
- ✅ Obstáculos (10%)
- ✅ Sombras (5%)
- ✅ Fog (5%)
- ✅ Pausa/Step (5%)
- ✅ Reshape (5%)
- ✅ Banking (10%)
