{ nixpkgs ? (import <nixpkgs> {}).fetchFromGitHub {
    owner  = "NixOS";
    repo   = "nixpkgs";
    rev    = "05f0934825c2a0750d4888c4735f9420c906b388";
    sha256 = "1g8c2w0661qn89ajp44znmwfmghbbiygvdzq0rzlvlpdiz28v6gy";
  }}:

with import nixpkgs {};

let
  ogre1_8 = stdenv.mkDerivation {
    name = "ogre-1.8.1";
    src = fetchurl {
      url = "mirror://sourceforge/ogre/1.8.1/ogre_src_v1-8-1.tar.bz2";
      sha256 = "1avadx87sdfdk8165wlffnd5dzks694dcdnkg3ijap966k4qm46s";
    };
    cmakeFlags = [ "-DOGRE_INSTALL_SAMPLES=yes" ]
                 ++ (map (x: "-DOGRE_BUILD_PLUGIN_${x}=on")
                 [ "BSP" "CG" "OCTREE" "PCZ" "PFX" ])
                 ++ (map (x: "-DOGRE_BUILD_RENDERSYSTEM_${x}=on") [ "GL" ]);
    buildInputs = [ cmake pkgconfig mesa
                    freetype freeimage zziplib
                    xorg.xorgproto xorg.libXrandr xorg.libXaw
                    xorg.libXmu xorg.libSM xorg.libXxf86vm
                    xorg.libICE xorg.libXrender xorg.libX11
                    xorg.libXxf86vm xorg.libXt
                    freeglut libpng boost ois
                    nvidia_cg_toolkit ];
    meta = {
      description = "A 3D engine";
      homepage = http://www.ogre3d.org3/;
      maintainers = [ stdenv.lib.maintainers.raskin ];
      platforms = stdenv.lib.platforms.linux;
      license = "MIT";
    };
  };

  cegui_7 = stdenv.mkDerivation rec {
      pname = "cegui";
      version = "0.7.5";
      src = fetchurl {
        url = "https://master.dl.sourceforge.net/project/crayzedsgui/CEGUI%20Mk-2/${version}/CEGUI-${version}.tar.gz";
        sha256 = "1cjccap7y25mnlxn1kky4hsfhbbddrsp2wlwf83lik9zl0qrw111";
      };
      buildInputs = [ ogre1_8 freetype boost expat
                      freeimage pkgconfig pcre fribidi ois
                      python2 ];
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
    ogre1_8
    cegui_7
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
