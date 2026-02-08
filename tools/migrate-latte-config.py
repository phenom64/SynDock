#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SynDock Configuration Migration Tool

This file is a part of the Atmo desktop experience's SynDock project for SynOS.
Copyright (C) 2026 Syndromatic Ltd. All rights reserved
Designed by Kavish Krishnakumar in Manchester.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This script migrates existing Latte Dock configurations to SynDock format.

Usage:
    ./migrate-latte-config.py [--dry-run] [--backup-dir PATH]
"""

import os
import sys
import shutil
import argparse
import configparser
from pathlib import Path
from datetime import datetime


def get_latte_config_dir() -> Path:
    """Returns the Latte Dock configuration directory."""
    config_home = os.environ.get('XDG_CONFIG_HOME', 
                                  os.path.expanduser('~/.config'))
    return Path(config_home) / 'latte'


def get_syndock_config_dir() -> Path:
    """Returns the SynDock configuration directory."""
    config_home = os.environ.get('XDG_CONFIG_HOME', 
                                  os.path.expanduser('~/.config'))
    return Path(config_home) / 'syndock'


def backup_configs(source_dir: Path, backup_dir: Path) -> bool:
    """
    Creates a timestamped backup of the configuration directory.
    
    Args:
        source_dir: Directory to back up
        backup_dir: Destination for backup
        
    Returns:
        True if backup succeeded, False otherwise
    """
    if not source_dir.exists():
        print(f"⚠️  Source directory does not exist: {source_dir}")
        return False
    
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    backup_path = backup_dir / f"latte_backup_{timestamp}"
    
    try:
        shutil.copytree(source_dir, backup_path)
        print(f"✅ Backed up to: {backup_path}")
        return True
    except Exception as e:
        print(f"❌ Backup failed: {e}")
        return False


def migrate_layout_file(latte_layout: Path, syndock_layout: Path, 
                        dry_run: bool = False) -> bool:
    """
    Migrates a single Latte Dock layout file to SynDock format.
    
    Key changes:
    - Renames 'latte-dock' references to 'syndock'
    - Updates containment plugin names
    - Adjusts icon sizes to SynOS defaults (64px)
    
    Args:
        latte_layout: Path to Latte layout file
        syndock_layout: Destination path for SynDock layout
        dry_run: If True, don't actually write changes
        
    Returns:
        True if migration succeeded
    """
    if dry_run:
        print(f"🔍 [DRY RUN] Would migrate: {latte_layout.name}")
        return True
    
    try:
        with open(latte_layout, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Replace Latte-specific entries
        replacements = {
            'org.kde.latte.containment': 'org.syndromatic.syndock.containment',
            'org.kde.latte.plasmoid': 'org.syndromatic.syndock.plasmoid',
            'latte-dock': 'syndock',
            'Latte Dock': 'SynDock',
        }
        
        for old, new in replacements.items():
            content = content.replace(old, new)
        
        # Write migrated content
        syndock_layout.parent.mkdir(parents=True, exist_ok=True)
        with open(syndock_layout, 'w', encoding='utf-8') as f:
            f.write(content)
        
        print(f"✅ Migrated: {latte_layout.name} → {syndock_layout.name}")
        return True
        
    except Exception as e:
        print(f"❌ Failed to migrate {latte_layout.name}: {e}")
        return False


def migrate_all_layouts(latte_dir: Path, syndock_dir: Path, 
                        dry_run: bool = False) -> int:
    """
    Migrates all layout files from Latte to SynDock.
    
    Args:
        latte_dir: Latte Dock config directory
        syndock_dir: SynDock config directory
        dry_run: If True, don't write changes
        
    Returns:
        Number of successfully migrated layouts
    """
    layouts_dir = latte_dir / 'layouts'
    
    if not layouts_dir.exists():
        print(f"⚠️  No layouts directory found at {layouts_dir}")
        return 0
    
    migrated = 0
    layout_files = list(layouts_dir.glob('*.layout.latte'))
    
    print(f"\n📂 Found {len(layout_files)} layout(s) to migrate...")
    
    for layout_file in layout_files:
        # Convert .layout.latte → .layout.syndock
        new_name = layout_file.stem.replace('.layout', '') + '.layout.syndock'
        new_path = syndock_dir / 'layouts' / new_name
        
        if migrate_layout_file(layout_file, new_path, dry_run):
            migrated += 1
    
    return migrated


def migrate_global_settings(latte_dir: Path, syndock_dir: Path,
                            dry_run: bool = False) -> bool:
    """
    Migrates global Latte Dock settings to SynDock.
    
    Args:
        latte_dir: Latte config directory
        syndock_dir: SynDock config directory
        dry_run: If True, don't write changes
        
    Returns:
        True if migration succeeded
    """
    latte_rc = latte_dir / 'lattedockrc'
    
    if not latte_rc.exists():
        print("⚠️  No lattedockrc found, skipping global settings")
        return True
    
    if dry_run:
        print("🔍 [DRY RUN] Would migrate: lattedockrc → syndockrc")
        return True
    
    try:
        config = configparser.ConfigParser()
        config.read(latte_rc)
        
        # Apply SynOS defaults
        if 'General' not in config:
            config['General'] = {}
        
        # Update defaults to SynOS standards
        config['General']['IconSize'] = '64'  # NSE::Defaults::IconSize
        config['General']['ZoomFactor'] = '1.40'  # 40% zoom
        
        syndock_rc = syndock_dir / 'syndockrc'
        syndock_dir.mkdir(parents=True, exist_ok=True)
        
        with open(syndock_rc, 'w', encoding='utf-8') as f:
            config.write(f)
        
        print(f"✅ Migrated: lattedockrc → syndockrc")
        return True
        
    except Exception as e:
        print(f"❌ Failed to migrate settings: {e}")
        return False


def main():
    """Main entry point for the migration script."""
    parser = argparse.ArgumentParser(
        description='Migrate Latte Dock configurations to SynDock'
    )
    parser.add_argument(
        '--dry-run', '-n',
        action='store_true',
        help='Show what would be done without making changes'
    )
    parser.add_argument(
        '--backup-dir', '-b',
        type=Path,
        default=Path.home() / '.local/share/syndock/backups',
        help='Directory to store configuration backups'
    )
    parser.add_argument(
        '--no-backup',
        action='store_true',
        help='Skip creating a backup (not recommended)'
    )
    
    args = parser.parse_args()
    
    print("=" * 60)
    print("  SynDock Configuration Migration Tool")
    print("  Copyright (C) 2026 Syndromatic Ltd.")
    print("=" * 60)
    
    latte_dir = get_latte_config_dir()
    syndock_dir = get_syndock_config_dir()
    
    print(f"\n📁 Latte config: {latte_dir}")
    print(f"📁 SynDock config: {syndock_dir}")
    
    if not latte_dir.exists():
        print("\n❌ No Latte Dock configuration found. Nothing to migrate.")
        return 1
    
    # Create backup unless skipped
    if not args.no_backup and not args.dry_run:
        print("\n📦 Creating backup...")
        if not backup_configs(latte_dir, args.backup_dir):
            print("⚠️  Backup failed. Proceeding with caution...")
    
    # Migrate layouts
    print("\n🔄 Migrating layouts...")
    layouts_migrated = migrate_all_layouts(latte_dir, syndock_dir, args.dry_run)
    
    # Migrate global settings
    print("\n🔄 Migrating global settings...")
    settings_ok = migrate_global_settings(latte_dir, syndock_dir, args.dry_run)
    
    # Summary
    print("\n" + "=" * 60)
    print("  Migration Summary")
    print("=" * 60)
    print(f"  Layouts migrated: {layouts_migrated}")
    print(f"  Settings migrated: {'✅' if settings_ok else '❌'}")
    
    if args.dry_run:
        print("\n🔍 This was a DRY RUN. No changes were made.")
        print("   Remove --dry-run to perform the actual migration.")
    else:
        print("\n✅ Migration complete!")
        print(f"   SynDock configs are now in: {syndock_dir}")
    
    return 0


if __name__ == '__main__':
    sys.exit(main())
