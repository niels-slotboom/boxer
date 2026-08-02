#pragma once

#include "AMReX_AmrCore.H"
#include <QMatrix4x4>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <qtmetamacros.h>

namespace boxer {
class VisualisationWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT
  public:
    VisualisationWidget() = delete;
    // Pass QWidget* parent to support Qt parent-child hierarchy
    VisualisationWidget(const amrex::AmrCore& container, bool showHalo, QWidget* parent = nullptr)
        : QOpenGLWidget(parent), container(container), showHalo(showHalo), coarsestDisplayLevel(0),
          finestDisplayLevel(container.finestLevel()) {}

    ~VisualisationWidget() override {
        // Clean up GPU memory when widget is destroyed
        makeCurrent();
        if (axesVAO)
            glDeleteVertexArrays(1, &axesVAO);
        if (axesVBO)
            glDeleteBuffers(1, &axesVBO);
        doneCurrent();
    };

    void setShowHalo(bool value) {
        if (showHalo != value) {
            showHalo = value;
            update(); // Schedules paintGL() safely via Qt event loop
        }
    }

    void setCoarsestDisplayLevel(int value) {
        if (coarsestDisplayLevel != value) {
            coarsestDisplayLevel = value;
            update(); // Schedules paintGL() safely via Qt event loop
        }
    }

    void setFinestDisplayLevel(int value) {
        if (finestDisplayLevel != value) {
            finestDisplayLevel = value;
            update(); // Schedules paintGL() safely via Qt event loop
        }
    }

  protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // Qt Input Event Handlers (Implemented in VisualisationWidget_Camera.cpp)
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

  private:
    void updateViewMatrix();

    const amrex::AmrCore& container;

    bool showHalo;
    int coarsestDisplayLevel;
    int finestDisplayLevel;

    // OpenGL Handles
    QOpenGLShaderProgram shaderProgram;
    GLuint axesVAO = 0;
    GLuint axesVBO = 0;
    int mvpUniformLoc = -1;

    // Matrices
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 viewMatrix;

    // Orbit Camera Parameters
    float cameraDistance = 5.0f;   // Distance r from origin
    float cameraAzimuth = 45.0f;   // Horizontal angle theta (degrees)
    float cameraElevation = 30.0f; // Vertical angle phi (degrees)

    QPoint lastMousePosition; // Mouse drag tracking
};
} // namespace boxer