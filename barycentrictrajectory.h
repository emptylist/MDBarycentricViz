#ifndef BARYCENTRICTRAJECTORY_H
#define BARYCENTRICTRAJECTORY_H

#include <datapoint.h>
#include <QVector3D>
#include <vector>
#include <GL/gl.h>

class BarycentricTrajectory
{
private:
    QVector3D m_v1;
    QVector3D m_v2;
    QVector3D m_v3;
    QVector3D m_v4;
    std::vector<DataPoint *> frames;
    GLfloat * m_vertices;
    size_t m_nVertices;

    bool f_projected;
    bool f_verticesCached;

    void project();

public:
    BarycentricTrajectory();
    ~BarycentricTrajectory();

    void newFrame(float, float, float, float);

    const QVector3D& v1() { return m_v1; }
    const QVector3D& v2() { return m_v2; }
    const QVector3D& v3() { return m_v3; }
    const QVector3D& v4() { return m_v4; }

    void v1(const QVector3D &v);
    void v2(const QVector3D &v);
    void v3(const QVector3D &v);
    void v4(const QVector3D &v);

    const GLfloat * vertices();
    size_t nVertices();

    const DataPoint& operator[](size_t);
};

#endif // BARYCENTRICTRAJECTORY_H
