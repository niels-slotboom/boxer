#include "Boxer.hpp"
#include "MainWindow.hpp"

#include <QApplication>
#include <QEventLoop>
#include <stdexcept>

namespace boxer {

void show(const amrex::AmrMesh& container, int ngrow, bool blocking) {
    // Ensure Qt event loop exists
    if (!QApplication::instance()) {
        static int dummy_argc = 1;
        static char dummy_name[] = "Boxer";
        static char* dummy_argv[] = {dummy_name, nullptr};
        static QApplication app(dummy_argc, dummy_argv);
    }

    // Allocate window on heap; WA_DeleteOnClose cleans it up when closed
    auto* window = new MainWindow(container, ngrow);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();

    if (blocking) {
        QEventLoop loop;
        QObject::connect(window, &QObject::destroyed, &loop, &QEventLoop::quit);
        loop.exec();
    } else {
        throw std::runtime_error("Non-blocking execution of boxer::show() has not been implemented yet.");
    }
}

} // namespace boxer