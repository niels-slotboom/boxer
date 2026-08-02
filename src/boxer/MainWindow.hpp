#pragma once

#include "VisualisationWidget.hpp"
#include <AMReX_AmrCore.H>

#include <QBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QMainWindow>
#include <QSpinBox>
#include <qtmetamacros.h>

namespace boxer {
class MainWindow : public QMainWindow {
    Q_OBJECT
  public:
    MainWindow() = delete;
    MainWindow(const amrex::AmrCore& container);

  private:
    void connectLevelSelect();
    void connectShowHalo();

  private:
    const amrex::AmrCore& container; // reference to the displayed data

    VisualisationWidget visualisation;

    QSpinBox coarsestDisplayLevel;
    QSpinBox finestDisplayLevel;
    QCheckBox showHalo;
};
}; // namespace boxer