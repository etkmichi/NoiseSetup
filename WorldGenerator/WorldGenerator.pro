#-------------------------------------------------
#
# Project created by QtCreator 2017-09-25T11:12:26
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = WorldGenerator
TEMPLATE = lib


SOURCES += main.cpp\
        mainwindow.cpp \
    mapview.cpp \
    mf_thread_ogl.cpp \
    mf_thread_noise.cpp \
    helper/mf_controls.cpp \
    mf_gl_view.cpp \
    mf_noise.cpp \
    mf_signal_ctrl.cpp \
    mf_xy_diagramm.cpp \
    mf_map_generator.cpp \
    mf_settings_noise.cpp \
    mf_settings_color.cpp \
    mf_noise_combinor.cpp \
    mf_noise_generator.cpp \
    mf_noise_selector.cpp \
    mf_noise_transformer.cpp \
    mf_noise_modifier.cpp \
    mf_engine_pipeline.cpp

HEADERS  += mainwindow.h\
    mapview.h \
    helper/mf_def.h \
    helper/mf_structs.h \
    mf_thread_ogl.h \
    mf_thread_noise.h \
    mf_gl_view.h \
    helper/mf_controls.hpp \
    mf_noise.h \
    mf_signal_ctrl.h \
    mf_xy_diagramm.h \
    mf_map_generator.h \
    mf_settings_color.h \
    mf_settings_noise.h \
    mf_noise_combinor.h \
    mf_noise_generator.h \
    mf_noise_selector.h \
    mf_noise_transformer.h \
    mf_noise_modifier.h \
    mf_engine_pipeline.h

LIBS    += -lnoise\
            -lGLEW\
            -lGL\
            -lGLU\
            -lglfw

FORMS    += mainwindow.ui \
    mf_noise_settings.ui \
    mf_noise_generator.ui \
    mf_noise_combinor.ui \
    mf_color_settings.ui \
    mf_xy_diagramm.ui \
    mf_noise_selector.ui \
    mf_noise_transformer.ui \
    mf_noise_modifier.ui

