// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
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
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    // Input events
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
private:
    struct ViewInfo* view;
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
