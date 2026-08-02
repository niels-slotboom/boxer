#include "VisualisationWidget.hpp"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>
#include <algorithm>

namespace boxer {
void VisualisationWidget::updateViewMatrix() {
    // Convert spherical coordinates (distance, azimuth, elevation) to Cartesian (X, Y, Z)
    float azRad = qDegreesToRadians(cameraAzimuth);
    float elRad = qDegreesToRadians(cameraElevation);

    float x = cameraDistance * qCos(elRad) * qSin(azRad);
    float y = cameraDistance * qSin(elRad);
    float z = cameraDistance * qCos(elRad) * qCos(azRad);

    QVector3D eye(x, y, z);
    QVector3D center(0.0f, 0.0f, 0.0f);
    QVector3D up(0.0f, 1.0f, 0.0f);

    viewMatrix.setToIdentity();
    viewMatrix.lookAt(eye, center, up);
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