cmake_minimum_required(VERSION 3.10)
project(pano-view)

set(CMAKE_CXX_STANDARD 11)
# set(GL_LIBS
#     "/usr/lib/aarch64-linux-gnu/libGL.so"
#     "/usr/lib/aarch64-linux-gnu/libGLU.so"
#     "/usr/lib/aarch64-linux-gnu/libglut.so"
# )

# OpenGL
find_package(OpenGL REQUIRED)
find_package(GLU REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLU_INCLUDE_DIRS})

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB_RECURSE My_Source_Files ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${My_Source_Files})

target_link_libraries(${PROJECT_NAME}
    -lpthread -lm
    ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLU_LIBRARIES}
    ${OpenCV_LIBS}
)
