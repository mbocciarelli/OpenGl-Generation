SET(GAME_TARGET_NAME Game)

find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

#configure_file(moche2.bmp ${CMAKE_CURRENT_BINARY_DIR}/Debug/moche2.bmp COPYONLY)

# Add all the .cpp and .h files in all directories recursively to the sources variable
file(GLOB_RECURSE sources
    "*.cpp"
    "*.h"
)

# Add all the files in the assets folder to the assets variable
file(COPY ${CMAKE_CURRENT_LIST_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)

# Add the sources to the executable
add_executable(${GAME_TARGET_NAME} ${sources})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ProceduralGeneration)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${GAME_TARGET_NAME} PRIVATE imgui::imgui)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${GAME_TARGET_NAME} PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${GAME_TARGET_NAME} PRIVATE glm::glm)

find_package(GLEW REQUIRED)
target_link_libraries(${GAME_TARGET_NAME} PRIVATE GLEW::GLEW)

find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
target_include_directories(${GAME_TARGET_NAME} PRIVATE ${STB_INCLUDE_DIRS})

target_link_libraries(${GAME_TARGET_NAME} PUBLIC ProceduralGeneration)