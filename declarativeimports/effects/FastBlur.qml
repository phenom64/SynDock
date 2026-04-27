import QtQuick
import QtQuick.Effects

MultiEffect {
    property real radius: 16
    property bool cached: false

    blurEnabled: true
    blur: Math.max(0, radius) / 64
}
