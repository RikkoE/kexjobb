import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

ApplicationWindow {
    title: qsTr("HealthyWay")
    visible: true
    Loader {id: pageLoader}
    Item {
        anchors.fill: parent

        Rectangle {
            width: parent.width
            height: parent.height
        }

        // This element displays a rectangle with a gradient and a border
        Rectangle {
            id: onButton
            anchors.right: parent.right
            anchors.rightMargin: 20
            y: 20
            width: parent.width*0.4
            height: parent.height*0.1
            radius: 8 // This gives rounded corners to the Rectangle
            gradient: Gradient { // This sets a vertical gradient fill
                GradientStop { position: 0.0; color: "aqua" }
                GradientStop { position: 1.0; color: "teal" }
            }
            //            border { width: 3; color: "white" } // This sets a 3px wide black border to be drawn
            Text {
                id: onLabel
                anchors.centerIn: parent
                text: "Bluetooth ON"
            }
            MouseArea{
                id: onMouseArea
                anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
                //onClicked handles valid mouse button clicks
                //onClicked: console.log(onLabel.text + " clicked" )
                onClicked: generator.onButtonClicked()
            }
        }

        // This rectangle is a plain color with no border
        Rectangle {
            id: offButton
            anchors.left: parent.left
            anchors.leftMargin: 20
            y: 20
            width: parent.width*0.4
            height: parent.height*0.1
            radius: 8 // This gives rounded corners to the Rectangle
            gradient: Gradient { // This sets a vertical gradient fill
                GradientStop { position: 0.0; color: "aqua" }
                GradientStop { position: 1.0; color: "teal" }
            }
            //            border { width: 3; color: "white" } // This sets a 3px wide black border to be drawn
            Text {
                id: offLabel
                anchors.centerIn: parent
                text: "Bluetooth OFF"
            }
            MouseArea{
                id: offMouseArea
                anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
                //onClicked handles valid mouse button clicks
                onClicked: generator.offButtonClicked()
            }
        }

        Connections {
            target: MyObject

        }

        Rectangle {
            id: scanButton
            anchors {
                left: parent.left
                leftMargin: 20;
                top: offButton.bottom;
                topMargin: 20;
            }
            width: parent.width-40
            height: parent.height*0.1
            radius: 8 // This gives rounded corners to the Rectangle
            gradient: Gradient { // This sets a vertical gradient fill
                GradientStop { position: 0.0; color: "aqua" }
                GradientStop { position: 1.0; color: "teal" }
            }
            //            border { width: 3; color: "black" } // This sets a 3px wide black border to be drawn
            Text {
                id: scanLabel
                anchors.centerIn: parent
                text: "Scan"
            }
            MouseArea{
                id: scanMouseArea
                anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
                //onClicked handles valid mouse button clicks
                onClicked: generator.scanLeDevice()
            }
        }

        //        Item {
        //            id: testView
        //            visible: false
        //            x:0
        //            y:50
        //            width: 640
        //            height: 430
        MessageDialog {
            id: connectMsg
            title: "Would you like to connect to this device?"
            onAccepted:
                Qt.quit()
        }


        ListView {
            property variant win;
            id: blueList
            model: devicesModel
            width: scanButton.width
            height: parent.height*0.6
            anchors {
                left: parent.left;
                top: scanButton.bottom;
                topMargin: 20;
                bottomMargin: 20;
                leftMargin: 20;
            }
            spacing: 40
            delegate: Rectangle  {
                //                anchors.topMargin: 40
                height: parent.parent.height*0.2
                width: parent.width
                border { width: 3; color: "black" } // This sets a 3px wide black border to be drawn
                Text {
                    anchors.centerIn: parent
                    text: display
                }
                MouseArea {
                    id: bluetoothDevices
                    anchors.fill: parent
                    onClicked: {
                        connectMsg.visible = true

                    }
                }
            }
        }
    }
}
