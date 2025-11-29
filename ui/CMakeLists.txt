add_subdirectory(settings)
add_subdirectory(components)

set(UI_RESOURCE_FILES
    resources/ear-blaster.ico
    resources/pictogrammers/headphones.svg
    resources/pictogrammers/headphones-off.svg
    resources/pictogrammers/microphone.svg
    resources/pictogrammers/microphone-off.svg
    resources/pictogrammers/cog.svg
    resources/pictogrammers/back-arrow.svg
    resources/pictogrammers/play.svg
    resources/pictogrammers/stop.svg
    resources/pictogrammers/trashcan.svg
    resources/pictogrammers/undo-arrow.svg
    PARENT_SCOPE
)

set(UI_QML_FILES
    ui/Main.qml
    ui/MainScreen.qml
    ui/SettingsScreen.qml
    PARENT_SCOPE
)
