#ifndef TRAJECTORYWINDOW_H
#define TRAJECTORYWINDOW_H

#include "barycentrictrajectory.h"
#include <QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QKeyEvent>
#include <QOpenGLWidget>

class TrajectoryWindow : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
private:
    QMatrix4x4 m_projection;
    BarycentricTrajectory * m_traj;
    GLuint loadShader(GLenum type, const char *source);

    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    GLfloat m_zoom;

    QOpenGLShaderProgram *m_program;

    GLfloat m_xrot;
    GLfloat m_yrot;
    GLfloat m_zrot;
    int m_mouseX;
    int m_mouseY;
    QVector3D m_translateVector;

    bool m_mouseModePan;
    bool m_mousePressed;
    void setZoom(GLfloat);
    void resetView();
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    //void keyPressEvent(QKeyEvent *);
    void wheelEvent(QWheelEvent *);

private slots:
    void toggleTranslate();
    void takeSnapshot();

public:
    TrajectoryWindow(QWidget *parent = 0);

    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

    void bindTrajectory(BarycentricTrajectory&);
};


#endif // TRAJECTORYWINDOW_H
