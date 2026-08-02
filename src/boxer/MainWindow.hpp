#pragma once

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

  private:
    const amrex::AmrCore& container; // reference to the displayed data

    QWidget openGLPlaceHolder;

    QSpinBox coarsestDisplayLevel;
    QSpinBox finestDisplayLevel;
    QCheckBox showHalo;
};
}; // namespace boxer