add_subdirectory(settings)
add_subdirectory(components)

qt_add_qml_module(${PROJECT_NAME}
    URI ui
    RESOURCES
        ../resources/ear-blaster.ico
        ../resources/pictogrammers/headphones.svg
        ../resources/pictogrammers/headphones-off.svg
        ../resources/pictogrammers/microphone.svg
        ../resources/pictogrammers/microphone-off.svg
        ../resources/pictogrammers/cog.svg
        ../resources/pictogrammers/back-arrow.svg
        ../resources/pictogrammers/play.svg
        ../resources/pictogrammers/stop.svg
        ../resources/pictogrammers/trashcan.svg
        ../resources/pictogrammers/undo-arrow.svg
    QML_FILES
        Main.qml
        MainScreen.qml
        SettingsScreen.qml
    DEPENDENCIES
        ui-settings
        ui-components
)
