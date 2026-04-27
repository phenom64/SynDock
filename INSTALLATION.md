# Installation

SynDock is currently a development port for Plasma 6 and SynOS Canora. The
supported target is a Wayland Plasma session with Qt 6.10.2, KDE Frameworks
6.24.0, and Plasma 6.6.

## Development Packages

Install the distribution packages that provide these CMake packages:

```text
ECM
Qt6 DBus Gui Qml Quick WaylandClient
KF6 Archive CoreAddons Crash DBusAddons Declarative GlobalAccel GuiAddons I18n
KF6 IconThemes KIO Kirigami NewStuff Notifications Package Svg WindowSystem XmlGui Config
Plasma PlasmaActivities PlasmaQuick KWayland
LayerShellQt
LibTaskManager
PlasmaWaylandProtocols
Wayland Client
QtWaylandScanner
```

## Build

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr
ninja -C build
```

## Install

Use a temporary prefix while validating package metadata and runtime behavior:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX="$PWD/install-root/usr"
ninja -C build install
```

System installation should be done only after the package metadata and Wayland
session smoke tests pass.
