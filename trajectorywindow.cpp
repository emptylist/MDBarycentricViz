#include "trajectorywindow.h"
#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include <QtGui/QMatrix4x4>

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
    : m_traj(NULL)
    , m_program(0)
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

void TrajectoryWindow::bindTrajectory(BarycentricTrajectory &traj) {
    m_traj = &traj;
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
     case Qt::Key_Space:
        if (animating()) { setAnimating(false); } else { setAnimating(true); }
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
    matrix.perspective(60.0f, 1.0f/1.0f, 0.1f, 100.0f);
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

    if (m_traj != NULL) {
        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, m_traj->vertices());
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, m_traj->colors());

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glLineWidth(m_traj->width());

        glDrawArrays(GL_LINE_STRIP, 0, m_traj->nVertices());
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    m_program->release();

    ++m_frame;
}
