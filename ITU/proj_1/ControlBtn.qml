import QtQuick 2.0
import "templates.js" as Template

Rectangle {
    id: btn
    width: 90
    height: 40

    /* Color bind */
    property color clr_border: Template.tmplControlBtn.border[Template.active]
    property color clr_normal: Template.tmplControlBtn.normal[Template.active]
    property color clr_pressed: Template.tmplControlBtn.pressed[Template.active]
    property color clr_btnText: Template.tmplControlBtn.btnText[Template.active]



    border.color: clr_border
    border.width: 1

    color: clr_normal

    signal clicked;

    /* Set aliases */
    property alias label: btnText.text
    property alias fontColor: btnText.color

    Text {
        anchors.centerIn: parent
        id: btnText
        text: "Click me!"
        color: clr_btnText
        font.pointSize: 12
    }

    MouseArea {
        anchors.fill: parent
        onClicked:  parent.clicked()
        onPressed:  parent.state = "pressed"
        onReleased: parent.state = "normal"
    }

    /* Set states */
    states: [
        State {
            name: "normal"
            PropertyChanges { target: btn; color: clr_normal }
        },
        State {
            name: "pressed"
            PropertyChanges { target: btn; color: clr_pressed }
        }

    ]
}
