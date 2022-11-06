import QtQuick

Window {
    id: root

    width: 640
    height: 480
    visible: true
    title: qsTr("Barch")

    Content {
        anchors.fill: parent

        model: barch_model

        onFileProcessingRequested: function(path) {
            barch_model.onProcessingRequested(path);
        }
    }

    ErrorPopup {
        id: popup

        parentWindow: root
    }

    Connections {
        target: barch_model

        function onErrorPopupRequested(message) {
            popup.message = message;
            popup.show();
        }
    }
}
