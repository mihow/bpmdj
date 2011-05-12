#!/bin/sh

# Installation script for Slackware systems

LPREFIX="/usr/local"
# The location prefix for the installed binaries (eg. "/usr/local" to install to "/usr/local/bin")

BINTGZDIR=".."
# ...where bpmdj-X.Y.bin.tgz is located

if test ! -e /etc/slackware-version; then
    echo "Using $0 is recommended only on Slackware systems!" 1>&2
    exit -1
fi
if test "`echo $LPREFIX | cut -b 1 `" != "/"; then
    echo "Install directory \"$LPREFIX\" is not an absolute path!" 1>&2
    echo "Edit the LPREFIX variable in this file" 1>&2
    exit -1
fi
LOCALDIR=$PWD
PRGVER=`ls $BINTGZDIR/*bin.tgz | rev | cut -d '.' -f 3- -| rev | cut -d '-' -f 2 -`
if test -z $PRGVER;then
    echo "Cannot find file $BINTGZDIR/bpmdj-X.Y.bin.tgz" 1>&2
    exit -1
fi
echo "Creating Slackware package for BpmDj-$PRGVER"
mkdir -p +-FAKEROOT-+$LPREFIX/lib
mkdir -p +-FAKEROOT-+$LPREFIX/bin
tar -xvzf $BINTGZDIR/bpmdj-$PRGVER.bin.tgz > /dev/null
mv bpmdj-$PRGVER +-FAKEROOT-+$LPREFIX/lib
mkdir +-FAKEROOT-+/install
echo "# HOW TO EDIT THIS FILE:" > +-FAKEROOT-+/install/slack-desc
echo "# The "handy ruler" below makes it easier to edit a package description.  Line" >> +-FAKEROOT-+/install/slack-desc
echo "# up the first '|' above the ':' following the base package name, and the '|'" >> +-FAKEROOT-+/install/slack-desc
echo "# on the right side marks the last column you can put a character in.  You must" >> +-FAKEROOT-+/install/slack-desc
echo "# make exactly 11 lines for the formatting to be correct.  It's also" >> +-FAKEROOT-+/install/slack-desc
echo "# customary to leave one space after the ':'." >> +-FAKEROOT-+/install/slack-desc
echo "     |-----handy-ruler--------------------------------------------------------|" >> +-FAKEROOT-+/install/slack-desc
echo "bpmdj: " >> +-FAKEROOT-+/install/slack-desc
echo "bpmdj: BpmDj: Free Dj Tools" >> +-FAKEROOT-+/install/slack-desc
echo "bpmdj: Copyright (C) 2001-2007 Werner Van Belle - werner.van.belle@gmail.com" >> +-FAKEROOT-+/install/slack-desc
echo "bpmdj: " >> +-FAKEROOT-+/install/slack-desc
echo "bpmdj: This program is a fully automatic BPM counter. Given an MP3 file it" >> +-FAKEROOT-+/install/slack-desc
echo "bpmdj: will automatically find the tempo. The package contains a bpm-counter," >> +-FAKEROOT-+/install/slack-desc
echo "bpmdj: a song-player which uses this information to play a song at a different" >> +-FAKEROOT-+/install/slack-desc
echo "bpmdj: tempo and a file selector that offers the possibility to manage a large" >> +-FAKEROOT-+/install/slack-desc
echo "bpmdj: amount of mp3's." >> +-FAKEROOT-+/install/slack-desc
echo "bpmdj: " >> +-FAKEROOT-+/install/slack-desc
echo "bpmdj: There is a complete manual online at http://bpmdj.sourceforge.net/" >> +-FAKEROOT-+/install/slack-desc
DIRLIST=`ls -d +-FAKEROOT-+$LPREFIX/lib/bpmdj-$PRGVER/*bpm* | rev | cut -d '/' -f 1 - | rev`
for CPF in $DIRLIST; do
    if test -x "+-FAKEROOT-+$LPREFIX/lib/bpmdj-$PRGVER/$CPF"; then
	ln -sf ../lib/bpmdj-$PRGVER/$CPF +-FAKEROOT-+$LPREFIX/bin/$CPF
    fi
done
cd $LOCALDIR/+-FAKEROOT-+
/sbin/makepkg -l y -c y $LOCALDIR/bpmdj-$PRGVER-`uname -m`-1.tgz
cd $LOCALDIR
rm -rf +-FAKEROOT-+
if test "$1" == "-i"; then
    /sbin/installpkg $LOCALDIR/bpmdj-$PRGVER-`uname -m`-1.tgz
fi