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
    property int fileIndex: 0
    property real surfaceViewportRatio: 1.5
    property Result result: null
    property var faces: result && result.ready ? result.faces : []

    FaceLoader {
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

        Item {
            id: frame
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true

            Flickable {
                id: flick
                contentX: 0
                contentY: 0
                Layout.fillWidth: true
                Layout.fillHeight: true
                contentWidth: width * surfaceViewportRatio
                contentHeight: height * surfaceViewportRatio
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: ScrollBar { }
                ScrollBar.horizontal: ScrollBar { }
                Rectangle {
                    id: photoFrame
                    width: image.width
                    height: image.height
                    scale: Math.min(frame.width / image.sourceSize.width, frame.height / image.sourceSize.height)
                    smooth: true
                    antialiasing: true
                    onXChanged: updateFaces()
                    onYChanged: updateFaces()
                    Image {
                        id: image
                        anchors.centerIn: parent
                        fillMode: Image.PreserveAspectFit
                        antialiasing: true
                        source: (files.length > fileIndex) ? files[fileIndex] : ''
                    }
                    MouseArea {
                        id: dragArea
                        hoverEnabled: true
                        anchors.fill: parent
                        drag.target: photoFrame
                        scrollGestureEnabled: false
                    }
                }
                Repeater {
                    id: repeater
                    model: []
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
                            text: modelData.text
                            x: modelData.bbox.x
                            y: modelData.bbox.y + modelData.bbox.height
                            color: "blue"
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

            Rectangle {
                id: progressRectangle
                x: 196
                y: 144
                width: 224
                height: 124
                color: "#ffffff"
                visible: result && !main.result.ready && !main.result.errorMessage
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    x: 35
                    y: 18
                    width: 133
                    height: 14
                    text: qsTr("File progress")
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 12
                }

                ProgressBar {
                    id: progressBar1
                    x: 12
                    y: 79
                    value: result ? main.result.percentCompleted / 100 : 0
                }
            }
            Rectangle {
                id: errorRectangle
                x: 196
                y: 144
                width: 380
                height: 124
                color: "#ff0000"
                visible: result && result.errorMessage !== undefined
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    x: 113
                    y: 18
                    width: 133
                    height: 14
                    text: qsTr("Error")
                    anchors.horizontalCenter: parent.horizontalCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 12
                }

                Text {
                    x: 35
                    y: 45
                    width: 363
                    height: 71
                    text: result && result.errorMessage !== undefined ? result.errorMessage : ''
                    anchors.horizontalCenter: parent.horizontalCenter
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }
    function setIndex(i) {
        main.fileIndex = i;
        main.result = loader.results[i];
    }
    function updateFaces() {
        var facesList = [];
        for (var face of main.faces)
            facesList.push({
                bbox: flick.mapFromItem(photoFrame, face.bbox.x, face.bbox.y, face.bbox.width, face.bbox.height),
                text: face.demographics.gender + ' - ' + face.demographics.age.mean.toFixed(1)
            });
        repeater.model = facesList;
    }

    Connections {
        target: toolButtonOpen
        onClicked: fileDialog.open()
    }

    Connections {
        target: dragArea
        onWheel: {
            photoFrame.scale += photoFrame.scale * wheel.angleDelta.y / 120 / 5;
            updateFaces();
        }
    }
    Connections {
        target: main
        onFacesChanged: updateFaces()
    }

    Connections {
        target: image
        onSourceSizeChanged: {
            photoFrame.scale = Math.min(frame.width / image.sourceSize.width, frame.height / image.sourceSize.height);
            photoFrame.x = (frame.width - image.sourceSize.width) / 2;
            photoFrame.y = (frame.height - image.sourceSize.height) / 2;
        }
    }
}













/*##^## Designer {
    D{i:11;anchors_x:0}D{i:13;anchors_x:0}D{i:21;invisible:true}
}
 ##^##*/
