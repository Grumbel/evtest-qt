{
  description = "A simple input device tester for Linux";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.05";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages = rec {
          default = evtest-qt;

          evtest-qt = pkgs.stdenv.mkDerivation {
            pname = "evtest-qt";
            version = "0.2.0";
            src = nixpkgs.lib.cleanSource ./.;
            cmakeFlags = [
              "-DWARNINGS=ON"
              "-DWERROR=ON"
            ];

            nativeBuildInputs = with pkgs; [
              cmake
              pkgconfig
              qt6.wrapQtAppsHook
            ];

            buildInputs = with pkgs; [
              qt6.qtbase
            ] ++ [
              tinycmmc.packages.${system}.default
            ];
           };
        };
      }
    );
}
