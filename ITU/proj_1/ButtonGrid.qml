import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import "templates.js" as Template

Rectangle {
    id: mwin
    width: body.width
    height: body.height
    color: clr_rectangle

    /* Color bind */
    property color clr_rectangle: Template.tmplButtonGrid.rect[Template.active]
    property color clr_t9BtnTrue: Template.tmplButtonGrid.t9BtnTrue[Template.active]
    property color clr_t9BtnFalse: Template.tmplButtonGrid.t9BtnFalse[Template.active]


    /* Active grid */
    property int activeLabels: textHandler.grid

    /* [primaryLabel, secondaryLabel, centerLabel] */
    property var switchLabels: [["", "", "123"], ["", "", "123"], ["", "", "?!#"],["", "", ":)"], ["", "", "abc"]]
    property var rColLabels: [["", "", "T9"], ["", "", "\u232B"], ["", "", "\u21E7"]]
    property var bRowLabels: [["0", "\u2423", ""], ["", "", "\u23CE"]]
    property var gridLabels: [
        [
            ["1", ",.", ""],   ["2", "abc", ""], ["3", "def", ""],
            ["4", "ghi", ""],  ["5", "jkl", ""], ["6", "mno", ""],
            ["7", "pqrs", ""], ["8", "tuv", ""], ["9", "wxyz", ""],
        ],
        [
            ["1", ",.", ""],   ["2", "ABC", ""], ["3", "DEF", ""],
            ["4", "GHI", ""],  ["5", "JKL", ""], ["6", "MNO", ""],
            ["7", "PQRS", ""], ["8", "TUV", ""], ["9", "WXYZ", ""],
        ],
        [
            ["", "", "1"], ["", "", "2"], ["", "", "3"],
            ["", "", "4"], ["", "", "5"], ["", "", "6"],
            ["", "", "7"], ["", "", "8"], ["", "", "9"],
        ],
        [
            ["", "", "."],  ["", "", ","], ["", "", "?"],
            ["", "", "!"],  ["", "", ":"], ["", "", "/"],
            ["", "", "\\"], ["", "", "+"], ["", "", "-"],
        ],
        [
            ["", "", ":)"],  ["", "", ":D"],    ["", "", ":("],
            ["", "", ";)"],  ["", "", ":|"],    ["", "", ":["],
            ["", "", "O.o"], ["", "", "(_._)"], ["", "", "-_-"],
        ]
    ]
    property var buttons: []

    /* Handle that button "index" was pressed[long] */
    signal buttonPressed(var index)
    signal buttonPressedLong(var index)
    signal buttonReleased(var index)

    /* Main body */
    Column {
        spacing: 2
        id: body

        /* Grid + right column */
        Row {
            spacing: parent.spacing

            /* Main 9 buttons */
            Grid {
                id: grid
                spacing: parent.spacing
                rows: 3
                columns: 3

                Repeater {
                    model: parent.rows * parent.columns
                    KeyButton {
                        primaryLabel:   gridLabels[activeLabels][index][0]
                        secondaryLabel: gridLabels[activeLabels][index][1]
                        centerLabel:    gridLabels[activeLabels][index][2]
                    }
                }
            }

            /* Column next to the grid */
            Column {
                id: rCol
                width: 50
                spacing: parent.spacing

                /* T9 button */
                KeyButton {
                    id: t9Btn
                    width: parent.width

                    centerLabel: rColLabels[0][2]
                    centerLabelColor: (textHandler.t9enabled) ? clr_t9BtnTrue : clr_t9BtnFalse
                }

                /* backspace */
                KeyButton {
                    id: bsBtn
                    width: parent.width
                    centerLabel: rColLabels[1][2]
                }

                /* shift */
                KeyButton {
                    id: shiftBtn
                    width: parent.width
                    centerLabel: rColLabels[2][2]
                }
            }
        }

        /* Row under grid */
        Row {
            id: bRow
            spacing: parent.spacing

            /* switcher */
            KeyButton {
                id: switchBtn
                centerLabel: mwin.switchLabels[mwin.activeLabels][2]
                width: 50
            }


            /* space */
            KeyButton {
                id: spaceBtn
                width: grid.children[0].width * 2 + parent.spacing
                primaryLabel:   mwin.bRowLabels[0][0]
                secondaryLabel: mwin.bRowLabels[0][1]
                centerLabel: mwin.bRowLabels[0][2]
            }

            /* enter */
            KeyButton {
                id: enterBtn
                width: 100
                centerLabel: mwin.bRowLabels[1][2]
            }
        }
    }

    Component.onCompleted: {
        spaceBtn.listIndex = mwin.buttons.length;
        mwin.buttons.push(spaceBtn)

        for (var i = 0; i < grid.children.length - 1; ++i) {
            grid.children[i].listIndex = mwin.buttons.length
            mwin.buttons.push(grid.children[i]);
        }

        t9Btn.listIndex = mwin.buttons.length;
        mwin.buttons.push(t9Btn);

        bsBtn.listIndex = mwin.buttons.length;
        mwin.buttons.push(bsBtn);

        shiftBtn.listIndex = mwin.buttons.length;
        mwin.buttons.push(shiftBtn);

        switchBtn.listIndex = mwin.buttons.length;
        mwin.buttons.push(switchBtn);

        enterBtn.listIndex = mwin.buttons.length;
        mwin.buttons.push(enterBtn);

    }
}
