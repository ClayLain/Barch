set(headers
    ui/files_model.h
    ui/gui.h
)

set(source
    main.cpp
    ui/files_model.cpp
    ui/gui.cpp
)

set(qml_source
    qml/Background.qml
    qml/Content.qml
    qml/DialogButton.qml
    qml/ErrorPopup.qml
    qml/FileDelegate.qml
    qml/MainWindow.qml
)

qt_add_qml_module(qml_module
    URI qmlmodule
    VERSION 1.0
    QML_FILES ${qml_source}
    STATIC
)

add_executable(BarchApp ${source} ${headers})
target_link_libraries(BarchApp PRIVATE Qt6::Core Qt6::Quick Qt6::Concurrent qml_module BarchLib)
