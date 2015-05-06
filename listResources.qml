import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

ApplicationWindow {
    id: root
    width: 100; height: 100

    Text {
        anchors.centerIn: parent
        text: qsTr("Hello World.")
    }
}
