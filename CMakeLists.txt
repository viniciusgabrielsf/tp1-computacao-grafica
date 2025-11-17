cmake_minimum_required(VERSION 3.12)
project(BoidsSimulation VERSION 1.0 LANGUAGES CXX C)

# C++17 padrão
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Adiciona flags de compilação
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Encontra pacotes necessários
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Arquivos fonte do projeto
set(SOURCES
    src/main.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Boid.cpp
    src/Flock.cpp
    external/glad/src/glad.c
)

set(HEADERS
    include/Shader.h
    include/Camera.h
    include/Boid.h
    include/Flock.h
)

# Cria executável
add_executable(${PROJECT_NAME} ${SOURCES})

# Diretórios de include
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${OPENGL_INCLUDE_DIR}
)

# Linka bibliotecas
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    ${CMAKE_DL_LIBS}
)

# Copia shaders para o diretório de build
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Mensagem de configuração
message(STATUS "Configuração do projeto Boids concluída")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
