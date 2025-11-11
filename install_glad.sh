#!/bin/bash

# Script para instalar GLAD automaticamente no projeto

echo "=========================================="
echo "  GLAD Installation Script"
echo "  For Boids Simulation Project"
echo "=========================================="
echo ""

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Verifica se está no diretório correto
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}Error: CMakeLists.txt not found!${NC}"
    echo "Please run this script from the project root directory."
    exit 1
fi

echo -e "${YELLOW}Step 1: Creating directory structure...${NC}"
mkdir -p external/glad/include
mkdir -p external/glad/src

echo -e "${GREEN}✓ Directories created${NC}"
echo ""

echo -e "${YELLOW}Step 2: Downloading GLAD...${NC}"
echo "This will download pre-configured GLAD files for OpenGL 3.3 Core"
echo ""

# URL do GLAD pre-configurado (você pode gerar seu próprio em https://glad.dav1d.de/)
# Por segurança, vamos instruir o usuário a baixar manualmente
echo -e "${YELLOW}Please follow these steps:${NC}"
echo ""
echo "1. Open your browser and go to: https://glad.dav1d.de/"
echo "2. Configure:"
echo "   - Language: C/C++"
echo "   - Specification: OpenGL"
echo "   - API gl: Version 3.3 (or higher)"
echo "   - Profile: Core"
echo "3. Click 'GENERATE'"
echo "4. Download the ZIP file"
echo ""
echo "5. Extract the ZIP file"
echo "6. Copy files to this structure:"
echo "   - glad/include/glad/glad.h -> external/glad/include/glad/glad.h"
echo "   - glad/include/KHR/khrplatform.h -> external/glad/include/KHR/khrplatform.h"
echo "   - glad/src/glad.c -> external/glad/src/glad.c"
echo ""

read -p "Press Enter when you have downloaded and extracted GLAD..."

# Verifica se o usuário tem um arquivo glad.zip no diretório atual
if [ -f "glad.zip" ]; then
    echo -e "${YELLOW}Found glad.zip in current directory${NC}"
    echo "Extracting..."

    unzip -q glad.zip -d external/glad_temp

    # Move arquivos para a estrutura correta
    if [ -d "external/glad_temp/include" ]; then
        cp -r external/glad_temp/include/* external/glad/include/
        cp -r external/glad_temp/src/* external/glad/src/

        echo -e "${GREEN}✓ GLAD extracted and installed${NC}"

        # Limpa arquivos temporários
        rm -rf external/glad_temp
        rm glad.zip
    else
        echo -e "${RED}Error: Unexpected ZIP structure${NC}"
        exit 1
    fi
else
    echo ""
    echo -e "${YELLOW}Manual installation detected${NC}"
    echo "Checking if files are in place..."
fi

echo ""
echo -e "${YELLOW}Step 3: Verifying installation...${NC}"

# Verifica se os arquivos necessários existem
MISSING=0

if [ ! -f "external/glad/include/glad/glad.h" ]; then
    echo -e "${RED}✗ Missing: external/glad/include/glad/glad.h${NC}"
    MISSING=1
else
    echo -e "${GREEN}✓ Found: glad.h${NC}"
fi

if [ ! -f "external/glad/include/KHR/khrplatform.h" ]; then
    echo -e "${RED}✗ Missing: external/glad/include/KHR/khrplatform.h${NC}"
    MISSING=1
else
    echo -e "${GREEN}✓ Found: khrplatform.h${NC}"
fi

if [ ! -f "external/glad/src/glad.c" ]; then
    echo -e "${RED}✗ Missing: external/glad/src/glad.c${NC}"
    MISSING=1
else
    echo -e "${GREEN}✓ Found: glad.c${NC}"
fi

echo ""

if [ $MISSING -eq 1 ]; then
    echo -e "${RED}Installation incomplete!${NC}"
    echo "Please manually copy GLAD files to the correct locations."
    echo "See SETUP.md for detailed instructions."
    exit 1
else
    echo -e "${GREEN}=========================================="
    echo "  ✓ GLAD Installation Complete!"
    echo "==========================================${NC}"
    echo ""
    echo "Next steps:"
    echo "1. mkdir build && cd build"
    echo "2. cmake .."
    echo "3. make"
    echo "4. ./BoidsSimulation"
    echo ""
fi
