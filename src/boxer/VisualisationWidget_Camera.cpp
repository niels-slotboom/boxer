#include "VisualisationWidget.hpp"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>
#include <algorithm>

namespace boxer {
void VisualisationWidget::updateViewMatrix() {
    float radAzimuth = -qDegreesToRadians(cameraAzimuth);
    float radElevation = qDegreesToRadians(cameraElevation);

    // Spherical coordinates mapped to Z-up:
    // X = right, Y = forward/depth, Z = up
    float x = -cameraDistance * std::cos(radElevation) * std::sin(radAzimuth);
    float y = -cameraDistance * std::cos(radElevation) * std::cos(radAzimuth);
    float z = cameraDistance * std::sin(radElevation);

    QVector3D eyePosition = cameraTarget + QVector3D(x, y, z);
    QVector3D upVector(0.0f, 0.0f, 1.0f); // Set Z as Up vector

    viewMatrix.setToIdentity();
    viewMatrix.lookAt(eyePosition, cameraTarget, upVector);

    update();
}

void VisualisationWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePosition = event->pos();
    }
}

void VisualisationWidget::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        QPoint delta = event->pos() - lastMousePosition;
        lastMousePosition = event->pos();

        // Sensitivity factor for orbit rotational speed
        constexpr float sensitivity = 0.25f;

        cameraAzimuth -= delta.x() * sensitivity;
        cameraElevation += delta.y() * sensitivity;

        // Clamp elevation to prevent screen flipping at the poles (-89 to +89 degrees)
        cameraElevation = std::clamp(cameraElevation, -89.0f, 89.0f);

        updateViewMatrix();
        update(); // Request GL redraw
    }
}

void VisualisationWidget::wheelEvent(QWheelEvent* event) {
    // QWheelEvent delta is typically 120 units per notch
    float numDegrees = event->angleDelta().y() / 8.0f;
    float numSteps = numDegrees / 15.0f;

    // Exponential zoom scale (10% change per step)
    float zoomFactor = (numSteps > 0) ? 0.9f : 1.1f;
    cameraDistance *= zoomFactor;

    // Enforce safe near/far camera distance limits
    cameraDistance = std::clamp(cameraDistance, 0.5f, 100.0f);

    updateViewMatrix();
    update(); // Request GL redraw
}
}; // namespace boxer