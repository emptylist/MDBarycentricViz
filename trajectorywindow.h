#ifndef TRAJECTORYWINDOW_H
#define TRAJECTORYWINDOW_H

#include "openglwindow.h"
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QKeyEvent>



class TrajectoryWindow : public OpenGLWindow
{
public:
    TrajectoryWindow();

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

    void setRotationV1();
    void setRotationV2();
    void setRotationV3();
    void setRotationV4();

    void keyPressEvent(QKeyEvent *);

private:
    GLuint loadShader(GLenum type, const char *source);

    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    QOpenGLShaderProgram *m_program;
    int m_frame;
    QVector3D m_rotationVector;
};


#endif // TRAJECTORYWINDOW_H
