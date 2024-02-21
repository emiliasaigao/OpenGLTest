cmake_minimum_required(VERSION 3.12)

project(OpenGLTest)

# 设置源文件
set(SOURCES src/Application.cpp)

# 将 include 目录添加到头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR}/includes)

# 将 lib 目录添加到库搜索路径
link_directories(${CMAKE_SOURCE_DIR}/lib)

# 声明可执行文件目标
add_executable(opengltest ${SOURCES} src/Camera.cpp src/FrameBuffer.cpp src/glad.c src/Mesh.cpp src/Model.cpp src/Shader.cpp src/Texture.cpp)

target_include_directories(opengltest PRIVATE src)

# 复制资源文件到生成的可执行文件目录
file(COPY ${CMAKE_SOURCE_DIR}/Source DESTINATION ${CMAKE_BINARY_DIR})

# 链接库到可执行文件
target_link_libraries(opengltest 
    ${CMAKE_SOURCE_DIR}/lib/assimp-vc140-mt.lib
    ${CMAKE_SOURCE_DIR}/lib/glfw3.lib  
)

add_custom_command(TARGET opengltest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/lib/assimp-vc140-mt.dll"
    $<TARGET_FILE_DIR:opengltest>)