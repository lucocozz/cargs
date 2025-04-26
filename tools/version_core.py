#!/usr/bin/env python3
"""Core implementation of version management functionality"""

import hashlib
import os
import re
import shutil
import subprocess
import tempfile
import time
from dataclasses import dataclass
from enum import Enum
from pathlib import Path
from typing import Dict, List, Optional, Tuple


class VersionBump(Enum):
    MAJOR = "major"
    MINOR = "minor"
    PATCH = "patch"


@dataclass
class Version:
    major: int
    minor: int
    patch: int

    @classmethod
    def from_string(cls, version_str: str) -> "Version":
        match = re.match(r"(\d+)\.(\d+)\.(\d+)", version_str)
        if not match:
            raise ValueError(f"Invalid version format: {version_str}")
        return cls(
            major=int(match.group(1)),
            minor=int(match.group(2)),
            patch=int(match.group(3))
        )

    def bump(self, bump_type: VersionBump) -> "Version":
        if bump_type == VersionBump.MAJOR:
            return Version(self.major + 1, 0, 0)
        elif bump_type == VersionBump.MINOR:
            return Version(self.major, self.minor + 1, 0)
        elif bump_type == VersionBump.PATCH:
            return Version(self.major, self.minor, self.patch + 1)
        else:
            raise ValueError(f"Unknown bump type: {bump_type}")

    def __str__(self) -> str:
        return f"{self.major}.{self.minor}.{self.patch}"


class VersionManager:
    def __init__(self, dry_run: bool = False, verbose: bool = False):
        self.dry_run = dry_run
        self.verbose = verbose
        self.repo_root = self._get_repo_root()
        self.current_version = self._get_current_version()
        self.files_to_update = self._collect_files_to_update()

    def _get_repo_root(self) -> Path:
        result = subprocess.run(
            ["git", "rev-parse", "--show-toplevel"],
            capture_output=True,
            text=True,
            check=True,
        )
        return Path(result.stdout.strip())

    def _run_command(self, cmd: List[str], cwd: Optional[Path] = None) -> str:
        if self.verbose:
            print(f"Running: {' '.join(cmd)}")
        
        if self.dry_run and not cmd[0] in ["git", "grep"]:
            return "DRY-RUN"
        
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            cwd=cwd or self.repo_root,
            check=True,
        )
        return result.stdout.strip()

    def _get_current_version(self) -> Version:
        meson_build_path = self.repo_root / "meson.build"
        with open(meson_build_path, "r") as f:
            content = f.read()
        
        match = re.search(r"version: '(\d+\.\d+\.\d+)'", content)
        if not match:
            raise ValueError("Could not find version in meson.build")
        
        return Version.from_string(match.group(1))

    def _collect_files_to_update(self) -> Dict[str, List[Tuple[str, str]]]:
        file_patterns = {
            "meson.build": [
                (r"version: '(\d+\.\d+\.\d+)'", "version: '{}'"),
            ],
            "includes/cargs.h": [
                (r'#define CARGS_VERSION "(\d+\.\d+\.\d+)"', '#define CARGS_VERSION "{}"'),
            ],
            "packaging/conan/conanfile.py": [
                (r'version = "(\d+\.\d+\.\d+)"', 'version = "{}"'),
            ],
            "packaging/vcpkg/vcpkg.json": [
                (r'"version": "(\d+\.\d+\.\d+)"', '"version": "{}"'),
            ],
            "packaging/installers/install.sh": [
                (r'VERSION="(\d+\.\d+\.\d+)"', 'VERSION="{}"'),
            ],
            "README.md": [
                (r'conan install libcargs/(\d+\.\d+\.\d+)@', 'conan install libcargs/{}@'),
                (r'curl -LO https://github.com/lucocozz/cargs/releases/download/v(\d+\.\d+\.\d+)/cargs-', 
                 'curl -LO https://github.com/lucocozz/cargs/releases/download/v{}/cargs-'),
            ],
        }
        
        result = {}
        for file_path, patterns in file_patterns.items():
            full_path = self.repo_root / file_path
            if full_path.exists():
                result[str(full_path)] = patterns
        
        return result

    def update_version(self, new_version: Version) -> None:
        print(f"Updating version from {self.current_version} to {new_version}")
        
        for file_path, patterns in self.files_to_update.items():
            if self.verbose:
                print(f"Updating {file_path}")
            
            with open(file_path, "r") as f:
                content = f.read()
            
            for pattern, replacement in patterns:
                replacement_formatted = replacement.format(new_version)
                content = re.sub(pattern, replacement_formatted, content)
            
            if not self.dry_run:
                with open(file_path, "w") as f:
                    f.write(content)
        
        if not self.dry_run:
            self._run_command(["git", "add", "."])
            self._run_command(["git", "commit", "-m", f"Bump version to {new_version}"])
            print(f"✅ Changes committed with message: 'Bump version to {new_version}'")

    def create_tag(self, version: Version) -> None:
        tag_name = f"v{version}"
        
        self._run_command(["git", "tag", "-a", tag_name, "-m", f"Version {version}"])
        self._run_command(["git", "push", "origin", tag_name])
        
        print(f"✅ Created and pushed tag: {tag_name}")

    def calculate_archive_hash(self, version: Version) -> str:
        if not self.dry_run:
            print("Waiting for GitHub to process the new tag...")
            time.sleep(5)
        
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_path = Path(temp_dir)
            archive_name = f"v{version}.tar.gz"
            archive_path = temp_path / archive_name
            archive_url = f"https://github.com/lucocozz/cargs/archive/refs/tags/{archive_name}"
            
            if not self.dry_run:
                max_attempts = 5
                for attempt in range(1, max_attempts + 1):
                    try:
                        self._run_command(["curl", "-sL", archive_url, "-o", str(archive_path)])
                        
                        if archive_path.exists() and archive_path.stat().st_size > 0:
                            break
                    except subprocess.CalledProcessError:
                        pass
                    
                    print(f"Attempt {attempt} failed, retrying in 3 seconds...")
                    time.sleep(3)
                
                if not archive_path.exists() or archive_path.stat().st_size == 0:
                    raise RuntimeError("Failed to download archive after multiple attempts")
                
                sha512 = hashlib.sha512()
                with open(archive_path, "rb") as f:
                    for chunk in iter(lambda: f.read(4096), b""):
                        sha512.update(chunk)
                return sha512.hexdigest()
            else:
                return "DRY_RUN_SAMPLE_HASH"

    def update_hashes(self, version: Version, sha512: str) -> None:
        # Update hash in conandata.yml
        conandata_path = self.repo_root / "packaging/conan/conandata.yml"
        if conandata_path.exists():
            with open(conandata_path, "r") as f:
                content = f.read()
            
            # Check if version already exists
            version_pattern = f'"{version}":'
            if version_pattern in content:
                # Update existing entry
                content = re.sub(
                    f'{version_pattern}.*?sha256: ".*?"',
                    f'{version_pattern}\n    url: "https://github.com/lucocozz/cargs/archive/refs/tags/v{version}.tar.gz"\n    sha256: "{sha512}"',
                    content,
                    flags=re.DOTALL
                )
            else:
                # Add new version entry
                content = re.sub(
                    r'sources:',
                    f'sources:\n  "{version}":\n    url: "https://github.com/lucocozz/cargs/archive/refs/tags/v{version}.tar.gz"\n    sha256: "{sha512}"',
                    content
                )
            
            if not self.dry_run:
                with open(conandata_path, "w") as f:
                    f.write(content)
        
        # Update hash in vcpkg portfile
        portfile_path = self.repo_root / "packaging/vcpkg/portfile.cmake"
        if portfile_path.exists():
            with open(portfile_path, "r") as f:
                content = f.read()
            
            # Update SHA512 hash
            content = re.sub(
                r'SHA512 [a-fA-F0-9]*',
                f'SHA512 {sha512}',
                content
            )
            
            # Update REF if needed
            if "REF v${VERSION}" not in content:
                content = re.sub(
                    r'REF v[0-9.]+',
                    f'REF v{version}',
                    content
                )
            
            if not self.dry_run:
                with open(portfile_path, "w") as f:
                    f.write(content)
        
        # Commit hash updates
        if not self.dry_run:
            self._run_command(["git", "add", "."])
            self._run_command(["git", "commit", "-m", f"Update SHA512 hash for version {version}"])
            self._run_command(["git", "push"])
            print(f"✅ Hash updates committed and pushed")

    def update_changelog(self, version: Version) -> None:
        changelog_path = self.repo_root / "CHANGELOG.md"
        
        if not changelog_path.exists():
            if self.verbose:
                print("Creating new CHANGELOG.md file")
            
            # Create new changelog file
            changelog_content = f"""# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [{version}] - {time.strftime("%Y-%m-%d")}

### Added
- [Add your new features here]

### Changed
- [Add your changes here]

### Fixed
- [Add your bug fixes here]

"""
            if not self.dry_run:
                with open(changelog_path, "w") as f:
                    f.write(changelog_content)
        else:
            with open(changelog_path, "r") as f:
                content = f.read()
            
            # Check if version already exists
            if f"## [{version}]" in content:
                print(f"Version {version} already exists in CHANGELOG.md")
                return
            
            # Add new version entry
            content = re.sub(
                r"# Changelog.*?## \[",
                f"# Changelog\n\nAll notable changes to this project will be documented in this file.\n\nThe format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),\nand this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).\n\n## [{version}] - {time.strftime('%Y-%m-%d')}\n\n### Added\n- [Add your new features here]\n\n### Changed\n- [Add your changes here]\n\n### Fixed\n- [Add your bug fixes here]\n\n## [",
                content,
                count=1,
                flags=re.DOTALL
            )
            
            if not self.dry_run:
                with open(changelog_path, "w") as f:
                    f.write(content)
        
        print(f"✅ Updated CHANGELOG.md with version {version}")
        
        if not self.dry_run:
            self._run_command(["git", "add", "CHANGELOG.md"])
            self._run_command(["git", "commit", "--amend", "--no-edit"])
            print("✅ Added CHANGELOG.md to version commit")

    def create_github_release(self, version: Version, prerelease: bool = False) -> None:
        try:
            self._run_command(["gh", "--version"])
        except (subprocess.CalledProcessError, FileNotFoundError):
            print("⚠️ GitHub CLI not installed. Please install it to create GitHub releases.")
            return
        
        try:
            self._run_command(["gh", "auth", "status"])
        except subprocess.CalledProcessError:
            print("⚠️ Not authenticated with GitHub CLI. Please run 'gh auth login'")
            return
        
        cmd = ["gh", "release", "create", f"v{version}"]
        cmd.extend(["--title", f"Version {version}"])
        
        changelog_path = self.repo_root / "CHANGELOG.md"
        if changelog_path.exists():
            with open(changelog_path, "r") as f:
                content = f.read()
            
            pattern = f"## \\[{version}\\].*?(?=## \\[|$)"
            match = re.search(pattern, content, re.DOTALL)
            if match:
                with tempfile.NamedTemporaryFile(mode="w", delete=False) as temp:
                    temp.write(match.group(0))
                    notes_path = temp.name
                
                cmd.extend(["--notes-file", notes_path])
        
        if prerelease:
            cmd.append("--prerelease")
        
        try:
            self._run_command(cmd)
            print(f"✅ Created GitHub release for version {version}")
        except subprocess.CalledProcessError as e:
            print(f"⚠️ Failed to create GitHub release: {e}")
        finally:
            if 'notes_path' in locals():
                os.unlink(notes_path)

    def update_conan_package(self, version, sha512: str, conan_index_path: Optional[str] = None):
        if conan_index_path is None:
            conan_index_path = os.path.expanduser("~/conan-center-index")
        
        conan_recipe_path = os.path.join(conan_index_path, "recipes/libcargs")
        
        if not os.path.exists(conan_recipe_path):
            print(f"⚠️ Could not find Conan recipe at {conan_recipe_path}")
            print(f"Please clone the conan-center-index repository or specify the correct path")
            return
        
        conan_data_path = os.path.join(conan_recipe_path, "all/conandata.yml")
        
        if self.verbose:
            print(f"Updating Conan package at {conan_recipe_path}")
        
        if not self.dry_run:
            # Create a backup
            shutil.copy(conan_data_path, f"{conan_data_path}.bak")
            
            with open(conan_data_path, "r") as f:
                content = f.read()
            
            # Check if version already exists
            version_pattern = f'"{version}":'
            if version_pattern in content:
                content = re.sub(
                    f'{version_pattern}.*?sha256: ".*?"',
                    f'{version_pattern}\n    url: "https://github.com/lucocozz/cargs/archive/refs/tags/v{version}.tar.gz"\n    sha256: "{sha512}"',
                    content,
                    flags=re.DOTALL
                )
            else:
                content = re.sub(
                    r'sources:',
                    f'sources:\n  "{version}":\n    url: "https://github.com/lucocozz/cargs/archive/refs/tags/v{version}.tar.gz"\n    sha256: "{sha512}"',
                    content
                )
            
            with open(conan_data_path, "w") as f:
                f.write(content)
            
            # Update conanfile.py if needed
            conanfile_path = os.path.join(conan_recipe_path, "all/conanfile.py")
            if os.path.exists(conanfile_path):
                with open(conanfile_path, "r") as f:
                    content = f.read()
                
                if re.search(r'name = "libcargs"', content):
                    content = re.sub(
                        r'version = "[0-9.]+"',
                        f'version = "{version}"',
                        content
                    )
                    
                    with open(conanfile_path, "w") as f:
                        f.write(content)
            
            branch_name = f"libcargs-{version}"
            print("\n" + "="*80)
            print(f"✅ Conan package updated successfully")
            print("Next steps for creating a Conan PR:")
            print(f"1. cd {conan_index_path}")
            print(f"2. git checkout -b {branch_name}")
            print(f"3. git add .")
            print(f'4. git commit -m "libcargs: update to {version}"')
            print(f"5. git push -u origin {branch_name}")
            print(f"6. Create a pull request on GitHub")
            print("="*80 + "\n")

    def update_vcpkg_package(self, version, sha512: str, vcpkg_path: Optional[str] = None):
        if vcpkg_path is None:
            vcpkg_path = os.path.expanduser("~/vcpkg")
        
        vcpkg_port_path = os.path.join(vcpkg_path, "ports/libcargs")
        
        if not os.path.exists(vcpkg_port_path):
            print(f"⚠️ Could not find vcpkg port at {vcpkg_port_path}")
            print(f"Please clone the vcpkg repository or specify the correct path")
            return
        
        portfile_path = os.path.join(vcpkg_port_path, "portfile.cmake")
        
        if self.verbose:
            print(f"Updating vcpkg port at {vcpkg_port_path}")
        
        if not self.dry_run:
            # Create a backup
            shutil.copy(portfile_path, f"{portfile_path}.bak")
            
            with open(portfile_path, "r") as f:
                content = f.read()
            
            # Update SHA512 hash
            content = re.sub(
                r'SHA512 [a-fA-F0-9]*',
                f'SHA512 {sha512}',
                content
            )
            
            # Update the version
            if "REF v${VERSION}" in content:
                vcpkg_json_path = os.path.join(vcpkg_port_path, "vcpkg.json")
                if os.path.exists(vcpkg_json_path):
                    with open(vcpkg_json_path, "r") as f:
                        json_content = f.read()
                    
                    json_content = re.sub(
                        r'"version": "[0-9.]+"',
                        f'"version": "{version}"',
                        json_content
                    )
                    
                    with open(vcpkg_json_path, "w") as f:
                        f.write(json_content)
            else:
                content = re.sub(
                    r'REF v[0-9.]+',
                    f'REF v{version}',
                    content
                )
            
            with open(portfile_path, "w") as f:
                f.write(content)
            
            branch_name = f"libcargs-{version}"
            print("\n" + "="*80)
            print(f"✅ vcpkg port updated successfully")
            print("Next steps for creating a vcpkg PR:")
            print(f"1. cd {vcpkg_path}")
            print(f"2. git checkout -b {branch_name}")
            print(f"3. git add .")
            print(f'4. git commit -m "[libcargs] Update to version {version}"')
            print(f"5. git push -u origin {branch_name}")
            print(f"6. Create a pull request on GitHub")
            print("="*80 + "\n")

    def release(self, new_version: Optional[Version] = None, 
                bump_type: Optional[VersionBump] = None, 
                skip_tag: bool = False, skip_packages: bool = False,
                skip_changelog: bool = False,
                skip_github_release: bool = False,
                conan_index_path: Optional[str] = None,
                vcpkg_path: Optional[str] = None) -> None:
        # Determine the new version
        if new_version is None and bump_type is None:
            raise ValueError("Either new_version or bump_type must be specified")
        
        if new_version is None:
            new_version = self.current_version.bump(bump_type)
        
        # Update version in all files
        self.update_version(new_version)
        
        # Update changelog
        if not skip_changelog:
            self.update_changelog(new_version)
        
        # Create git tag
        if not skip_tag:
            self.create_tag(new_version)
            
            # Calculate archive hash
            sha512 = self.calculate_archive_hash(new_version)
            
            # Update hash values
            self.update_hashes(new_version, sha512)
            
            # Update package managers
            if not skip_packages:
                self.update_conan_package(new_version, sha512, conan_index_path)
                self.update_vcpkg_package(new_version, sha512, vcpkg_path)
        
        # Create GitHub release
        if not skip_github_release and not skip_tag:
            self.create_github_release(new_version)
        
        # Final message
        print("\n" + "="*80)
        print(f"🎉 Version {new_version} released successfully!")
        print("="*80)
