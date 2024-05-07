#ifndef MESH_MODEL_H
#define MESH_MODEL_H

#include <vector>
#include <string>
#include <glew.h>
#include <tiny_obj_loader.h>
#include "VertexStandard.h"

class Mesh_model {
public:
    Mesh_model(const std::string& FilePath);
    ~Mesh_model();

    void Update(float DeltaTime);
    void Render(GLuint Program, const glm::mat4& MVP, GLuint textureID);

private:
    GLuint VAO, VBO;
    GLsizei DrawCount;
};

#endif // MESH_MODEL_H
