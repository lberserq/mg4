TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -fopenmp
QMAKE_LFLAGS += -fopenmp
SOURCES += main.cpp \
    Tracer.cpp \
    Scene.cpp \
    l3ds.cpp \
    EasyBMP/EasyBMP.cpp \
    Mesh.cpp \
    model.cpp \
    texture.cpp \
    Types.cpp

HEADERS += \
    Camera.h \
    Types.h \
    Tracer.h \
    Scene.h \
    Mesh.h \
    l3ds.h \
    EasyBMP/EasyBMP_BMP.h \
    EasyBMP/EasyBMP.h \
    EasyBMP/EasyBMP_VariousBMPutilities.h \
    EasyBMP/EasyBMP_DataStructures.h \
    model.h \
    texture.h

