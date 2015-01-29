import QtQuick 2.0
import QtGraphicalEffects 1.0
import "templates.js" as Template

ListView {
    width: 100
    height: 30

    spacing: 30
    model: textHandler.predictCount
    clip: true
    orientation: ListView.Horizontal

    delegate:
        Rectangle {
            color: "transparent"
        Text {
            id: predicate
            anchors.centerIn: parent
            text: textHandler.predictionAt(index)
            font.pixelSize: 17
            font.bold: mouse.pressed
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            onClicked: textHandler.choosePrediction(index);
        }

        /* Fixed size */
        Component.onCompleted: {
            width = predicate.contentWidth + 10
            height = predicate.contentHeight + 10
        }
    }

}





