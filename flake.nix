# SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later

{
  description = "A simple input device tester for Linux";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;

        # VERSION is the source of truth (e.g. "0.3.0-dev"). Nix appends a
        # short git revision so each evaluation is uniquely identifiable.
        # A datestring is intentionally not used: shortRev is stable per
        # commit and sufficient for uniqueness.
        versionBase = lib.strings.removeSuffix "\n" (builtins.readFile ./VERSION);
        gitRev = self.shortRev or self.dirtyShortRev or "dirty";
        version = "${versionBase}+g${gitRev}";
      in {
        packages = rec {
          default = evtest-qt;

          evtest-qt = pkgs.stdenv.mkDerivation {
            pname = "evtest-qt";
            inherit version;

            src = ./.;

            cmakeFlags = [
              "-DWARNINGS=ON"
              "-DWERROR=ON"
              "-DPROJECT_VERSION_FULL=${version}"
            ];

            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
              qt6.wrapQtAppsHook
            ];

            buildInputs = with pkgs; [
              qt6.qtbase
              systemd  # libudev
              libevdev
            ];
          };
        };
      }
    );
}
