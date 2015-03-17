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

    QVector3D m_color1;
    QVector3D m_color2;
    QVector3D m_color3;

    std::vector<DataPoint *> frames;
    GLfloat * m_vertices;
    GLfloat * m_colors;
    size_t m_nVertices;
    size_t m_nColors;
    GLfloat m_lineWidth;

    bool f_projected;
    bool f_verticesCached;
    bool f_colorsCached;

    void project();
    void recalculateVertices();
    void recalculateColors();

public:
    BarycentricTrajectory();
    ~BarycentricTrajectory();

    void newFrame(float, float, float, float);

    const QVector3D& v1() const { return m_v1; }
    const QVector3D& v2() const { return m_v2; }
    const QVector3D& v3() const { return m_v3; }
    const QVector3D& v4() const { return m_v4; }

    void v1(const QVector3D &v);
    void v2(const QVector3D &v);
    void v3(const QVector3D &v);
    void v4(const QVector3D &v);

    const GLfloat * vertices();
    const GLfloat * colors();
    size_t nVertices() const;
    GLfloat width() { return m_lineWidth; }

    const DataPoint& operator[](size_t) const;
};

#endif // BARYCENTRICTRAJECTORY_H
