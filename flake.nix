{
  description = "NekoCat Project";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:

    flake-utils.lib.eachDefaultSystem (system:

      let
        pkgs = nixpkgs.legacyPackages.${system};

      in 
      {
        devShells.default = pkgs.mkShell 
        {
          buildInputs = with pkgs; 
          [
            gcc
            cmake
            gnumake
            raylib
            raylib
            pkg-config

            raylib
            libX11
          ];
        };
      }
    );
}
