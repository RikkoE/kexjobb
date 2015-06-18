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

    color: "#EEEEE9"

    // A rectangle representing the menu bar at the top of the app
    Rectangle {
        id: menuBar
        width: parent.width
        height: parent.height*0.1

        color: "#02163C"

        // A rectangle representing the button which scans for devices
        Rectangle {
            id: scanButton
            anchors {
                right: parent.right
            }
            width: parent.width*0.2
            height: parent.height
            color: "#02163C"

            Text {
                id: scanLabel
                anchors.centerIn: parent
                text: "Scan"
                color: "white"
            }
            MouseArea{
                id: scanMouseArea
                anchors.fill: parent
                // Change color when the button is pressed
                onPressed: scanButton.color = "gray"
                onReleased: scanButton.color = "#02163C"
                onClicked: {
                    // Starts the scan thread in HealthyWayFunction
                    generator.startScanThread();
                }
            }
        }
    }

    // Waits for signals when scanning starts and stops to disable or enable the buttons
    Connections {
        target: generator

        onScanningStarted: {
            loadingScreen.visible = true
            scanMouseArea.enabled = false
            blueList.enabled = false
        }
        onScanningStopped: {
            loadingScreen.visible = false
            scanMouseArea.enabled = true
            blueList.enabled = true
        }
    }

    // A rectangle representing the list of devices
    Rectangle {
        id: deviceList
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
            id: blueList
            // Connects the list to the deviceList in HealthyWayFunctions
            model: generator.deviceList
            width: parent.width
            height: parent.height
            // Each element in the list won't go outside the border of the list
            clip: true

            // Spacing between the items in the list
            spacing: 20
            // A style template for each item in the list
            delegate: Rectangle  {
                id: delegateRect
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
                // Activates when a device in the list is chosen
                MouseArea {
                    id: bluetoothDevices
                    anchors.fill: parent
                    onClicked: {
                        // Picks out the index of the pressed device
                        blueList.currentIndex = index;
                        // Sends the index to HealthyWayFunctions
                        generator.deviceClicked(blueList.currentIndex);
                        // Fetches the services of the device
                        generator.listServices();
                        // Switch the screen to the service page
                        pageLoader.source = "servicePage.qml"
                    }
                }
            }
        }
    }

    // A rectangle representing the loading screen for scanning
    Rectangle {
        id: loadingScreen
        anchors.fill: parent
        color: "black"
        visible: false
        opacity: 0.8

        Rectangle {
            id: loadingWindow
            anchors.centerIn: parent
            color: "#EEEEE9"
            width: parent.width-120
            height: parent.height*0.2
            radius: 20

            Text {
                id: loadingText
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.horizontalCenterOffset: -80
                text: "Scanning"
                font.pixelSize: 100
            }

            // The indicator next to the text "Scanning"
            BusyIndicator {
                id: busyIndication
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: loadingText.right
                anchors.leftMargin: 40
            }
        }
    }

    // A loader that lets the QML switch screens by using "source"
    Loader {
        id: pageLoader
        anchors.fill: parent
    }
}
