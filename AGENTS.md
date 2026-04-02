# AGENTS.md

## Releases And Changesets

Treat [`docs/releases.md`](docs/releases.md) as the source of truth for the release flow.

- For a normal PR into `development` that changes ST-LIB, add exactly one new file under [`.changesets`](.changesets).
- Use [`.changesets/README.md`](.changesets/README.md) and [`.changesets/TEMPLATE.md`](.changesets/TEMPLATE.md) for the exact format.
- Changesets must follow this shape:

```text
release: patch
summary: Short user-visible summary in one line

Optional extra context in markdown.
```

- Choose `release:` like this:
  - `major` for breaking API or contract changes.
  - `minor` for backwards-compatible features.
  - `patch` for backwards-compatible fixes.
  - `none` only for internal, docs, or CI changes that should appear in release notes without bumping the version by themselves.
- If the change affects public behavior, API, contracts, or shipped functionality, do not use `none`.
- Do not manually bump [`VERSION`](VERSION) or edit [`CHANGELOG.md`](CHANGELOG.md) for normal feature or fix work. Release automation updates them on `release/next`.
- Expect CI to validate the changeset, roll pending changesets into `release/next`, archive processed files, tag `vX.Y.Z`, and publish the GitHub Release after the `release/next` PR merges.
