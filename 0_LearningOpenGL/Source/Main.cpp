#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

static GLFWwindow* createWindow()
{
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // 创建窗口并将窗口绑定到当前线程的上下文
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    // 通过 GLAD 获取 gl 函数地址
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // 设定 viewport 的 size
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

static unsigned int createVertexShader()
{
    // 使用字符串硬编码 shader 源码
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        // vertex shader 将 color 输出，以便后面 fragment shader 使用
        "out vec3 ourColor;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   ourColor = aColor;\n"
        "}\0";

    // 创建 Shader 及其对象
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // 将 Shader 源码绑定到 shader 对象
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // 编译 shader
    glCompileShader(vertexShader);

    // 可省略，用于获取 shader 编译失败后的错误信息
    int  success; // GL_TRUE: 1, GL_FALSE: 0
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return vertexShader;
}

static unsigned int createFragShader()
{
    // 片段着色器
    const char* fragShaderSource = "#version 330 core\n"
        "out vec4 fragColor;"
        "in vec3 ourColor;"
        "uniform float ratio;"
        "void main()\n"
        "{\n"
        "   fragColor = vec4(ourColor * ratio, 1.0);\n"
        "}\0";
    unsigned int fragShader;
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderSource, NULL);
    // 编译 shader
    glCompileShader(fragShader);

    // 可省略，用于获取 shader 编译失败后的错误信息
    int  success; // GL_TRUE: 1, GL_FALSE: 0
    char infoLog[512];
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return fragShader;
}

static unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragShader)
{
    // 创建 shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    // attach 并 link 2 个 shader
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    // 可省略，用于获取 shader program link 失败后的错误信息
    int  success; // GL_TRUE: 1, GL_FALSE: 0
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    // 删除 shader
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    return shaderProgram;
}


int main(int argc, char* arv[])
{
    GLFWwindow* window = createWindow();
    unsigned int vertexShader = createVertexShader();
    unsigned int fragShader = createFragShader();
    unsigned int shaderProgram = createShaderProgram(vertexShader, fragShader);

    // 定义三角形在正则坐标下的坐标值
    float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 右上角
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 右下角
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // 左下角
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f // 左上角
    };

    unsigned int indices[] = {
        // 注意索引从0开始!
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形

        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    // 创建 VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // 将 VAO 绑定到当前上下文
    glBindVertexArray(VAO);

    unsigned int VBO;
    // 生成 1 个 buffer，buffer index 赋值给 VBO
    glGenBuffers(1, &VBO);
    // 指定 VBO 对应的 buffer 类型为 GL_ARRAY_BUFFER
    // 将 VBO 对应的 buffer 绑定到上下文，后续的操作都是基于当前 buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 把的顶点数据复制到 buffer 中
    // GL_STATIC_DRAW 表示数据不会或几乎不会改变
    // 若指定为 GL_DYNAMIC_DRAW 或 GL_STREAM_DRAW，GPU 会把数据放在能够高速写入的内存部分
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 如何解释内存中的顶点数据，以及如何将顶点数据链接到 shader 的属性上
    // @param0：标识当前 vertex 的属性，相当于将当前 vertex 数据传到 vertex shader 中的 location = 0 指定的变量 aPos
    // @param1：指定一个 vertex 的元素数量为 3
    // @param2：指定 vertex 的类型为 float
    // @param3：表明 vertex 数据不需要被标准化到 [0, 1]
    // @param4：步长，表明每组 vertex 属性的元素个数，比如 {x, y, z, r, g, b, a} 的时候为 7
    // @param5：偏移，表明 vertex 从哪个位置开始取，比如 {x, y, z, r, g, b, a} 如果想去颜色就设为 3
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // 启用顶点属性
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 线框模式
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 循环处理输入并渲染
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 使用 program，后续每个 Shader 调用和渲染调用都会用到这个 program
        glUseProgram(shaderProgram);

        float timeValue = glfwGetTime();
        float ratio = (sin(timeValue) / 2.0f) + 0.5f;
        int ratioLocation = glGetUniformLocation(shaderProgram, "ratio");
        // glUniform4f 之前必须先调用 glUseProgram，因为需要在当前激活的 shader program 中设置 uniform
        glUniform1f(ratioLocation, ratio);

        // 绑定 VAO，在这里其实不绑定也行，因为我们只有一个 VAO
        // 实际的项目中会有多个 VAO，就需要根据不同的逻辑绑定不同的 VAO
        glBindVertexArray(VAO);
        // @param2：表示索引 VAO 的第 0 个位置的 VBO
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 释放资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}


