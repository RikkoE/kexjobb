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
//                generator.startDataThread()
//                graph.visible = true;
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
                graph.visible = false;
                batteryIndicator.visible = false;
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

    Connections {
        target: generator
        onShowEcgCanvas: {
            graph.visible = true;
        }
        onShowBatteryCanvas: {
            batteryIndicator.visible = true;
        }
    }

    Canvas {
        id: graph
        width: parent.width-40
        height: parent.height*0.5
        visible: false

        anchors {
            leftMargin: 20
            left: parent.left
            topMargin:20
            top: dataValueLabel.bottom
        }

        property int linex: 0;
        property int liney: 200;
        property int count: 1;

        Connections {
            target: generator
            onExperimentYChanged: {
                graph.requestPaint();
            }
        }

        transform: Scale {
            yScale: 4
        }

        onPaint: {
            // Get drawing context
            var context = graph.getContext("2d");

            if(generator.experiment > count*(parent.width-40)) {
                context.clearRect(0, 0, graph.width, graph.height);
                linex = 0;
                count += 1;
            }

            context.beginPath();
            context.lineWidth = 1;
            context.moveTo(linex, liney);

            linex = generator.experiment - (count-1)*(parent.width-40);
            liney = generator.experimentY;

            context.strokeStyle = "red"
            context.lineTo(linex, liney);
            context.stroke();
        }
    }

    Canvas {
        id: batteryIndicator
        width: parent.width-40
        height: parent.height*0.5
        visible: false

        anchors {
            leftMargin: 20
            left: parent.left
            topMargin:20
            top: dataValueLabel.bottom
        }

        property int batteryPercent: (generator.batteryLevel/100)*batteryIndicator.width;

        Connections {
            target: generator
            onBatteryLevelChanged: {
                batteryIndicator.requestPaint();
            }
        }

        onPaint: {
            // Get drawing context
            var context = batteryIndicator.getContext("2d");

            context.clearRect(0, 0, batteryIndicator.width, batteryIndicator.height);

            context.beginPath();
            context.lineWidth = 50;
            context.moveTo(0, 300);
            if(generator.batteryLevel > 50) {
                context.strokeStyle = "green"
            } else if(generator.batteryLevel > 20) {
                context.strokeStyle = "yellow"
            } else {
                context.strokeStyle = "red"
            }
            context.lineTo(batteryPercent, 300);
            context.stroke();
        }
    }
}

