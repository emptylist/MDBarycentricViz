#include "trajectorywindow.h"
#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include <QtGui/QMatrix4x4>

struct BezierCurve {
    GLfloat * vertices;
    GLfloat * colors;
    int size;

    ~BezierCurve();
};

BezierCurve calcCurve(QVector3D &init, QVector3D &end, QVector3D &control, int resolution) {
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

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";

TrajectoryWindow::TrajectoryWindow()
    : m_program(0)
    , m_frame(0)
    , m_rotationVector(1.0f, 1.0f, 1.0f)
{}

void TrajectoryWindow::setRotationV1() {
    m_rotationVector.setX(1.0f);
    m_rotationVector.setY(1.0f);
    m_rotationVector.setZ(1.0f);
}

void TrajectoryWindow::setRotationV2() {
    m_rotationVector.setX(1.0f);
    m_rotationVector.setY(-1.0f);
    m_rotationVector.setZ(-1.0f);
}

void TrajectoryWindow::setRotationV3() {
    m_rotationVector.setX(-1.0f);
    m_rotationVector.setY(1.0f);
    m_rotationVector.setZ(-1.0f);
}

void TrajectoryWindow::setRotationV4() {
    m_rotationVector.setX(-1.0f);
    m_rotationVector.setY(-1.0f);
    m_rotationVector.setZ(1.0f);
}

void TrajectoryWindow::keyPressEvent(QKeyEvent * k) {
    switch (k->key()) {
    case Qt::Key_Q:
        setRotationV1();
        break;
    case Qt::Key_W:
        setRotationV2();
        break;
    case Qt::Key_E:
        setRotationV3();
        break;
    case Qt::Key_R:
        setRotationV4();
        break;
    default:
        break;
    }
}

GLuint TrajectoryWindow::loadShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    return shader;
}

void TrajectoryWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");
    glLineWidth(3.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void TrajectoryWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(90.0f, 1.0f/1.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -4);
    matrix.rotate(10.0f * m_frame / screen()->refreshRate(), m_rotationVector);

    m_program->setUniformValue(m_matrixUniform, matrix);

    GLfloat vertices[] = {
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f
    };

    GLfloat * colors = (GLfloat *)malloc(4 * sizeof(GLfloat) * 8);
    for (int idx = 0; idx < 8; idx++) {
        colors[4*idx] = 0.0f;
        colors[(4*idx) + 1] = 0.0f;
        colors[(4*idx) + 2] = 0.0f;
        colors[(4*idx) + 3] = 1.0f;
    }
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(m_colAttr, 4, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glLineWidth(2.0);

    glDrawArrays(GL_LINE_STRIP, 0, 8);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    free(colors);

    QVector3D v1(1.0f, 1.0f, 1.0f);
    QVector3D v2(1.0f, -1.0f, -1.0f);
    QVector3D v3(0.0f, 0.0f, 0.0f);

    BezierCurve curve = calcCurve(v1, v2, v3, 200);
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, curve.vertices);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, curve.colors);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glLineWidth(4.0);

    glDrawArrays(GL_LINE_STRIP, 0, curve.size);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    m_program->release();

    ++m_frame;
}
