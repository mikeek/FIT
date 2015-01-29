import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0
import "templates.js" as Template

Window {
    id: setWin
    title: "Settings"

    /* Set size */
    height: col.height + controls.height + 20
    width: col.width + menu.width + 20

    /* Fixed size */
    minimumHeight: height
    minimumWidth: width
    maximumHeight: minimumHeight
    maximumWidth: minimumWidth

    /* Set modality */
    modality: Qt.WindowModal

    /* Color bind */
    property color clr_bg: Template.tmplSetupWindow.bg[Template.active]
    property color clr_menu: Template.tmplSetupWindow.menu[Template.active]
    property color clr_lvText: Template.tmplSetupWindow.lvText[Template.active]
    property color clr_gridLabel: Template.tmplSetupWindow.gridLabel[Template.active]
    property color clr_t9colGridLabel: Template.tmplSetupWindow.t9colGridLabel[Template.active]

    /* tmp variable for selected dictionary */
    property int selectedDict: textHandler.dict

    /* Background */
    Rectangle {
        id: bg
        anchors.fill: parent
        color: clr_bg
    }

    function copyKeys() {
        var tmp = []
        for (var i = 0; i < keyboard.keys.length; ++i) {
            tmp.push(keyboard.keys[i]);
        }
        return tmp;
    }

    function createLabels() {
        var tmp = []
        for (var i = 0; i < btnGrid.tmpKeys.length; ++i) {
            tmp.push(["", "", keyboard.key2str(btnGrid.tmpKeys[i])]);
        }
        return tmp;
    }

    property int lastBody: 0

    /* Menu background */
    Rectangle {
        anchors.fill: menu
        color: clr_menu
    }

    Column {
        id: menu
        width: 120
        height: setWin.height

        property var btns: ["Keyboard", "T9", "Appearance"];

        ListView {
            id: lv
            width: parent.width
            height: parent.height
            spacing: 2

            model: menu.btns.length
            clip: true
            orientation: ListView.Vertical

            highlight: Rectangle {
                z: 2; color: bg.color
                Text {
                    anchors.centerIn: parent
                    font.pointSize: 14
                    color: clr_lvText
                    text: lv.currentItem.label
                }
            }

            /* Create menu buttons */
            delegate: MenuBtn {
                id: menuBtn
                z: 1
                width: menu.width
                label: menu.btns[index]
                onClicked: {
                    lv.currentIndex = index
                    body.children[lastBody].visible = false
                    body.children[index].visible = true
                    lastBody = index
                }
            }
        }
    }

    Item {
        id: body

        /* Column with all elements in window (= column layout ) */
        Column {
            id: col
            visible: true
            x: menu.width + 10
            spacing: 6
            focus: true

            /* Grid label */
            Text {
                text: "Controls settings (click to change)"
                font.pointSize: 14
                color: clr_gridLabel
            }

            /* Buttons */
            ButtonGrid {
                id: btnGrid
                property var tmpKeys: copyKeys()
                property var tmpLabels: createLabels()
                property int last: 0;
                property string lastLabel;

                gridLabels: [[
                        tmpLabels[1], tmpLabels[2], tmpLabels[3],
                        tmpLabels[4], tmpLabels[5], tmpLabels[6],
                        tmpLabels[7], tmpLabels[8], tmpLabels[9]
                    ]]

                rColLabels: [tmpLabels[10], tmpLabels[11], tmpLabels[12]]
                bRowLabels: [tmpLabels[0], tmpLabels[14]]
                switchLabels: [tmpLabels[13]]

                onButtonPressed: {
                    /* Click on activated button - deactivate it */
                    if (last == index && buttons[index].activated) {
                        buttons[index].centerLabel = lastLabel
                        buttons[index].deactivate()
                        return
                    }

                    /* New key NOT set, deactivate last button and set previous label */
                    if (buttons[last].activated) {
                        buttons[last].centerLabel = lastLabel
                        buttons[last].deactivate()
                    }

                    /* Update last */
                    last = index

                    /* Activate new button */
                    lastLabel = buttons[last].centerLabel;
                    buttons[last].centerLabel = ""
                    buttons[last].activate()
                }
            }


            /* New assignment */
            Keys.onPressed: {
                /* No active button, ignore key event */
                if (!btnGrid.buttons[btnGrid.last].activated) {
                    return
                }

                /* Avoid conflicts */
                for (var i = 0; i < btnGrid.tmpKeys.length; ++i) {
                    if (btnGrid.tmpKeys[i] == event.key) {
                        btnGrid.tmpKeys[i] = btnGrid.tmpKeys[btnGrid.last];
                        btnGrid.buttons[i].centerLabel = btnGrid.lastLabel
                        break;
                    }
                }

                /* Change assigned key */
                btnGrid.tmpKeys[btnGrid.last] = event.key

                /* Change label and deatcivate button */
                btnGrid.buttons[btnGrid.last].primaryLabel = keyboard.key2str(event.key);
                btnGrid.buttons[btnGrid.last].deactivate();
            } /* Keys */
        } /* Column */

        /* T9 Settings */
        Column {
            id: t9col
            x: menu.width + 10
            visible: false
            spacing: 6

            property var t9Labels: ["CZ", "SK"]

            /* Grid label */
            Text {
                text: "T9 settings"
                font.pointSize: 14
                color: clr_t9colGridLabel
            }

            ScomboBox {
               id: epicComboBox
               source: ["CZ", "SK"]
               width: 150

               /*
                * Assign selected dictionary into temporary variable.
                * Don't override real dict until "OK" pressed
                */
               onChoosedVal: setWin.selectedDict = epicComboBox.selected;
            }

        }/* Column*/

        /* Appearence settings */
        Column {
            id: appearence
            x: menu.width + 10
            visible: false
            spacing: 6

            /* Label */
            Text {
                id: tt
                text: "Appearence settings"
                font.pointSize: 14
                color: clr_t9colGridLabel
            }

            /* Appearence ComboBox */
            ScomboBox {
               id: epicComboBox2
               width: 150

               source: ["Default", "Tmpl1", "Tmpl2"]
               label: "Template"


               /*
                * Assign selected dictionary into temporary variable.
                * Don't override real dict until "OK" pressed
                */
               onChoosedVal: Template.active = epicComboBox.selected;
            }/* ScomboBox*/
        }/* Column*/
    } /* Item */

    Row {
        id: controls
        /* Pin to the right */
        width: 200
        height: 50
        x: setWin.width - width
        y: setWin.height - height
        spacing: 6

        /* Confirm button */
        ControlBtn {
            label: "OK"

            /* Save key bindings */
            onClicked: {
                for (var i = 0; i < keyboard.keys.length; ++i) {
                    keyboard.keys[i] = btnGrid.tmpKeys[i];
                }

                /* Save keys for next program run */
                keyboard.saveKeys();

                /* Save selected dictionary */
                textHandler.dict = setWin.selectedDict

                setWin.close();
            }

            /* TODO: save T9 language */
        }

        /* Cancel button */
        ControlBtn {
            label: "Cancel"

            /* Don't save anything */
            onClicked: {
                setWin.close()
            }
        }
    }

} /* Window */
