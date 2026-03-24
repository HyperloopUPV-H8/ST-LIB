# Changesets

Each pull request targeting `development` must add exactly one file to this directory.

Use this format:

```text
release: patch
summary: Fix scheduler race condition when registering timers

Optional extra context in markdown.
```

Allowed values for `release:` are:

- `major` for breaking API or contract changes
- `minor` for backwards-compatible features
- `patch` for backwards-compatible fixes
- `none` for changes that should wait for the next real release without bumping the version on their own

The release workflows aggregate all pending changesets, calculate the next semantic version, update
`VERSION`, prepend `CHANGELOG.md`, and archive the processed files under `.changesets/archive/`.
