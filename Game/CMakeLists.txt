SET(GAME_TARGET_NAME Game)

find_package(GLEW REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)

#configure_file(moche2.bmp ${CMAKE_CURRENT_BINARY_DIR}/Debug/moche2.bmp COPYONLY)

add_executable(${GAME_TARGET_NAME}
        main.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ProceduralGeneration)

target_link_libraries(${GAME_TARGET_NAME}
        PUBLIC ProceduralGeneration
        PRIVATE GLEW::GLEW
        PRIVATE imgui::imgui
        PRIVATE ImGui-SFML::ImGui-SFML
        PRIVATE sfml-system sfml-network sfml-graphics sfml-window
)