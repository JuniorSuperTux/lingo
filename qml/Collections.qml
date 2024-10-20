import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import lingo

Item {
    Dialog {
        id: newCollectionDialog
        standardButtons: Dialog.Ok | Dialog.Cancel
        modal: true // User cannot interact with bg
        anchors.centerIn: parent
        TextField {
            id: collectionName
            placeholderText: "Enter collection name"
        }
        onAccepted: {
            console.log("OK CLICKED")
            Collections.create_collection(collectionName.text)
            Collections.update_collections()
        }
    }

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
                    console.log("POPPING")
                    mainView.pop()
                }
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }
            Label {
                text: "Collections"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }
            Button {
                text: " + "
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                onClicked: {
                    newCollectionDialog.open()
                }
            }
        }
    }

    ListView {
        id: collectionsList
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: Collections.collections
        delegate: Row {
            width: collectionsList.width
            height: 20
            Button {
                width: parent.width
                Label {
                    id: collectionNameLabel
                    text: modelData
                }
                onClicked: {
                    mainView.push(Qt.resolvedUrl("VocabList.qml"),
                                  {"collectionName": collectionNameLabel.text})
                }
            }
            // TODO Add some more stuff here
        }
    }
}
