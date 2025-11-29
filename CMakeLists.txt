cmake_minimum_required(VERSION 3.19)
project(ear-blaster LANGUAGES CXX)

if (NOT WIN32)
    message(FATAL_ERROR "${PROJECT_NAME} only supports Windows as of now due to the use of global low-level keyboard hook for hotkey handling.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_MIN_VERSION 6.9)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Qml
    Quick
    Multimedia
    LinguistTools
)

qt_standard_project_setup(
    REQUIRES ${QT_MIN_VERSION}
    I18N_TRANSLATED_LANGUAGES pl
)

qt_add_translations(${PROJECT_NAME}
    TS_FILE_DIR i18n
    LUPDATE_OPTIONS
        -noobsolete
        -locations none
)

qt_add_executable(${PROJECT_NAME}
    resources/icon.rc
)

add_subdirectory(cpp)
add_subdirectory(ui)

qt_add_qml_module(${PROJECT_NAME}
    URI ui
    RESOURCES
        ${UI_RESOURCE_FILES}
    QML_FILES
        ${UI_QML_FILES}
    DEPENDENCIES
        ui-settings
        ui-components
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Qml
        Qt6::Quick
        Qt6::Multimedia
)

include(InstallRequiredSystemLibraries)
install(TARGETS ${PROJECT_NAME})

qt_generate_deploy_qml_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
)
install(SCRIPT ${deploy_script})

