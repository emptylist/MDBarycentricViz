#include "datapoint.h"

DataPoint::DataPoint(float d1, float d2, float d3, float d4)
{
    setX(d1);
    setY(d2);
    setZ(d3);
    setW(d4);
}

DataPoint::~DataPoint() {}

void DataPoint::project(const QVector3D& v1,
                        const QVector3D& v2,
                        const QVector3D& v3,
                        const QVector3D& v4) {

    QVector4D barycentric = QVector4D(1.0f, 1.0f, 1.0f, 1.0f) - normalized();
    projected.setX(barycentric[0] * v1.x() +
                   barycentric[1] * v2.x() +
                   barycentric[2] * v3.x() +
                   barycentric[3] * v4.x());

    projected.setY(barycentric[0] * v1.y() +
                   barycentric[1] * v2.y() +
                   barycentric[2] * v3.y() +
                   barycentric[3] * v4.y());

    projected.setZ(barycentric[0] * v1.z() +
                   barycentric[1] * v2.z() +
                   barycentric[2] * v3.z() +
                   barycentric[3] * v4.z());
}
