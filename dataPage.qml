import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle {

    color: "#EEEEE9"

    // A rectangle representing the menu bar at the top of the app
    Rectangle {
        id: menuBar
        width: parent.width
        height: parent.height*0.1

        color: "#02163C"

        // A rectangle representing the button which goes to previous page
        Rectangle {
            id: backButton
            anchors {
                left: parent.left
            }
            width: parent.width*0.4
            height: parent.height

            color: "#02163C"

            Text {
                id: backLabel
                anchors.centerIn: parent
                text: "Back"
                color: "white"
            }
            MouseArea{
                id: backMouseArea
                anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
                // Change color when the button is pressed
                onPressed: backButton.color = "gray"
                onReleased: backButton.color = "#02163C"
                onClicked: {
                    // Disconnects the notification of a characteristic if there is any
                    generator.disconnectNotification()
                    // Hides the ECG graph
                    graph.visible = false;
                    // Hides the Battery indicator
                    batteryIndicator.visible = false;
                    pageLoader.source = "servicePage.qml"
                }
            }
        }
    }

    // This is the container that is the "Data: " label
    Text {
        id: dataValueLabel
        width: parent.width-40
        height: parent.height*0.1
        anchors {
            left: parent.left;
            topMargin: 20;
            top: menuBar.bottom;
            bottomMargin: 20;
            leftMargin: 20;
        }
        // Get text input from bleData
        text: "Data: " + generator.bleData
        font.pixelSize: 160
    }

    // Catches signals from HealthyWayFunctions to show different graphics depending on the characteristic
    Connections {
        target: generator
        id: showGraphics
        onShowEcgCanvas: {
            graph.visible = true;
        }
        onShowBatteryCanvas: {
            batteryIndicator.visible = true;
        }
    }

    // Holds all the graphics that can be used for characteristics
    Rectangle {
        id: canvasArea
        width: parent.width-40
        height: parent.height*0.5

        anchors {
            leftMargin: 20
            left: parent.left
            topMargin:20
            top: dataValueLabel.bottom
        }

        color: "white"
        radius: 10

        // The ECG graph
        Canvas {
            id: graph
            width: parent.width
            height: parent.height
            visible: false

            property int linex: 0;
            property int liney: 0;
            property int count: 1;

            // Repaint the graph when new values are sent
            Connections {
                target: generator
                onEcgReadingChanged: {
                    graph.requestPaint();
                }
            }

            // Draws lines from the previous point in the graph to the next
            onPaint: {
                // Get drawing context
                var context = graph.getContext("2d");

                // Resets the view so the ECG values can run indefinately
                if(generator.ecgTimeStamp > count*(parent.width-40)) {
                    context.clearRect(0, 0, graph.width, graph.height);
                    linex = 0;
                    count += 1;
                }

                context.beginPath();
                context.lineWidth = 5;
                context.moveTo(linex, liney);

                linex = generator.ecgTimeStamp - (count-1)*(parent.width-40);
                liney = generator.ecgReading;

                context.strokeStyle = "red"
                context.lineTo(linex, liney);
                context.stroke();
            }
        }

        Canvas {
            id: batteryIndicator
            width: parent.width
            height: parent.height
            visible: false

            property int batteryPercent: (generator.batteryLevel/100)*batteryIndicator.width;

            // Repaint the battery indicator when new values are sent
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
}

