import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import lingo

Item {
    required property string collectionName
    ToolBar {
        id: toolbar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 50
        RowLayout {
            anchors.fill: parent
            Button {
                text: " ⇚ "
                onClicked: {
                    mainView.pop()
                }
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }
            Label {
                text: collectionName
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }
        }
    }
    ListView {
        id: vocabList
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        model: Collections.load_collection(collectionName)
        Component.onDestruction: {
            vocabList.model.listview_disown()
        }
        delegate: Button {
            width: vocabList.width
            height: 50
            Label {
                text: modelData
            }
        }
    }

    Button {
        id: addButton
        Label {
            text: "+"
            anchors.centerIn: parent
            color: "white"
        }
        background: Rectangle {
            color: "#6200EE"
            // radius: 15
        }

        height: 56
        width: 56
        z: 1
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 16
        onClicked: {

        }
    }
    MultiEffect {
        source: addButton
        anchors.fill: addButton
        shadowBlur: 1.0
        shadowEnabled: true
        shadowColor: "black"
        shadowVerticalOffset: 6
        shadowHorizontalOffset: 6
    }
}
