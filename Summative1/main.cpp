#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <string>
#include "ShaderLoader.h"
#include "CCamera.h"
#include "Input.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "VertexStandard.h"
#include "MeshModel.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <gtc/matrix_inverse.hpp>

// ---------------------------- Global Variable ------------------------------------------- //
CCamera camera(glm::vec3(0.0f, 0.0f, 100.0f), 800.0f / 600.0f);
GLFWwindow* window = nullptr;

// Programs
GLuint Program_Palm_Tree_Texture;
GLuint Texture_Acient_World;
GLuint Texture_Model;
int World_ImageWidth, World_ImageHeight, World_ImageComponents;

GLuint Program_Quad;
GLuint Texture_Quad_Default;
GLuint Texture_Quad_Hover;
GLuint Quad_VAO, Quad_VBO;
int Quad_ImageWidth, Quad_ImageHeight, Quad_ImageComponents;

bool isHovering = false;
bool repeatTexture = false;

GLfloat quadVertices[] = {
    // Positions        // Texture Coords
    -0.5f, 0.5f, 0.0f,  0.0f, 1.0f, // Top-left
     0.5f, 0.5f, 0.0f,  1.0f, 1.0f, // Top-right
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Bottom-right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f  // Bottom-left
};

// Models
Mesh_model* statueModel;
glm::vec3 statuePosition = glm::vec3(0.0f, 0.0f, 0.0f);
float safeRadius = 10.0f;

Mesh_model* palmTreeModel;
std::vector<GLuint> textures;
std::vector<glm::mat4> instanceTransforms;
int NumberPalmTree = 1000;
bool quadTextureTiled = false;
// Function declarations
void SceneSetup();
void InitialSetup();
void Update(float deltaTime);
void Render();
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main() {
    SceneSetup();
    Input::Initialize(window);
    InitialSetup();
    float lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // ----------------------------------- Camera Adjustment Based on Input --------------------------- //
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrame;
        lastFrame = currentFrameTime;

        Update(deltaTime);

        Render();
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }
        glfwSwapBuffers(window);
    }

    // Clean up
    delete statueModel;
    delete palmTreeModel;
    for (GLuint tex : textures) {
        glDeleteTextures(1, &tex);
    }
    glfwTerminate();
    return 0;
}

void InitialSetup() {
    // ---------------------------------------- SHADER PROGRAM ---------------------------------------- //
    Texture_Acient_World = ShaderLoader::CreateProgram("Resources/Shaders/Texture.vert", "Resources/Shaders/Texture.frag");

    Program_Palm_Tree_Texture = ShaderLoader::CreateProgram("Resources/Shaders/PalmTreeTexture.vert", "Resources/Shaders/Texture.frag");

    Program_Quad = ShaderLoader::CreateProgram("Resources/Shaders/Quad_Texture.vert", "Resources/Shaders/Quad_Texture.frag");

    // ---------------------------------------- IMPORT MODELS ---------------------------------------- //
    statueModel = new Mesh_model("Resources/Models/AncientEmpire/SM_Prop_Statue_01.obj");

    palmTreeModel = new Mesh_model("Resources/Models/AncientEmpire/SM_Env_Tree_Palm_01.obj");

    // ----------------------------------------- SETUP Image Textures ---------------------------------------------------------------- //
    // ----------------------------------------- Ancient World Texture
    stbi_set_flip_vertically_on_load(true);
    unsigned char* ImageData0 = stbi_load("Resources/Textures/PolygonAncientWorlds_Texture_01_B.png", &World_ImageWidth, &World_ImageHeight, &World_ImageComponents, 0);

    glGenTextures(1, &Texture_Model);
    glBindTexture(GL_TEXTURE_2D, Texture_Model);
    GLint LoadedComponent0 = (World_ImageComponents == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponent0, World_ImageWidth, World_ImageHeight, 0, LoadedComponent0, GL_UNSIGNED_BYTE, ImageData0);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(ImageData0);

    glBindTexture(GL_TEXTURE_2D, 0);

    // ----------------------------------------- Quad Texture
    unsigned char* quadImageData = stbi_load("Resources/Textures/Hero_Run.png", &Quad_ImageWidth, &Quad_ImageHeight, &Quad_ImageComponents, 0);

    glGenTextures(1, &Texture_Quad_Default);
    glBindTexture(GL_TEXTURE_2D, Texture_Quad_Default);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Quad_ImageWidth, Quad_ImageHeight, 0, Quad_ImageComponents == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, quadImageData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(quadImageData);

    unsigned char* hoverImageData = stbi_load("Resources/Textures/Alien.png", &Quad_ImageWidth, &Quad_ImageHeight, &Quad_ImageComponents, 0);
    glGenTextures(1, &Texture_Quad_Hover);
    glBindTexture(GL_TEXTURE_2D, Texture_Quad_Hover);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Quad_ImageWidth, Quad_ImageHeight, 0, Quad_ImageComponents == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, hoverImageData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(hoverImageData);

    glGenVertexArrays(1, &Quad_VAO);
    glGenBuffers(1, &Quad_VBO);
    glBindVertexArray(Quad_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Texture Coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ------------------------------------------ Prepare for PalmTree Rendering ------------------------------------------ // 
    palmTreeModel->GenerateInstanceTransforms(instanceTransforms, NumberPalmTree);
    palmTreeModel->InitInstances(instanceTransforms);
}

void Update(float deltaTime)
{
    glfwPollEvents();
    Input::Update();

    // ------------------------------ Toggle Keys ---------------------------- //
    // Toggle cursor visibility
    if (Input::IsKeyToggled(GLFW_KEY_Z)) {
        int cursorMode = glfwGetInputMode(window, GLFW_CURSOR);
        glfwSetInputMode(window, GLFW_CURSOR, cursorMode == GLFW_CURSOR_NORMAL ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    // Toggle wireframe mode
    if (Input::IsKeyToggled(GLFW_KEY_X)) {
        static bool wireframe = false;
        wireframe = !wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    }

    // Print cursor coordinates
    if (Input::IsKeyToggled(GLFW_KEY_C)) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        std::cout << "Cursor Position: (" << xpos << ", " << ypos << ")" << std::endl;
    }
    camera.Update(deltaTime);

    // ------------------------------ Transform Statue Model ---------------------------- //
    glm::vec3 forward = camera.GetForwardVector();
    glm::vec3 right = camera.GetRightVector();
    glm::vec3 up = glm::vec3(0, 1, 0);

    float moveSpeed = 5.0f * deltaTime;
    if (Input::IsKeyPressed(GLFW_KEY_W)) {
        statuePosition += forward * moveSpeed;
    }
    if (Input::IsKeyPressed(GLFW_KEY_S)) {
        statuePosition -= forward * moveSpeed;
    }
    if (Input::IsKeyPressed(GLFW_KEY_A)) {
        statuePosition -= right * moveSpeed;
    }
    if (Input::IsKeyPressed(GLFW_KEY_D)) {
        statuePosition += right * moveSpeed;
    }
    if (Input::IsKeyPressed(GLFW_KEY_Q)) {
        statuePosition -= up * moveSpeed;
    }
    if (Input::IsKeyPressed(GLFW_KEY_E)) {
        statuePosition += up * moveSpeed;
    }
    camera.Update(deltaTime);

    // ------------------------------ Camera toggle Manual/Auto Mode ---------------------------- //
    if (Input::IsKeyPressed(GLFW_KEY_SPACE)) {
        camera.SetMode(camera.GetMode() == CCamera::MANUAL ? CCamera::AUTO : CCamera::MANUAL);
    }
    if (camera.GetMode() == CCamera::MANUAL) {
        float angleDelta = Input::IsKeyPressed(GLFW_KEY_RIGHT) - Input::IsKeyPressed(GLFW_KEY_LEFT);
        float radiusDelta = Input::IsKeyPressed(GLFW_KEY_UP) - Input::IsKeyPressed(GLFW_KEY_DOWN);
        // Speed up movement if Shift is pressed
        if (Input::IsShiftPressed()) {
            angleDelta *= 10;
            radiusDelta *= 10;
        }
        camera.AdjustOrbit(angleDelta * 0.05f, radiusDelta);
    }
    else if (camera.GetMode() == CCamera::AUTO) {
        float speedFactor = Input::IsShiftPressed() ? 10.0f : 1.0f;
        // Automatic movement speed adjustment
        camera.AdjustOrbit(0.02f * deltaTime * speedFactor, 0);
    }
    camera.Update(deltaTime);

}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 ProjectionMat = camera.GetProjMat();
    glm::mat4 ViewMat = camera.GetViewMat();

    // ------------------------- Render Statue Model -------------------------- //
    glm::mat4 StatueModelMat = glm::translate(glm::mat4(1.0f), statuePosition) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    glm::mat4 StatueMVP = ProjectionMat * ViewMat * StatueModelMat;
    statueModel->Render(Texture_Acient_World, StatueMVP, Texture_Model);

    // ------------------------- Render Palm Tree Model -------------------------- //
    glm::mat4 PalmTreeViewProjection = camera.GetProjMat() * camera.GetViewMat();
    palmTreeModel->RenderInstanced(Program_Palm_Tree_Texture, PalmTreeViewProjection, Texture_Model, NumberPalmTree);

    // ------------------------- Render The Quad -------------------------- //
    glm::mat4 QuadProjectionMat = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 QuadModelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)); // Position the quad
    QuadModelMat = glm::scale(QuadModelMat, glm::vec3(0.5f, 0.5f, 1.0f)); // Scale the quad

    glm::mat4 MVP = QuadProjectionMat * QuadModelMat;
    glUseProgram(Program_Quad);
    glUniformMatrix4fv(glGetUniformLocation(Program_Quad, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    xpos = xpos / 800 * 2 - 1;
    ypos = ypos / 800 * 2 - 1;
    ypos = -ypos;

    glm::vec4 mousePos = glm::vec4(xpos, ypos, 1.0, 1.0);
    glm::vec4 transformedMousePos = glm::inverse(MVP) * mousePos;

    bool isHovering = transformedMousePos.x >= -0.5f && transformedMousePos.x <= 0.5f && transformedMousePos.y >= -0.5f && transformedMousePos.y <= 0.5f;

    GLuint currentTexture = isHovering ? Texture_Quad_Hover : Texture_Quad_Default;
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    if (isHovering && repeatTexture) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glUniform1f(glGetUniformLocation(Program_Quad, "textureScale"), 4.0f);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glUniform1f(glGetUniformLocation(Program_Quad, "textureScale"), 1.0f);
    }
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(Program_Quad, "texture0"), 0);
    glBindVertexArray(Quad_VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glUseProgram(0);
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // Setup bg Color.
    glClearColor(1.0f, 0.5f, 0.2f, 0.0f);
    glViewport(0, 0, 800, 800);

    // ------------------------------------------ Setup Camera ------------------------------------------------------ //
    float cameraHeight = 20.0f;  // Height of the camera above the scene
    float angleDegrees = 15.0f;  // Downward angle in degrees

    float angleRadians = glm::radians(angleDegrees);
    float offset = cameraHeight / tan(angleRadians);

    // ------------------------------------------ Depth Buffer ------------------------------------------------------ //
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        repeatTexture = !repeatTexture;
    }
}