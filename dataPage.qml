import QtQuick 2.0
Rectangle {

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
        radius: 8 // This gives rounded corners to the Rectangle
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "aqua" }
            GradientStop { position: 1.0; color: "teal" }
        }
        //            border { width: 3; color: "black" } // This sets a 3px wide black border to be drawn

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
        radius: 8 // This gives rounded corners to the Rectangle
        gradient: Gradient { // This sets a vertical gradient fill
            GradientStop { position: 0.0; color: "aqua" }
            GradientStop { position: 1.0; color: "teal" }
        }
        //            border { width: 3; color: "black" } // This sets a 3px wide black border to be drawn
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
                pageLoader.source = "test.qml"
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

