# SynDock

A modern Qt 6 dock designed for SynOS. SynDock provides an elegant and intuitive task management experience with a fluid parabolic zoom effect, intelligent auto‑hide, and Wayland‑native layer‑shell integration with first-class support for Retina Displays.

**The New Syndromatic Experience (NSE) Dock**

Developed and maintained by Syndromatic Ltd. Based on "Latte Dock" (2016, Michail Vourlakos et al.), completely modernised for Qt 6 / Plasma 6.

---

## ✨ Highlights

- **Qt 6 / KDE 6 Native:** Built for Qt 6.7+, KF6, and Plasma 6.0+
- **Wayland‑Only:** Exclusive LayerShellQt integration — no X11 dependencies
- **Parabolic Zoom:** Fluid icon magnification with configurable easing curves
- **NSE Namespace:** All components follow the Syndromatic naming convention
- **HiDPI Ready:** Native Qt 6 high‑DPI scaling for crisp visuals
- **C++20:** Modern language features throughout the codebase
- **Configuration Migration:** Script to migrate existing Latte Dock configs

> **Note:** SynDock requires Wayland. X11 is not supported.

---

## 📦 Dependencies

SynDock targets SynOS Canora (Kubuntu 26.04 LTS base) with Qt 6 and KF6.

### Required (Debian/Ubuntu with KDE Neon or Kubuntu 25.10+)

```
build-essential cmake extra-cmake-modules ninja-build

# Qt 6
qt6-base-dev qt6-declarative-dev qt6-wayland-dev

# KDE Frameworks 6
libkf6coreaddons-dev libkf6guiaddons-dev libkf6dbusaddons-dev
libkf6windowsystem-dev libkf6globalaccel-dev libkf6crash-dev
libkf6notifications-dev libkf6archive-dev libkf6iconthemes-dev
libkf6kio-dev libkf6xmlgui-dev libkf6svg-dev libkf6package-dev
libkf6declarative-dev

# Plasma 6 (includes Activities, which moved from KF5 to Plasma in KF6)
libplasma-dev libplasmaactivities-dev plasma-wayland-protocols

# Layer Shell
layer-shell-qt-dev
```

---

## 🚀 Build & Install

```bash
git clone https://github.com/phenom64/syndock.git
cd syndock
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_INSTALL_PREFIX=/usr
ninja -j$(nproc)
sudo ninja install
```

### What Gets Installed
- `syndock` binary into `/usr/bin`
- Plasma containment and applet plugins
- QML components for NSE parabolic effects
- D‑Bus service `org.syndromatic.SynDock`
- Default layouts and indicator styles

### Uninstall
```bash
sudo ninja uninstall
```

---

## 🧩 Migrating from Latte Dock

A Python migration script is provided to convert existing Latte Dock configurations:

```bash
# Dry run (preview changes)
python3 tools/migrate-latte-config.py --dry-run

# Perform migration with backup
python3 tools/migrate-latte-config.py
```

The script:
- Backs up existing configs to `~/.config/latte-backup-YYYYMMDD-HHMMSS/`
- Renames `lattedockrc` to `syndockrc`
- Updates layout directory references
- Applies SynOS default settings (64px icons, 40% zoom, 30% opacity)

---

## 🧱 Project Structure

```
syndock/
├── app/                    # Core application
│   ├── NSETypes.h          # Unified namespace defaults
│   ├── nsecoronainterface.h # Corona (session manager)
│   ├── view/               # Dock view components
│   │   ├── nseparaboliceffect.cpp/h  # Parabolic zoom
│   │   └── ...
│   └── wm/                 # Window management
│       ├── nsewaylandinterface.cpp/h # LayerShellQt
│       └── ...
├── containment/            # Plasma containment plugin
├── plasmoid/               # Tasks plasmoid
├── indicators/             # Visual indicator styles
├── tools/                  # Utilities
│   └── migrate-latte-config.py
└── CMakeLists.txt          # Qt 6 / KF6 build system
```

---

## ⚙️ SynOS Default Configuration

SynDock uses the "NSE" default settings optimised for SynOS:

| Setting | Value |
|---------|-------|
| Icon Size | 64px |
| Zoom Factor | 1.40 (40% magnification) |
| Background Opacity | 0.30 (30%) |
| Easing Curve | OutCubic |
| Exclusive Zone | Enabled |

These can be adjusted via the dock settings or by editing the configuration files.

---

## 🔧 Development Notes

- **Namespace:** `NSE` (Native Syndromatic Experience)
- **D‑Bus Interface:** `org.syndromatic.SynDock`
- **C++ Standard:** C++20
- **Qt Version:** 6.7+ (no Qt 5 support)
- **Wayland Protocol:** LayerShellQt for panel surfaces
- **Window Tracking:** wlr‑foreign‑toplevel‑management-v1

### Key NSE Files
| File | Purpose |
|------|---------|
| `NSETypes.h` | Unified namespace and default values |
| `nsecoronainterface.h` | Session/corona management |
| `nsewaylandinterface.cpp` | LayerShellQt Wayland integration |
| `nseparaboliceffect.cpp` | Enhanced parabolic zoom effect |

---

## 🧪 Troubleshooting

**Dock not appearing:**
- Ensure you're running a Wayland session (SynDock does not support X11)
- Check `journalctl --user -f` for error messages

**Parabolic zoom not working:**
- Verify compositor is running with effects enabled
- Check that `QT_QPA_PLATFORM=wayland` is set

**Configuration not loading:**
- After migrating from Latte, restart the Plasma session
- Check `~/.config/syndockrc` exists

---

## 🙏 Credits

- **Original:** Latte Dock © 2016–2024, Michail Vourlakos, Smith AR, and contributors
- **SynDock/NSE:** © 2026 Syndromatic Ltd.

This project is distributed under the **GNU General Public License, version 2 or later (GPL‑2.0‑or‑later)**.

---

## 🌐 Links

- Syndromatic: https://syndromatic.com
- SynOS: https://syndromatic.com/synos
- Issue Tracker: https://github.com/phenom64/syndock/issues
