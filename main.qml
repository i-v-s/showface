import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import showface.backend 1.0

Window {
    id: main
    visible: true
    width: 640
    height: 480
    title: qsTr("Show faces")
    property var files: []
    property var faces: []
    property int fileIndex: 0
    property real surfaceViewportRatio: 1.5
    property Result result: null
    Loader {
        id: loader
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a files to process"
        selectExisting: true
        selectMultiple: true
        nameFilters: [ "Image files (*.jpg *.png *.gif)", "All files (*)" ]
        onAccepted: {
            loader.files = fileDialog.fileUrls
            main.files = fileDialog.fileUrls;
            setIndex(0);
        }
    }
    function postPhoto(fn) {
        var request = new XMLHttpRequest()
        request.open('POST', '')
    }

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        ToolBar {
            id: toolBar
            width: 360
            Layout.fillWidth: true

            ToolButton {
                id: toolButtonOpen
                x: 0
                y: 0
                width: 110
                height: 40
                text: qsTr("Open images")
            }

            ProgressBar {
                id: progressBar
                x: 124
                y: 17
                value: loader.percentCompleted / 100
            }

            Text {
                id: progressText
                x: 366
                y: 13
                text: loader.percentCompleted.toFixed(1) + '%'
                font.pixelSize: 12
            }
        }

        Frame {
            id: frame
            width: 200
            height: 200
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true

            Flickable {
                id: flick
                x: 20
                y: 10
                Layout.fillWidth: true
                Layout.fillHeight: true
                contentWidth: width * surfaceViewportRatio
                contentHeight: height * surfaceViewportRatio
                Rectangle {
                    id: photoFrame
                    width: image.width * (1 + 0.10 * image.height / image.width)
                    height: image.height * 1.10
                    scale: defaultSize / Math.max(image.sourceSize.width, image.sourceSize.height)
                    Behavior on scale { NumberAnimation { duration: 100 } }
                    Behavior on x { NumberAnimation { duration: 100 } }
                    Behavior on y { NumberAnimation { duration: 100 } }
                    border.color: "black"
                    border.width: 2
                    smooth: true
                    antialiasing: true

                    Image {
                        id: image
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit
                        antialiasing: true
                        source: (files.length > fileIndex) ? files[fileIndex] : ''
                        Repeater {
                            model: (result && result.ready) ? result.faces : []
                            Item {
                                Rectangle {
                                    width: modelData.bbox.width
                                    height: modelData.bbox.height
                                    x: modelData.bbox.x
                                    y: modelData.bbox.y
                                    border.color: "green"
                                    border.width: 2
                                    color: "transparent"
                                }
                                Text {
                                    text: modelData.demographics.gender + ' - ' + modelData.demographics.age.mean.toFixed(1)
                                    x: modelData.bbox.x
                                    y: modelData.bbox.y + modelData.bbox.height
                                    color: "blue"
                                }
                            }
                        }

                    }
                    MouseArea {
                        id: dragArea
                        hoverEnabled: true
                        anchors.fill: parent
                        drag.target: photoFrame
                        scrollGestureEnabled: false  // 2-finger-flick gesture should pass through to the Flickable
                        onWheel: {
                            var scaleBefore = photoFrame.scale;
                            photoFrame.scale += photoFrame.scale * wheel.angleDelta.y / 120 / 5;
                        }
                    }
                }
            }

            Button {
                id: buttonLeft
                y: 185
                width: 48
                height: 42
                text: qsTr("🢀")
                visible: fileIndex > 0
                onClicked: setIndex(main.fileIndex - 1)
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.verticalCenter: parent.verticalCenter
                topPadding: 4
                bottomPadding: 6
                font.pointSize: 20
            }

            Button {
                id: buttonRight
                x: 568
                y: 185
                width: 48
                height: 42
                text: qsTr("🢂")
                visible: fileIndex < files.length - 1
                onClicked: setIndex(main.fileIndex + 1)
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: 20
            }
        }

    }
    function setIndex(index) {
        main.fileIndex = index;
        main.result = loader.results[index];
    }

    Connections {
        target: toolButtonOpen
        onClicked: fileDialog.open()
    }
}



/*##^## Designer {
    D{i:11;anchors_x:0}D{i:13;anchors_x:0}
}
 ##^##*/
