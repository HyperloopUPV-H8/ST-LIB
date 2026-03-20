#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import re
import shutil
import subprocess
import sys
from collections import defaultdict
from dataclasses import dataclass
from datetime import date
from pathlib import Path


ALLOWED_RELEASE_TYPES = ("none", "patch", "minor", "major")
RELEASE_PRIORITY = {name: index for index, name in enumerate(ALLOWED_RELEASE_TYPES)}
VERSION_PATTERN = re.compile(
    r"^(?P<major>0|[1-9]\d*)\.(?P<minor>0|[1-9]\d*)\.(?P<patch>0|[1-9]\d*)"
    r"(?:-(?P<prerelease>[0-9A-Za-z.-]+))?$"
)
COMMENT_MARKER = "<!-- st-lib-release-plan -->"
IGNORED_CHANGESET_FILES = {"README.md", "TEMPLATE.md"}
SECTION_TITLES = {
    "major": "Breaking Changes",
    "minor": "Features",
    "patch": "Fixes",
    "none": "Internal",
}


@dataclass(frozen=True)
class SemVer:
    major: int
    minor: int
    patch: int
    prerelease: str | None = None

    @property
    def core(self) -> str:
        return f"{self.major}.{self.minor}.{self.patch}"

    def __str__(self) -> str:
        if self.prerelease:
            return f"{self.core}-{self.prerelease}"
        return self.core


@dataclass(frozen=True)
class Changeset:
    path: Path
    release: str
    summary: str
    details: str


def repo_root(explicit_root: str | None) -> Path:
    if explicit_root:
        return Path(explicit_root).resolve()
    return Path(__file__).resolve().parents[1]


def version_path(root: Path) -> Path:
    return root / "VERSION"


def changelog_path(root: Path) -> Path:
    return root / "CHANGELOG.md"


def changeset_dir(root: Path) -> Path:
    return root / ".changesets"


def parse_version(raw_version: str) -> SemVer:
    match = VERSION_PATTERN.fullmatch(raw_version.strip())
    if not match:
        raise ValueError(f"Invalid semantic version: {raw_version!r}")
    return SemVer(
        major=int(match.group("major")),
        minor=int(match.group("minor")),
        patch=int(match.group("patch")),
        prerelease=match.group("prerelease"),
    )


def read_version(root: Path) -> str:
    return version_path(root).read_text(encoding="utf-8").strip()


def bump_version(current_version: str, release_type: str) -> str:
    if release_type not in RELEASE_PRIORITY:
        raise ValueError(f"Unknown release type: {release_type}")

    current = parse_version(current_version)
    if release_type == "none":
        return str(current)

    if current.prerelease:
        if release_type == "patch":
            return current.core
        if release_type == "minor":
            return f"{current.major}.{current.minor + 1}.0"
        if release_type == "major":
            return f"{current.major + 1}.0.0"

    if release_type == "patch":
        return f"{current.major}.{current.minor}.{current.patch + 1}"
    if release_type == "minor":
        return f"{current.major}.{current.minor + 1}.0"
    return f"{current.major + 1}.0.0"


def active_changeset_paths(root: Path) -> list[Path]:
    directory = changeset_dir(root)
    if not directory.exists():
        return []

    return sorted(
        path
        for path in directory.glob("*.md")
        if path.name not in IGNORED_CHANGESET_FILES
    )


def parse_changeset(path: Path) -> Changeset:
    lines = path.read_text(encoding="utf-8").splitlines()
    non_empty_indexes = [index for index, line in enumerate(lines) if line.strip()]
    if len(non_empty_indexes) < 2:
        raise ValueError(f"{path.name}: expected at least two non-empty lines")

    release_line = lines[non_empty_indexes[0]].strip()
    summary_line = lines[non_empty_indexes[1]].strip()
    if not release_line.startswith("release: "):
        raise ValueError(f"{path.name}: first non-empty line must start with 'release: '")
    if not summary_line.startswith("summary: "):
        raise ValueError(f"{path.name}: second non-empty line must start with 'summary: '")

    release = release_line.split(": ", 1)[1].strip().lower()
    if release not in RELEASE_PRIORITY:
        allowed = ", ".join(ALLOWED_RELEASE_TYPES)
        raise ValueError(f"{path.name}: release must be one of {allowed}")

    summary = summary_line.split(": ", 1)[1].strip()
    if not summary:
        raise ValueError(f"{path.name}: summary cannot be empty")

    details_start = non_empty_indexes[1] + 1
    details = "\n".join(lines[details_start:]).strip()
    return Changeset(path=path, release=release, summary=summary, details=details)


def load_changesets(root: Path) -> list[Changeset]:
    return [parse_changeset(path) for path in active_changeset_paths(root)]


def highest_release(changesets: list[Changeset]) -> str:
    if not changesets:
        return "none"
    return max(changesets, key=lambda item: RELEASE_PRIORITY[item.release]).release


def next_version(root: Path) -> str:
    current_version = read_version(root)
    return bump_version(current_version, highest_release(load_changesets(root)))


def release_ready(root: Path) -> bool:
    return next_version(root) != read_version(root)


def build_preview_payload(root: Path) -> dict[str, object]:
    current_version = read_version(root)
    changesets = load_changesets(root)
    highest = highest_release(changesets)
    computed_next_version = bump_version(current_version, highest)
    return {
        "current_version": current_version,
        "next_version": computed_next_version,
        "highest_release": highest,
        "release_ready": computed_next_version != current_version,
        "pending_changesets": len(changesets),
        "changesets": [
            {
                "path": str(changeset.path.relative_to(root)),
                "release": changeset.release,
                "summary": changeset.summary,
            }
            for changeset in changesets
        ],
    }


def build_preview_markdown(root: Path) -> str:
    payload = build_preview_payload(root)
    lines = [
        "## ST-LIB Release Plan",
        "",
        f"- Current version: `{payload['current_version']}`",
        f"- Pending changesets: `{payload['pending_changesets']}`",
        f"- Highest requested bump: `{payload['highest_release']}`",
    ]

    if payload["release_ready"]:
        lines.append(f"- Next version if merged now: `{payload['next_version']}`")
    else:
        lines.append(
            f"- Next releasable version: no bump yet, current stays at `{payload['current_version']}`"
        )

    if payload["changesets"]:
        lines.extend(["", "### Pending changes", ""])
        for item in payload["changesets"]:
            lines.append(f"- `{item['release']}` {item['summary']} ({item['path']})")
    else:
        lines.extend(["", "No pending changesets were found."])

    return "\n".join(lines).strip() + "\n"


def relevant_changeset_path(root: Path, path_str: str) -> Path | None:
    path = Path(path_str)
    if (
        len(path.parts) == 2
        and path.parts[0] == ".changesets"
        and path.suffix == ".md"
        and path.name not in IGNORED_CHANGESET_FILES
    ):
        return root / path
    return None


def git_changed_changesets(root: Path, base: str, head: str) -> tuple[list[Path], list[Path]]:
    result = subprocess.run(
        ["git", "-c", "diff.renames=false", "diff", "--name-status", f"{base}...{head}", "--"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    changed_files: list[Path] = []
    deleted_files: list[Path] = []

    for raw_line in result.stdout.splitlines():
        if not raw_line.strip():
            continue

        parts = raw_line.split("\t")
        status = parts[0]

        if status.startswith(("R", "C")):
            if len(parts) < 3:
                continue
            old_path = relevant_changeset_path(root, parts[1])
            new_path = relevant_changeset_path(root, parts[2])
            if old_path is not None and new_path is None:
                deleted_files.append(old_path)
            elif new_path is not None:
                changed_files.append(new_path)
            continue

        if len(parts) < 2:
            continue

        path = relevant_changeset_path(root, parts[1])
        if path is None:
            continue
        if status == "D":
            deleted_files.append(path)
        else:
            changed_files.append(path)

    return sorted(set(changed_files)), sorted(set(deleted_files))


def validate_pr_changeset(root: Path, base: str, head: str) -> int:
    changed_changesets, deleted_changesets = git_changed_changesets(root, base, head)
    if deleted_changesets:
        joined = ", ".join(str(path.relative_to(root)) for path in deleted_changesets)
        raise ValueError(
            "PRs must not delete changeset files under .changesets/. "
            f"Deleted changesets: {joined}"
        )
    if len(changed_changesets) != 1:
        joined = ", ".join(str(path.relative_to(root)) for path in changed_changesets) or "none"
        raise ValueError(
            "PRs must add or update exactly one changeset file under .changesets/. "
            f"Changed changesets: {joined}"
        )

    parse_changeset(changed_changesets[0])
    return 0


def build_changelog_entry(version: str, changesets: list[Changeset]) -> str:
    grouped: dict[str, list[Changeset]] = defaultdict(list)
    for changeset in changesets:
        grouped[changeset.release].append(changeset)

    lines = [f"## v{version} - {date.today().isoformat()}", ""]
    for release_type in ("major", "minor", "patch", "none"):
        items = grouped.get(release_type)
        if not items:
            continue
        lines.append(f"### {SECTION_TITLES[release_type]}")
        lines.append("")
        for item in items:
            lines.append(f"- {item.summary}")
            if item.details:
                for detail_line in item.details.splitlines():
                    lines.append(f"  {detail_line}" if detail_line else "  ")
        lines.append("")

    return "\n".join(lines).rstrip()


def render_changelog_with_entry(existing: str, entry: str) -> str:
    heading_match = re.search(r"^## ", existing, flags=re.MULTILINE)
    if heading_match:
        insertion_point = heading_match.start()
        return (
            existing[:insertion_point].rstrip()
            + "\n\n"
            + entry
            + "\n\n"
            + existing[insertion_point:].lstrip()
        )
    else:
        return existing.rstrip() + "\n\n" + entry + "\n"


def archive_changesets(root: Path, version: str, changesets: list[Changeset]) -> list[tuple[Path, Path]]:
    archive_directory = changeset_dir(root) / "archive" / f"v{version}"
    archive_directory.mkdir(parents=True, exist_ok=True)
    destinations = [(changeset.path, archive_directory / changeset.path.name) for changeset in changesets]

    for _, destination in destinations:
        if destination.exists():
            raise FileExistsError(f"Archive destination already exists: {destination}")

    moved_changesets: list[tuple[Path, Path]] = []
    try:
        for changeset in changesets:
            destination = archive_directory / changeset.path.name
            shutil.move(str(changeset.path), destination)
            moved_changesets.append((changeset.path, destination))
    except Exception:
        rollback_archived_changesets(root, moved_changesets)
        raise
    return moved_changesets


def rollback_archived_changesets(root: Path, moved_changesets: list[tuple[Path, Path]]) -> None:
    archive_root = changeset_dir(root) / "archive"
    for source, destination in reversed(moved_changesets):
        if destination.exists():
            shutil.move(str(destination), source)

        current = destination.parent
        while current != archive_root.parent:
            try:
                current.rmdir()
            except OSError:
                break
            current = current.parent
            if current == archive_root.parent:
                break


def apply_release(root: Path) -> str:
    changesets = load_changesets(root)
    if not changesets:
        raise ValueError("No pending changesets found")

    current_version = read_version(root)
    release_type = highest_release(changesets)
    computed_next_version = bump_version(current_version, release_type)
    if computed_next_version == current_version:
        raise ValueError(
            "Pending changesets exist, but all are marked 'none'; nothing to release yet"
        )

    version_file = version_path(root)
    changelog_file = changelog_path(root)
    original_version = version_file.read_text(encoding="utf-8")
    original_changelog = changelog_file.read_text(encoding="utf-8")
    updated_changelog = render_changelog_with_entry(
        original_changelog, build_changelog_entry(computed_next_version, changesets)
    )

    moved_changesets: list[tuple[Path, Path]] = []
    try:
        moved_changesets = archive_changesets(root, computed_next_version, changesets)
        changelog_file.write_text(updated_changelog, encoding="utf-8")
        version_file.write_text(computed_next_version + "\n", encoding="utf-8")
    except Exception:
        if moved_changesets:
            rollback_archived_changesets(root, moved_changesets)
        changelog_file.write_text(original_changelog, encoding="utf-8")
        version_file.write_text(original_version, encoding="utf-8")
        raise
    return computed_next_version


def latest_release_notes(root: Path) -> str:
    lines = changelog_path(root).read_text(encoding="utf-8").splitlines()
    start = None
    end = None
    for index, line in enumerate(lines):
        if line.startswith("## "):
            if start is None:
                start = index
            else:
                end = index
                break
    if start is None:
        raise ValueError("No release entries found in CHANGELOG.md")
    selected = lines[start:end]
    return "\n".join(selected).strip() + "\n"


def command_preview(args: argparse.Namespace) -> int:
    root = repo_root(args.repo_root)
    if args.format == "markdown":
        sys.stdout.write(build_preview_markdown(root))
        return 0
    payload = build_preview_payload(root)
    if args.format == "json":
        json.dump(payload, sys.stdout, indent=2)
        sys.stdout.write("\n")
        return 0

    lines = [
        f"current_version={payload['current_version']}",
        f"next_version={payload['next_version']}",
        f"highest_release={payload['highest_release']}",
        f"release_ready={str(payload['release_ready']).lower()}",
        f"pending_changesets={payload['pending_changesets']}",
    ]
    sys.stdout.write("\n".join(lines) + "\n")
    return 0


def command_validate_pr(args: argparse.Namespace) -> int:
    return validate_pr_changeset(repo_root(args.repo_root), args.base, args.head)


def command_next_version(args: argparse.Namespace) -> int:
    sys.stdout.write(next_version(repo_root(args.repo_root)) + "\n")
    return 0


def command_pending_count(args: argparse.Namespace) -> int:
    sys.stdout.write(str(len(load_changesets(repo_root(args.repo_root)))) + "\n")
    return 0


def command_apply(args: argparse.Namespace) -> int:
    root = repo_root(args.repo_root)
    version = apply_release(root)
    if args.output:
        Path(args.output).write_text(version + "\n", encoding="utf-8")
    sys.stdout.write(version + "\n")
    return 0


def command_latest_notes(args: argparse.Namespace) -> int:
    sys.stdout.write(latest_release_notes(repo_root(args.repo_root)))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="ST-LIB release helper")
    parser.add_argument("--repo-root", help="Repository root override")

    subparsers = parser.add_subparsers(dest="command", required=True)

    preview_parser = subparsers.add_parser("preview", help="Show pending release information")
    preview_parser.add_argument("--format", choices=("text", "markdown", "json"), default="text")
    preview_parser.set_defaults(func=command_preview)

    validate_parser = subparsers.add_parser("validate-pr", help="Validate the changeset touched by a PR")
    validate_parser.add_argument("--base", required=True, help="PR base commit or ref")
    validate_parser.add_argument("--head", required=True, help="PR head commit or ref")
    validate_parser.set_defaults(func=command_validate_pr)

    next_version_parser = subparsers.add_parser("next-version", help="Compute the next semantic version")
    next_version_parser.set_defaults(func=command_next_version)

    pending_count_parser = subparsers.add_parser("pending-count", help="Count pending changesets")
    pending_count_parser.set_defaults(func=command_pending_count)

    apply_parser = subparsers.add_parser("apply", help="Apply the next release to VERSION and CHANGELOG")
    apply_parser.add_argument("--output", help="Optional file where the computed version will be written")
    apply_parser.set_defaults(func=command_apply)

    notes_parser = subparsers.add_parser("latest-notes", help="Print the latest changelog entry")
    notes_parser.set_defaults(func=command_latest_notes)

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    try:
        return args.func(args)
    except Exception as exc:  # noqa: BLE001
        print(f"error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
