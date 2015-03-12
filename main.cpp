#include "trajectorywindow.h"
#include <QtGui/QGuiApplication>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);
    format.setSwapInterval(1);

    TrajectoryWindow window;
    window.setFormat(format);
    window.resize(960, 960);
    window.show();

    window.setAnimating(true);

    return app.exec();
}



