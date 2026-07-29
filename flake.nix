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
      in {
        packages = rec {
          default = evtest-qt;

          evtest-qt = pkgs.stdenv.mkDerivation {
            pname = "evtest-qt";
            version = "0.2.0";

            src = ./.;

            cmakeFlags = [
              "-DWARNINGS=ON"
              "-DWERROR=ON"
            ];

            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
              qt6.wrapQtAppsHook
            ];

            buildInputs = with pkgs; [
              qt6.qtbase
            ];
           };
        };
      }
    );
}
