#include <QtGui/QApplication>
#include <QTimer>
#include "glwindow_qt.h"

int InitWindowAndLoop(int argc, char** argv)
{
    QApplication a(argc, argv);
    GLWindow w;
    w.show();
    return a.exec();
}


GLProxy::GLProxy(QWidget* parent) : QGLWidget(parent)
{}

GLProxy::~GLProxy()
{}

void GLProxy::InitializeGL()
{}

void GLProxy::resizeGL(int w, int h)
{}

void GLProxy::paintGL()
{
    static float r = 1.0f;
    glClearColor(r, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(r == 1.0f) r = 0.0f;
    else r = 1.0f;
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
