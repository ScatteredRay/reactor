// Copyright (c) 2010, Nicholas "Indy" Ray. All rights reserved.
// See the LICENSE file for usage, modification, and distribution terms.
#include <QtGui/QApplication>
#include <QTimer>
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
