#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include "ShaderLoader.h"
#include "CCamera.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "VertexStandard.h"
#include "MeshModel.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>



// ---------------------------- Global Variable ------------------------------------------- //
CCamera camera(glm::vec3(0.0f, 0.0f, 100.0f), 800.0f / 600.0f);
GLFWwindow* window = nullptr;
std::vector<GLuint> textures;

float animationTime = 0.0f;
int currentFrame = 0;
const float frameDuration = 0.1f;

// Programs
GLuint Program_Texture;
GLuint Texture_Model;

Mesh_model* model;

// Load the Image data
int ImageWidth;
int ImageHeight;
int ImageComponents;

float CurrentTime;

float QuadRotationAngle = 0.0f;

float HexagonRotationAngle = 0.0f;
float SecondHexagonRotationAngle = 0.0f;

// Function declarations
void SceneSetup();
void InitialSetup();
//void LoadTextures();
void Update(float deltaTime);
void Render();
void Cleanup();

int main() {
    SceneSetup();
    InitialSetup();
    //LoadTextures();

    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrame;
        lastFrame = currentFrameTime;

        Update(deltaTime);
        Render();
    }

    Cleanup();
    glfwTerminate();
    return 0;
}

void SceneSetup() {
    // ------------------------------------------ Setup Rendering Window ------------------------------------------------------ //
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 800, "First OpenGL Window", NULL, NULL);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window. Terminating Program." << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW. Terminating Program." << std::endl;
        glfwTerminate();
        exit(-1);
    }
    // Setup bg Color.
    glClearColor(0.7f, 0.5f, 0.5f, 0.2f);
    glViewport(0, 0, 800, 800);
}

void InitialSetup() {
    // Create the shader program.
    Program_Texture = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");
    if (Program_Texture == 0) {
        std::cerr << "Failed to create shader program." << std::endl;
        exit(1);
    }

    model = new Mesh_model("Resources/Models/AncientEmpire/SM_Prop_Statue_01.obj");

    // ----------------------------------------- SETUP Image Textures ---------------------------------------------------------------- //
    // Image 0
    stbi_set_flip_vertically_on_load(true);
    unsigned char* ImageData0 = stbi_load("Resources/Textures/PolygonAncientWorlds_Statue_01.png", &ImageWidth, &ImageHeight, &ImageComponents, 0);
    if (!ImageData0) {
        std::cerr << "Failed to load texture." << std::endl;
        exit(1);
    }

    glGenTextures(1, &Texture_Model);
    glBindTexture(GL_TEXTURE_2D, Texture_Model);
    GLint LoadedComponent0 = (ImageComponents == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponent0, ImageWidth, ImageHeight, 0, LoadedComponent0, GL_UNSIGNED_BYTE, ImageData0);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(ImageData0);

    glBindTexture(GL_TEXTURE_2D, 0);
}



void Update(float deltaTime)
{
    glfwPollEvents();

}
void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 ProjectionMat = camera.GetProjMat();
    glm::mat4 ViewMat = camera.GetViewMat();
    glm::mat4 ModelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    glm::mat4 MVP = ProjectionMat * ViewMat * ModelMat;

    model->Render(Program_Texture, MVP, Texture_Model);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
    glfwSwapBuffers(window);
}

void Cleanup() {
    delete model;

    for (GLuint tex : textures) {
        glDeleteTextures(1, &tex);
    }
}