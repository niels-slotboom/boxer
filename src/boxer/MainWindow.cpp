#include "MainWindow.hpp"

namespace boxer {
MainWindow::MainWindow(const amrex::AmrCore& container) : container(container) {
    setMinimumSize(800, 600);

    auto* centralWidget = new QWidget(this);

    auto* layout = new QVBoxLayout(centralWidget);
    layout->addWidget(&openGLPlaceHolder, 1);

    auto* settingsLayout = new QHBoxLayout();

    int currentFinestLevel = container.finestLevel();

    settingsLayout->addWidget(new QLabel("Coarsest Level:"));
    settingsLayout->addWidget(&coarsestDisplayLevel);
    coarsestDisplayLevel.setFixedWidth(60);
    coarsestDisplayLevel.setMinimum(0);
    coarsestDisplayLevel.setValue(0);
    coarsestDisplayLevel.setMaximum(currentFinestLevel);

    settingsLayout->addWidget(new QLabel("Finest Level:"));
    settingsLayout->addWidget(&finestDisplayLevel);
    finestDisplayLevel.setFixedWidth(60);
    finestDisplayLevel.setMinimum(coarsestDisplayLevel.value());
    finestDisplayLevel.setValue(currentFinestLevel);
    finestDisplayLevel.setMaximum(currentFinestLevel);

    settingsLayout->addWidget(new QLabel("Show Halo:"));
    settingsLayout->addWidget(&showHalo);
    settingsLayout->addStretch(1);
    layout->addLayout(settingsLayout, 0);

    connectLevelSelect();

    setCentralWidget(centralWidget);
}

void MainWindow::connectLevelSelect() {
    // When coarsestDisplayLevel changes, finestLevel can't drop below it
    connect(&coarsestDisplayLevel, &QSpinBox::valueChanged, this, [this](int newCoarsest) {
        finestDisplayLevel.setMinimum(newCoarsest);
        // finestDisplayLevel.maximum stays permanently at container.finestLevel()
    });

    // When finestDisplayLevel changes, coarsestLevel can't exceed it
    connect(&finestDisplayLevel, &QSpinBox::valueChanged, this, [this](int newFinest) {
        coarsestDisplayLevel.setMaximum(newFinest);
        // coarsestDisplayLevel.minimum stays permanently at 0
    });
}
}; // namespace boxer