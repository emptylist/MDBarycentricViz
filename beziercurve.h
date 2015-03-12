#ifndef BEZIERCURVE
#define BEZIERCURVE

#include <QVector3D>
struct BezierCurve {
    GLfloat * vertices;
    GLfloat * colors;
    int size;

    ~BezierCurve();
};

BezierCurve calcCurve(QVector3D &init,
                      QVector3D &end,
                      QVector3D &control,
                      int resolution) {
    BezierCurve curve;
    curve.size = resolution + 1;
    curve.vertices = (decltype(curve.vertices))malloc(3 * sizeof *curve.vertices * curve.size);
    curve.colors = (decltype(curve.colors))malloc(3 * sizeof *curve.colors * curve.size);
    float t;
    QVector3D p1;
    QVector3D p2;
    QVector3D result;

    QVector3D color1(0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
    QVector3D color2(255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f);
    QVector3D colorT;

    for (int i = 0; i < curve.size; i++) {
        t = (float)i / (curve.size - 1);
        p1 = ((1 - t) * init) + (t * control);
        p2 = ((1 - t) * control) + (t * end);
        result = ((1 - t) * p1) + (t * p2);
        curve.vertices[3*i] = result.x();
        curve.vertices[3*i+1] = result.y();
        curve.vertices[3*i+2] = result.z();

        colorT = ((1 - t) * color1) + (t * color2);
        curve.colors[3*i] = colorT.x();
        curve.colors[3*i+1] = colorT.y();
        curve.colors[3*i+2] = colorT.z();
    }

    return curve;
}

BezierCurve::~BezierCurve() {
    free(vertices);
    free(colors);
}
#endif // BEZIERCURVE

