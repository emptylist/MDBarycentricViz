#include "trajectorywindow.h"
#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include <QtGui/QMatrix4x4>

#include <iostream>
#include <QImage>
#include <QOpenGLFramebufferObject>

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

TrajectoryWindow::TrajectoryWindow(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_traj(NULL)
    , m_zoom(60.0f)
    , m_program(0)
    , m_xrot(0.0f)
    , m_yrot(0.0f)
    , m_zrot(0.0f)
    , m_translateVector(0.0f, -0.50f, -4.0f)
    , m_mouseModePan(false)
    , m_mousePressed(false)
{}

void TrajectoryWindow::bindTrajectory(BarycentricTrajectory &traj) {
    m_traj = &traj;
}

void TrajectoryWindow::mouseMoveEvent(QMouseEvent *ev) {
    if (m_mousePressed) {
        if (!m_mouseModePan) {
            m_xrot = fmod(m_xrot + (GLfloat)(ev->y() - m_mouseY) / 5.0f, 360.0f);
            m_yrot = fmod(m_yrot + (GLfloat)(ev->x() - m_mouseX) / 5.0f, 360.0f);
        } else {
            m_translateVector.setX(m_translateVector.x() + (GLfloat)(ev->x() - m_mouseX) / 100.0f);
            m_translateVector.setY(m_translateVector.y() - (GLfloat)(ev->y() - m_mouseY) / 100.0f);
        }
        m_mouseX = ev->x();
        m_mouseY = ev->y();
    }
    update();
}

void TrajectoryWindow::mousePressEvent(QMouseEvent *ev) {
    m_mouseX = ev->x();
    m_mouseY = ev->y();
    m_mousePressed = true;
}

void TrajectoryWindow::mouseReleaseEvent(QMouseEvent *ev) {
    Q_UNUSED(ev)
    m_mousePressed = false;
}

void TrajectoryWindow::toggleTranslate() {
    if (m_mouseModePan) {
        m_mouseModePan = false;
    } else { m_mouseModePan = true; }
}

void TrajectoryWindow::resetView() {
    m_yrot = 0.0f;
    m_xrot = 0.0f;
    m_zrot = 0.0f;
    m_translateVector = QVector3D(0.0f, 0.0f, -4.0f);
    update();
}

void TrajectoryWindow::setZoom(GLfloat zoom) {
    std::cout << zoom << std::endl;
    if ((zoom > 10.0f) && (zoom < 170.0f)) {
        m_zoom = zoom;
    }
    update();
}

GLfloat sgn(GLfloat val) { return val < 0.0f ? -1.0f : 1.0f; }

void TrajectoryWindow::wheelEvent(QWheelEvent *ev) {
    GLfloat zoomDelta = (GLfloat)ev->angleDelta().y() / 60.0f;
    GLfloat zoomDeltaFinal = fabs(zoomDelta) < 5.0f ? zoomDelta : sgn(zoomDelta) * 5.0f;
    setZoom(m_zoom - zoomDeltaFinal);
}

GLuint TrajectoryWindow::loadShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    return shader;
}

void TrajectoryWindow::initializeGL()
{
    initializeOpenGLFunctions();
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

void TrajectoryWindow::resizeGL(int w, int h) {
    m_projection.setToIdentity();
    m_projection.perspective(60.0f, w / float(h), 0.01f, 1000.0f);
}

void TrajectoryWindow::paintGL()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(m_zoom, 1.0f/1.0f, 0.1f, 100.0f);
    matrix.translate(m_translateVector);
    matrix.rotate(m_xrot, 1.0f, 0.0f, 0.0f);
    matrix.rotate(m_yrot, 0.0f, 1.0f, 0.0f);
    matrix.rotate(m_zrot, 0.0f, 0.0f, 1.0f);

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

    GLfloat colors[4*8*sizeof(GLfloat)];
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
}

void TrajectoryWindow::takeSnapshot() {
    QOpenGLFramebufferObjectFormat fmt;
    fmt.setSamples(16);
    QOpenGLFramebufferObject fbo(width(), height(), fmt);
    fbo.bind();
    paintGL();
    //glFlush();
    //glReadBuffer(GL_FRONT);
    QImage snapshot = fbo.toImage();
    if (!snapshot.save("test.jpeg")) {
        std::cout << "ERROR" << std::endl;
    }
    fbo.release();
}


/*
void TrajectoryWindow::keyPressEvent(QKeyEvent * k) {
    switch (k->key()) {
    case Qt::Key_S:
        m_xrot = fmod(m_xrot + 2.0f, 360.0f);
        break;
    case Qt::Key_W:
        m_xrot = fmod(m_xrot - 2.0f, 360.0f);
        break;
    case Qt::Key_D:
        m_yrot = fmod(m_yrot + 2.0f, 360.0f);
        break;
    case Qt::Key_A:
        m_yrot = fmod(m_yrot - 2.0f, 360.0f);
        break;
    case Qt::Key_Q:
        m_zrot = fmod(m_zrot + 2.0f, 360.0f);
        break;
    case Qt::Key_E:
        m_zrot = fmod(m_zrot - 2.0f, 360.0f);
        break;
    case Qt::Key_Space:
        resetView();
        break;
    case Qt::Key_T:
        if (m_mouseModePan) { m_mouseModePan = false; } else { m_mouseModePan = true; }
    default:
        break;
    }
    update();
}
*/
