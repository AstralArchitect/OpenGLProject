@echo off

echo Telechargement de GLFW-3.4...
wget https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip --quiet

echo Extraction de l'archive...
unzip -q glfw-3.4.zip
del glfw-3.4.zip

rem Vérifie si cmake est déjà installé
where cmake >nul 2>&1
if %errorlevel% equ 0 (
    echo Telechargement de CMake...
    wget https://github.com/Kitware/CMake/releases/download/v3.29.3/cmake-3.29.3-windows-x86_64.msi --quiet

    echo Installation de CMake...
    msiexec /i cmake-3.29.3-windows-x86_64.msi /quiet /qn /norestart

    del cmake-3.29.3-windows-x86_64.msi

    rem Recharge le PATH
    powershell -Command "RefreshEnv"
)

cd glfw-3.4
mkdir build
cd build

rem Configure CMake pour générer glfw3.dll
cmake -DBUILD_SHARED_LIBS=ON -G "Unix Makefiles" ..

cmake --build .

rem Copier glfw3.dll dans le dossier source
copy src\glfw3.dll ..\..\

cd ..\..\

rmdir /s /q glfw-3.4