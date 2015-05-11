import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.3

Rectangle {
    visible: true
    anchors.fill: parent
    gradient: Gradient { // This sets a vertical gradient fill
        GradientStop { position: 0.0; color: "black" }
        GradientStop { position: 1.0; color: "white" }
    }
    // This element displays a rectangle with a gradient and a border
    Rectangle {
        id: onButton
        anchors.right: parent.right
        anchors.rightMargin: 20
        y: 20
        width: (parent.width-60)/2
        height: parent.height*0.1
        smooth: true
        radius: 800 // This gives rounded corners to the Rectangle
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "white" }
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
        width: (parent.width-60)/2
        height: parent.height*0.1
        radius: 800 // This gives rounded corners to the Rectangle
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "white" }
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

    BusyIndicator {
     id: busyIndication
     visible: false
     anchors.centerIn: parent
//     onRunningChanged: generator.scanLeDevices()
     // 'running' defaults to 'true'
    Component.onCompleted: {
            console.log("Busyindicator has loaded")
        }
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
        radius: 800 // This gives rounded corners to the Rectangle
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "white" }
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
            //                onClicked: generator.scanButtonClicked();
            onClicked: {
                busyIndication.visible = true
                generator.startScanThread();
            }
        }
    }

    ListView {
        id: blueList
        model: generator.deviceList
        width: parent.width-40
        height: parent.height*0.6
        clip: true
//        boundsBehavior: Flickable.StopAtBounds
        anchors {
            left: parent.left;
            top: scanButton.bottom;
            topMargin: 20;
            bottomMargin: 20;
            leftMargin: 20;
        }
        spacing: 40
        delegate: Rectangle  {
            gradient: Gradient { // This sets a vertical gradient fill
                GradientStop { position: 0.0; color: "grey" }
                GradientStop { position: 1.0; color: "blue" }
            }
            //                anchors.topMargin: 40
            height: parent.parent.height*0.2
            width: parent.width
            border { width: 3; color: "black" } // This sets a 3px wide black border to be drawn
            Text {
                color: "white"
                anchors.centerIn: parent
                text: modelData
            }
            MouseArea {
                id: bluetoothDevices
                anchors.fill: parent
                onClicked: {
                    //                        test2.visible=true
                    blueList.currentIndex = index;
//                    console.log("button index: " + blueList.currentIndex);
                    generator.deviceClicked(blueList.currentIndex);
                    generator.listServices();
                    //                        test.visible=false
                    pageLoader.source = "test.qml"
                    // listDialog.visible = true

                }
            }
        }
    }
    //    }
    Loader {
        id: pageLoader
        anchors.fill: parent
    }
}
