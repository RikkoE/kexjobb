import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle {

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

    Rectangle {
        id: updateButton

        anchors {
            left: parent.left
            leftMargin: 20;
            topMargin: 20;
        }
        y: 20
        width: (parent.width-60)/2
        height: parent.height*0.1
        smooth: true
        radius: 20 // This gives rounded corners to the Rectangle
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "white" }
        }
        border.color: "black"
        border.width: 5
        Text {
            id: updateLabel
            anchors.centerIn: parent
            text: "Update"
        }
        MouseArea{
            id: updateMouseArea
            anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
            onPressed: updateButton.scale = 0.7
            onReleased: updateButton.scale = 1.0
            //onClicked handles valid mouse button clicks
            onClicked: {
                generator.testThreads()
//                generator.updateData();
            }
        }
    }

    Rectangle {
        id: backButton
        anchors {
            right: parent.right
            rightMargin: 20;
            topMargin: 20;
        }
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
            id: backLabel
            anchors.centerIn: parent
            text: "Back"
        }
        MouseArea{
            id: backMouseArea
            anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
            onPressed: backButton.scale = 0.7
            onReleased: backButton.scale = 1.0
            //onClicked handles valid mouse button clicks
            onClicked: {
                generator.disconnectNotification()
                pageLoader.source = "servicePage.qml"
            }
        }
    }

    Text {
        id: dataValueLabel
        width: parent.width-40
        height: parent.height*0.1
        anchors {
            left: parent.left;
            topMargin: 20;
            top: backButton.bottom;
            bottomMargin: 20;
            leftMargin: 20;
        }
        text: "Data: " + generator.bleData
        font.pixelSize: 160
    }
}

