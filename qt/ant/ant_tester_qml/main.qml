import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

Window {
    id: window
    property int size: 350

    title: qsTr("ANT Tester")
    width: Screen.width / 8
    height: Screen.height / 4
    minimumWidth: size
    minimumHeight: size
    maximumWidth: size
    maximumHeight: size
    x: Screen.width/2 - width/2                                         // comment out for design mode
    y: Screen.height/2 - height/2                                       // comment out for design mode


    visible: true
    color: "#000000"


    Rectangle  {
        id: rootrect
        width: parent.width
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 0

        LinearGradient {
            anchors.fill: parent
            start: Qt.point(0, 0)
            end: Qt.point(width, height)
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#ffff00" }
                GradientStop { position: 1.0; color: "#ff0000" }
            }
        }

        GridLayout {
            columns: 2
            anchors.fill: parent
            anchors.margins: 10
            rowSpacing: 20
            columnSpacing: 10

            //----------------------------------------------------

            Label {
                text: "SSID"
                font.bold: true
                color: "#000000"
            }
            TextField {
                id: ssid
                //text: "infowars.com"
                Layout.fillWidth: true
            }

            //----------------------------------------------------

            Label {
                text: "Password"
                font.bold: true
                color: "#000000"
            }
            TextField {
                id: password
                Layout.fillWidth: true
            }

            //----------------------------------------------------

            Item {
                Layout.fillHeight: true
                Layout.columnSpan: 2
            }

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                Layout.columnSpan: 2

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 40
                    Button {
                        id: abortbutton
                        //text: "Abort"
                        text: ant.buttit

                        enabled: true
                        style: ButtonStyle {
                            background: Rectangle {
                                implicitWidth: 100
                                implicitHeight: 25
                                border.width: 1
                                border.color: "#000000"
                                radius: 1
                                color: control.hovered ? "#008000": "#cccccc"
                                /*
                                  gradient: Gradient {
                                      GradientStop { position: 0 ; color: control.pressed ? "#ff8080" : "#00ff00" }
                                      GradientStop { position: 1 ; color: control.pressed ? "#ff0000" : "#40c040" }
                                  }
                                  */
                            }
                        }
                        onClicked: {
                            window.close();
                        }
                    }                   // Button
                    Button {
                        text: "Enumerate ANT Sticks"
                        onClicked: {
                            ant.enumerate_sticks();
                        }
                    }
                }
            }
            StatusBar {
                opacity: .5
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.margins: 0

                RowLayout {
                    //margins: 0
                    //anchors.fill: parent
                    //ancors.
                    Label {
                        text: ant.statstr
                    }
                }
            }
        }                   // GridLayout
    }                       // Rectangle

    /*
    // this kills design mode!
    Connections	{
        target: sleepDetector
        onApplicationFellAsleep:	{
            console.log("sleep detector went off")
            //pageSwitcher.backward("ConnectionFormPage.qml");
            //riderControl.stop();
        }
    }
    */

}                           // Window

