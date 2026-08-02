#include "Boxer.hpp"
#include "AMReX_AmrCore.H"
#include "MainWindow.hpp"

#include <QApplication>
#include <qapplication.h>
#include <qnamespace.h>

namespace boxer {
void show(const amrex::AmrCore& container, int ngrow, bool blocking) {
    // Ensure Qt event loop
    if (!QApplication::instance()) {
        static int dummy_argc = 1;
        static char dummy_name[] = "Boxer";
        static char* dummy_argv[] = {dummy_name, nullptr};
        static QApplication app(dummy_argc, dummy_argv);
    }

    // open a window
    MainWindow* window = new MainWindow(container, ngrow);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();

    // conditionally run blocking logic
    if (blocking) {
        QEventLoop loop;
        QObject::connect(window, &QObject::destroyed, &loop, &QEventLoop::quit);
        loop.exec();
    } else {
        throw std::runtime_error("Non-blocking execution of boxer::show() has not been implemented yet.");
    }
}
} // namespace boxer