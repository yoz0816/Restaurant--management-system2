#include "loginwindow.h"

#include <QApplication>
#include <QScreen>
#include <QIcon>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QApplication::setApplicationName("Restaurant Management System");

    app.setStyle("Fusion");

    LoginWindow window;
    window.resize(400, 300);

    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        int x = screenGeometry.x() + (screenGeometry.width() - window.width()) / 2;
        int y = screenGeometry.y() + (screenGeometry.height() - window.height()) / 2;
        window.move(x, y);
    }

    window.show();

    return app.exec();
}