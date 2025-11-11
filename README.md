# Simulação de Boids 3D - Trabalho Prático de Computação Gráfica

Simulação de bando de pássaros virtuais (Boids) em ambiente 3D usando C++, OpenGL, GLFW, GLAD e GLM.

## Descrição

Este projeto implementa uma simulação de Boids que seguem três regras principais:
- **Separação**: Mantêm distância mínima dos vizinhos
- **Coesão**: Mantêm-se unidos no bando
- **Alinhamento**: Movem-se na mesma direção e velocidade

## Estrutura do Projeto

```
tp1-computacao-grafica/
├── CMakeLists.txt
├── include/
│   ├── Shader.h
│   ├── Camera.h
│   ├── Boid.h
│   └── Flock.h
├── src/
│   ├── main.cpp
│   ├── Shader.cpp
│   ├── Camera.cpp
│   ├── Boid.cpp
│   └── Flock.cpp
├── shaders/
│   ├── vertex_shader.glsl
│   ├── fragment_shader.glsl
│   ├── shadow_vertex.glsl
│   └── shadow_fragment.glsl
├── external/
│   └── glad/
└── README.md
```

## Requisitos

### Bibliotecas Necessárias

- **C++17** ou superior
- **CMake** 3.12+
- **OpenGL** 3.3+
- **GLFW3** - Gerenciamento de janelas e input
- **GLAD** - Loader de funções OpenGL
- **GLM** - Biblioteca de matemática para gráficos

### Instalação de Dependências

#### macOS (Homebrew)
```bash
brew install cmake glfw glm
```

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install cmake libglfw3-dev libglm-dev build-essential
```

#### Windows (vcpkg)
```bash
vcpkg install glfw3:x64-windows glm:x64-windows
```

### GLAD

O GLAD precisa ser baixado e configurado manualmente:

1. Acesse https://glad.dav1d.de/
2. Configure:
   - Language: C/C++
   - Specification: OpenGL
   - API gl: Version 3.3+
   - Profile: Core
3. Clique em "GENERATE"
4. Baixe o arquivo ZIP
5. Extraia e coloque os arquivos na estrutura:
   ```
   external/glad/
   ├── include/
   │   ├── glad/
   │   │   └── glad.h
   │   └── KHR/
   │       └── khrplatform.h
   └── src/
       └── glad.c
   ```

## Compilação

### Linux/macOS

```bash
# Criar diretório de build
mkdir build
cd build

# Configurar com CMake
cmake ..

# Compilar
make

# Executar
./BoidsSimulation
```

### Windows (Visual Studio)

```bash
# Criar diretório de build
mkdir build
cd build

# Configurar com CMake
cmake .. -G "Visual Studio 16 2019"

# Abrir solução no Visual Studio
# Ou compilar via linha de comando:
cmake --build . --config Release

# Executar
Release/BoidsSimulation.exe
```

## Controles

### Controle do Boid-Objetivo
- **W / Seta para Cima**: Move para frente
- **S / Seta para Baixo**: Move para trás
- **A / Seta para Esquerda**: Move para esquerda
- **D / Seta para Direita**: Move para direita
- **Q**: Move para cima (eixo Y+)
- **E**: Move para baixo (eixo Y-)

### Câmera
- **C**: Alterna entre os 3 modos de câmera:
  1. **Torre**: Visão do alto da torre central
  2. **Atrás do Bando**: Segue atrás do bando
  3. **Lateral**: Vista perpendicular ao movimento

### Gerenciamento de Boids
- **+ (ou =)**: Adiciona um boid ao bando
- **- (ou _)**: Remove um boid do bando

### Funcionalidades Extras
- **P**: Pausa/Resume a simulação
- **N**: Avança um passo (quando pausado)
- **F**: Liga/Desliga névoa (fog)
- **H**: Liga/Desliga sombras

### Geral
- **ESC**: Sair da aplicação

## Funcionalidades Implementadas

### Requisitos Básicos (80%)

1. ✅ **Lógica Boids**: Separação, Coesão e Alinhamento
2. ✅ **Boid-Objetivo**: Controlável via teclado
3. ✅ **Mundo**: Chão plano e torre cônica central
4. ✅ **Modelo 3D**: Boids como poliedros (5 pirâmides)
5. ✅ **Animação**: Batimento de asas independente
6. ✅ **Controles**: Adicionar (+) e remover (-) boids
7. ✅ **Iluminação**: Modelo Phong com ambiente, difusa e especular
8. ✅ **3 Modos de Câmera**: Torre, Atrás, Lateral

### Requisitos Extras (20%)

1. ✅ **Obstáculos (10%)**: Esferas e cones na cena que boids evitam
2. ✅ **Sombras (5%)**: Projeção paralela simples no chão
3. ✅ **Fog (5%)**: Névoa habilitável via tecla F
4. ✅ **Modo Pausa (5%)**: Pausa (P) e passo-a-passo (N)
5. ✅ **Reshape (5%)**: Redimensionamento de janela
6. ✅ **Banking (10%)**: Rotação roll baseada na curvatura

## Detalhes de Implementação

### Algoritmo de Boids

Cada boid calcula três vetores de força:

1. **Separação**: Repulsão de vizinhos próximos
   ```cpp
   força = soma(posição - vizinho) / distância²
   ```

2. **Coesão**: Atração ao centro do bando
   ```cpp
   centro = média(posições dos vizinhos)
   força = direção(centro - posição)
   ```

3. **Alinhamento**: Ajuste à velocidade média
   ```cpp
   força = velocidade_média - velocidade_atual
   ```

### Banking (Roll)

O ângulo de banking é calculado baseado na aceleração lateral:

```cpp
aceleração = (velocidade - velocidade_anterior) / deltaTime
aceleração_lateral = dot(aceleração, vetor_direita)
ângulo_banking = aceleração_lateral * fator
```

### Sistema de Câmera

#### Modo 1 - Torre
```cpp
posição = (0, altura_torre, 0)
olhar_para = centro_do_bando
```

#### Modo 2 - Atrás
```cpp
posição = centro_bando - direção_bando * distância
olhar_para = centro_do_bando
```

#### Modo 3 - Lateral
```cpp
lado = cross(direção_bando, vetor_cima)
posição = centro_bando + lado * distância
olhar_para = centro_do_bando
```

### Iluminação (Phong)

O modelo de iluminação implementado usa:
- **Luz Ambiente**: 30% da luz total
- **Luz Difusa**: Baseada no ângulo da normal com a luz
- **Luz Especular**: Reflexo com shininess = 32

### Névoa (Fog)

Fog exponencial implementado no fragment shader:
```glsl
fogFactor = exp(-densidade * distância)
cor_final = mix(cor_fog, cor_objeto, fogFactor)
```

## Arquitetura do Código

### Classes Principais

- **Shader**: Gerencia carregamento e uso de shaders GLSL
- **Camera**: Implementa 3 modos de visualização
- **Boid**: Representa um pássaro individual com física e comportamento
- **Flock**: Gerencia o bando completo e obstáculos

### Fluxo de Execução

1. Inicialização (GLFW, GLAD, OpenGL)
2. Carregamento de shaders
3. Criação do bando e mundo
4. Loop principal:
   - Processamento de input
   - Atualização de física (Boids)
   - Atualização de câmera
   - Renderização (boids, mundo, sombras)
5. Limpeza e finalização

## Resolução de Problemas

### Erros de Compilação

**Erro: "GLFW not found"**
```bash
# Instale GLFW conforme instruções acima
# Ou configure manualmente no CMakeLists.txt
```

**Erro: "GLM not found"**
```bash
# Instale GLM ou adicione manualmente:
# GLM é header-only, basta copiar a pasta para include/
```

### Erros de Execução

**Janela não abre**
- Verifique se sua GPU suporta OpenGL 3.3+
- Atualize drivers de vídeo

**Shaders não carregam**
- Verifique se a pasta `shaders/` está no mesmo diretório do executável
- CMake copia automaticamente, mas pode ser necessário copiar manualmente

**FPS baixo**
- Reduza o número de boids (tecla -)
- Desative sombras (tecla H)
- Desative fog (tecla F)

## Créditos

Trabalho Prático desenvolvido para a disciplina de Computação Gráfica
- **Universidade**: UFMG
- **Professor**: Renato Ferreira
- **Aluno**: [Seu Nome]

## Referências

- [Craig Reynolds - Boids](https://www.red3d.com/cwr/boids/)
- [LearnOpenGL](https://learnopengl.com/)
- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [GLM Documentation](https://github.com/g-truc/glm)

## Licença

Este projeto foi desenvolvido para fins educacionais.
