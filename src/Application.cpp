#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "FrameBuffer.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include"stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#define sprintf sprintf_s
#include"stb_image_write.h"

GLFWwindow* Init(int width, int height, std::string windowName = "渲染窗口");
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window);

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f) {
    return a + f * (b - a);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int load_cubeMap(const vector<std::string>& faces);
void saveCubeMapFaceToImage(GLenum target, int width, int height, const std::string& filename);
void renderSphere();
void renderCube();

// 屏幕宽高
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 主摄像机
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
Camera mainCamera(cameraPos);

bool firstMouse = true;
float lastX = 800.f / 2.0;
float lastY = 600.f / 2.0;

float deltaTime = 0.0f;	
float preTime = 0.0f;

// 某些测试设置了↑↓键对一些效果进行开关
bool flag = false;

void matrixTest() {
    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 顶点数组
    //float vertices[] = {
    //   //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
    //        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
    //        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
    //        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
    //        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
    //};
    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    /**************************************************************/
    float firstTriangle[] = {
       -0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // left 
       0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // right
       0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f // top 
    };
    float secondTriangle[] = {
        0.0f, -0.5f, 0.0f,  // left
        0.9f, -0.5f, 0.0f,  // right
        0.45f, 0.5f, 0.0f   // top 
    };

    unsigned int VAOS[2];
    unsigned int VBOS[2];
    glGenVertexArrays(2, VAOS);
    glGenBuffers(2, VBOS);

    glBindVertexArray(VAOS[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAOS[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    /**************************************************************/


    unsigned int indices[] = {
        0,1,3,
        1,2,3
    };

    // 顶点数组对象
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 顶点缓冲对象
    unsigned int VBO;
    glGenBuffers(1, &VBO); // GPU创建缓存，返回序号给VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // 将指定位置的GPU缓存绑定为arraybuffer类型
    // 向当前arraybuffer写入数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 元素缓冲对象
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 链接顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    //glEnableVertexAttribArray(2);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Shader myshader("Source/shader/texVertexShader.glsl", "Source/shader/texFragmentShader.glsl");
    
    stbi_set_flip_vertically_on_load(true);
    // 加载纹理
        // 生成纹理
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
        // 为当前绑定纹理设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // x轴超出部分重复映射
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // y轴超出部分重复映射
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
        // 从文件中读数据
    unsigned char* data = stbi_load("Source/texture/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        // 把纹理数据导入当前绑定的纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // 为纹理生成mipmap
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "加载纹理失败！" << std::endl;
    }
    stbi_image_free(data);

    // 生成纹理
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 为当前绑定纹理设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // x轴超出部分重复映射
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // y轴超出部分重复映射
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 从文件中读数据
    unsigned char* data1 = stbi_load("Source/texture/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data1) {
        // 把纹理数据导入当前绑定的纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
        // 为纹理生成mipmap
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "加载纹理失败！" << std::endl;
    }
    stbi_image_free(data1);
    

    float mixvalue = 0.2;
    float speed = 1.f;
    float cameraSpeed = 5.f;
    myshader.use();
    myshader.setInt("uTexture1", 0);
    myshader.setInt("uTexture2", 1);

    
    

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;
        // 检测用户是否输入esc，若是则关闭页面
        processInput(window);
        //glm::mat4 trans(1.0f);
        //trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 1.0f));
        //trans = glm::rotate(trans, (float)glfwGetTime() * speed , glm::vec3(0.f, 0.f, -1.f));
        glm::mat4 model(1.f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.f, 0.f));
        glm::mat4 view(1.f);
        view = mainCamera.getViewMatrix();
        glm::mat4 projection(1.f);
        projection = glm::perspective(glm::radians(mainCamera.Zoom), float(8.f / 6.f), 0.1f, 100.f);

        myshader.setMat4f("model",glm::value_ptr(model));
        myshader.setMat4f("view",glm::value_ptr(view));
        myshader.setMat4f("projection",glm::value_ptr(projection));

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            mainCamera.processKeyBoard(Camera_Movement::FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            mainCamera.processKeyBoard(Camera_Movement::BACK, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            mainCamera.processKeyBoard(Camera_Movement::LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            mainCamera.processKeyBoard(Camera_Movement::RIGHT, deltaTime);
            


        if (mixvalue <= 1.0f && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            mixvalue += 0.001f;
        if (mixvalue >= 0.0f && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            mixvalue -= 0.001f;
        if (speed <= 10.f && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            speed += 0.001f;
        if (speed >= 1.f && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            speed -= 0.001f;
        myshader.setFloat("mixvalue", mixvalue);
        /* Render here */
        //auto time = glfwGetTime();
        //float timeSinValue = sin(time);
        //float timeCosValue = cos(time);
        //myshader.setFloat("timeSinValue", timeSinValue);
        //myshader.setFloat("timeCosValue", timeCosValue);
        glClearColor(0.1f, 0.2f, 0.44f, 0.2f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glBindVertexArray(VAOS[0]);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //glBindVertexArray(VAOS[1]);
        //glDrawArrays(GL_TRIANGLES, 0, 3);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwTerminate();
}

// BlinPhong光照模型+模型读入
void BlinPhongTest() {
    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;

    glEnable(GL_DEPTH_TEST); // 开启深度测试
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 鼠标在屏幕范围内的时候不显示

    float vertices[] = {
         //positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };


    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Shader lightShader("Source/shader/lightVertexShader.glsl", "Source/shader/lightFragmentShader.glsl");
    Shader BPShader("Source/shader/BlinPhongVertexShader.glsl", "Source/shader/BlinPhongFragmentShader.glsl");
    Texture diff("Source/texture/container_diff.png");
    Texture spec("Source/texture/container_spec.png");

    BPShader.use();
    BPShader.setInt("meterial.diffuse", 0);
    BPShader.setInt("meterial.specular", 1);

    Model myModel("Source/model/seele/seele.fbx");
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;

        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        BPShader.use();
        glm::mat4 View(1.f);
        View = mainCamera.getViewMatrix();
        glm::mat4 Projection(1.f);
        Projection = glm::perspective(glm::radians(mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);
        BPShader.setMat4f("uView", glm::value_ptr(View));
        BPShader.setMat4f("uProjection", glm::value_ptr(Projection));
        glm::mat4 Model(1.f);
        BPShader.setMat4f("uModel", glm::value_ptr(Model));
        
        BPShader.setVec3f("uCameraPos", glm::value_ptr(mainCamera.Position));
        BPShader.setFloat("meterial.shininess", 32);
        
        for (int i = 0; i < 4; ++i) {
            std::string str = std::string("pLight[") + std::to_string(i);
            BPShader.setVec3f(str+std::string("].position"), glm::value_ptr(pointLightPositions[0]));
            BPShader.setVec3f(str+std::string("].diffuse"), 0.8f, 0.8f, 0.8f);
            BPShader.setVec3f(str+std::string("].specular"), 1.0f, 1.0f, 1.0f);
            BPShader.setFloat(str+std::string("].Kc"), 1.0f);
            BPShader.setFloat(str+std::string("].Kl"),0.09f);
            BPShader.setFloat(str+std::string("].Kq"), 0.032f);
            BPShader.setFloat(str+std::string("].intensity"), 1.f);
        }

        BPShader.setVec3f("dLight.diffuse", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("dLight.specular", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("dLight.direction", -0.2f, -1.0f, -0.3f);
        BPShader.setFloat("dLight.intensity", 1.f);
        BPShader.setVec3f("fLight.position", glm::value_ptr(mainCamera.Position));
        BPShader.setVec3f("fLight.diffuse", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("fLight.specular", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("fLight.direction", glm::value_ptr(mainCamera.Front));
        BPShader.setFloat("fLight.intensity", 1.f);
        BPShader.setFloat("fLight.cutOff", glm::cos(glm::radians(12.5f)));
        BPShader.setFloat("fLight.outerCutOff", glm::cos(glm::radians(17.5f)));
        BPShader.setFloat("fLight.Kc", 1.f);
        BPShader.setFloat("fLight.Kl", 0.09f);
        BPShader.setFloat("fLight.Kq", 0.032f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diff.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, spec.ID);

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            glm::mat3 normModel = glm::mat3(model);
            normModel = glm::transpose(glm::inverse(normModel));
            BPShader.setMat3f("uNormModel", glm::value_ptr(normModel));
            BPShader.setMat4f("uModel", glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        Model = glm::scale(Model, glm::vec3(0.5f, 0.5f, 0.5f));
        Model = glm::translate(Model, glm::vec3(0.f, 100.f, 2.f));
        Model = glm::rotate(Model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        glm::mat3 normModel = glm::mat3(Model);
        normModel = glm::transpose(glm::inverse(normModel));
        BPShader.setMat3f("uNormModel", glm::value_ptr(normModel));
        BPShader.setMat4f("uModel", glm::value_ptr(Model));
        myModel.Draw(BPShader);
         glDrawArrays(GL_TRIANGLES, 0, 36);

        lightShader.use();
        lightShader.setMat4f("uView", glm::value_ptr(View));
        lightShader.setMat4f("uProjection", glm::value_ptr(Projection));
        Model = glm::translate(Model, lightPos);
        Model = glm::scale(Model, glm::vec3(0.1f));
         glm::mat4 rotate_Matrix = glm::rotate(glm::mat4(1.0f), 0.00005f, glm::vec3(0.0, 1.0, 0.0));
         Model = rotate_Matrix * Model;
         lightPos = glm::vec3(rotate_Matrix * glm::vec4(lightPos,1.0));
         lightShader.setMat4f("uModel", glm::value_ptr(Model));
        
        glBindVertexArray(lightVAO);
        for (int i = 0; i < 4; ++i) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightShader.setMat4f("uModel", glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
}

// 模板测试实现描边效果
void stencilTest() {
    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE); // 背面剔除 
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 鼠标在屏幕范围内的时候不显示

    Shader BPShader("Source/shader/BlinPhongVertexShader.glsl", "Source/shader/BlinPhongFragmentShader.glsl");
    Shader OutLineShader("Source/shader/outLineVertexShader.glsl", "Source/shader/outLineFragmentShader.glsl");
    Model myModel("Source/model/seele/seele.fbx");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;

        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 View(1.f);
        View = mainCamera.getViewMatrix();
        glm::mat4 Projection(1.f);
        Projection = glm::perspective(glm::radians(mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);
        OutLineShader.use();
        OutLineShader.setMat4f("uView", glm::value_ptr(View));
        OutLineShader.setMat4f("uProjection", glm::value_ptr(Projection));

        BPShader.use();
        BPShader.setMat4f("uView", glm::value_ptr(View));
        BPShader.setMat4f("uProjection", glm::value_ptr(Projection));

        glm::mat4 Model(1.f);
        BPShader.setMat4f("uModel", glm::value_ptr(Model));

        BPShader.setVec3f("uCameraPos", glm::value_ptr(mainCamera.Position));
        BPShader.setFloat("meterial.shininess", 32);

        BPShader.setVec3f("dLight.diffuse", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("dLight.specular", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("dLight.direction", 0.f, 1.0f, 1.f);
        BPShader.setFloat("dLight.intensity", 1.f);
        BPShader.setVec3f("fLight.position", glm::value_ptr(mainCamera.Position));
        BPShader.setVec3f("fLight.diffuse", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("fLight.specular", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("fLight.direction", glm::value_ptr(mainCamera.Front));
        BPShader.setFloat("fLight.intensity", 0.f);
        BPShader.setFloat("fLight.cutOff", glm::cos(glm::radians(12.5f)));
        BPShader.setFloat("fLight.outerCutOff", glm::cos(glm::radians(17.5f)));
        BPShader.setFloat("fLight.Kc", 1.f);
        BPShader.setFloat("fLight.Kl", 0.09f);
        BPShader.setFloat("fLight.Kq", 0.032f);

        glm::mat4 M(1.0f);
        M = glm::scale(M, glm::vec3(0.5f, 0.5f, 0.5f));
        M = glm::translate(M, glm::vec3(0.f, 0.f, 2.f));
        M = glm::rotate(M, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        glm::mat3 normModel = glm::mat3(M);
        normModel = glm::transpose(glm::inverse(normModel));
        BPShader.setMat3f("uNormModel", glm::value_ptr(normModel));
        BPShader.setMat4f("uModel", glm::value_ptr(M));

        // 第一趟渲染，正常绘制，并更新模板缓冲
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        myModel.Draw(BPShader);

        // 第二趟渲染，禁用模板缓冲，在模板缓冲不为1的地方绘制outline
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); // 禁止模板缓冲的写入
        glDisable(GL_DEPTH_TEST);
        float outLine = 0.005f;
        OutLineShader.use();
        OutLineShader.setFloat("uOutLine", outLine);
        OutLineShader.setMat4f("uModel", glm::value_ptr(M));
        myModel.Draw(OutLineShader);
        glStencilMask(0xFF);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

// 帧缓冲测试
void frameBufferTest() {
    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;
    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 鼠标在屏幕范围内的时候不显示

    vector<vector<float>> quadVertices(4,vector<float>());
    quadVertices[0] = {
        // 左上角第一个窗口（两个三角形组成的矩形）
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  1.0f,  1.0f, 1.0f
    };

    
    quadVertices[1] = {
        // 左边第二个窗口（两个三角形组成的矩形）
        // positions   // texCoords
        -1.0f,  0.5f,  0.0f, 1.0f,
        -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.0f,  1.0f, 0.0f,

        -1.0f,  0.5f,  0.0f, 1.0f,
        -0.5f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  1.0f, 1.0f
    };

    quadVertices[2] = {
        // 左边第三个窗口（两个三角形组成的矩形）
        // positions   // texCoords
        -1.0f,  0.0f,  0.0f, 1.0f,
        -1.0f, -0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f,  1.0f, 0.0f,

        -1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  1.0f, 0.0f,
        -0.5f,  0.0f,  1.0f, 1.0f
    };

    quadVertices[3] = {
        // 左边第三个窗口（两个三角形组成的矩形）
        // positions   // texCoords
        -1.0f, -0.5f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        -0.5f, -1.0f,  1.0f, 0.0f,

        -1.0f, -0.5f,  0.0f, 1.0f,
        -0.5f, -1.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  1.0f, 1.0f
    };

    unsigned int VAO[4];
    unsigned int VBO[4];
    glGenVertexArrays(4, VAO);
    glGenBuffers(4, VBO);
    for (int i = 0; i < 4; ++i) {
        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * quadVertices[i].size(), &quadVertices[i][0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)(2 * sizeof(float)));
    }


    Shader BPShader("Source/shader/BlinPhongVertexShader.glsl", "Source/shader/BlinPhongFragmentShader.glsl");
    Model myModel("Source/model/seele/seele.fbx");

    FrameBuffer frameBuffer(SCR_WIDTH, SCR_HEIGHT);

    Shader inversionShader("Source/shader/simpleVertexShader.glsl", "Source/shader/inversionFragmentShader.glsl");
    Shader grayscaleShader("Source/shader/simpleVertexShader.glsl", "Source/shader/grayscaleFragmentShader.glsl");
    Shader kernelShader("Source/shader/simpleVertexShader.glsl", "Source/shader/kernelFragmentShader.glsl");
    Shader edgeDetectionShader("Source/shader/simpleVertexShader.glsl", "Source/shader/edgeDetectionFragmentShader.glsl");
    vector<Shader*> shaders = { &inversionShader, &grayscaleShader, &kernelShader, &edgeDetectionShader };

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;

        processInput(window);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 View(1.f);
        View = mainCamera.getViewMatrix();
        glm::mat4 Projection(1.f);
        Projection = glm::perspective(glm::radians(mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);

        BPShader.use();
        BPShader.setMat4f("uView", glm::value_ptr(View));
        BPShader.setMat4f("uProjection", glm::value_ptr(Projection));

        glm::mat4 Model(1.f);
        BPShader.setMat4f("uModel", glm::value_ptr(Model));

        BPShader.setVec3f("uCameraPos", glm::value_ptr(mainCamera.Position));
        BPShader.setFloat("meterial.shininess", 32);

        BPShader.setVec3f("dLight.diffuse", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("dLight.specular", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("dLight.direction", 0.f, 1.0f, 1.f);
        BPShader.setFloat("dLight.intensity", 1.f);
        BPShader.setVec3f("fLight.position", glm::value_ptr(mainCamera.Position));
        BPShader.setVec3f("fLight.diffuse", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("fLight.specular", 1.0f, 1.0f, 1.0f);
        BPShader.setVec3f("fLight.direction", glm::value_ptr(mainCamera.Front));
        BPShader.setFloat("fLight.intensity", 0.f);
        BPShader.setFloat("fLight.cutOff", glm::cos(glm::radians(12.5f)));
        BPShader.setFloat("fLight.outerCutOff", glm::cos(glm::radians(17.5f)));
        BPShader.setFloat("fLight.Kc", 1.f);
        BPShader.setFloat("fLight.Kl", 0.09f);
        BPShader.setFloat("fLight.Kq", 0.032f);

        glm::mat4 M(1.0f);
        M = glm::scale(M, glm::vec3(0.5f, 0.5f, 0.5f));
        M = glm::translate(M, glm::vec3(0.f, 0.f, 2.f));
        M = glm::rotate(M, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        glm::mat3 normModel = glm::mat3(M);
        normModel = glm::transpose(glm::inverse(normModel));
        BPShader.setMat3f("uNormModel", glm::value_ptr(normModel));
        BPShader.setMat4f("uModel", glm::value_ptr(M));

        myModel.Draw(BPShader);

        // 切换帧缓冲对象，在这上面再画一次
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.ID);
        glEnable(GL_DEPTH_TEST); 
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        BPShader.use();
        myModel.Draw(BPShader);
        glBindVertexArray(0);

        // 切换回默认帧缓冲，关掉深度缓冲，把窗口画在最前面
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        
        for (int i = 0; i < 4; ++i) {
            shaders[i]->use();
            glBindVertexArray(VAO[i]);
            glBindTexture(GL_TEXTURE_2D, frameBuffer.textureID);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

// 天空盒测试 + 几何着色器测试
void geoShaderTest(void) {
    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;
    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 鼠标在屏幕范围内的时候不显示
    
    vector<std::string> faces(6, "Source/skybox/sea/");
    faces[0] += "left.jpg";
    faces[1] += "right.jpg";
    faces[2] += "top1.jpg";
    faces[3] += "bottom1.jpg";
    faces[4] += "back.jpg";
    faces[5] += "front.jpg";
    stbi_set_flip_vertically_on_load(false);
    unsigned int cubeMapID = load_cubeMap(faces);

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    Shader myModelShader("Source/shader/myModelVertexShader.glsl", "Source/shader/myModelFragmentShader.glsl");
    Model myModel("Source/model/nanosuit_reflection/nanosuit.obj");
    Shader skyboxShader("Source/shader/skyboxVertexShader.glsl", "Source/shader/skyboxFragmentShader.glsl");
    Shader geoShader("Source/shader/geometry/geoTestVertexShader.glsl",
        "Source/shader/geometry/geoTestFragmentShader.glsl",
        "Source/shader/geometry/geoTestGeometryShader.glsl");
    Shader explosionShader("Source/shader/geometry/explosionVertexShader.glsl",
        "Source/shader/geometry/explosionFragmentShader.glsl",
        "Source/shader/geometry/explosionGeometryShader.glsl");

    Texture cubeTexture("Source/texture/container.jpg");

    skyboxShader.use();
    skyboxShader.setInt("skybox", 4);
    myModelShader.use();
    myModelShader.setInt("uCubeMap", 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;

        processInput(window);
        glEnable(GL_DEPTH_TEST);
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 View(1.f);
        View = mainCamera.getViewMatrix();
        glm::mat4 Projection(1.f);
        Projection = glm::perspective(glm::radians(mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);
        glm::mat4 Model(1.f);
        Model = glm::scale(Model, glm::vec3(0.2, 0.2, 0.2));
        myModelShader.use();
        myModelShader.setVec3f("uCameraPos", glm::value_ptr(mainCamera.Position));
        myModelShader.setMat4f("uModel", glm::value_ptr(Model));
        glm::mat3 normModel = glm::mat3(glm::transpose(glm::inverse(Model)));
        myModelShader.setMat3f("uNormModel", glm::value_ptr(normModel));
        myModelShader.setMat4f("uView", glm::value_ptr(View));
        myModelShader.setMat4f("uProjection", glm::value_ptr(Projection));
        myModel.Draw(myModelShader);

        geoShader.use();
        geoShader.setVec3f("uCameraPos", glm::value_ptr(mainCamera.Position));
        geoShader.setMat4f("uModel", glm::value_ptr(Model));
        geoShader.setMat3f("uNormModel", glm::value_ptr(normModel));
        geoShader.setMat4f("uView", glm::value_ptr(View));
        geoShader.setMat4f("uProjection", glm::value_ptr(Projection));
        myModel.Draw(geoShader);

        Model = glm::translate(Model, glm::vec3(10.f, 0.f, 0.f));
        normModel = glm::mat3(glm::transpose(glm::inverse(Model)));
        explosionShader.use();
        explosionShader.setFloat("time", currentFrame);
        explosionShader.setMat4f("uModel", glm::value_ptr(Model));
        explosionShader.setMat3f("uNormModel", glm::value_ptr(normModel));
        explosionShader.setMat4f("uView", glm::value_ptr(View));
        explosionShader.setMat4f("uProjection", glm::value_ptr(Projection));
        myModel.Draw(explosionShader);

        skyboxShader.use();
        glm::mat4 view = glm::mat4(glm::mat3(View)); // remove translation from the view matrix
        skyboxShader.setMat4f("uView", glm::value_ptr(view));
        skyboxShader.setMat4f("uProjection", glm::value_ptr(Projection));



        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
}

// 实例化测试
void instancingTest() {
    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;
    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 鼠标在屏幕范围内的时候不显示

    vector<std::string> faces(6, "Source/skybox/sea/");
    faces[0] += "left.jpg";
    faces[1] += "right.jpg";
    faces[2] += "top1.jpg";
    faces[3] += "bottom1.jpg";
    faces[4] += "back.jpg";
    faces[5] += "front.jpg";
    stbi_set_flip_vertically_on_load(false);
    unsigned int cubeMapID = load_cubeMap(faces);

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    Shader skyboxShader("Source/shader/skyboxVertexShader.glsl", "Source/shader/skyboxFragmentShader.glsl");
    Model planet("Source/model/planet/planet.obj");
    Model rock("Source/model/rock/rock.obj");

    Texture cubeTexture("Source/texture/container.jpg");
    Shader modelShader("Source/shader/myModelVertexShader.glsl", "Source/shader/myModelFragmentShader.glsl");
    Shader instanceShader("Source/shader/instanceVertexShader.glsl", "Source/shader/myModelFragmentShader.glsl");

    unsigned int amount = 1000;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // 初始化随机种子    
    float radius = 50.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model(1.0f);
        // 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // 让行星带的高度比x和z的宽度要小
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. 缩放：在 0.05 和 0.25f 之间缩放
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. 添加到矩阵的数组中
        modelMatrices[i] = model;
    }

    unsigned int instanceBuffer;
    glGenBuffers(1, &instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
    for (unsigned int i = 0; i < rock.meshes.size(); ++i) {
        unsigned int VAO = rock.meshes[i].VAO;
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }

    skyboxShader.use();
    skyboxShader.setInt("skybox", 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;

        processInput(window);
        glEnable(GL_DEPTH_TEST);
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 View(1.f);
        View = mainCamera.getViewMatrix();
        glm::mat4 Projection(1.f);
        Projection = glm::perspective(glm::radians(mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);
        glm::mat4 Model(1.f);
        modelShader.use();
        modelShader.setMat4f("uView", glm::value_ptr(View));
        modelShader.setMat4f("uProjection", glm::value_ptr(Projection));
        
        Model = glm::translate(Model, glm::vec3(0.0f, -3.0f, 0.0f));
        Model = glm::scale(Model, glm::vec3(4.0f, 4.0f, 4.0f));
        modelShader.setMat4f("uModel", glm::value_ptr(Model));
        planet.Draw(modelShader);

        // 绘制小行星
        instanceShader.use();
        instanceShader.setMat4f("uView", glm::value_ptr(View));
        instanceShader.setMat4f("uProjection", glm::value_ptr(Projection));
        instanceShader.setInt("meterial.texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].ID);
        for (unsigned int i = 0; i < rock.meshes.size(); i++)
        {
            glBindVertexArray(rock.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
            //glBindVertexArray(0);
        }
        
        skyboxShader.use();
        glm::mat4 view = glm::mat4(glm::mat3(View)); // remove translation from the view matrix
        skyboxShader.setMat4f("uView", glm::value_ptr(view));
        skyboxShader.setMat4f("uProjection", glm::value_ptr(Projection));


        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    delete[] modelMatrices;

    glfwTerminate();

}

// 阴影技术测试
void shadowTest(void) {
    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;
    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 鼠标在屏幕范围内的时候不显示

    float lightcube[] = {
         //positions         
        -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        
        -0.5f, -0.5f,  0.5f, 
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 
        
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 
        
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        
        -0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 
        
        -0.5f,  0.5f, -0.5f, 
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f
    };
    unsigned int lightVAO, lightVBO;
    glGenBuffers(1, &lightVBO);
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightcube), lightcube, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVBO, planeVAO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // 平台贴图
    Texture planeTexture("Source/texture/metal.png");
    Model seele("Source/model/seele/seele.fbx");

    const unsigned int shadowMapSize = 4096;
    FrameBuffer shadowFrameBuffer(shadowMapSize, shadowMapSize, true);

    float quadVertices[] = {
        // 左上角第一个窗口（两个三角形组成的矩形）
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  1.0f,  1.0f, 1.0f
    };
    unsigned int VAO,VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    Shader depthMapShader("Source/shader/simpleVertexShader.glsl", "Source/shader/simpleFragmentShader.glsl");
    depthMapShader.use();
    depthMapShader.setInt("uDepthMap", 4);
    Shader shadowMapShader("Source/shader/shadow/shadowMapVertexShader.glsl", "Source/shader/shadow/shadowMapFragmentShader.glsl");
    Shader shadowShader("Source/shader/shadow/shadowVertexShader.glsl", "Source/shader/shadow/shadowFragmentShader.glsl");
    shadowShader.use();
    shadowShader.setInt("uShadowMap", 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadowFrameBuffer.textureID);

    Shader modelShader("Source/shader/myModelVertexShader.glsl", "Source/shader/myModelFragmentShader.glsl");
    Shader lightCubeShader("Source/shader/lightVertexShader.glsl", "Source/shader/lightFragmentShader.glsl");
    Shader instanceShader("Source/shader/instanceVertexShader.glsl", "Source/shader/myModelFragmentShader.glsl");


    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;

        processInput(window);
        glEnable(GL_DEPTH_TEST);

        glViewport(0, 0, shadowMapSize, shadowMapSize);
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer.ID);
        glClear(GL_DEPTH_BUFFER_BIT);

        float n_plane = 0.1f;
        float f_plane = 100.f;
        auto lightProjection = glm::ortho(-100.f, 100.f, -100.f, 100.f, n_plane, f_plane);
        auto lightPos = glm::vec3(-2.0 + 5 * glm::sin(glm::radians(glfwGetTime() * 20)), 7.0, 7.0);
        auto lightView = glm::lookAt(lightPos, glm::vec3(0., 0., 0.), glm::vec3(0., 1., 0.));
        shadowMapShader.use();
        shadowMapShader.setMat4f("uView", glm::value_ptr(lightView));
        shadowMapShader.setMat4f("uProjection", glm::value_ptr(lightProjection));
        shadowMapShader.setVec3f("uLightPos", glm::value_ptr(lightPos));

        glm::mat4 Model(1.f);
        shadowMapShader.use();
        shadowMapShader.setMat4f("uModel", glm::value_ptr(Model));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glm::mat4 M(1.0f);
        M = glm::scale(M, glm::vec3(3.f, 3.f, 3.f));
        M = glm::translate(M, glm::vec3(0.f, -0.16f, 0.f));
        M = glm::rotate(M, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        M = glm::rotate(M, (float)glm::radians(glfwGetTime()*10), glm::vec3(0., 0., 1.));

        shadowMapShader.setMat4f("uModel", glm::value_ptr(M));
        seele.Draw(shadowMapShader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 View = mainCamera.getViewMatrix();
        shadowShader.use();
        shadowShader.setMat4f("uView", glm::value_ptr(View));
        shadowShader.setMat4f("uModel", glm::value_ptr(Model));
        glm::mat4 Projection = glm::perspective(glm::radians(mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);
        shadowShader.setMat4f("uProjection", glm::value_ptr(Projection));
        auto normModel = glm::transpose(glm::inverse(glm::mat3(Model)));
        shadowShader.setMat3f("uNormModel", glm::value_ptr(normModel));
        auto lightMVP = lightProjection * lightView * Model;
        shadowShader.setMat4f("uLightMVP", glm::value_ptr(lightMVP));
        shadowShader.setInt("meterial.texture_diffuse1", 0);
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, planeTexture.ID);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        shadowShader.use();
        shadowShader.setMat4f("uView", glm::value_ptr(View));
        shadowShader.setMat4f("uProjection", glm::value_ptr(Projection));
        shadowShader.setMat4f("uModel", glm::value_ptr(M));
        normModel = glm::transpose(glm::inverse(glm::mat3(M)));
        shadowShader.setMat3f("uNormModel", glm::value_ptr(normModel));
        lightMVP = lightProjection * lightView * M;
        shadowShader.setMat4f("uLightMVP", glm::value_ptr(lightMVP));
        seele.Draw(shadowShader);

        
        lightCubeShader.use();
        lightCubeShader.setMat4f("uView", glm::value_ptr(View));
        lightCubeShader.setMat4f("uProjection", glm::value_ptr(Projection));
        Model = glm::translate(Model, lightPos);
        Model = glm::scale(Model, glm::vec3(0.2f,0.2f,0.2f));
        lightCubeShader.setMat4f("uModel", glm::value_ptr(Model));
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDisable(GL_DEPTH_TEST);
        
        depthMapShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

// 法线贴图测试
void normalMapTest() {
    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;
    glEnable(GL_DEPTH_TEST);
    //stbi_set_flip_vertically_on_load(true);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 鼠标在屏幕范围内的时候不显示

    float lightcube[] = {
        //positions         
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
    };
    unsigned int lightVAO, lightVBO;
    glGenBuffers(1, &lightVBO);
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightcube), lightcube, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // positions
    glm::vec3 pos1(-1.0, 1.0, 0.0);
    glm::vec3 pos2(-1.0, -1.0, 0.0);
    glm::vec3 pos3(1.0, -1.0, 0.0);
    glm::vec3 pos4(1.0, 1.0, 0.0);
    // texture coordinates
    glm::vec2 uv1(0.0, 1.0);
    glm::vec2 uv2(0.0, 0.0);
    glm::vec2 uv3(1.0, 0.0);
    glm::vec2 uv4(1.0, 1.0);
    // normal vector
    glm::vec3 nm(0.0, 0.0, 1.0);

    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;
    // - triangle 1
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);

    // - triangle 2
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent2 = glm::normalize(tangent2);


    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent2 = glm::normalize(bitangent2);


    GLfloat quadVertices[] = {
        // Positions            // normal         // TexCoords  // Tangent                          // Bitangent
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
    };
    // Setup plane VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));


    // 墙面贴图+法线贴图
    Texture wallTexture("Source/texture/brickwall.jpg");
    Texture wallNormalTex("Source/texture/brickwall_normal.jpg");

    Shader parallaxMapShader("Source/shader/normalMap/normalMapVertexShader.glsl",
        "Source/shader/normalMap/normalMapFragmentShader.glsl");
    Shader lightCubeShader("Source/shader/lightVertexShader.glsl", "Source/shader/lightFragmentShader.glsl");

    parallaxMapShader.use();
    parallaxMapShader.setInt("meterial.texture_diffuse1", 0);
    parallaxMapShader.setInt("uNormalMap", 1);
    parallaxMapShader.setFloat("meterial.shininess", 32.f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, wallTexture.ID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wallNormalTex.ID);
    // Light position
    glm::vec3 lightPos(0.5f, 1.0f, 0.3f);
    parallaxMapShader.setVec3f("uLightPos", glm::value_ptr(lightPos));
    std::string str = std::string("pLight");
    parallaxMapShader.setVec3f(str+std::string(".diffuse"), 0.8f, 0.8f, 0.8f);
    parallaxMapShader.setVec3f(str+std::string(".specular"), 1.0f, 1.0f, 1.0f);
    parallaxMapShader.setFloat(str+std::string(".Kc"), 1.0f);
    parallaxMapShader.setFloat(str+std::string(".Kl"),0.09f);
    parallaxMapShader.setFloat(str+std::string(".Kq"), 0.032f);
    parallaxMapShader.setFloat(str+std::string(".intensity"), 1.f);
  
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;

        processInput(window);
        glEnable(GL_DEPTH_TEST);


        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 View = mainCamera.getViewMatrix();
        glm::mat4 Projection = glm::perspective(glm::radians(mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);
        parallaxMapShader.use();
        parallaxMapShader.setVec3f("uCameraPos", glm::value_ptr(mainCamera.Position));
        parallaxMapShader.setMat4f("uView", glm::value_ptr(View));
        parallaxMapShader.setMat4f("uProjection", glm::value_ptr(Projection));
        glm::mat4 Model(1.0f);
        glm::mat4 M(1.0f);
        //M = glm::rotate(M, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        auto normModel = glm::transpose(glm::inverse(glm::mat3(M)));
        parallaxMapShader.setMat4f("uModel", glm::value_ptr(M));
        parallaxMapShader.setMat3f("uNormModel", glm::value_ptr(normModel));

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        lightCubeShader.use();
        lightCubeShader.setMat4f("uView", glm::value_ptr(View));
        lightCubeShader.setMat4f("uProjection", glm::value_ptr(Projection));
        Model = glm::translate(Model, lightPos);
        Model = glm::scale(Model, glm::vec3(0.2f,0.2f,0.2f));
        lightCubeShader.setMat4f("uModel", glm::value_ptr(Model));
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

}

// 视差贴图测试
void parallaxTest() {
    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;
    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 鼠标在屏幕范围内的时候不显示

    float lightcube[] = {
        //positions         
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
    };
    unsigned int lightVAO, lightVBO;
    glGenBuffers(1, &lightVBO);
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightcube), lightcube, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // positions
    glm::vec3 pos1(-1.0, 1.0, 0.0);
    glm::vec3 pos2(-1.0, -1.0, 0.0);
    glm::vec3 pos3(1.0, -1.0, 0.0);
    glm::vec3 pos4(1.0, 1.0, 0.0);
    // texture coordinates
    glm::vec2 uv1(0.0, 1.0);
    glm::vec2 uv2(0.0, 0.0);
    glm::vec2 uv3(1.0, 0.0);
    glm::vec2 uv4(1.0, 1.0);
    // normal vector
    glm::vec3 nm(0.0, 0.0, 1.0);

    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;
    // - triangle 1
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);

    // - triangle 2
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent2 = glm::normalize(tangent2);


    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent2 = glm::normalize(bitangent2);


    GLfloat quadVertices[] = {
        // Positions            // normal         // TexCoords  // Tangent                          // Bitangent
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
    };
    // Setup plane VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));


    // 墙面贴图+法线贴图
    Texture wallTexture("Source/texture/bricks2.jpg");
    Texture wallNormalTex("Source/texture/bricks2_normal.jpg");
    Texture wallDispTex("Source/texture/bricks2_disp.jpg");

    Shader parallaxMapShader("Source/shader/normalMap/normalMapVertexShader.glsl",
        "Source/shader/normalMap/parallaxMapFragmentShader.glsl");
    Shader lightCubeShader("Source/shader/lightVertexShader.glsl", "Source/shader/lightFragmentShader.glsl");

    parallaxMapShader.use();
    parallaxMapShader.setInt("uNormalMap", 0);
    parallaxMapShader.setInt("uTexture", 1);
    parallaxMapShader.setInt("uDispMap", 2);
    parallaxMapShader.setFloat("uHeightScale", 0.05f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, wallNormalTex.ID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wallTexture.ID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, wallDispTex.ID);

    // Light position
    glm::vec3 lightPos(0.5f, 1.0f, 0.3f);
    parallaxMapShader.setVec3f("uLightPos", glm::value_ptr(lightPos));

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;

        processInput(window);
        glEnable(GL_DEPTH_TEST);


        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 View = mainCamera.getViewMatrix();
        glm::mat4 Projection = glm::perspective(glm::radians(mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);
        parallaxMapShader.use();
        parallaxMapShader.setBool("parallax", flag);
        parallaxMapShader.setVec3f("uCameraPos", glm::value_ptr(mainCamera.Position));
        parallaxMapShader.setMat4f("uView", glm::value_ptr(View));
        parallaxMapShader.setMat4f("uProjection", glm::value_ptr(Projection));
        glm::mat4 Model(1.0f);
        glm::mat4 M(1.0f);
        //M = glm::rotate(M, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        auto normModel = glm::transpose(glm::inverse(glm::mat3(M)));
        parallaxMapShader.setMat4f("uModel", glm::value_ptr(M));
        parallaxMapShader.setMat3f("uNormModel", glm::value_ptr(normModel));

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        lightCubeShader.use();
        lightCubeShader.setMat4f("uView", glm::value_ptr(View));
        lightCubeShader.setMat4f("uProjection", glm::value_ptr(Projection));
        Model = glm::translate(Model, lightPos);
        Model = glm::scale(Model, glm::vec3(0.2f, 0.2f, 0.2f));
        lightCubeShader.setMat4f("uModel", glm::value_ptr(Model));
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

// SSAO测试
void SSAOTest() {
    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;
    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 鼠标在屏幕范围内的时候不显示

    float lightcube[] = {
        //positions         
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
        
    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
        
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
        
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
        
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
        
    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
    };
    unsigned int lightVAO, lightVBO;
    glGenBuffers(1, &lightVBO);
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightcube), lightcube, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    float planeVertices[] = {
        // positions            // normals         // texcoords
            25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
            -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
        
            25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
            25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVBO, planeVAO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
        
    // 平台贴图
    Texture planeTexture("Source/texture/metal.png");

    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // GBuffer帧缓冲
    unsigned int GBufferFBO;
    glGenFramebuffers(1, &GBufferFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, GBufferFBO);
    
    Texture position = Texture::CreateTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT);
    glBindTexture(GL_TEXTURE_2D, position.ID);
    // 采样点是有可能拥有超出纹理范围的纹理坐标的，如果设置成循环可能会有奇怪的效果
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position.ID, 0);

    Texture normalMap = Texture::CreateTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalMap.ID, 0);

    Texture albedoMap = Texture::CreateTexture(SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_RGB, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedoMap.ID, 0);

    // - 告诉OpenGL我们将要使用(帧缓冲的)哪种颜色附件来进行渲染
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    // 绑定渲染缓冲对象
    unsigned int depthRBO;
    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);

    /*
     *  如果已经成功地将渲染缓冲对象（RBO）附加到一个帧缓冲对象（FBO），
     *  然后想要在绑定其他的 FBO 时共享同一个 RBO，
     *  只需要在切换到其他 FBO 之前确保正确绑定 RBO 即可
     *  不需要再次调用 glFramebufferRenderbuffer 函数
    */
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "帧缓冲生成不完整！" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // SSAO帧缓冲
    unsigned int SSAOFBO;
    glGenFramebuffers(1, &SSAOFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO);

    Texture SSAOTexture = Texture::CreateTexture(SCR_WIDTH, SCR_HEIGHT, GL_RED, GL_RGB, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOTexture.ID, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "帧缓冲生成不完整！" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // SSAO模糊化帧缓冲
    unsigned int SSAOblurFBO;
    glGenFramebuffers(1, &SSAOblurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, SSAOblurFBO);

    Texture SSAOblurTexture = Texture::CreateTexture(SCR_WIDTH, SCR_HEIGHT, GL_RED, GL_RGB, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOblurTexture.ID, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "帧缓冲生成不完整！" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 随机样本点生成（切线空间）
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    vector<glm::vec3> ssaoKernel;
    for (GLuint i = 0; i < 64; ++i) {
        // xy随机采样到[-1,1]，z只能采样到[0,1]（正半球）
        // 为什么是z不是y？因为TBN矩阵N在最后，因此最后一维对应法线方向
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0,
                        randomFloats(generator) * 2.0 - 1.0, 
                        randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 64.0;

        // 集中更多采样点至片元附近
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // 噪音向量贴图，用来随机获取一个切线方向，变相旋转了样本点集
    vector<glm::vec3> ssaoNoise;
    for (GLuint i = 0; i < 16; i++) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, 
                        randomFloats(generator) * 2.0 - 1.0, 
                        0.0f); 
        ssaoNoise.push_back(noise);
    }
    GLuint noiseTexture; 
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    Shader GBufferShader("Source/shader/GBuffer/GBufferVertexShader.glsl",
        "Source/shader/GBuffer/GBufferFragmentShader.glsl");
    Shader SSAOShader("Source/shader/SSAO/SSAOVertexShader.glsl",
        "Source/shader/SSAO/SSAOFragmentShader.glsl");
    Shader SSAOblurShader("Source/shader/SSAO/SSAOblurVertexShader.glsl",
        "Source/shader/SSAO/SSAOblurFragmentShader.glsl");
    Shader myModelShader("Source/shader/DelayShade/DelayShadeVertexShader.glsl", 
        "Source/shader/DelayShade/DelayShadeFragmentShader.glsl");
    Shader outLineShader("Source/shader/outLineVertexShader.glsl", 
        "Source/shader/outLineFragmentShader.glsl");
    Model seele("Source/model/seele/seele.fbx");
    Shader debugShader("Source/shader/simpleVertexShader.glsl",
        "Source/shader/texFragmentShader.glsl");
    Shader lightCubeShader("Source/shader/lightVertexShader.glsl", 
        "Source/shader/lightFragmentShader.glsl");
    debugShader.use();
    debugShader.setInt("texture1", 0);
    SSAOShader.use();
    SSAOShader.setInt("uPosition", 0);
    SSAOShader.setInt("uNormal", 1);
    SSAOShader.setInt("uNoiseMap", 2);
    for (int i = 0; i < 64; ++i) {
        SSAOShader.setVec3f(std::string("samples[") + std::to_string(i) + std::string("]"),
            glm::value_ptr(ssaoKernel[i]));
    }
    SSAOblurShader.use();
    SSAOblurShader.setInt("uSSAOTexture", 0);
    myModelShader.use();
    myModelShader.setInt("uAlbedo", 0);
    myModelShader.setInt("uPosition", 1);
    myModelShader.setInt("uNormal", 2);
    myModelShader.setInt("uSSAOblurTexture", 3);

    glm::vec3 lightPos = glm::vec3(3., 3., 3.);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;

        processInput(window);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, GBufferFBO);
        glDepthFunc(GL_LESS);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 View = mainCamera.getViewMatrix();
        glm::mat4 Projection = glm::perspective(glm::radians(mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);
        glm::mat4 Model(1.0f);

        glm::mat4 M(1.0f);
        M = glm::rotate(M, glm::radians(-90.f), glm::vec3(1.0, 0.0, 0.0));
        auto normModel = glm::transpose(glm::inverse(glm::mat3(M)));
    
        GBufferShader.use();
        glStencilMask(0x00); // 禁止模板缓冲的写入
        GBufferShader.setMat4f("uView", glm::value_ptr(View));
        GBufferShader.setMat4f("uProjection", glm::value_ptr(Projection));
        GBufferShader.setInt("meterial.texture_diffuse1", 0);
        Model = glm::rotate(Model, glm::radians(90.f), glm::vec3(1.0, 0.0, 0.0));
        Model = glm::translate(Model, glm::vec3(0., 0.3, 0.));
        GBufferShader.setMat4f("uModel", glm::value_ptr(Model));
        glBindTexture(GL_TEXTURE_2D, planeTexture.ID);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        GBufferShader.setMat4f("uModel", glm::value_ptr(M));
        // 第一趟渲染，正常绘制，并更新模板缓冲
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        seele.Draw(GBufferShader);

        // 第二趟渲染，禁用模板缓冲，在模板缓冲不为1的地方绘制outline
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00); // 禁止模板缓冲的写入
        glDisable(GL_DEPTH_TEST);
        float outLine = 0.003f;
        outLineShader.use();
        outLineShader.setMat4f("uProjection", glm::value_ptr(Projection));
        outLineShader.setMat4f("uView", glm::value_ptr(View));
        outLineShader.setFloat("uOutLine", outLine);
        outLineShader.setMat4f("uModel", glm::value_ptr(M));
        seele.Draw(outLineShader);

        glStencilMask(0xFF);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 渲染SSAO效果
        glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SSAOShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, position.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap.ID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        SSAOShader.setMat4f("uProjection", glm::value_ptr(Projection));
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 渲染模糊的SSAO效果
        glBindFramebuffer(GL_FRAMEBUFFER, SSAOblurFBO);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SSAOblurShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SSAOTexture.ID);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 渲染模型到屏幕加上SSAO效果
        glDisable(GL_DEPTH_TEST);
        myModelShader.use();
        glm::vec4 lightCameraPos = View * glm::vec4(lightPos, 1.0);
        lightCameraPos = lightCameraPos / lightCameraPos.w;
        myModelShader.setVec3f("light.Position", lightCameraPos.x, lightCameraPos.y, lightCameraPos.z);
        myModelShader.setVec3f("light.Color", 0.5, 0.5, 0.5);
        myModelShader.setFloat("light.Linear", 0.09);
        myModelShader.setFloat("light.Quadratic", 0.032);
        myModelShader.setBool("useSSAO", flag);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedoMap.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, position.ID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normalMap.ID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, SSAOblurTexture.ID);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // DEBUG用
        //glDisable(GL_DEPTH_TEST);
        //debugShader.use();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, SSAOTexture.ID);
        //glBindVertexArray(quadVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        /*glDisable(GL_DEPTH_TEST);
        lightCubeShader.use();
        lightCubeShader.setMat4f("uView", glm::value_ptr(View));
        lightCubeShader.setMat4f("uProjection", glm::value_ptr(Projection));
        Model = glm::translate(Model, lightPos);
        Model = glm::scale(Model, glm::vec3(0.2f, 0.2f, 0.2f));
        lightCubeShader.setMat4f("uModel", glm::value_ptr(Model));
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);*/

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

}

// PBR测试
void pbrTest() {
    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;

    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 鼠标在屏幕范围内的时候不显示

    // lights
    // ------
    glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3(10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;

    Texture albedo("Source/texture/rustediron2_basecolor.png");
    Texture metallic("Source/texture/rustediron2_metallic.png");
    Texture roughness("Source/texture/rustediron2_roughness.png");
    
    Shader pbrShader("Source/shader/pbr/pbrVertexShader.glsl", 
        "Source/shader/pbr/pbrFragmentShader.glsl");
    pbrShader.use();
    pbrShader.setInt("uAlbedoMap", 0);
    pbrShader.setInt("uMetallicMap", 1);
    pbrShader.setInt("uRoughnessMap", 2);
    
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;

        processInput(window);
        glEnable(GL_DEPTH_TEST);


        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 View = mainCamera.getViewMatrix();
        glm::mat4 Projection = glm::perspective(glm::radians(mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);
        pbrShader.use();

        pbrShader.setVec3f("uCameraPos", glm::value_ptr(mainCamera.Position));
        pbrShader.setMat4f("uView", glm::value_ptr(View));
        pbrShader.setMat4f("uProjection", glm::value_ptr(Projection));
        glm::mat4 Model(1.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo.ID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, metallic.ID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, roughness.ID);
        glActiveTexture(GL_TEXTURE0);
        
        glm::mat4 model = glm::mat4(1.0f);
        for (int row = 0; row < nrRows; ++row)
        {
            for (int col = 0; col < nrColumns; ++col)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (float)(col - (nrColumns / 2)) * spacing,
                    (float)(row - (nrRows / 2)) * spacing,
                    0.0f
                ));
                pbrShader.setMat4f("uModel", glm::value_ptr(model));
                renderSphere();
            }
        }

        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        // keeps the codeprint small.
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            //newPos = lightPositions[i];
            pbrShader.setVec3f("light[" + std::to_string(i) + "].Position", glm::value_ptr(newPos));
            pbrShader.setVec3f("light[" + std::to_string(i) + "].Radiance", glm::value_ptr(lightColors[i]));

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            pbrShader.setMat4f("uModel", glm::value_ptr(model));
            renderSphere();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

}

// splitSum测试
void splitSumTest() {

    GLFWwindow* window = Init(SCR_WIDTH, SCR_HEIGHT);
    if (window == nullptr) return;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 鼠标在屏幕范围内的时候不显示

    glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3(10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };

    // 覆盖屏幕的两个三角形，用于debug或生成2维贴图
    /****************************************************************/
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    /****************************************************************/

    // 生成用于转换等距柱状投影图至立方体贴图的帧缓冲对象
    /****************************************************************/
    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "帧缓冲生成不完整！" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    /****************************************************************/

    // 读入hdr图像
    /****************************************************************/
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf("Source/texture/thatch_chapel_4k.hdr", &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }
    /****************************************************************/

    // 转换hdr等距柱状投影图至立方体贴图
    /****************************************************************/
    Shader E2CShader("Source/shader/pbr/equirectangular2CubemapVertexShader.glsl",
        "Source/shader/pbr/equirectangular2CubemapFragmentShader.glsl");
    unsigned int envCubemap;
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // zoom拉到90，相当于一个无限大平面的正交投影，其实这里只是为了包含某个方向上的所有顶点
    // 使得位于顶点内的片元的各个坐标能够正确插值
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    E2CShader.use();
    E2CShader.setInt("equirectangularMap", 0);
    E2CShader.setMat4f("uProjection", glm::value_ptr(captureProjection));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    // 贴图的分辨率是512，修改视口大小以匹配
    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    // 切换绘制六个面到立方体贴图里
    for (unsigned int i = 0; i < 6; ++i) {
        E2CShader.setMat4f("uView", glm::value_ptr(captureViews[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
        //saveCubeMapFaceToImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 1024, 1024, "Source/debug/cubemap_face_" + std::to_string(i) + ".png");
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    /****************************************************************/

    // 生成漫反射卷积立方体贴图，用来算漫反射的光照
    /****************************************************************/
    Shader irGenShader("Source/shader/pbr/irradianceMapGenVertexShader.glsl",
        "Source/shader/pbr/irradianceMapGenFragmentShader.glsl");
    unsigned int irradianceMap;
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 64, 64, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 64, 64);

    irGenShader.use();
    irGenShader.setInt("uEnvCubeMap", 0);
    irGenShader.setMat4f("uProjection", glm::value_ptr(captureProjection));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    // Texture debugTexture = Texture::CreateTexture(64, 64, GL_RGB16F, GL_RGB, GL_FLOAT);
    // 贴图的分辨率是32，修改视口大小以匹配
    glViewport(0, 0, 64, 64);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    // 切换绘制六个面到立方体贴图里
    for (unsigned int i = 0; i < 6; ++i) {
        irGenShader.setMat4f("uView", glm::value_ptr(captureViews[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    /****************************************************************/

    // 生成预卷积光照贴图
    /****************************************************************/
    Shader LprefilterShader("Source/shader/pbr/LprefilterVertexShader.glsl",
        "Source/shader/pbr/LprefilterFragmentShader.glsl");
    unsigned int prefilterMap;
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


    LprefilterShader.use();
    LprefilterShader.setInt("uEnvCubeMap", 0);
    LprefilterShader.setMat4f("uProjection", glm::value_ptr(captureProjection));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        LprefilterShader.setFloat("uRoughness", roughness);
        for (unsigned int i = 0; i < 6; ++i) {
            LprefilterShader.setMat4f("uView", glm::value_ptr(captureViews[i]));
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /****************************************************************/

    // 生成LUT贴图
    /****************************************************************/
    Shader LutGenShader("Source/shader/pbr/LutGenVertexShader.glsl",
        "Source/shader/pbr/LutGenFragmentShader.glsl");
    Texture brdfLUTTexture = Texture::CreateTexture(512, 512, GL_RG16F, GL_RG, GL_FLOAT);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture.ID, 0);

    glViewport(0, 0, 512, 512);
    LutGenShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    /****************************************************************/

    Shader debugShader("Source/shader/simpleVertexShader.glsl",
        "Source/shader/texFragmentShader.glsl");
    debugShader.use();
    debugShader.setInt("texture1", 0);
    Shader skyboxShader("Source/shader/skyboxVertexShader.glsl", "Source/shader/skyboxFragmentShader.glsl");
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    Shader splitSumTexShader("Source/shader/pbr/splitSumVertexShader.glsl",
        "Source/shader/pbr/splitSumTexFragmentShader.glsl");
    Shader splitSumShader("Source/shader/pbr/splitSumVertexShader.glsl",
        "Source/shader/pbr/splitSumFragmentShader.glsl");

    splitSumTexShader.use();
    splitSumTexShader.setInt("uIrrandianceMap", 4);
    splitSumTexShader.setInt("uPrefilterMap", 5);
    splitSumTexShader.setInt("LUT", 6);
    splitSumShader.use();
    splitSumShader.setInt("uIrrandianceMap", 4);
    splitSumShader.setInt("uPrefilterMap", 5);
    splitSumShader.setInt("LUT", 6);

    Model myModel("Source/model/Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX");

    // 小球的生成行列数及间距
    int nrRows = 7;
    int nrColumns = 7;
    float spacing = 2.5;

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - preTime;
        preTime = currentFrame;

        processInput(window);
        glEnable(GL_DEPTH_TEST);
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        glm::mat4 View(1.f);
        View = mainCamera.getViewMatrix();
        glm::mat4 Projection(1.f);
        Projection = glm::perspective(glm::radians(mainCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.f);
        glm::mat4 Model(1.f);
        splitSumShader.use();
        splitSumShader.setMat4f("uProjection", glm::value_ptr(Projection));
        splitSumShader.setMat4f("uView", glm::value_ptr(View));
        splitSumShader.setVec3f("uCameraPos", glm::value_ptr(mainCamera.Position));

        Model = glm::rotate(Model, (float)glm::radians(90.), glm::vec3(1., 0., 0.));
        Model = glm::rotate(Model, (float)glm::radians(glfwGetTime() * 10), glm::vec3(0., 1., 0.));
        Model = glm::translate(Model, glm::vec3(0., 10., 5.));
        Model = glm::scale(Model, glm::vec3(0.05, 0.05, 0.05));
        splitSumTexShader.use();
        splitSumTexShader.setMat4f("uProjection", glm::value_ptr(Projection));
        splitSumTexShader.setMat4f("uView", glm::value_ptr(View));
        splitSumTexShader.setMat4f("uModel", glm::value_ptr(Model));
        splitSumTexShader.setVec3f("uCameraPos", glm::value_ptr(mainCamera.Position));

        // 渲染材质小球
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture.ID);

        glActiveTexture(GL_TEXTURE0);
        myModel.Draw(splitSumTexShader);

        glm::mat4 model = glm::mat4(1.0f);
        splitSumShader.use();
        splitSumShader.setVec3f("uAlbedo", 0.5, 0., 0.);
        for (int row = 0; row < nrRows; ++row)
        {
            splitSumShader.setFloat("uMetallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col)
            {
                /*splitSumShader.setVec3f("uAlbedo", (float)row / (float)nrRows,
                    (float)col / (float)nrColumns, 0.);*/
                    // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                    // on direct lighting.
                splitSumShader.setFloat("uRoughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(
                    (col - (nrColumns / 2)) * spacing,
                    (row - (nrRows / 2)) * spacing,
                    0.0f
                ));
                splitSumShader.setMat4f("uModel", glm::value_ptr(model));
                renderSphere();
            }
        }

        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            splitSumShader.setVec3f("light[" + std::to_string(i) + "].Position", glm::value_ptr(newPos));
            splitSumShader.setVec3f("light[" + std::to_string(i) + "].Radiance", glm::value_ptr(lightColors[i]));

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            splitSumShader.setMat4f("uModel", glm::value_ptr(model));
            renderSphere();
        }

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        glm::mat4 view = glm::mat4(glm::mat3(View));
        skyboxShader.setMat4f("uView", glm::value_ptr(view));
        skyboxShader.setMat4f("uProjection", glm::value_ptr(Projection));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        renderCube();
        glDepthFunc(GL_LESS);

        // Debug用
        //glDisable(GL_DEPTH_TEST);
        //debugShader.use();
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, debugTexture.ID);
        //glBindVertexArray(quadVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 6);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

int main(void) {
    //BlinPhongTest();
    //stencilTest();
    //frameBufferTest();
    //geoShaderTest();
    //instancingTest();
    //shadowTest();
    //normalMapTest();
    //parallaxTest();
    //SSAOTest();
    //pbrTest();
    splitSumTest();
    return 0;
}

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
             // bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             // top face
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

unsigned int load_cubeMap(const vector<std::string>& faces) {
    unsigned int cubeMapID;
    glGenTextures(1, &cubeMapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);

    int width, height, nrChannels;
    for (int i = 0; i < faces.size(); ++i) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else {
            std::cout << "立方体贴图加载失败！" << std::endl;
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 三个方向均设置为超过边界则强制等于边界值
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return cubeMapID;
}

GLFWwindow* Init(int width ,int height, std::string windowName){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    ///* Make the window's context current */
    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD初始化失败！" << std::endl;
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    return window;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        xpos = lastX;
        ypos = lastY;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    // glfwSetCursorPosCallback返回给mouse_callback函数的(x, y) 是鼠标相对于窗口左【上】角的位置
    // 所以需要将(ypos - lastY) 取反
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    mainCamera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    mainCamera.processMouseScroll(yoffset);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainCamera.processKeyBoard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainCamera.processKeyBoard(Camera_Movement::BACK, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainCamera.processKeyBoard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainCamera.processKeyBoard(Camera_Movement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        flag = true;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        flag = false;
}

// 保存立方体贴图的一个面为图像
void saveCubeMapFaceToImage(GLenum target, int width, int height, const std::string& filename) {
    // 申请足够的空间存储像素数据
    std::vector<unsigned char> pixels(3 * width * height);

    // 读取立方体贴图中的一个面的像素数据
    glReadBuffer(target);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // 反转图像数据（OpenGL的坐标原点在左下角，而图像的坐标原点通常在左上角）
    for (int y = 0; y < height / 2; ++y) {
        for (int x = 0; x < width * 3; ++x) {
            std::swap(pixels[y * width * 3 + x], pixels[(height - 1 - y) * width * 3 + x]);
        }
    }

    // 使用stb_image库保存图像数据到文件
    if (stbi_write_png(filename.c_str(), width, height, 3, pixels.data(), 0) == 0) {
        std::cerr << "Error saving image: " << filename << "\n";
    }
    else {
        std::cout << "Image saved successfully: " << filename << "\n";
    }
}