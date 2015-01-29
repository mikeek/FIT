import QtQuick 2.0
import QtGraphicalEffects 1.0
import "templates.js" as Template

/* Button on keyboard */
Rectangle {
    id: mainBody
    width: 100
    height: 62
    radius: 1

    color: clr_normal
    border.color: clr_border
    border.width: 1

    /* Color bind */
    property color clr_border: Template.tmplKeyButton.border[Template.active]
    property color clr_normal: Template.tmplKeyButton.normal[Template.active]
    property color clr_pressed: Template.tmplKeyButton.pressed[Template.active]
    property var clr_textNormal: Template.tmplKeyButton.textNormal[Template.active]
    property var clr_textPressed: Template.tmplKeyButton.textPressed[Template.active]
    property color clr_primaryText: Template.tmplKeyButton.primaryText[Template.active]
    property color clr_secondaryText: Template.tmplKeyButton.secondaryText[Template.active]
    property color clr_centerText: Template.tmplKeyButton.centerText[Template.active]

    /* Is button active (pressed) - mainly for SetupWindow */
    property bool activated: false

    /* Index in list */
    property int listIndex: 0

    /* Primary and secondary labels */
    property alias primaryLabel: primaryText.text
    property alias primaryLabelColor: primaryText.color
    property alias secondaryLabel: secondaryText.text
    property alias secondaryLabelColor: secondaryText.color
    property alias centerLabel: centerText.text
    property alias centerLabelColor: centerText.color

    /* Signals */
    signal pressedLong;
    signal pressed;
    signal clicked;
    signal released;
    signal activate
    signal deactivate

    /* Function called on key down */
    function press() {
        state = "pressed"
        pressed()
    }

    function pressLong() {
        state = "pressed";
        pressedLong()
    }

    /* Function called on key up */
    function release() {
        if (!activated) {
            state = "normal"
        }
        released();
    }

    onActivate: {
        activated = true
        state = "pressed"
    }
    onDeactivate: {
        activated = false;
        state = "normal"
    }

    /* Default press handler */
    property var pressHandler: mwin;
    onReleased: pressHandler.buttonReleased(listIndex)
    onPressed:  pressHandler.buttonPressed(listIndex)
    onPressedLong: pressHandler.buttonPressedLong(listIndex)

    Text {
        id: primaryText
        anchors.horizontalCenter: parent.horizontalCenter
        y: 7

        font.pointSize: 18
        color: clr_primaryText
    }

    Text {
        id: secondaryText
        anchors.horizontalCenter: parent.horizontalCenter
        y: 33
        font.pointSize: 12
        color: clr_secondaryText
    }

    Text {
        id: centerText
        anchors.centerIn: parent
        font.pixelSize: 22
        color: clr_centerText
        maximumLineCount: 3

    }

    MouseArea {
        anchors.fill: parent
        onClicked: parent.clicked();
        onPressed:  parent.press();
        onReleased: parent.release();
        onPressAndHold: parent.pressLong();
    }

    states: [
        State {
            name: "normal"
            PropertyChanges { target: mainBody; color: clr_normal }
        },
        State {
            name: "pressed"
            PropertyChanges { target: mainBody; color: clr_pressed }
            PropertyChanges { target: primaryText; color: clr_textPressed[0] }
            PropertyChanges { target: secondaryText; color: clr_textPressed[1] }
            PropertyChanges { target: centerText; color: clr_textPressed[2] }
        }
    ]
}
