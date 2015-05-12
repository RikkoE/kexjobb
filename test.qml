import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0

Rectangle {
    visible: true

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
        text:"Pungluder"
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

    Rectangle {
        id: disconnectButton
        anchors.left: parent.left
        anchors.leftMargin: 20
        y: 20
        width: parent.width-40
        height: parent.height*0.1
        smooth: true
        radius: 20
        border {
            color: "black"
            width: 5
        }
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 1.0; color: "transparent"}
        }
        Text {
            id: disconnectLabel
            anchors.centerIn: parent
            text: "Disconnect"
        }
        MouseArea{
            id: disconnectMouseArea
            anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
            //onClicked handles valid mouse button clicks
            //onClicked: console.log(onLabel.text + " clicked" )
            onPressed: disconnectButton.scale = 0.7
            onReleased: disconnectButton.scale = 1.0
            onClicked: {
                generator.disconnectDevice()
                pageLoader.source = "main.qml"
            }
        }
    }

    ListView {
        id: servicesList
        model: generator.serviceList
        width: parent.width-40
        height: parent.height*0.6
        boundsBehavior: Flickable.StopAtBounds
        anchors {
            left: parent.left;
            top: disconnectButton.bottom;
            topMargin: 20;
            bottomMargin: 20;
            leftMargin: 20;
        }
        spacing: 40
        delegate: Rectangle  {
            //                anchors.topMargin: 40
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
            Text {
                anchors.centerIn: parent
                text: modelData
            }
            MouseArea {
                id: characMouseArea
                anchors.fill: parent
                onClicked: {
                    servicesList.currentIndex = index;
                    generator.getCharacteristicData(servicesList.currentIndex);
                    pageLoader.source = "dataPage.qml"
                }
            }
        }
    }
}




