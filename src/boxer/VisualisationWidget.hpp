#pragma once

#include "AMReX_AmrCore.H"
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <iostream>

namespace boxer {

class VisualisationWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

  public:
    struct BoxInstanceData {
        float lo[3]; // AMReX Box Low bounds  (x, y, z)
        float hi[3]; // AMReX Box High bounds (x, y, z)
        int level;   // Refinement level
        int isHalo;  // 0 = interior/normal, 1 = halo (32-bit for GPU attribute alignment)

        // Default constructor
        BoxInstanceData() : lo{0.0f, 0.0f, 0.0f}, hi{0.0f, 0.0f, 0.0f}, level(0), isHalo(0) {}

        // Constructor taking amrex::RealBox
        BoxInstanceData(const amrex::RealBox& rb, int lev, bool halo = false) : level(lev), isHalo(halo ? 1 : 0) {
            lo[0] = static_cast<float>(rb.lo(0));
            lo[1] = static_cast<float>(rb.lo(1));
            lo[2] = static_cast<float>(rb.lo(2));

            hi[0] = static_cast<float>(rb.hi(0));
            hi[1] = static_cast<float>(rb.hi(1));
            hi[2] = static_cast<float>(rb.hi(2));
        }
    };

    VisualisationWidget() = delete;
    VisualisationWidget(const amrex::AmrCore& container, bool showHalo, int ngrow, QWidget* parent = nullptr)
        : QOpenGLWidget(parent), container(container), showHalo(showHalo), coarsestDisplayLevel(0),
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

    const amrex::AmrCore& container;

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