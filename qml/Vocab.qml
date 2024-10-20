import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels
import lingo
Item {
    required property Wrapper wrapper

    ToolBar {
        id: toolbar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 50
        RowLayout{
            anchors.fill: parent
            Label {
                text: "HALLO"
                elide: Label.ElideRight
                // horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
            }
        }
    }

    TabBar {
        id: tabbar
        anchors.top: toolbar.botttom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        TabButton {
            text: qsTr("Definition")
        }
        TabButton {
            text: qsTr("Sentences")
        }
    }

    Component {
        id: wordclass_delegate
        Rectangle {
            required property wordclass_t modelData
            color: "#41b5d9"
            Layout.fillWidth: true
            Layout.bottomMargin: 5
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            radius: 8
            implicitHeight: childrenRect.height
            implicitWidth: childrenRect.width
            ColumnLayout {
                width: parent.width
                Label {
                    text: modelData.attribute
                    Layout.fillWidth: true
                    padding: 9
                }
                ColumnLayout {
                    width: parent.width
                    spacing: 5
                    Repeater {
                        function wordclass_determine_delegate() {
                            var children_delegate = modelData.determine_delegate()
                            if (children_delegate === DelegateEnum.Category)
                                return modelData.categories
                            else if (children_delegate === DelegateEnum.Definition)
                                return modelData.shortcut_definitions()
                            else if (children_delegate === DelegateEnum.Phrase)
                                return modelData.shortcut_phrases()
                        }
                        model: wordclass_determine_delegate()
                        DelegateChooser {
                            role: "repr"
                            DelegateChoice {
                                roleValue: DelegateEnum.Category
                                delegate: category_delegate
                            }
                            DelegateChoice {
                                roleValue: DelegateEnum.Definition
                                delegate: definition_delegate
                            }
                            DelegateChoice {
                                roleValue: DelegateEnum.Phrase
                                delegate: phrase_delegate
                            }
                        }
                    }
                }
            }
        }
    }


    Component {
        id: category_delegate
        Rectangle {
            required property category_t modelData
            color: "#e6d525"
            Layout.fillWidth: true
            Layout.bottomMargin: 5
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            radius: 8
            implicitHeight: childrenRect.height
            implicitWidth: childrenRect.width
            ColumnLayout {
                width: parent.width
                Label {
                    text: modelData.category
                    Layout.fillWidth: true
                    padding: 9
                }
                ColumnLayout {
                    width: parent.width
                    spacing: 5
                    Repeater {
                        function category_determine_delegate() {
                            var children_delegate = modelData.determine_delegate()
                            if (children_delegate === DelegateEnum.Definition)
                                return modelData.definitions
                            else if (children_delegate === DelegateEnum.Phrase)
                                return modelData.shortcut_phrases()
                        }
                        model: category_determine_delegate()
                        DelegateChooser {
                            role: "repr"
                            DelegateChoice {
                                roleValue: DelegateEnum.Definition
                                delegate: definition_delegate
                            }
                            DelegateChoice {
                                roleValue: DelegateEnum.Phrase
                                delegate: phrase_delegate
                            }
                        }
                    }
                }
            }
        }
    }


    Component {
        id: definition_delegate
        Rectangle {
            required property definition_t modelData
            color: "#e88931"
            Layout.fillWidth: true
            Layout.bottomMargin: 5
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            radius: 8
            implicitHeight: childrenRect.height
            implicitWidth: childrenRect.width
            ColumnLayout {
                width: parent.width
                Label {
                    text: modelData.definition
                    Layout.fillWidth: true
                    padding: 9
                }
                Repeater {
                    model: modelData.phrases
                    delegate: phrase_delegate
                }
            }
        }
    }

    Component {
        id: phrase_delegate
        Rectangle {
            required property phrase_t modelData
            color: "#32a854"
            Layout.fillWidth: true
            Layout.bottomMargin: 5
            Layout.leftMargin: 5
            Layout.rightMargin: 5
            radius: 8
            implicitHeight: childrenRect.height
            implicitWidth: childrenRect.width
            ColumnLayout {
                Label {
                    Layout.fillWidth: true
                    text: modelData.phrase
                    font.bold: true
                    padding: 9
                }
                Label {
                    Layout.fillWidth: true
                    text: modelData.def
                    padding: 9
                }
            }
        }
    }

    StackLayout {
        anchors.top: tabbar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: tabbar.currentIndex
        ScrollView {
            id: definitions_scroll_view
            Layout.fillHeight: true
            Layout.fillWidth: true
            contentWidth: availableWidth
            ColumnLayout {
                width: parent.width
                spacing: 5
                Repeater {
                    model: gae.get().classes
                    delegate: wordclass_delegate
                }
            }
        }
        ScrollView {
            id: sentences_scroll_view
            Layout.fillHeight: true
            Layout.fillWidth: true
            contentWidth: availableWidth
        }
    }
}
