#include "VisualisationWidget.hpp"
namespace boxer {
static const char* vertexShaderSource = R"(
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

static const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vertColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vertColor, 1.0);
}
)";

void VisualisationWidget::initializeGL() {
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glLineWidth(2.0f);
    glClearColor(0.05f, 0.05f, 0.06f, 1.0f);

    // 1. Build and Link Shader Program using Qt's helper class
    shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    shaderProgram.link();

    mvpUniformLoc = shaderProgram.uniformLocation("mvp");

    // 2. Define Axis Vertices: Position (x,y,z), Color (r,g,b)
    // Interleaved layout: [X, Y, Z, R, G, B]
    std::vector<float> axisVertices = {// X Axis (Red) - from (-1.0, 0, 0) to (1.0, 0, 0)
                                       -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

                                       // Y Axis (Green) - from (0, -1.0, 0) to (0, 1.0, 0)
                                       0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

                                       // Z Axis (Blue) - from (0, 0, -1.0) to (0, 0, 1.0)
                                       0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    glGenVertexArrays(1, &axesVAO);
    glGenBuffers(1, &axesVBO);

    glBindVertexArray(axesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
    glBufferData(GL_ARRAY_BUFFER, axisVertices.size() * sizeof(float), axisVertices.data(), GL_STATIC_DRAW);

    // Attribute 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Attribute 1: Position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    updateViewMatrix();
}

void VisualisationWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    // Compute Perspective Projection Matrix
    float aspectRatio = static_cast<float>(w) / static_cast<float>(h > 0 ? h : 1);

    projectionMatrix.setToIdentity();
    // Field of View: 45 deg, Aspect Ratio, Near plane: 0.1, Far plane: 100.0
    projectionMatrix.perspective(45.0f, aspectRatio, 0.1f, 100.0f);
}

void VisualisationWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind Shader Program
    shaderProgram.bind();

    // Compute Model View Projection (MVP) Matrix
    QMatrix4x4 modelMatrix; // Identity
    QMatrix4x4 mvp = projectionMatrix * viewMatrix * modelMatrix;

    // Pass MVP matrix to shader
    shaderProgram.setUniformValue(mvpUniformLoc, mvp);

    // Draw Axes
    glBindVertexArray(axesVAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);

    shaderProgram.release();
}
} // namespace boxer