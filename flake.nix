{
  description = "A simple input device tester for Linux";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in rec {
        packages = flake-utils.lib.flattenTree {
          evtest-qt = pkgs.stdenv.mkDerivation {
            pname = "evtest-qt";
            version = "0.2.0";
            src = nixpkgs.lib.cleanSource ./.;
            nativeBuildInputs = [
              pkgs.cmake
              pkgs.pkgconfig
              pkgs.qt5.wrapQtAppsHook
            ];
            buildInputs = [
              pkgs.qt5.qtbase
            ];
           };
        };
        defaultPackage = packages.evtest-qt;
      });
}
