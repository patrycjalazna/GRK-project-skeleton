#include "ExampleTerrain.h"
#include "ShaderLoader.h"
#include "objload.h"

#define BUFFER_OFFSET(i) ((char*)nullptr + (i))

ExampleTerrain::ExampleTerrain()
{
}

ExampleTerrain::~ExampleTerrain()
{
}

void ExampleTerrain::init()
{
    obj::Model model = obj::loadModelFromFile("models/terrain.obj");
    faceCount_ = model.faces["default"].size();

    Core::ShaderLoader shaderLoader;
    program_ = shaderLoader.CreateProgram(
        "shaders/shader_terrain.vert",
        "shaders/shader_terrain.frag");

    glGenVertexArrays(1, &vertexArray_);
    glBindVertexArray(vertexArray_);
    glGenBuffers(1, &vertexIndexBuffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
        faceCount_ * sizeof(unsigned short),
        model.faces["default"].data(), GL_STATIC_DRAW);

    glGenBuffers(1, &vertexBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    size_t vsize = model.vertex.size() * sizeof(float);
    size_t nsize = model.normal.size() * sizeof(float);
    size_t tsize = model.texCoord.size() * sizeof(float);
    size_t bufsize = vsize + nsize + tsize;
    glBufferData(GL_ARRAY_BUFFER, bufsize, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, model.vertex.data());
    glBufferSubData(GL_ARRAY_BUFFER, vsize, nsize, model.normal.data());
    if (tsize > 0) {
        glBufferSubData(GL_ARRAY_BUFFER, vsize + nsize, tsize, model.texCoord.data());
    }

    auto glPos = glGetAttribLocation(program_, "vertexPosition");
    glEnableVertexAttribArray(glPos);
    glVertexAttribPointer(glPos, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    auto glNor = glGetAttribLocation(program_, "vertexNormal");
    glEnableVertexAttribArray(glNor);
    glVertexAttribPointer(glNor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vsize));

    auto glUV = glGetAttribLocation(program_, "vertexTexCoord");
    glEnableVertexAttribArray(glUV);
    glVertexAttribPointer(glUV, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vsize+nsize));

    glBindVertexArray(0);
}

glm::mat4 ExampleTerrain::getModelMatrix() const
{
    return glm::mat4();
}

void ExampleTerrain::update(float time)
{
}

void ExampleTerrain::render(RenderData& data)
{
    glUseProgram(program_);
    glBindVertexArray(vertexArray_);

    glm::mat4 modelViewProjMatrix = data.viewProjMatrix * getModelMatrix();
    glm::mat4 modelMatrix = getModelMatrix();

    glUniformMatrix4fv(glGetUniformLocation(program_,
        "modelViewProjectionMatrix"), 1, GL_FALSE,
        (float*)&modelViewProjMatrix);

    glUniformMatrix4fv(glGetUniformLocation(program_,
        "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

    glUniform3f(glGetUniformLocation(program_, "lightSource"),
        data.lightSource.x, data.lightSource.y, data.lightSource.z);

    glDrawElements(GL_TRIANGLES, faceCount_, GL_UNSIGNED_SHORT, (void*)0);

    glBindVertexArray(0);
    glUseProgram(0);
}

int ExampleTerrain::getRows() const { return 100; }

int ExampleTerrain::getCols() const { return 100; }

float ExampleTerrain::getSizeX() const { return 100.0f; }

float ExampleTerrain::getSizeZ() const { return 100.0f; }

float ExampleTerrain::getHeight(float x, float z) const
{
    z = -z;
    return sin(x / 12.3)*cos(z / 13.12) * 3 +
        40 / ((x - 5)*(x - 5) / 10 + 2) * 1 / ((z - 5)*(z - 5) / 10 + 2) +
        30 / ((x - 14)*(x - 14) / 10 + 2) * 1 / ((z - 10)*(z - 10) / 10 + 2) -
        40 / ((x - 14)*(x - 14) / 40 + 2) * 1 / ((z + 15)*(z + 15) / 50 + 2);
}