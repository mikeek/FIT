import QtQuick 2.0
import "templates.js" as Template

Rectangle {
    width: grid.width
    height: grid.height
    focus: true
    color: clr_rectangle

    /* Color bind */
    property color clr_rectangle: Template.tmplKeyboard.rectangle[Template.active]


    /* Number of grids */
    property int activeGrid: 0

    /* Keys assigned to grid buttons */
    property var keys: loadKeys()

    /* Info about long presses */
    property var longs: []

    /* Load keys from saved settings */
    function loadKeys() {
        var tmpKeys = []
        for (var i = 0; i < trans.keyCount(); ++i) {
            tmpKeys.push(trans.getKey(i))
        }
        return tmpKeys
    }

    /* Save keys binding */
    function saveKeys() {
        for (var i = 0; i < keys.length; ++i) {
            trans.saveKey(i, keys[i]);
        }
    }

    /* Translate code */
    function key2str(index) {
        return trans.key2str(index);
    }


    /* Set new key code for button */
    function setKey(index, key) {
        keys[index] = key;
    }

    function getKeys() {
        var tmp = [];
        for (var i = 0; i < keys.length; ++i) {
            tmp.push(["", "", keys[i]]);
        }
        return tmp;
    }

    ButtonGrid {
        id: grid

        onButtonPressed:
            textHandler.press(index, buttons[index].primaryLabel, buttons[index].secondaryLabel, buttons[index].centerLabel);
        onButtonPressedLong: textHandler.pressLong(index);
    }

    property alias buttons: grid.buttons
    /* Proccess key events */
    Keys.onPressed: {
        /*
         * This (long press recognizing) should be done in C++,
         * but now is too late and we are too lazy
         */

        for (var i = 0; i < keys.length; ++i) {
            /* Find if key is assigned to some button */
            if (event.key == keys[i]) {
                /* Enable autorepeat for backspace */
                /* TODO: better checking BS button */
                if (i == 11) {
                    buttons[i].press();
                    return;
                }

                /* Recognize press length */
                if (event.isAutoRepeat) {
                    /* Long press not proccessed */
                    if (longs[i] == false) {
                        buttons[i].pressLong();
                        longs[i] = true;
                    }
                } else {
                    /* Short press */
                    buttons[i].press();
                    longs[i] = false;
                }
                break;
            }
        }
    }

    /* Release button */
    Keys.onReleased: {
        for (var i = 0; i < keys.length; ++i) {
            if (event.key == keys[i]) {
                buttons[i].release();
                break;
            }
        }
    }

    Component.onCompleted: {
        /* Set all long presses on false */
        for (var i = 0; i < keys.length; ++i) {
            longs.push(false);
        }
    }
}
