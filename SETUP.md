# Guia de Configuração - Boids Simulation

Este guia detalha passo a passo como configurar o ambiente para compilar e executar o projeto.

## Passo 1: Instalar Dependências

### macOS

```bash
# Instalar Homebrew (se não tiver)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Instalar dependências
brew install cmake glfw glm
```

### Ubuntu/Debian Linux

```bash
sudo apt-get update
sudo apt-get install -y cmake libglfw3-dev libglm-dev build-essential git
```

### Windows

Recomenda-se usar **vcpkg** para gerenciar dependências:

```bash
# Clonar vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# Bootstrap
.\bootstrap-vcpkg.bat

# Instalar dependências
.\vcpkg install glfw3:x64-windows glm:x64-windows

# Integrar com Visual Studio
.\vcpkg integrate install
```

## Passo 2: Configurar GLAD

GLAD é um loader de funções OpenGL que precisa ser configurado manualmente.

### Baixar GLAD

1. Acesse: https://glad.dav1d.de/
2. Configure as opções:
   - **Language**: C/C++
   - **Specification**: OpenGL
   - **API gl**: Version 3.3 ou superior
   - **Profile**: Core
   - **Extensions**: (deixe as opções padrão)
3. Marque: "Generate a loader"
4. Clique em **GENERATE**
5. Clique em **glad.zip** para baixar

### Instalar GLAD no Projeto

```bash
# No diretório raiz do projeto
mkdir -p external/glad/include
mkdir -p external/glad/src

# Extraia o arquivo glad.zip baixado
# Copie os arquivos para a estrutura correta:

# Estrutura esperada:
# external/glad/
# ├── include/
# │   ├── glad/
# │   │   └── glad.h
# │   └── KHR/
# │       └── khrplatform.h
# └── src/
#     └── glad.c
```

### Script de Instalação Automática (Linux/macOS)

Você pode usar o script `install_glad.sh` fornecido:

```bash
chmod +x install_glad.sh
./install_glad.sh
```

Este script irá:
1. Baixar GLAD automaticamente
2. Extrair na estrutura correta
3. Limpar arquivos temporários

## Passo 3: Verificar Estrutura do Projeto

Após configurar tudo, sua estrutura deve estar assim:

```
tp1-computacao-grafica/
├── CMakeLists.txt
├── README.md
├── SETUP.md
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
└── external/
    └── glad/
        ├── include/
        │   ├── glad/
        │   │   └── glad.h
        │   └── KHR/
        │       └── khrplatform.h
        └── src/
            └── glad.c
```

## Passo 4: Compilar o Projeto

### Linux/macOS

```bash
# Criar diretório de build
mkdir build
cd build

# Configurar projeto
cmake ..

# Compilar
make -j$(nproc)  # Linux
make -j$(sysctl -n hw.ncpu)  # macOS

# Executar
./BoidsSimulation
```

### Windows (Visual Studio)

```bash
# Criar diretório de build
mkdir build
cd build

# Gerar projeto Visual Studio
cmake .. -G "Visual Studio 16 2019"

# Compilar
cmake --build . --config Release

# Executar
Release\BoidsSimulation.exe
```

## Passo 5: Testar

Se tudo estiver configurado corretamente:

1. Uma janela OpenGL deve abrir
2. Você verá boids voando em 3D
3. O chão e torre devem estar visíveis
4. Os controles do teclado devem funcionar

### Testes Básicos

- Pressione **C** para alternar entre câmeras
- Pressione **+** para adicionar boids
- Pressione **-** para remover boids
- Use **WASD** para controlar o boid-objetivo
- Pressione **F** para ligar/desligar fog
- Pressione **H** para ligar/desligar sombras

## Resolução de Problemas Comuns

### Erro: "GLFW not found"

```bash
# macOS
brew install glfw

# Linux
sudo apt-get install libglfw3-dev
```

### Erro: "GLM not found"

```bash
# macOS
brew install glm

# Linux
sudo apt-get install libglm-dev
```

### Erro: "glad/glad.h: No such file or directory"

O GLAD não está instalado corretamente. Verifique:
1. Se o diretório `external/glad` existe
2. Se os arquivos estão na estrutura correta
3. Execute o script `install_glad.sh` novamente

### Erro: "OpenGL version 3.3 not supported"

Sua GPU ou drivers não suportam OpenGL 3.3+:
1. Atualize seus drivers de vídeo
2. Verifique se sua GPU suporta OpenGL 3.3+
3. Em máquinas virtuais, habilite aceleração 3D

### Shaders não carregam

```bash
# Verifique se a pasta shaders está no diretório correto
ls shaders/

# Se estiver na pasta build, copie manualmente
cp -r ../shaders .
```

### Performance ruim (FPS baixo)

1. Reduza o número de boids (tecla **-**)
2. Desative sombras (tecla **H**)
3. Desative fog (tecla **F**)
4. Verifique se está usando a GPU dedicada (não integrada)

## Suporte

Para mais informações, consulte:
- README.md - Documentação completa
- Código fonte - Comentários detalhados
- [LearnOpenGL](https://learnopengl.com/) - Tutoriais de OpenGL

## Checklist de Instalação

- [ ] CMake instalado (versão 3.12+)
- [ ] GLFW instalado
- [ ] GLM instalado
- [ ] GLAD baixado e instalado em `external/glad/`
- [ ] Compilador C++ com suporte a C++17
- [ ] OpenGL 3.3+ disponível no sistema
- [ ] Projeto compilado sem erros
- [ ] Executável roda e mostra janela OpenGL
- [ ] Controles funcionando

Se todos os itens estiverem marcados, seu projeto está pronto para uso!
