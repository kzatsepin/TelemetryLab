#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

int main(int argc, char *argv[]) {
    // Create the core application object (required for non-GUI Qt apps)
    QCoreApplication app(argc, argv);

    qDebug() << "Application started. It will quit in 3 seconds...";

    // Connect a single-shot timer to quit the application after 3000 milliseconds (3 seconds)
    QTimer::singleShot(3000, &app, &QCoreApplication::quit);

    // Start the Qt event loop
    return app.exec();
}