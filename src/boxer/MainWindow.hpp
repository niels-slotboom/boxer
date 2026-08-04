#pragma once

#include "AmrMeshWrapper.hpp"
#include "VisualisationWidget.hpp"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QObject>
#include <QSpinBox>
#include <QVBoxLayout>

namespace amrex {
class AmrMesh;
}

namespace boxer {
class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    MainWindow(const amrex::AmrMesh& container, int ngrow = 0);

  private:
    void connectLevelSelect();
    void connectShowHalo();

  private:
    AmrMeshWrapper container; // reference to the displayed data

    VisualisationWidget visualisation;

    QSpinBox coarsestDisplayLevel;
    QSpinBox finestDisplayLevel;
    QCheckBox showHalo;
};
}; // namespace boxer