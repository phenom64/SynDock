# SynDock

SynDock is the SynOS dock for KDE Plasma 6. It is currently being reset from
KDE SynDock's `work/plasma6` branch and ported for SynOS Canora 1.0.

The target runtime is:

- KDE Plasma 6.6
- Qt 6.10.2
- KDE Frameworks 6.24.0
- Wayland only

The first goal is functional parity with SynDock on Plasma 6: layouts,
indicators, templates, task grouping, parabolic zoom, auto-hide and dodge modes,
multi-screen handling, shortcuts, and settings dialogs. Visual redesign work is
intentionally secondary until the port builds and runs reliably.

## Public IDs

- Executable: `syndock`
- D-Bus service and interface: `org.syndromatic.SynDock`
- Desktop/AppStream ID: `org.syndromatic.syndock`
- Main config file: `syndockrc`
- User data root: `~/.local/share/syndock`
- Plasma packages:
  - `org.syndromatic.syndock.containment`
  - `org.syndromatic.syndock.plasmoid`
  - `org.syndromatic.syndock.shell`

## Build

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr
ninja -C build
```

## Migration

SynDock configs can be copied into SynDock locations with:

```sh
tools/migrate-latte-config.py --dry-run
tools/migrate-latte-config.py
```

The migration writes backups before replacing SynDock files. To reverse the
copy for testing:

```sh
tools/migrate-latte-config.py --reverse
```
