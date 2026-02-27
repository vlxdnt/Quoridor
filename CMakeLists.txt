cmake_minimum_required(VERSION 4.1.2)
project(Quoridor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)

set(SOURCE_FILES
        main.cpp
        src/ComputerAI.cpp
        src/GameLogic.cpp
        src/GameRender.cpp
        src/Menus.cpp
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

add_executable(Quoridor ${SOURCE_FILES})

target_include_directories(Quoridor PRIVATE includes)

target_link_libraries(Quoridor PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)
