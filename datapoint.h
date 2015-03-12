#ifndef DATAPOINT_H
#define DATAPOINT_H

#include <QVector4D>
#include <QVector3D>

class DataPoint : protected QVector4D
{
private:
    QVector3D projected;
    DataPoint();
public:
    DataPoint(float d1, float d2, float d3, float d4);
    ~DataPoint();

    void project(const QVector3D&,
                 const QVector3D&,
                 const QVector3D&,
                 const QVector3D&);

    float projectedX() { return projected.x(); }
    float projectedY() { return projected.y(); }
    float projectedZ() { return projected.z(); }
};

#endif // DATAPOINT_H
