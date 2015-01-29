import QtQuick 2.0
import QtQuick.Controls 1.2
import "templates.js" as Template

Rectangle {
    id:comboBox
    width: 15
    height: 40;
    smooth: true;
    color: clr_comboBox

    /* Color bind */
    property color clr_comboBox: Template.tmplDropMenu.comboBox[Template.active]
    property color clr_dropText: Template.tmplDropMenu.dropText[Template.active]
    property color clr_dropDown: Template.tmplDropMenu.dropDown[Template.active]
    property color clr_dropDownLv: Template.tmplDropMenu.dropDownLv[Template.active]

    /* Items in menu - pairs [image_source, title] */
    property variant items: []

    /* Something selected */
    signal comboClicked(var ix);

    property alias mainIcon: dropMenu.iconSource
    property alias selectedItem: listView.currentIndex

    /* Main button */
    ToolButton {
        id: dropMenu
        width: 20
        height:comboBox.height;

        Text {
            anchors.centerIn: parent
            color: clr_dropText
            text: "\u22EE"
            font.pointSize: 24
        }

        onClicked: {
            /* Drop down menu */
            comboBox.state = comboBox.state === "dropDown" ? "" : "dropDown"
        }
    }

    Rectangle {
        id: dropDown
        width: 50;
        height:0;
        clip:true;
        radius:4;
        /* drop down */
//        anchors.top: dropMenu.bottom;
        /* drop up */
        anchors.bottom: dropMenu.top;
        anchors.right: dropMenu.right
        anchors.margins: 2;
        color: clr_dropDown

        ListView {
            id: listView
            spacing: 1
            height: 500;
            model: comboBox.items
            currentIndex: 0
            delegate: Rectangle {
                    width: 50
                    height: comboBox.height
                    color: clr_dropDownLv
                    radius: 1

                    ToolButton {
                        anchors.fill: parent
                        iconSource: modelData[0]
                        tooltip: modelData[1]

                        onClicked: {
                            comboBox.state = ""
                            listView.currentIndex = index
                            comboBox.comboClicked(index)
                        }
                    }
                }
        }

        Behavior on height { NumberAnimation { duration: 200 } }
    }

    states: State {
        name: "dropDown";
        PropertyChanges { target: dropDown; height: (comboBox.height + listView.spacing) * comboBox.items.length }
    }
}
