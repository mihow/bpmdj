#!/bin/bash
rm -- bpm-mixed.raw automix.log
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize  --rescantempo "./index/i/Ionized[AstralProjection]2.idx"
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize  --rescantempo "./index/i/Ionized[AstralProjection]2.idx" "./index/m/MomentOfTruth[ManWithNoName].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize  --rescantempo "./index/m/MomentOfTruth[ManWithNoName].idx" "./index/s/SonicEmpire[MembersOfMayday].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize  --rescantempo "./index/s/SonicEmpire[MembersOfMayday].idx" "./index/s/ShakeyShaker[Hallucinogen].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize  --rescantempo "./index/s/ShakeyShaker[Hallucinogen].idx" "./index/t/Tsjokmoimok[Jocid].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize  --rescantempo "./index/t/Tsjokmoimok[Jocid].idx" "./index/t/TwoFullMoonsAndATrout[UnionJack].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize  --rescantempo "./index/t/TwoFullMoonsAndATrout[UnionJack].idx" "./index/s/Spooky[Bonky].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --split --beforemix    --number
for a in *.raw; do sox -r 44100 -s -w -c 2  "$a" "`basename $a .raw`".wav; rm --  "$a"; done
bladeenc -del *.wav