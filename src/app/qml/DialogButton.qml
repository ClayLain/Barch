import QtQuick

Item {
    id: root

    property alias text: label.text

    signal clicked()

    implicitHeight: label.implicitHeight + (internal.padding * 2)
    implicitWidth: label.implicitWidth + (internal.padding * 2)

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

        radius: 5
    }

    Text {
        id: label

        anchors.centerIn: parent
    }

    QtObject {
        id: internal

        readonly property int padding: 8
    }
}
