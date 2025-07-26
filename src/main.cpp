#include <QApplication>
#include <QDebug>
#include "mainwindow.h"
#include "database/databasemanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Initialize database
    if (!DatabaseManager::instance().initialize()) {
        return -1;
    }

    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}