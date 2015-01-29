import QtQuick 2.0

Rectangle {
    id: t9btn
    width: 50
    height: 30
    radius: 3

    property color c_normal: "#474747"
    property color c_pressed: "#606060"

    property color c_tx_normal: "white"
    property color c_tx_pressed: "black"

    property alias t9Text: t9btnText.text
    property alias t9Color: t9btnText.color

    property bool pressed: false

    color: c_normal
    border.color: "#3A3A3A"
    border.width: 1

    Text {
        id: t9btnText
        text: "test_text"
        anchors.centerIn: parent
        font.pointSize: 18
        color: "#B3B3B3"
    }

    MouseArea {
        anchors.fill: parent
        //onClicked: parent.clicked()
        onPressed: parent.state = "pressed"
        onReleased: parent.state = "released"
    }

    // TODO: dorobit zapinanie

    states: [
        State {
            name: "pressed"
            PropertyChanges { target: t9btn; color: c_pressed; t9Color: "#00CF96" }
        },

        State {
            name: "released"
            PropertyChanges { target: t9btn; color: c_normal }
        }
    ]
}
