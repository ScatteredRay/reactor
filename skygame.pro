QT += opengl
TARGET = SkyGame
TEMPLATE = app
LIBS += -framework SDL
INCLUDEPATH += /Library/Frameworks/SDL.framework/Headers/
SOURCES += main.cpp sys_qt.cpp sky_view.cpp simple_shader.cpp simple_mesh.cpp reporting.cpp editor_meshes.cpp obj_load.cpp input.cpp core_systems.cpp
HEADERS += glwindow_qt.h
