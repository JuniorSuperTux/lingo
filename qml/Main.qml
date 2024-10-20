import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    StackView {
        id: mainView
//        initialItem: Vocab {
//            wrapper: gae
//        }
        initialItem: Item {
            GridLayout {
                width: mainView.width
                height: mainView.height
                columns: 2
                columnSpacing: 10
                rowSpacing: 10
                Button {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Label {
                        text: "Collections"
                    }
                    onClicked: {
                        mainView.push(Qt.resolvedUrl("Collections.qml"))
                    }
                }
                Button {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Label {
                        text: "Volcabulary cards"
                    }
                }
                Button {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Label {
                        text: "Settings"
                    }
                }
            }
        }

        anchors.fill: parent
    }

}
