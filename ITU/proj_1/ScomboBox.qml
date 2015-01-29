import QtQuick 2.0
import QtQuick.Controls 1.2
import "templates.js" as Template

Rectangle {
    id: styledComboBox
    width: 100
    height: 30
    smooth: true
    color: clr_styledComboBox
    border.width: 1
    border.color: clr_border

    /* Color bind */
    property color clr_styledComboBox: Template.tmplScomboBox.styledComboBox[Template.active]
    property color clr_border: Template.tmplScomboBox.border[Template.active]
    property color clr_fItemText: Template.tmplScomboBox.fItemText[Template.active]
    property color clr_glyph: Template.tmplScomboBox.glyph[Template.active]
    property color clr_popup: Template.tmplScomboBox.popup[Template.active]
    property color clr_popupItemsLv: Template.tmplScomboBox.popupItemsLv[Template.active]

    property variant source: []
    property string choosed: ""
    property int lvFlag: popupItems.currentIndex

    property alias selected: popupItems.currentIndex
    property alias label: fItemText.text

    signal choosedVal(var ix);

    ToolButton {
        id: firstItem
        width: parent.width


        Text {
            id: fItemText
            anchors.centerIn: parent
            color: clr_fItemText
            text: "Language"
            font.pixelSize: 18
        }

        Rectangle {
            id: glyph
            width: 20
            height: 20
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
            color: clr_glyph

            Image {
                id: arrow
                source: "/img/Arrow_down.png"
                anchors.fill: parent
            }

        }

        onClicked: {
            styledComboBox.state = styledComboBox.state === "roll" ? "hide" : "roll"
        }
    }

    Rectangle {
        id: popup
        width: parent.width
        height: 0
        clip: true

        anchors.top: firstItem.bottom
        anchors.right: firstItem.right
        color: clr_popup

        ListView {
            id: popupItems
            spacing: 1
            height: 500
            model: styledComboBox.source
            currentIndex: -1

            delegate: Rectangle {
                width: popup.width
                height: styledComboBox.height
                color: clr_popupItemsLv

                ToolButton {
                    anchors.fill: parent
                    text: styledComboBox.source[index]

                    onClicked: {
                        styledComboBox.state = "hide"
                        popupItems.currentIndex = index
                        styledComboBox.choosed = styledComboBox.source[index]
                        styledComboBox.choosedVal(index)
                    }
                }
            }
        }

        Behavior on height { NumberAnimation { duration: 200 } }
    }

    states: [ State {
                name: "roll"
                PropertyChanges {
                    target: popup
                    height: (styledComboBox.height + popupItems.spacing) * styledComboBox.source.length
                }
                PropertyChanges {
                    target: arrow
                    rotation: 180
                }
              },

              State {
                name: "hide"
                PropertyChanges {
                    target: fItemText
                    text: (lvFlag < 0) ? fItemText.text : styledComboBox.choosed
                }
                PropertyChanges {
                    target: arrow
                    rotation: 0
                }
              }
    ]
}
