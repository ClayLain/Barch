import QtQuick

Item {
    id: root

    property alias model: view.model

    signal fileProcessingRequested(var path)

    implicitHeight: view.implicitHeight
    implicitWidth: view.implicitWidth

    ListView {
        id: view

        anchors.fill: parent

        interactive: true // QtQuick.Controls is forbidden. So no ScrollBar

        delegate: FileDelegate {
            width: 320

            fileName: model.fileName ? model.fileName : ""
            fileSize: model.sizeDescription ? model.sizeDescription : ""
            useAlternativeColor: index % 2

            onClicked: root.fileProcessingRequested(model.filePath)
        }
    }
}
