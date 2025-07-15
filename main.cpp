#include "mainwindow.h"
#include <QApplication>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QFont>
#include <QDebug>

#ifdef Q_OS_LINUX
#include <unistd.h>
#include <sys/sysinfo.h>
#include <libnotify/notify.h>
#include <csignal>
#endif

void setupLinuxEnvironment() {
#ifdef Q_OS_LINUX
    signal(SIGPIPE, SIG_IGN);

    struct sysinfo sys_info;
    if (sysinfo(&sys_info) == 0) {
        const double free_ram = (sys_info.freeram * sys_info.mem_unit) / (1024.0 * 1024.0);
        if (free_ram < 512.0) {
            qWarning() << "Low system memory (only" << free_ram << "MB free)";
        }
    }

    if (!notify_init("SeaBattle")) {
        qWarning() << "Failed to initialize libnotify";
    }
#endif
}

void configureApplicationStyles() {
    QStringList availableStyles = QStyleFactory::keys();
    if (availableStyles.contains("Fusion")) {
        QApplication::setStyle("Fusion");
    } else if (!availableStyles.isEmpty()) {
        QApplication::setStyle(availableStyles.first());
    }

    QFont defaultFont("Arial", 10);

#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0) && defined(Q_OS_LINUX)
    defaultFont.setFamilies({"Arial", "DejaVu Sans", "Liberation Sans", "Noto Sans", "Ubuntu"});
#else
    defaultFont.setFamily("Arial");
#endif

    QApplication::setFont(defaultFont);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53,53,53));
    palette.setColor(QPalette::WindowText, Qt::white);
    QApplication::setPalette(palette);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("SeaBattle");
    a.setApplicationVersion("1.0");
    a.setWindowIcon(QIcon(":/icons/app_icon.png"));

#ifdef Q_OS_LINUX
    setupLinuxEnvironment();
#endif

    configureApplicationStyles();

    MainWindow w;
    w.show();

#ifdef Q_OS_LINUX
    QObject::connect(&a, &QApplication::aboutToQuit, []() {
        notify_uninit();
    });
#endif

    return a.exec();
}
