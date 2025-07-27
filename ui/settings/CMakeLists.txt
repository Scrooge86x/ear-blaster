set_source_files_properties(AppSettings.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(ui-settings STATIC
    URI ui.settings
    QML_FILES
        AppSettings.qml
        SettingConnections.qml
)
