#!/bin/bash
#Author: Michail Vourlakos, Smith Ar
#Summary: Development installation script for SynDock
set -e

build_type=$1
build_type=${build_type:="Release"}

enable_make_unique=OFF

if [ "$1" == "--enable-make-unique" ] || [ "$2" == "--enable-make-unique" ]  ; then
    enable_make_unique=ON
fi

if ! [ -a build ] ; then
    cmake -S . -B build -G Ninja -DCMAKE_INSTALL_PREFIX=/usr -DKDE_L10N_AUTO_TRANSLATIONS=OFF -DCMAKE_BUILD_TYPE=$build_type
fi

cd build

if [ -a po ] ; then
    sudo rm -fr po
fi
if [ -a locale ] ; then
    sudo rm -fr locale
fi

if [ "$1" == "--translations" ] ; then
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DKDE_L10N_BRANCH=trunk -DKDE_L10N_AUTO_TRANSLATIONS=ON -DENABLE_MAKE_UNIQUE=$enable_make_unique -DCMAKE_BUILD_TYPE=$build_type ..
    ninja fetch-translations
elif [ "$1" == "--translations-stable" ] ; then
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DKDE_L10N_BRANCH=stable -DKDE_L10N_AUTO_TRANSLATIONS=ON -DENABLE_MAKE_UNIQUE=$enable_make_unique -DCMAKE_BUILD_TYPE=$build_type ..
    ninja fetch-translations
else
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DKDE_L10N_AUTO_TRANSLATIONS=OFF -DENABLE_MAKE_UNIQUE=$enable_make_unique -DCMAKE_BUILD_TYPE=$build_type ..
    ninja
fi

sudo ninja install
