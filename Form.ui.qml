import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {

    Rectangle {
        color: "#272822"
        width: parent.width
        height: parent.height
    }

    // This element displays a rectangle with a gradient and a border
    Rectangle {
        anchors.right: parent.right
        anchors.rightMargin: 20
        y: 20
        width: parent.width*0.4
        height: 100
        radius: 8 // This gives rounded corners to the Rectangle
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "aqua" }
            GradientStop { position: 1.0; color: "teal" }
        }
        border { width: 3; color: "white" } // This sets a 3px wide black border to be drawn
        Text {
                id: onLabel
                anchors.centerIn: parent
                text: "Bluetooth ON"
        }
        MouseArea{
                id: onMouseArea
                anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
                        //onClicked handles valid mouse button clicks
                onClicked: console.log(onLabel.text + " clicked" )
        }
    }

    // This rectangle is a plain color with no border
    Rectangle {
        anchors.left: parent.left
        anchors.leftMargin: 20
        y: 20
        width: parent.width*0.4
        height: 100
        radius: 8 // This gives rounded corners to the Rectangle
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "aqua" }
            GradientStop { position: 1.0; color: "teal" }
        }
        border { width: 3; color: "white" } // This sets a 3px wide black border to be drawn
        Text {
                id: offLabel
                anchors.centerIn: parent
                text: "Bluetooth OFF"
        }
        MouseArea{
                id: offMouseArea
                anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
                        //onClicked handles valid mouse button clicks
                onClicked: console.log(offLabel.text + " clicked" )
        }
    }
}
