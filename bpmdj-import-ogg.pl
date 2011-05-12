#!/usr/bin/perl
#
# Primitive .ogg import script for bpm-dj
# (C)2003 by Tom Vogt <tom@lemuria.org>
# Released under the GPL
# 
# Use like this:
#
# find . -name '*.ogg' -exec bpmdj-import-ogg.pl {} \;
# mv *.idx ~/index/ (or wherever your index is)
#
# requires ogginfo
#

$file = shift @ARGV || die "usage: $0 file\n";
open(OGG, "ogginfo $file |");
while (<OGG>) {
    if (/^ *(.*)=(.*)$/) {
        # got a tag/comment
        if ("$2" ne "") {
            # it isn't empty
            $Tag=$2;
            if ($1 =~ /artist/i) {
                $Artist=$Tag;
                next;
            }
            if ($1 =~ /title/i) {
                $Title=$Tag;
                next;
            }
            if ($1 =~ /album/i) {
                $Album=$Tag;
                next;
            }
            if ($1 =~ /tracknumber/i) {
                $Tracknumber=$Tag;
                next;
            }
        }
    }
}

$idx=$file;
$idx=~s/(.*\/)?([^\/]*)\.ogg/$2.idx/i;

if ("$Title" eq "") { $Title="(unknown)"; }
if ("$Artist" eq "") { $Artist="(unknown)"; }

open(IDX,">$idx");
print IDX "version  : BpmDj v2.2\n";
print IDX "file     : $file\n";
print IDX "title    : $Title\n";
print IDX "author   : $Artist\n";
print IDX "tag      : New\n";
close(IDX);

