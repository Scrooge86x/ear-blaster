qt_add_library(AudioSystem STATIC
    soundeffect.h soundeffect.cpp
    audiosystem.h audiosystem.cpp
    microphonepassthrough.h microphonepassthrough.cpp
    audiodevice.h audiodevice.cpp
    audiooutput.h audiooutput.cpp
    texttospeech.h texttospeech.cpp
    audioshared.h
)

target_link_libraries(AudioSystem
    PUBLIC
        Qt6::Multimedia
        Qt6::TextToSpeech
)
target_include_directories(AudioSystem PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
