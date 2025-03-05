cmake_minimum_required(VERSION 3.28)
project(CG_MidTerm)

set(CMAKE_CXX_STANDARD 17)

add_executable(CG_MidTerm main.cpp InitShader.cpp glad.c)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(CG_MidTerm ${PROJECT_SOURCE_DIR}/lib/glfw3.dll winmm)

