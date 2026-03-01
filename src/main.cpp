#include <QApplication>
#include <QFile>
#include <QIcon>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("LaunchPad Manager");
    app.setOrganizationName("LaunchPadManager");
    app.setWindowIcon(QIcon::fromTheme("applications-all"));

    // 加载 macOS 11 样式表
    QFile qss(":/styles/macos11.qss");
    if (qss.open(QFile::ReadOnly)) {
        app.setStyleSheet(qss.readAll());
        qss.close();
    }

    MainWindow w;
    w.show();

    return app.exec();
}