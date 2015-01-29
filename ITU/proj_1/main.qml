import QtQuick 2.3
import QtQuick.Controls 1.2
import "templates.js" as Template
//import ITU 0.1

/**
  * TODO:
  *   - T9
  *   - přečuhující text v settings
  */

ApplicationWindow {
    id: mainWindow
    visible: true

    /* Set size */
    width: col.width
    height: col.height

    /* Fixed size */
    minimumHeight: height
    minimumWidth: width
    maximumHeight: minimumHeight
    maximumWidth: minimumWidth

    title: qsTr("Keyboard simulator")

    /* Child window */
    property variant win;

    /* Color bind */
    property color clr_rectangle: Template.tmplMain.rectangle[Template.active]
    property color clr_letterCounter: Template.tmplMain.letterCounter[Template.active]


    function openSettings() {
        textHandler.grid = 0;

        var component = Qt.createComponent("SetupWindow.qml")
        win = component.createObject(mainWindow);
        win.show()
    }

    /* Column backgroud */
    Rectangle {
        width: col.width
        height: col.height
        color: clr_rectangle
    }

    Column {
        id: col

        /* Show display */
        Display {
            id: display
            color: keyboard.color
            width: keyboard.width
            height: 150

            text: textHandler.text

            /* Letter counter */
            Text {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 3
                text: display.text.length
                color: clr_letterCounter
            }
        }

        /* Prediction panel with menu button */
        Row {
            height: dict.height

            /* Dictionary panel */
            DictionaryPanel {
                id: dict
                width: keyboard.width - dropMenu.width
            }

            /* Dropdown menu */
            DropMenu {
                id: dropMenu
                height: dict.height

                items: [
                    ["img/copy.png", "Copy"],
                    ["img/paste.png", "Paste"],
                    ["img/remall.png", "Remove all"],
                    ["img/settings.png", "Settings"]
                ]

                onComboClicked: {
                    switch (dropMenu.selectedItem) {
                    case 0: /* Copy */
                        display.copy()
                        break;
                    case 1: /* Paste */
                        display.paste()
                        break;
                    case 2: /* Remall */
                        display.removeAll()
                        break;
                    case 3: /* Settings */
                        mainWindow.openSettings()
                        break;
                    }
                }
            } /* DropMenu */
        } /* Row */

        /* Show keyboard */
        Keyboard {
            id: keyboard
        }
    } /* Column */
}
