# ST-LIB Releases

ST-LIB uses a semiautomated release flow based on per-PR changesets.

## Pull Requests

Each PR into `development` must add exactly one file under `.changesets/`.

Minimal example:

```text
release: minor
summary: Add compile-time Board owner mapping contract checks
```

Rules:

- `major`: breaking API or contract change
- `minor`: backwards-compatible feature
- `patch`: backwards-compatible fix
- `none`: internal/docs/CI change that should be included in the next real release notes without forcing a version bump

The PR workflow validates the file and comments with the projected next semantic version.

## Preparing A Release

When pending changesets reach `development`, the release workflow updates a branch named
`release/next` with:

- `VERSION`
- `CHANGELOG.md`
- archived processed changesets

That branch is exposed as a normal PR back into `development`.

## Publishing

When the `release/next` PR is merged into `development`, the publish workflow creates:

- a git tag named `vX.Y.Z`
- a GitHub Release using the latest changelog entry

If `VERSION` contains a prerelease suffix such as `-beta`, the GitHub Release is marked as a
prerelease.
