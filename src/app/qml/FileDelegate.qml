import QtQuick
import QtQuick.Layouts 1.15

Item {
    id: root

    property alias fileName: fileName.text
    property alias fileSize: fileSize.text
    property bool useAlternativeColor: false

    signal clicked()

    implicitHeight: layout.implicitHeight
    implicitWidth: layout.implicitWidth

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        hoverEnabled: true

        onClicked: root.clicked()
    }

    Background {
        anchors.fill: parent

        hovered: mouseArea.containsMouse
        active: mouseArea.containsPress

        defaultColor: internal.backgroundColor
        radius: 0
    }

    RowLayout {
        id: layout

        anchors.fill: parent
        spacing: 8

        Text {
            id: fileName

            Layout.fillWidth: true
            Layout.preferredHeight: 24
            Layout.leftMargin: 8

            elide: Text.ElideRight
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            id: fileSize

            Layout.preferredHeight: 24
            Layout.rightMargin: 8

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }
    }

    QtObject {
        id: internal

        readonly property color defaultColor: "#f0f4f5"
        readonly property color alternativeColor: Qt.lighter(defaultColor)
        readonly property color backgroundColor: root.useAlternativeColor ? defaultColor : alternativeColor
    }
}
