#!/usr/bin/env python3
"""
version.py - Modern version management for the cargs library
"""

import argparse
import os
import sys
from typing import Optional

from version_core import Version, VersionBump, VersionManager


def main():
    """Parse command line arguments and execute requested actions"""
    parser = argparse.ArgumentParser(description="cargs version management tool")
    
    parser.add_argument("--dry-run", action="store_true", help="Show what would be done without making changes")
    parser.add_argument("-v", "--verbose", action="store_true", help="Show detailed output")
    
    subparsers = parser.add_subparsers(dest="command", required=True)
    
    get_parser = subparsers.add_parser("get", help="Get the current version")
    
    bump_parser = subparsers.add_parser("bump", help="Bump the version")
    bump_parser.add_argument("type", choices=["major", "minor", "patch"], help="Type of version bump")
    
    set_parser = subparsers.add_parser("set", help="Set a specific version")
    set_parser.add_argument("version", help="New version (X.Y.Z)")
    
    release_parser = subparsers.add_parser("release", help="Perform a complete release")
    version_group = release_parser.add_mutually_exclusive_group(required=True)
    version_group.add_argument("--bump", choices=["major", "minor", "patch"], help="Bump version by major, minor, or patch")
    version_group.add_argument("--version", help="Set specific version (X.Y.Z)")
    
    release_parser.add_argument("--skip-tag", action="store_true", help="Skip creating git tag")
    release_parser.add_argument("--skip-packages", action="store_true", help="Skip updating package managers")
    release_parser.add_argument("--skip-changelog", action="store_true", help="Skip updating changelog")
    release_parser.add_argument("--skip-github-release", action="store_true", help="Skip creating GitHub release")
    release_parser.add_argument("--conan-path", help="Path to conan-center-index repository")
    release_parser.add_argument("--vcpkg-path", help="Path to vcpkg repository")
    
    args = parser.parse_args()
    
    # Initialize version manager
    version_manager = VersionManager(dry_run=args.dry_run, verbose=args.verbose)
    
    # Execute command
    if args.command == "get":
        print(f"Current version: {version_manager.current_version}")
    
    elif args.command == "bump":
        bump_type = VersionBump(args.type)
        new_version = version_manager.current_version.bump(bump_type)
        version_manager.update_version(new_version)
    
    elif args.command == "set":
        new_version = Version.from_string(args.version)
        version_manager.update_version(new_version)
    
    elif args.command == "release":
        # Determine the new version
        if args.version:
            new_version = Version.from_string(args.version)
        else:
            bump_type = VersionBump(args.bump)
            new_version = None
        
        version_manager.release(
            new_version=new_version,
            bump_type=bump_type if args.bump else None,
            skip_tag=args.skip_tag,
            skip_packages=args.skip_packages,
            skip_changelog=args.skip_changelog,
            skip_github_release=args.skip_github_release,
            conan_index_path=args.conan_path,
            vcpkg_path=args.vcpkg_path
        )
    

if __name__ == "__main__":
    main()
