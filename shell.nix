{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    cmake
    pkg-config
    gnumake
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
}
