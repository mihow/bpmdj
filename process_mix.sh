#!/bin/bash
rm -- bpm-mixed.raw automix.log
merger --mix 4 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize  --rescantempo "./index/ATodaCubaLeGusta[AfroCubanAllStars].idx"
merger --mix 4 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize  --rescantempo "./index/ATodaCubaLeGusta[AfroCubanAllStars].idx" "./index/g/GottaJazz{Kruder&Dorfmeister}[CountBasic].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --split  --middlemix   --number
for a in *.raw; do sox -r 44100 -s -w -c 2  "$a" "`basename $a .raw`".wav; rm --  "$a"; done
bladeenc -del *.wav