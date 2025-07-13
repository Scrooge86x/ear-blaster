cmake_minimum_required(VERSION 3.16)
project(ear-blaster LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS
    Quick
    Multimedia
    LinguistTools
    Core
)

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES pl)

qt_add_translations(ear-blaster
    TS_FILE_DIR i18n
    LUPDATE_OPTIONS -noobsolete
)

qt_add_executable(${PROJECT_NAME}
    resources/icon.rc
    source/main.cpp
    source/globalkeylistener.h source/globalkeylistener.cpp
    source/translator.h source/translator.cpp
    source/audiosystem/soundeffect.h source/audiosystem/soundeffect.cpp
    source/audiosystem/audiosystem.h source/audiosystem/audiosystem.cpp
    source/audiosystem/microphonepassthrough.h source/audiosystem/microphonepassthrough.cpp
    source/audiosystem/audiodevice.h source/audiosystem/audiodevice.cpp
)

set_source_files_properties(qml/AppSettings.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(${PROJECT_NAME}
    URI ear-blaster
    VERSION 1.0
    RESOURCES
        resources/ear-blaster.ico
    QML_FILES
        qml/Main.qml
        qml/MainScreen.qml
        qml/SettingsScreen.qml
        qml/SoundElement.qml
        qml/SoundList.qml
        qml/SequenceInput.qml
        qml/AppSettings.qml
        qml/VolumeInput.qml
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt6::Qml
        Qt6::Multimedia
        Qt6::Core
)
