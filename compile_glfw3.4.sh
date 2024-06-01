#! /usr/bin/bash

echo "Téléchargement de GLFW-3.4..."
wget "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip" --quiet

echo "Extraction de l'archive..."
unzip -q glfw-3.4.zip
rm glfw-3.4.zip

# Vérifie si cmake est déjà installé
if command -v cmake &> /dev/null
then
    echo "CMake est déjà installé."
else
    echo "Téléchargement de CMake..."
    wget https://github.com/Kitware/CMake/releases/download/v3.29.3/cmake-3.29.3-linux-x86_64.sh --quiet

    echo "Installation de CMake..."
    chmod +x cmake-3.29.3-linux-x86_64.sh
    sudo ./cmake-3.29.3-linux-x86_64.sh --skip-license --prefix=/usr/local

    rm cmake-3.29.3-linux-x86_64.sh

    # Recharge le PATH
    source ~/.bashrc
fi

cd glfw-3.4
mkdir build
cd build

# Configure CMake pour générer glfw3.so (équivalent de dll sur Linux)
cmake -DBUILD_SHARED_LIBS=ON ..

cmake --build .

# Copier glfw3.so dans le dossier source
cp src/libglfw.so.3 ../../lib/

cd ../..

cp lib/libglfw.so.3 lib/libglfw.so

rm -rf glfw-3.4