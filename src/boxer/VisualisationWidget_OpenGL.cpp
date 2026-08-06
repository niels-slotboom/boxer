#include "VisualisationWidget.hpp"

#include <QOpenGLShader>
#include <cstddef>
#include <vector>

namespace boxer {

// --- Axis Shaders ---
static const char* axisVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertColor;
uniform mat4 mvp;

void main() {
    vertColor = aColor;
    gl_Position = mvp * vec4(aPos, 1.0);
}
)";

static const char* axisFragmentShaderSource = R"(
#version 330 core
in vec3 vertColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vertColor, 1.0);
}
)";

// --- Instanced Box Shaders ---
static const char* boxVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aUnitPos;
layout (location = 1) in vec3 aBoxLo;
layout (location = 2) in vec3 aBoxHi;
layout (location = 3) in int  aLevel;
layout (location = 4) in int  aIsHalo;

out vec3 levelColor;
flat out int vLevel;  
flat out int vIsHalo; 

uniform mat4 mvp;

const vec3 palette[8] = vec3[8](
    vec3(0.90, 0.30, 0.30), // Level 0
    vec3(0.20, 0.75, 0.50), // Level 1
    vec3(0.25, 0.55, 0.95), // Level 2
    vec3(0.95, 0.60, 0.15), // Level 3
    vec3(0.70, 0.35, 0.85), // Level 4
    vec3(0.15, 0.80, 0.85), // Level 5
    vec3(0.95, 0.40, 0.65), // Level 6
    vec3(0.90, 0.85, 0.25)  // Level 7+
);

void main() {
    vec3 worldPos = aBoxLo + aUnitPos * (aBoxHi - aBoxLo);

    if (aLevel == -1) {
        levelColor = vec3(0.60, 0.60, 0.65);
    } else {
        levelColor = palette[min(aLevel, 7)];
    }

    vLevel = aLevel;
    vIsHalo = aIsHalo;

    vec4 clipPos = mvp * vec4(worldPos, 1.0);

    // FIX FOR Z-FIGHTING
    if (aLevel >= 0) {
        clipPos.z += float(aLevel + 1) * 0.0001 * clipPos.w;
    }

    gl_Position = clipPos;
}
)";

static const char* boxFragmentShaderSource = R"(
#version 330 core
in vec3 levelColor;
flat in int vLevel;
flat in int vIsHalo;

uniform bool uIsFacePass; // true when rendering transparent box faces

out vec4 FragColor;

void main() {
    float alpha = 1.0;

    if (vLevel > 0) {
        // Exponential decay: starts at 1.0, decays toward a floor of 0.25
        float minAlpha = 0.25;
        float decayRate = 0.35;
        
        alpha = minAlpha + (1.0 - minAlpha) * exp(-decayRate * float(vLevel));
    }
    
    // reduce opacity for ghost/halo regions
    if (vIsHalo != 0) {
        alpha *= 0.5;
    }

    // Scale down face opacity so nested inner levels remain visible
    if (uIsFacePass) {
        alpha *= 0.1;
    }
    
    FragColor = vec4(levelColor, alpha);
}
)";

void VisualisationWidget::initializeGL() {
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    // RGB colors blend normally based on alpha, but the framebuffer alpha is kept at 1.0
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);
    glClearColor(0.05f, 0.05f, 0.06f, 1.0f);

    // ==========================================
    // 1. Build Axis Shader Program & Geometry
    // ==========================================
    axisShaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, axisVertexShaderSource);
    axisShaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, axisFragmentShaderSource);
    axisShaderProgram.link();
    axisMvpUniformLoc = axisShaderProgram.uniformLocation("mvp");

    // Coordinate Axes (X: Red, Y: Green, Z: Blue from origin)
    std::vector<float> axisVertices = {// X Axis (Red)
                                       0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                       // Y Axis (Green)
                                       0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                                       // Z Axis (Blue)
                                       0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    glGenVertexArrays(1, &axesVAO);
    glGenBuffers(1, &axesVBO);

    glBindVertexArray(axesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
    glBufferData(GL_ARRAY_BUFFER, axisVertices.size() * sizeof(float), axisVertices.data(), GL_STATIC_DRAW);

    // Attrib 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // Attrib 1: Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ==========================================
    // 2. Build Box Instancing Shader Program
    // ==========================================
    boxShaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, boxVertexShaderSource);
    boxShaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, boxFragmentShaderSource);
    boxShaderProgram.link();
    boxMvpUniformLoc = boxShaderProgram.uniformLocation("mvp");

    // Unit Cube Wireframe Lines (12 lines = 24 vertices)
    float unitCubeLineVertices[] = {// Bottom Face
                                    0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                    // Top Face
                                    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                                    1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                                    // Vertical Pillars
                                    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
                                    0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f};

    // Unit Cube Faces (12 triangles = 36 vertices)
    float unitCubeTriangleVertices[] = {// Front face (z = 1)
                                        0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1,
                                        // Back face (z = 0)
                                        0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0,
                                        // Top face (y = 1)
                                        0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0,
                                        // Bottom face (y = 0)
                                        0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1,
                                        // Right face (x = 1)
                                        1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1,
                                        // Left face (x = 0)
                                        0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0};

    // Generate shared instance VBO and mesh VBOs
    glGenBuffers(1, &boxVBO);
    glGenBuffers(1, &unitCubeVBO);
    glGenBuffers(1, &boxFaceVBO);

    glGenVertexArrays(1, &boxVAO);
    glGenVertexArrays(1, &boxFaceVAO);

    // ----------------------------------------------------
    // Setup Wireframe Line VAO (boxVAO)
    // ----------------------------------------------------
    glBindVertexArray(boxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, unitCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unitCubeLineVertices), unitCubeLineVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AmrMeshWrapper::BoxInstanceData),
                          reinterpret_cast<void*>(offsetof(AmrMeshWrapper::BoxInstanceData, lo)));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(AmrMeshWrapper::BoxInstanceData),
                          reinterpret_cast<void*>(offsetof(AmrMeshWrapper::BoxInstanceData, hi)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glVertexAttribIPointer(3, 1, GL_INT, sizeof(AmrMeshWrapper::BoxInstanceData),
                           reinterpret_cast<void*>(offsetof(AmrMeshWrapper::BoxInstanceData, level)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glVertexAttribIPointer(4, 1, GL_INT, sizeof(AmrMeshWrapper::BoxInstanceData),
                           reinterpret_cast<void*>(offsetof(AmrMeshWrapper::BoxInstanceData, isHalo)));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    // ----------------------------------------------------
    // Setup Solid Face VAO (boxFaceVAO)
    // ----------------------------------------------------
    glBindVertexArray(boxFaceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, boxFaceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(unitCubeTriangleVertices), unitCubeTriangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 0);

    // Reuse identical instance attributes from boxVBO
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AmrMeshWrapper::BoxInstanceData),
                          reinterpret_cast<void*>(offsetof(AmrMeshWrapper::BoxInstanceData, lo)));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(AmrMeshWrapper::BoxInstanceData),
                          reinterpret_cast<void*>(offsetof(AmrMeshWrapper::BoxInstanceData, hi)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glVertexAttribIPointer(3, 1, GL_INT, sizeof(AmrMeshWrapper::BoxInstanceData),
                           reinterpret_cast<void*>(offsetof(AmrMeshWrapper::BoxInstanceData, level)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glVertexAttribIPointer(4, 1, GL_INT, sizeof(AmrMeshWrapper::BoxInstanceData),
                           reinterpret_cast<void*>(offsetof(AmrMeshWrapper::BoxInstanceData, isHalo)));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    updateBoxData();
    updateViewMatrix();
}

void VisualisationWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    float aspectRatio = static_cast<float>(w) / static_cast<float>(h > 0 ? h : 1);

    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0f, aspectRatio, 0.1f, 1000.0f);
}

void VisualisationWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 mvp = projectionMatrix * viewMatrix;

    // 1. Draw Coordinate Axes
    axisShaderProgram.bind();
    axisShaderProgram.setUniformValue(axisMvpUniformLoc, mvp);
    glBindVertexArray(axesVAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);
    axisShaderProgram.release();

    // 2. Draw Instanced AMReX Boxes
    if (instanceCount > 0) {
        boxShaderProgram.bind();
        boxShaderProgram.setUniformValue(boxMvpUniformLoc, mvp);

        // PASS 1: Render Transparent Faces (no depth writes to prevent self-occlusion)
        glDepthMask(GL_FALSE);
        boxShaderProgram.setUniformValue("uIsFacePass", true);
        glBindVertexArray(boxFaceVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instanceCount);

        // PASS 2: Render Crisp Wireframe Outlines (depth writing enabled)
        glDepthMask(GL_TRUE);
        boxShaderProgram.setUniformValue("uIsFacePass", false);
        glBindVertexArray(boxVAO);
        glDrawArraysInstanced(GL_LINES, 0, 24, instanceCount);

        glBindVertexArray(0);
        boxShaderProgram.release();
    }
}

} // namespace boxer