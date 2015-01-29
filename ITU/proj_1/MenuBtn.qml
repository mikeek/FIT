import QtQuick 2.0
import "templates.js" as Template

Rectangle {
    width: 100
    height: 50
    color: clr_rectangle

    /* Color bind */
    property color clr_rectangle: Template.tmplMenuBtn.rectangle[Template.active]

    property alias label: mainText.text

    signal clicked()

    Text {
        id: mainText
        anchors.centerIn: parent
        text: ""
        font.pointSize: 14
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            parent.clicked()
        }
    }
}
