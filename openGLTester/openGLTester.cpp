#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

struct shaderSources {
    std::string vertexSource;
    std::string fragmentSource;
};

static shaderSources parseShader(const std::string& fileP) {
    std::ifstream file(fileP);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << fileP << std::endl;
        return { "", "" };
    }

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream strS[2];
    ShaderType type = ShaderType::NONE;
    while (getline(file, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            strS[(int)type] << line << '\n';
        }
    }

    file.close();
    return { strS[0].str(), strS[1].str() };
}

static unsigned int compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* srcChar = source.c_str();
    glShaderSource(id, 1, &srcChar, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile shader\n";
    }

    return id;
}

static unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vtxShadr = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int frgShadr = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vtxShadr);
    glAttachShader(program, frgShadr);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vtxShadr);
    glDeleteShader(frgShadr);

    return program;
}

int main(void) {
    GLFWwindow* window;

    if (!glfwInit()) {
        return -1;
    }

    window = glfwCreateWindow(640, 480, "Colored Square", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Glew not ok :(\n";
    }

    std::cout << glGetString(GL_VERSION) << std::endl;


    shaderSources shaderSources = parseShader("basicShaderFile.shader");
    std::string vertexShader = shaderSources.vertexSource;
    std::string fragmentShader = shaderSources.fragmentSource;


    std::cout << "Vertex Shader Source:\n" << vertexShader << "\n\n";
    std::cout << "Fragment Shader Source:\n" << fragmentShader << "\n\n";


    unsigned int shader = createShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    glUniform4f(glGetUniformLocation(shader, "uColor"), 0.0f, 1.0f, 0.0f, 1.0f);


    std::vector<float> positions = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    unsigned int buf;

    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);


        glDrawArrays(GL_QUADS, 0, positions.size() / 2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
