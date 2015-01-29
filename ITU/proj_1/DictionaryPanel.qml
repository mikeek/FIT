import QtQuick 2.0
import QtGraphicalEffects 1.0
import "templates.js" as Template

Rectangle {
    id: dictPanel
    width: 100
    height: 30

    /* Color bind */
    property color clr_rectangle: Template.tmplDictionaryPanel.rectangle[Template.active]
    property color clr_gradientBottom: Template.tmplDictionaryPanel.gradientBottom[Template.active]
    property color clr_gradientTop: Template.tmplDictionaryPanel.gradientTop[Template.active]


    color: clr_rectangle

    /* Background of panel */
    gradient: Gradient {
        GradientStop {position: 0.0; color: clr_gradientBottom}
        GradientStop {position: 1.0; color: clr_gradientTop}
    }

    DictionaryOverview {
        anchors.fill: parent
    }
 }
