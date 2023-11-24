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
        state.cpp \
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
    state.h \
    util.h \
    queue.hpp \
    video_output.h


mac {
    INCLUDEPATH += /Users/mac/WorkSpace/3rdparty/FFmpeg-n4.2.1/install/include
    LIBS += -L/Users/mac/WorkSpace/3rdparty/FFmpeg-n4.2.1/install/lib -lavcodec -lavdevice -lavfilter -lavformat -lavutil  -lswscale -lswresample

    INCLUDEPATH += /usr/local/include/SDL2
    LIBS += /usr/local/lib/libSDL2.dylib
}
