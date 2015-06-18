import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0

Rectangle {
    visible: true

    color: "#EEEEE9"

    // A rectangle representing the menu bar at the top of the app
    Rectangle {
        id: menuBar
        width: parent.width
        height: parent.height*0.1

        color: "#02163C"

        // A rectangle representing the button which disconnects from the remote device
        Rectangle {
            id: disconnectButton
            anchors {
                left: parent.left
            }
            width: parent.width*0.4
            height: parent.height

            color: "#02163C"

            Text {
                id: disconnectLabel
                anchors.centerIn: parent
                text: "Disconnect"
                color: "white"
            }
            MouseArea{
                id: disconnectMouseArea
                anchors.fill: parent //anchor all sides of the mouse area to the rectangle's anchors
                // Change color when the button is pressed
                onPressed: disconnectButton.color = "gray"
                onReleased: disconnectButton.color = "#02163C"
                onClicked: {
                    // Disconnects from the remote device
                    generator.disconnectDevice()
                    // Switch the screen to the main page
                    pageLoader.source = "main.qml"
                }
            }
        }
    }

    // A rectangle representing the list of services on the remote device
    Rectangle {
        id: serviceList
        width: parent.width-40
        height: parent.height*0.6

        anchors {
            left: parent.left;
            top: menuBar.bottom;
            topMargin: 20;
            bottomMargin: 20;
            leftMargin: 20;
        }

        color: "white"
        radius: 10

        ListView {
            id: servicesList
            // Connects the list to the serviceList in HealthyWayFunctions
            model: generator.serviceList
            width: parent.width
            height: parent.height
            // Each element in the list won't go outside the border of the list
            clip: true

            // Spacing between the items in the list
            spacing: 20
            // A style template for each item in the list
            delegate: Rectangle  {
                height: parent.parent.height*0.2
                width: parent.width

                Rectangle {
                    id: bottomBorder
                    width: parent.width
                    height: 5

                    anchors.bottom: parent.bottom

                    RadialGradient {
                        anchors.fill: parent
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "black" }
                            GradientStop { position: 0.5; color: "transparent" }
                        }
                    }
                }
                Rectangle {
                    id: topBorder
                    width: parent.width
                    height: 5

                    anchors.top: parent.top

                    RadialGradient {
                        anchors.fill: parent
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "black" }
                            GradientStop { position: 0.5; color: "transparent" }
                        }
                    }
                }

                Text {
                    color: "black"
                    anchors.centerIn: parent
                    text: modelData
                }
                MouseArea {
                    id: characMouseArea
                    anchors.fill: parent
                    onClicked: {
                        // Picks out the index of the pressed service
                        servicesList.currentIndex = index;
                        // Sends the index to HealthyWayFunctions
                        generator.getCharacteristicData(servicesList.currentIndex);
                        // Switch the screen to the data page
                        pageLoader.source = "dataPage.qml"
                    }
                }
            }
        }
    }
}




