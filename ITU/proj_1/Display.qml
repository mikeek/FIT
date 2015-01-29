import QtQuick 2.0
import QtQuick.Controls 1.2
import "templates.js" as Template

Rectangle {
    width: 200
    height: 100
    color: clr_rectangle
    border.width: 0
    border.color: clr_border

    /* Color bind */
    property color clr_rectangle: Template.tmplDisplay.rectangle[Template.active]
    property color clr_border: Template.tmplDisplay.border[Template.active]


    property alias text: mainText.text

    /* Write string */
    function write(str) {
        mainText.insert(mainText.length, str);
    }

    /* Remove all characters */
    function removeAll() {
        mainText.remove(0, mainText.length);
    }

    /* Copy to clipboard */
    function copy() {
        mainText.selectAll()
        mainText.copy()
        mainText.select(0, 0)
    }

    /* Paste from clipboard */
    function paste() {
        mainText.selectAll()
        mainText.paste()
    }

    /* Remove last character */
    function removeLast() {
        if (mainText.length > 0) {
            mainText.remove(mainText.length - 1, mainText.length);
        }
    }

    /* Text box */
    TextArea {
        id: mainText
        anchors.fill: parent
        font.family: "Helvetica"
        font.pointSize: 20

        wrapMode: TextEdit.Wrap
        activeFocusOnPress: false
        activeFocusOnTab: false
        selectByKeyboard: false
    }

    /* Place holder */
    TextField {
        anchors.fill: parent
        visible: mainText.text.length > 0 ? false : true
        placeholderText: "Enter message"
        font.pointSize: 32

        activeFocusOnPress: false
        activeFocusOnTab: false
    }
}
