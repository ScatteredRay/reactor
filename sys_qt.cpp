// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include <QtGui/QApplication>
#include <QTimer>
#include <QMouseEvent>
#include <assert.h>
#include "glwindow_qt.h"

#include "view_decl.h"

int InitWindowAndLoop(int argc, char** argv)
{
    QApplication a(argc, argv);
    GLWindow w;
    w.show();
    return a.exec();
}

GLProxy::GLProxy(QWidget* parent) : QGLWidget(parent)
{
    view = 0;
}

GLProxy::~GLProxy()
{
    FinishView(view);
}

void GLProxy::initializeGL()
{
    assert(!view); // Might have to clean up the view if this fires.
    view = InitView();
}

void GLProxy::resizeGL(int w, int h)
{
    ResizeView(view, w, h);
}

void GLProxy::paintGL()
{
    UpdateView(view);
}

// Input

int GetKeyCodeFromAscii(char key)
{
    if(key >= '0' && key <= '9')
        return (int)(key - '0' + Qt::Key_0);
    if(key >= 'A' && key <= 'Z')
        return (int)(key - 'A' + Qt::Key_A);
    if(key >= 'a' && key <= 'z')
        return (int)(key - 'a' + Qt::Key_A);

    return -1;
}

::MouseButtons ConvertQtButtons(Qt::MouseButtons buttons)
{
    ::MouseButtons ret = No_Button;
    if(buttons & Qt::LeftButton)
        ret = (::MouseButtons)(ret | Left_Button);
    if(buttons & Qt::RightButton)
        ret = (MouseButtons)(ret | Right_Button);
    if(buttons & Qt::MidButton)
        ret = (MouseButtons)(ret | Mid_Button);

    return ret;
}

void GLProxy::mousePressEvent(QMouseEvent* e)
{
    InputMouseEvent(GetInputHandler(view), e->x(), e->y(), ConvertQtButtons(e->buttons()), Mouse_Down);
}

void GLProxy::mouseReleaseEvent(QMouseEvent* e)
{
    InputMouseEvent(GetInputHandler(view), e->x(), e->y(), ConvertQtButtons(e->buttons()), Mouse_Up);
}

void GLProxy::mouseMoveEvent(QMouseEvent* e)
{
    InputMouseEvent(GetInputHandler(view), e->x(), e->y(), ConvertQtButtons(e->buttons()), Mouse_Move);
}

void keyPressEvent(QKeyEvent* e)
{
    InputReceiveKey(GetInputHandler(view), e->key(), Key_Down);
}

void keyReleaseEvent(QKeyEvent* e)
{
    InputReceiveKey(GetInputHandler(view), e->key(), Key_Up);
}

GLWindow::GLWindow(QWidget* parent) : QMainWindow(parent)
{
    gl = new GLProxy(this);
    setCentralWidget(gl);

    resize(1280, 768);

    // start a timer to run the loop.
    loop_timer = new QTimer(this);
    connect(loop_timer, SIGNAL(timeout()), gl, SLOT(updateGL()));
    loop_timer->start(20); // 50hz
}

GLWindow::~GLWindow()
{
    // Parentship should take care of the children, right?
}
