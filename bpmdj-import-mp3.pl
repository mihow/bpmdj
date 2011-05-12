#!/usr/bin/perl
#
# Primitive .mp3 import script for bpm-dj
# (C)2003 by Tom Vogt <tom@lemuria.org>
# Released under the GPL
# 
# Use like this:
#
# find . -name '*.mp3' -exec bpmdj-import-mp3.pl {} \;
# mv *.idx ~/index/ (or wherever your index is)
#
# requires perl-mp3-tag

use MP3::Tag;


sub id3read {
    my $song = shift;
    my $tags = MP3::Tag->new($song->{'filename'});
    $tags->get_tags;
    my $info;
    my $name;
    my $val;
    if (exists $tags->{ID3v2}) {
        $song->{ID3v2} = $tags->{ID3v2};
        $song->{ID3v2}->{'title'} = $tags->{ID3v2}->song();
        $song->{ID3v2}->{'artist'} = $tags->{ID3v2}->artist();
        $song->{ID3v2}->{'album'} = $tags->{ID3v2}->album();
        $song->{ID3v2}->{'tracknumber'} = $tags->{ID3v2}->track();
        $song->{ID3v2}->{'date'} = $tags->{ID3v2}->get_frame('TYER');
        $song->{ID3v2}->{'genre'} = $tags->{ID3v2}->get_frame('TCON');
    }

    if (exists $tags->{ID3v1}) {
        $song->{ID3v1}= $tags->{ID3v1};
        $song->{ID3v1}->{'date'} = $tags->{ID3v1}->year();
        $song->{ID3v1}->{'tracknumber'} = $tags->{ID3v1}->track();
        $song->{ID3v1}->{'title'} = $tags->{ID3v1}->song();

    }
    $tags->close();
}


$file = shift @ARGV || die "usage: $0 file\n";
my $song = {};
$song->{'filename'} = $file;
&id3read($song);

$idx=$file;
$idx=~s/(.*\/)?([^\/]*)\.mp3/$2.idx/i;

$Title=$song->{ID3v2}->{'title'};
if ("$Title" eq "") { $Title=$song->{ID3v1}->{'title'}; }
if ("$Title" eq "") { $Title="(unknown)"; }
$Artist=$song->{ID3v2}->{'artist'};
if ("$Artist" eq "") { $Artist=$song->{ID3v1}->{'artist'}; }
if ("$Artist" eq "") { $Artist="(unknown)"; }

open(IDX,">$idx");
print IDX "version  : BpmDj v2.2\n";
print IDX "file     : $file\n";
print IDX "title    : $Title\n";
print IDX "author   : $Artist\n";
print IDX "tag      : New\n";
close(IDX);

