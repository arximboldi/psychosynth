{ nixpkgs ? (import <nixpkgs> {}).fetchFromGitHub {
    owner  = "NixOS";
    repo   = "nixpkgs";
    rev    = "05f0934825c2a0750d4888c4735f9420c906b388";
    sha256 = "1g8c2w0661qn89ajp44znmwfmghbbiygvdzq0rzlvlpdiz28v6gy";
  }}:

with import nixpkgs {};

let
  cegui = stdenv.mkDerivation rec {
      pname = "cegui";
      version = "0.8.7";
      src = fetchurl {
        url = "mirror://sourceforge/crayzedsgui/${pname}-${version}.tar.bz2";
        sha256 = "067562s71kfsnbp2zb2bmq8zj3jk96g5a4rcc5qc3n8nfyayhldk";
      };
      buildInputs = [ cmake ogre1_9 freetype boost expat
                      freeimage pkgconfig pcre fribidi ois ];
      meta = with stdenv.lib; {
        homepage = http://cegui.org.uk/;
        description = "C++ Library for creating GUIs";
        license = licenses.mit;
        platforms = platforms.linux;
      };
  };

in

stdenv.mkDerivation rec {
  name = "psynth-dev";
  buildInputs = [
    cmake
    pkgconfig
    ccache
    boost
    ogre1_9
    cegui
    ois
    liblo
    libxml2
    alsaLib
    jack2
    libsndfile
    soundtouch
    libvorbis
  ];
  hardeningDisable = [ "fortify" ];
}
