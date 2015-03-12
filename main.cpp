#include "trajectorywindow.h"
#include "barycentrictrajectory.h"
#include <QtGui/QGuiApplication>
#include <QVector4D>
#include <random>
#include <iostream>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);
    format.setSwapInterval(1);

    QVector4D pos(0.0f, 0.0f, 0.0f, 0.0f);
    QVector4D v1(-1.0f, 1.0f, 1.0f, 1.0f);
    QVector4D v2(1.0f, -1.0f, 1.0f, 1.0f);
    QVector4D v3(1.0f, -1.0f, -1.0f, 1.0f);
    QVector4D v4(1.0f, 1.0f, -1.0f, -1.0f);
    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0f, 1.0f);
    BarycentricTrajectory trajectory;
    for (int i = 0; i < 100000; i++) {
        pos.setX(pos.x() + distribution(generator));
        pos.setY(pos.y() + distribution(generator));
        pos.setZ(pos.z() + distribution(generator));
        pos.setW(pos.w() + distribution(generator));
        trajectory.newFrame(pos.x(), pos.y(), pos.z(), pos.w());
        //trajectory.newFrame((v1-pos).length(), (v2-pos).length(), (v3-pos).length(), (v4-pos).length());
        //std::cout << pos.x() << std::endl;
    }

    TrajectoryWindow window;
    window.setFormat(format);
    window.resize(960, 960);
    window.bindTrajectory(trajectory);
    window.show();

    window.setAnimating(true);

    return app.exec();
}



