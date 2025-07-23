add_subdirectory(settings)
add_subdirectory(components)

qt_add_qml_module(${PROJECT_NAME}
    URI ui
    RESOURCES
        ../resources/ear-blaster.ico
    QML_FILES
        Main.qml
        MainScreen.qml
        SettingsScreen.qml
    DEPENDENCIES
        ui-settings
        ui-components
)
