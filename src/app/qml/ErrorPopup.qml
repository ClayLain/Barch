import QtQuick
import QtQuick.Layouts 1.15

Item {
    id: root

    property Window parentWindow: null
    property string message: ""

    function show() {
        dialogLoader.active = true;
    }

    function close() {
        dialogLoader.active = false;
    }

    Loader {
        id: dialogLoader

        active: false

        sourceComponent: Window { // Didn't use "Dialog" because QtQuick.Controls is forbidden
            visible: true

            width: content.implicitWidth + internal.popupPadding
            height: content.implicitHeight + internal.popupPadding

            transientParent: root.parentWindow
            modality: Qt.WindowModal
            flags: Qt.Dialog

            onVisibleChanged: {
                if (!visible) {
                    dialogLoader.active = false;
                }
            }

            Item {
                id: container

                anchors.fill: parent
                focus: true

                Keys.onEnterPressed: root.close()
                Keys.onReturnPressed: root.close()

                ColumnLayout {
                    id: content

                    anchors.centerIn: parent

                    Text {
                        Layout.alignment: Qt.AlignCenter

                        text: root.message
                    }

                    DialogButton {
                        Layout.alignment: Qt.AlignCenter

                        text: qsTr("Ok")

                        onClicked: root.close()
                    }
                }
            }
        }
    }

    QtObject {
        id: internal

        readonly property int popupPadding: 24
    }
}
