import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.3
import QtGraphicalEffects 1.0

Rectangle {
    id: mainWindow
    visible: true
    anchors.fill: parent

    gradient: Gradient { // This sets a vertical gradient fill
        GradientStop { position: 1.0; color: "white" }
        GradientStop { position: 0.5; color: "light blue" }
        GradientStop { position: 0.0; color: "white" }
    }

    Text {
        opacity: 0.2
        font.pixelSize: 200
        visible: true
        id: backGroundLabel
        anchors.centerIn: parent
        text:"Healthyway"
        color: "white"
        smooth: true
    }
    DropShadow {
        opacity: 0.2
        anchors.fill: backGroundLabel
        horizontalOffset: 0
        verticalOffset: 20
        fast: true
        radius: 20.0
        samples: 16
        spread: 0.5
        color: "#000000"
        source: backGroundLabel
    }

    FastBlur {
        transparentBorder: true
        anchors.fill: backGroundLabel
        source: backGroundLabel
        radius: 64
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
        radius: 20 // This gives rounded corners to the Rectangle
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "white" }
        }
        border {
            color: "black"
            width: 5
        }

        Text {
            id: onLabel
            anchors.centerIn: parent
            text: "Bluetooth ON"
        }

        MouseArea{
            id: onMouseArea
            anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
            onPressed: onButton.scale = 0.7
            onReleased: onButton.scale = 1.0
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
        radius: 20 // This gives rounded corners to the Rectangle
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "white" }
        }
        border {
            color: "black"
            width: 5
        }

        Text {
            id: offLabel
            anchors.centerIn: parent
            text: "Bluetooth OFF"
        }
        MouseArea{
            id: offMouseArea
            anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
            //onClicked handles valid mouse button clicks
            onPressed: offButton.scale = 0.7
            onReleased: offButton.scale = 1.0
            onClicked: generator.offButtonClicked()
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
        radius: 20 // This gives rounded corners to the Rectangle
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "white" }
        }
        border {
            color: "black"
            width: 5
        }
        Text {
            id: scanLabel
            anchors.centerIn: parent
            text: "Scan"
        }
        MouseArea{
            id: scanMouseArea
            anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
            onPressed: scanButton.scale = 0.7
            onReleased: scanButton.scale = 1.0
            onClicked: {
                loadingScreen.visible = true
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

        anchors {
            left: parent.left;
            top: scanButton.bottom;
            topMargin: 20;
            bottomMargin: 20;
            leftMargin: 20;
        }

        spacing: 40
        delegate: Rectangle  {
            height: parent.parent.height*0.2
            width: parent.width

            radius: 20
            border {
                color: "black"
                width: 5
            }
            gradient: Gradient { // This sets a vertical gradient fill
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 1.0; color: "transparent"}
            }
            anchors.topMargin: 40

            Text {
                color: "black"
                anchors.centerIn: parent
                text: modelData
            }
            MouseArea {
                id: bluetoothDevices
                anchors.fill: parent
                onClicked: {
                    blueList.currentIndex = index;
                    generator.deviceClicked(blueList.currentIndex);
                    generator.listServices();
                    pageLoader.source = "test.qml"
                }
            }
        }
    }

    Rectangle {
        id: loadingScreen
        anchors.fill: parent
        color: "black"
        visible: false
        opacity: 0.8

        Rectangle {
            id: loadingWindow
            anchors.centerIn: parent
            color: "white"
            width: parent.width-120
            height: parent.height*0.2
            radius: 20

            Text {
                id: loadingText
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.horizontalCenterOffset: -80
                text: "Scanning..."
                font.pixelSize: 100
            }

            BusyIndicator {
                id: busyIndication
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: loadingText.right
                anchors.leftMargin: 40
            }
        }
    }

    Loader {
        id: pageLoader
        anchors.fill: parent
    }
}
