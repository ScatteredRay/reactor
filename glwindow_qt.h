#ifndef _GLWINDOW_QT_H_
#define _GLWINDOW_QT_H_

#include <QMainWindow>
#include <QGLWidget>

class GLProxy : public QGLWidget
{
    Q_OBJECT
public:
    GLProxy(QWidget* parent = 0);
    ~GLProxy();
protected:
    void InitializeGL();
    void resizeGL(int w, int h);
    void paintGL();
};

class GLWindow : public QMainWindow
{
    Q_OBJECT
public:
    GLWindow(QWidget* parent = 0);
    ~GLWindow();
protected:
    GLProxy* gl;
    QTimer* loop_timer;
};

#endif //_GLWINDOW_QT_H_
