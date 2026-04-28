import QtQuick
import QtQuick.Effects

MultiEffect {
    property real radius: 8
    property int samples: 16
    property color color: "black"
    property real horizontalOffset: 0
    property real verticalOffset: 0
    property bool cached: false
    property bool fast: false

    shadowEnabled: true
    shadowBlur: Math.max(0, radius) / 64
    shadowColor: color
    shadowHorizontalOffset: horizontalOffset
    shadowVerticalOffset: verticalOffset
}
