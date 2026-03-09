#include<stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include<glad/glad.h>
#include<glfw/glfw3.h>
#include<stb_image/stb_image.h>

GLuint programID;
GLuint vertexShaderID, fragmentShaderID;
GLuint vertexArrayObjectID, vertexBufferID, indexBufferID;


class File{
public:
    File(const std::string& filePath) : path(filePath) {}

    std::string read() const {
        std::ifstream file(path);
        if (!file.is_open()) {
            printf("File couldn't be open(check path)\n");
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf(); // read entire file
        return buffer.str();
    }

    const char* read_cstring() {
        content = read();
        return content.c_str();
    }

private:
    std::string path;
    std::string content;
};

void shader_panic(GLuint& shaderID, const std::string& name){
    GLint success;
    GLchar log[512];
    
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(shaderID, 512, NULL, log);
        printf("%s shader compilation error: \n%s", name.c_str(), log);
    }
}
void program_panic(GLuint& programID, const std::string& name){
    GLint success;
    GLchar log[512];
    
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(programID, 512, NULL, log);
        printf("%s program linking error: \n%s", name.c_str(), log);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}



void data_upon_load(){
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        glfwTerminate();
        printf("Problems with GLAD.");
    }

    GLfloat vertices[] = { //x, y, r, g, b (normalized)
        -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,
        -0.5f, +0.5f,   1.0f, 0.0f, 0.0f,
        +0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
        +0.5f, +0.5f,   0.0f, 0.0f, 1.0f, 
    };

    GLushort indices[]={
        0, 1, 3,
        0, 3, 2,
    };

    glGenVertexArrays(1,&vertexArrayObjectID);
    glBindVertexArray(vertexArrayObjectID);


    glGenBuffers(1, &vertexBufferID);               //generate 1 buffer for vertices
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);  //pass it to gpu
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(vertices),  //pass size in bytes of vertices
        vertices, GL_STATIC_DRAW            //pass vertices
    );
    
    glEnableVertexAttribArray(0); //set location 0
    glVertexAttribPointer(
        0, 2,                   //write at location 0, number of components(x, y)
        GL_FLOAT, GL_FALSE,     //type: float, no normalization
        sizeof(GLfloat)*5, 0    //move through 5 indices, start at 0
    );

    glEnableVertexAttribArray(1);   //set location 1
    glVertexAttribPointer(
        1, 3,                   //write at location 1, number of components(r, g, b)
        GL_FLOAT, GL_FALSE,     //type: float, no normalization
        sizeof(GLfloat)*5,      //move through 5 indices
        (void*)(sizeof(GLfloat)*2) //start at 2 (skip x, y)
    );


    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void load_shaders(){
    File VertexShaderGLSL("../src/shaders/vertex_shader.glsl"); //!IMPORTANT runs from build directory(open_gl)
    File FragmentShaderGLSL("../src/shaders/fragment_shader.glsl"); // not from src directory!

    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


    const char* shaderText = VertexShaderGLSL.read_cstring();
    glShaderSource(vertexShaderID, 1, &shaderText, NULL);
    shaderText = FragmentShaderGLSL.read_cstring();
    glShaderSource(fragmentShaderID, 1, &shaderText, NULL);
    

    glCompileShader(vertexShaderID);
    glCompileShader(fragmentShaderID);
    shader_panic(vertexShaderID, "Vertex");
    shader_panic(fragmentShaderID, "Fragment");


    programID = glCreateProgram();
    glAttachShader(programID, fragmentShaderID);
    glAttachShader(programID, vertexShaderID);

    glLinkProgram(programID);
    program_panic(programID, "Program");
    glUseProgram(programID);
}


void upon_load(){
    data_upon_load();
    load_shaders();
}

void loop(){
    glClearColor(0.2f, 0.3f, 0.2f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void after(){
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &indexBufferID);

    glDeleteVertexArrays(1, &vertexArrayObjectID);
    
    glDeleteProgram(programID);

    glfwTerminate();
}


int main(){
    if (!glfwInit()) return -1;


    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", NULL, NULL);
    if (!window){glfwTerminate(); return -1;}

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    upon_load();


    while (!glfwWindowShouldClose(window)){

        loop();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    after();

    return 0;
}