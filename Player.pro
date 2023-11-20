TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        audio_output.cpp \
        av_publish_time.cpp \
        decoder.cpp \
        demuxer.cpp \
        main.cpp \
        player.cpp \
        util.cpp \
        video_output.cpp

HEADERS += \
    audio_output.h \
    av_publish_time.h \
    decoder.h \
    demuxer.h \
    frame_queue.hpp \
    packet_queue.hpp \
    player.h \
    util.h \
    queue.hpp \
    video_output.h


mac {
    INCLUDEPATH += "/usr/local/include/"
        INCLUDEPATH += /usr/local/include/SDL2
    LIBS += -L/usr/local/lib -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswscale -lswresample
LIBS += /usr/local/lib/libSDL2.dylib
}
