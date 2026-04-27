#!/usr/bin/env python3
"""
Migrate Latte Dock user configuration to SynDock.

The migration is intentionally file based and reversible. It copies Latte's
configuration into SynDock locations, writes timestamped backups before
overwriting SynDock files, and rewrites only known public identifiers.
"""

from __future__ import annotations

import argparse
import os
import shutil
from datetime import datetime
from pathlib import Path


LATTE_TO_SYNDOCK = {
    "org.kde.latte.containment": "org.syndromatic.syndock.containment",
    "org.kde.latte.plasmoid": "org.syndromatic.syndock.plasmoid",
    "org.kde.latte.shell": "org.syndromatic.syndock.shell",
    "org.kde.latte-dock": "org.syndromatic.syndock",
    "org.kde.latte": "org.syndromatic.syndock",
    "Latte Dock": "SynDock",
    "Latte": "SynDock",
    "lattedockrc": "syndockrc",
    "lattedock": "syndock",
    "latte-dock": "syndock",
}


TEXT_SUFFIXES = {
    ".conf",
    ".desktop",
    ".json",
    ".knsrc",
    ".latte",
    ".qml",
    ".rc",
    ".xml",
}


def xdg_config_home() -> Path:
    return Path(os.environ.get("XDG_CONFIG_HOME", Path.home() / ".config"))


def xdg_data_home() -> Path:
    return Path(os.environ.get("XDG_DATA_HOME", Path.home() / ".local" / "share"))


def rewrite_text(text: str, reverse: bool) -> str:
    mapping = {v: k for k, v in LATTE_TO_SYNDOCK.items()} if reverse else LATTE_TO_SYNDOCK
    for source, target in mapping.items():
        text = text.replace(source, target)
    return text


def backup_path(path: Path, stamp: str) -> Path:
    return path.with_name(f"{path.name}.bak.{stamp}")


def copy_file(source: Path, target: Path, stamp: str, dry_run: bool) -> None:
    if not source.exists():
        print(f"skip missing file: {source}")
        return

    print(f"copy file: {source} -> {target}")
    if dry_run:
        return

    target.parent.mkdir(parents=True, exist_ok=True)
    if target.exists():
        shutil.copy2(target, backup_path(target, stamp))
    shutil.copy2(source, target)


def copy_tree(source: Path, target: Path, stamp: str, dry_run: bool) -> None:
    if not source.exists():
        print(f"skip missing directory: {source}")
        return

    print(f"copy directory: {source} -> {target}")
    if dry_run:
        return

    if target.exists():
        backup = backup_path(target, stamp)
        if backup.exists():
            shutil.rmtree(backup)
        shutil.copytree(target, backup)
    shutil.copytree(source, target, dirs_exist_ok=True)


def rewrite_paths(paths: list[Path], reverse: bool, dry_run: bool) -> None:
    for root in paths:
        if root.is_file():
            candidates = [root]
        elif root.exists():
            candidates = [p for p in root.rglob("*") if p.is_file()]
        else:
            continue

        for path in candidates:
            if path.suffix not in TEXT_SUFFIXES and path.name not in {"syndockrc", "lattedockrc"}:
                continue
            try:
                original = path.read_text(encoding="utf-8")
            except UnicodeDecodeError:
                continue
            rewritten = rewrite_text(original, reverse)
            if rewritten != original:
                print(f"rewrite ids: {path}")
                if not dry_run:
                    path.write_text(rewritten, encoding="utf-8")


def migrate(reverse: bool, dry_run: bool, copy_data: bool) -> None:
    stamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    config_home = xdg_config_home()
    data_home = xdg_data_home()

    if reverse:
        source_rc = config_home / "syndockrc"
        target_rc = config_home / "lattedockrc"
        source_dirs = [data_home / "syndock"]
        target_dir = config_home / "latte"
    else:
        source_rc = config_home / "lattedockrc"
        target_rc = config_home / "syndockrc"
        source_dirs = [config_home / "latte", data_home / "latte"]
        target_dir = data_home / "syndock"

    copy_file(source_rc, target_rc, stamp, dry_run)
    if copy_data:
        for source_dir in source_dirs:
            if source_dir.exists():
                copy_tree(source_dir, target_dir, stamp, dry_run)
                break
        else:
            print("skip missing Latte/SynDock layout directory")

    rewrite_targets = [target_rc, target_dir] if copy_data else [target_rc]
    rewrite_paths(rewrite_targets, reverse, dry_run)

    print("done")


def main() -> int:
    parser = argparse.ArgumentParser(description="Migrate Latte Dock config to SynDock.")
    parser.add_argument("--reverse", action="store_true", help="copy SynDock config back to Latte names")
    parser.add_argument("--dry-run", action="store_true", help="show planned operations without writing")
    parser.add_argument("--no-copy-data", action="store_true", help="only migrate the rc file")
    args = parser.parse_args()

    migrate(reverse=args.reverse, dry_run=args.dry_run, copy_data=not args.no_copy_data)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
