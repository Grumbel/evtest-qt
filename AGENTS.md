<!--
SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
SPDX-License-Identifier: GPL-3.0-or-later
-->

# AGENTS.md

Guidance for AI agents and contributors working on **evtest-qt**.

## Project overview

`evtest-qt` is a Qt-based graphical tester for Linux input devices (evdev). It lists attached devices and visualizes absolute axes, relative axes, buttons, and multitouch state in real time.

Build system: CMake (version and warning flags are set directly in `CMakeLists.txt`). UI: Qt5/Qt6. Core logic lives under `src/evdev/` and widgets under `src/widgets/`.

## Versioning

The **`VERSION` file is the sole source of truth** for the project version. Do not invent or hard-code version numbers elsewhere.

Format on the development branch:

```
0.3.0-dev
```

Meaning:

- The file holds the *next* release version with a `-dev` suffix while work is in progress.
- **Before a release:** strip `-dev` (e.g. `0.3.0`), commit, and tag that commit (e.g. `v0.3.0`).
- **After a release:** bump the number and re-attach `-dev` (e.g. `0.4.0-dev`) on the following commit.

CMake reads `VERSION` in `CMakeLists.txt` and exposes it as `PROJECT_VERSION` / `EVTEST_QT_VERSION`.

Nix (`flake.nix`) reads the same file and appends a short git revision for unique package identities:

```nix
versionBase = lib.strings.removeSuffix "\n" (builtins.readFile ./VERSION);
gitRev = self.shortRev or self.dirtyShortRev or "dirty";
version = "${versionBase}+g${gitRev}";
```

A calendar datestring is **not** appended. `shortRev` / `dirtyShortRev` already uniquely identify the tree; a date adds noise without improving reproducibility. Prefer the git revision form above.

Agents must not change `VERSION` unless the task is explicitly a release or a version bump.

## Working conventions

- Prefer small, focused changes that match existing style (GPL-3.0 headers, namespaces, include guards).
- Keep UI widgets lightweight; heavy logic belongs in `evdev/` types (`EvdevDevice`, `EvdevInfo`, `EvdevState`).
- Event code names come from **libevdev** (`libevdev_event_code_get_name`); do not reintroduce generated `*_list.x` tables.
- When adding or changing UI feedback (e.g. showing raw values), keep the existing verification-mode behaviour intact unless the task explicitly changes it.
- Test builds with both Debug and Release when practical (`cmake -DCMAKE_BUILD_TYPE=...`).
- Warning flags: `-DWARNINGS=ON` and `-DWERROR=ON` (defined in `CMakeLists.txt`, not an external module).

## Commit messages

**Each change shall be described in a detailed git commit message at the end of the chat.**

Write the message as a complete, ready-to-use `git commit` body: a short subject line (≤ ~72 characters), a blank line, then a detailed explanation of *what* changed and *why*. Include relevant paths, behaviour before/after, and any user-visible effects. Do not leave the commit message as a vague summary; it must be suitable to paste directly into `git commit`.
