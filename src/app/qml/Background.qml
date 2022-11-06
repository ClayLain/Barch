import QtQuick

Item {
    id: root

    property bool hovered: false
    property bool active: false

    property color defaultColor: "azure"
    property color hoveredColor: Qt.darker(defaultColor)
    property color activeColor: Qt.darker(hoveredColor)

    property alias radius: background.radius

    Rectangle {
        id: background

        anchors.fill: parent

        states: [
            State {
                name: "normal"
                when: !root.hovered && !root.active

                PropertyChanges {
                    target: background
                    color: root.defaultColor
                }
            },
            State {
                name: "hovered"
                when: root.hovered && !root.active

                PropertyChanges {
                    target: background
                    color: root.hoveredColor
                }
            },
            State {
                name: "active"
                when: root.active

                PropertyChanges {
                    target: background
                    color: root.activeColor
                }
            }
        ]
    }
}
