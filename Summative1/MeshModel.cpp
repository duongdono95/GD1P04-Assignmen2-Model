#include "MeshModel.h"
#include <iostream>
#include "ShaderLoader.h"
#include <gtc/type_ptr.hpp>

Mesh_model::Mesh_model(const std::string& FilePath)
    : DrawCount(0) { 
    std::vector<VertexStandard> Vertices;
    tinyobj::ObjReader Reader;
    tinyobj::ObjReaderConfig ReaderConfig;

    if (!Reader.ParseFromFile(FilePath, ReaderConfig)) {
        if (!Reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << Reader.Error() << std::endl;
        }
        exit(1);
    }

    if (!Reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << Reader.Warning() << std::endl;
    }

    auto& Attrib = Reader.GetAttrib();
    auto& Shapes = Reader.GetShapes();

    for (size_t ShapeIndex = 0; ShapeIndex < Shapes.size(); ++ShapeIndex) {
        const auto& Shape = Shapes[ShapeIndex];
        size_t ReadIndexOffset = 0;

        for (size_t FaceIndex = 0; FaceIndex < Shape.mesh.num_face_vertices.size(); ++FaceIndex) {
            size_t FaceVertexCount = size_t(Shape.mesh.num_face_vertices[FaceIndex]);

            for (size_t VertexIndex = 0; VertexIndex < FaceVertexCount; ++VertexIndex) {
                VertexStandard Vertex{};
                tinyobj::index_t TinyObjVertex = Shape.mesh.indices[ReadIndexOffset + VertexIndex];

                Vertex.position = {
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 0],
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 1],
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 2]
                };

                if (TinyObjVertex.texcoord_index >= 0) {
                    Vertex.texcoord = {
                        Attrib.texcoords[2 * size_t(TinyObjVertex.texcoord_index) + 0],
                        Attrib.texcoords[2 * size_t(TinyObjVertex.texcoord_index) + 1]
                    };
                }

                Vertices.push_back(Vertex);
            }

            ReadIndexOffset += FaceVertexCount;
        }
    }

    DrawCount = static_cast<GLsizei>(Vertices.size());

    // OpenGL buffer setup
    GLuint VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(VertexStandard), &Vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, texcoord));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Mesh_model::~Mesh_model() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh_model::Update(float DeltaTime) {
   
}

void Mesh_model::Render(GLuint Program, const glm::mat4& MVP, GLuint textureID) {
    // Use the specified shader program.
    glUseProgram(Program);

    // Pass the MVP matrix to the shader program.
    GLint MVP_Location = glGetUniformLocation(Program, "MVP");
    glUniformMatrix4fv(MVP_Location, 1, GL_FALSE, glm::value_ptr(MVP));

    // Bind the specified texture.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(Program, "Texture0"), 0);

    // Render the model.
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, DrawCount);
    glBindVertexArray(0);

    // Reset OpenGL state.
    glUseProgram(0);
}
