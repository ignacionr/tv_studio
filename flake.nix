{
  description = "tv_studio development environment and build package";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [
        "x86_64-darwin"
        "aarch64-darwin"
        "x86_64-linux"
        "aarch64-linux"
      ];
      forEachSupportedSystem = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
        pkgs = import nixpkgs {
          inherit system;
          config = { };
        };
      });
    in
    {
      devShells = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            cmake
            pkg-config
            SDL2
            SDL2_image
            SDL2_ttf
            libwebp
            libjpeg
            libpng
            libtiff
            harfbuzz
            freetype
            gnumake
          ];

          shellHook = ''
            echo "========================================================"
            echo " Welcome to the tv_studio Nix development environment!   "
            echo " To build using CMake:                                  "
            echo "   cmake -B build                                       "
            echo "   cmake --build build                                  "
            echo " To build using the Makefile:                           "
            echo "   cd build_mac && make                                 "
            echo "========================================================"
          '';
        };
      });

      packages = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.stdenv.mkDerivation {
          pname = "tv_studio";
          version = "0.1.0";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            pkg-config
          ];

          buildInputs = with pkgs; [
            SDL2
            SDL2_image
            SDL2_ttf
            libwebp
            libjpeg
            libpng
            libtiff
            harfbuzz
            freetype
          ];

          installPhase = ''
            mkdir -p $out/bin
            cp tv_studio $out/bin/
            cp -r $src/rsrc $out/bin/
          '';
        };
      });
    };
}
