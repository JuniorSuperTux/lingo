import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import lingo.dict_types
Item {
    required property Wrapper wrapper

    TabBar {
        id: tabbar
        anchors.top: parent.top
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
        id: wordclass_collection
        ColumnLayout {
            id: root_layout
            width: parent.width
            spacing: 5
            Repeater {
                model: target_model.length
                Rectangle {
                    required property int index
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
                            text: target_model[index].attribute
                            Layout.fillWidth: true
                            padding: 9
                        }
                        Loader {
                            id: wordclass_sub_loader
                            property var target_model
                            readonly property var sub: parent.target_model[index]
                            function determine_model () {
                                if (wordclass_sub_loader.sourceComponent === category_collection)
                                    return sub.get_category_value()
                                else if (wordclass_sub_loader.sourceComponent === definition_collection)
                                    return wordclass_sub_loader.sub.get_definition_value()
                                else
                                    return wordclass_sub_loader.sub.get_definition_value()[0].phrases
                            }
                            function determine_component () {
                                if (wordclass_sub_loader.sub.has_categories())
                                    return category_collection
                                else {
                                    definition_model = wordclass_sub_loader.sub.get_definition_value()
                                    if (definition_model.length === 1 && !(definition_model[0].definition))
                                        return phrase_collection
                                    return definition_collection
                                }
                            }

                            Layout.fillWidth: true
                            // sourceComponent: has_categories ? category_collection : definition_collection
                            sourceComponent: determine_component()
                            onSourceComponentChanged: {
                                if (wordclass_sub_loader.sourceComponent)
                                    wordclass_sub_loader.target_model = wordclass_sub_loader.determine_model()
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: category_collection
        ColumnLayout {
            width: parent.width
            spacing: 5
            Repeater {
                model: target_model.length
                Rectangle {
                    required property int index
                    color: "#e6d525"
                    Layout.fillWidth: true
                    Layout.bottomMargin: 2.5
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5
                    radius: 8
                    implicitHeight: childrenRect.height
                    implicitWidth: childrenRect.width
                    ColumnLayout {
                        width: parent.width
                        Label {
                            Layout.fillWidth: true
                            text: target_model[index].category
                            padding: 9
                        }
                        Loader {
                            property var definition_models: target_model[index].definitions
                            id: definition_loader
                            Layout.fillWidth: true
                            sourceComponent: definition_collection
                        }
                    }
                }
            }
        }
    }

    Component {
        id: definition_collection
        ColumnLayout {
            width: parent.width
            spacing: 5
            Repeater {
                model: target_model.length
                Rectangle {
                    required property int index
                    color: "#e88931"
                    Layout.fillWidth: true
                    Layout.bottomMargin: 2.5
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5
                    radius: 8
                    implicitHeight: childrenRect.height
                    implicitWidth: childrenRect.width
                    ColumnLayout {
                        width: parent.width
                        Label {
                            Layout.fillWidth: true
                            text: target_model[index].definition
                            padding: 9
                        }
                        // Loader {
                        //     property var sentence_models: definition_models[index].sentences
                        //     id: sentence_loader
                        //     Layout.fillWidth: true
                        //     Layout.preferredHeight: childrenRect.height + 10
                        //     sourceComponent: sentence_collection
                        // }
                    }
                }
            }
        }
    }

    Component {
        id: phrase_collection
        ColumnLayout {
            width: parent.width
            spacing: 5
            Repeater {
                model: phrase_models.length
                Rectangle {
                    required property int index
                    color: "#32a852"
                    Layout.fillWidth: true
                    Layout.bottomMargin: 2.5
                    Layout.leftMargin: 5
                    Layout.rightMargin: 5
                    radius: 8
                    implicitHeight: childrenRect.height
                    implicitWidth: childrenRect.width
                    ColumnLayout {
                        Label {
                            Layout.fillWidth: true
                            text: phrase_models[index].phrase
                            font.bold: true
                            padding: 9
                        }
                        Label {
                            Layout.fillWidth: true
                            text: phrase_models[index].def
                            padding: 9
                        }
                    }
                }
            }
        }
    }

    // Component {
    //     id: sentence_collection
    //     ColumnLayout {
    //         width: parent.width
    //         spacing: 5
    //         Repeater {
    //             model: sentence_models.length
    //             Rectangle {
    //                 required property int index
    //                 color: "#32a852"
    //                 Layout.fillWidth: true
    //                 ColumnLayout {
    //                     width: parent.width
    //                     Text {
    //                         text: sentence_models[index].eng
    //                     }
    //                     Text {
    //                         text: sentence_models[index].chi
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }

    StackLayout {
        id: gglayout
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
            Loader {
                property var target_model: wrapper.get().classes
                id: wordclass_loader
                width: parent.width
                sourceComponent: wordclass_collection
            }
        }
    }
}
