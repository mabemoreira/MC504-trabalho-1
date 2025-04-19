cmake_minimum_required(VERSION 3.10)
project(MC504Trabalho1)

# Configuração para arquivos C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -pthread")

# Define o diretório de saída para os executáveis
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/exec)

# Adiciona um executável para cada arquivo C na pasta src
file(GLOB SRC_FILES src/*.c)
foreach(SRC_FILE ${SRC_FILES})
    # Obtém o nome do arquivo sem o caminho e extensão
    get_filename_component(EXEC_NAME ${SRC_FILE} NAME_WE)
    add_executable(${EXEC_NAME} ${SRC_FILE})
    set_target_properties(${EXEC_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach()

# Configuração para o arquivo C++ com SFML
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -pthread")

# Encontra a biblioteca SFML
find_package(SFML 2.5 COMPONENTS system window graphics network REQUIRED)

# Adiciona o executável para o arquivo C++ na pasta interface
add_executable(sfml_app interface/sfml_app.cpp)
set_target_properties(sfml_app PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Vincula o executável à biblioteca SFML
target_link_libraries(sfml_app sfml-system sfml-window sfml-graphics sfml-network)