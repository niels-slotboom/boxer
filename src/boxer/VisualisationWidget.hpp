#pragma once

#include "AmrMeshWrapper.hpp"
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

namespace amrex {
class AmrMesh;
}

namespace boxer {

class VisualisationWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

  public:
    VisualisationWidget() = delete;
    VisualisationWidget(const amrex::AmrMesh& container_, bool showHalo, int ngrow, QWidget* parent = nullptr)
        : QOpenGLWidget(parent), container(container_), showHalo(showHalo), coarsestDisplayLevel(0),
          finestDisplayLevel(container.finestLevel()), ngrow(ngrow) {}

    ~VisualisationWidget() override {
        makeCurrent();
        if (axesVAO)
            glDeleteVertexArrays(1, &axesVAO);
        if (axesVBO)
            glDeleteBuffers(1, &axesVBO);
        if (boxVAO)
            glDeleteVertexArrays(1, &boxVAO);
        if (boxFaceVAO)
            glDeleteVertexArrays(1, &boxFaceVAO);
        if (unitCubeVBO)
            glDeleteBuffers(1, &unitCubeVBO);
        if (boxFaceVBO)
            glDeleteBuffers(1, &boxFaceVBO);
        if (boxVBO)
            glDeleteBuffers(1, &boxVBO);
        doneCurrent();
    }

    void setShowHalo(bool value) {
        if (showHalo != value) {
            showHalo = value;
            updateBoxData();
        }
    }

    void setCoarsestDisplayLevel(int value) {
        if (coarsestDisplayLevel != value) {
            coarsestDisplayLevel = value;
            updateBoxData();
        }
    }

    void setFinestDisplayLevel(int value) {
        if (finestDisplayLevel != value) {
            finestDisplayLevel = value;
            updateBoxData();
        }
    }

    void updateBoxData();

  protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

  private:
    void updateViewMatrix();

    AmrMeshWrapper container;

    bool showHalo;
    int coarsestDisplayLevel;
    int finestDisplayLevel;
    int ngrow;

    // --- Shader Programs ---
    QOpenGLShaderProgram axisShaderProgram;
    QOpenGLShaderProgram boxShaderProgram;

    int axisMvpUniformLoc = -1;
    int boxMvpUniformLoc = -1;

    // --- OpenGL Handles ---
    // Axes
    GLuint axesVAO = 0;
    GLuint axesVBO = 0;

    // Instanced Boxes (Wireframe & Solid Faces)
    int instanceCount = 0;
    GLuint boxVAO = 0;      // VAO for wireframe outlines (GL_LINES)
    GLuint boxFaceVAO = 0;  // VAO for filled faces (GL_TRIANGLES)
    GLuint unitCubeVBO = 0; // VBO for 24-vertex wireframe cube
    GLuint boxFaceVBO = 0;  // VBO for 36-vertex triangulated cube
    GLuint boxVBO = 0;      // Shared instance VBO (BoxInstanceData array)

    // Matrices
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;

    // Orbit Camera
    float cameraDistance = 5.0f;
    float cameraAzimuth = 45.0f;
    float cameraElevation = 30.0f;

    QVector3D cameraTarget{0.0f, 0.0f, 0.0f};

    QPoint lastMousePosition;
};

} // namespace boxer