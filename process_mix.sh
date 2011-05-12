#!/bin/bash
rm -- bpm-mixed.raw automix.log
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/v/Voices[SpaceFish].idx"
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/v/Voices[SpaceFish].idx" "./index/i/InNovation{originalmix}[AstralProjection]2.idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/i/InNovation{originalmix}[AstralProjection]2.idx" "./index/s/SpiralSpitTrick[Katayama].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/s/SpiralSpitTrick[Katayama].idx" "./index/t/TimeBeganWithTheUniverse[AstralProjection]3.idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/t/TimeBeganWithTheUniverse[AstralProjection]3.idx" "./index/i/InnerReflexion[AviAlgranatiAndBansi].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/i/InnerReflexion[AviAlgranatiAndBansi].idx" "./index/d/DifferentWorlds[AnalogMohicans].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/d/DifferentWorlds[AnalogMohicans].idx" "./index/s/SunStorm[Aeternum].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/s/SunStorm[Aeternum].idx" "./index/p/PowerDrain[Dynamic].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/p/PowerDrain[Dynamic].idx" "./index/s/Snarling{Tld}_5[Hallucinogen].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/s/Snarling{Tld}_5[Hallucinogen].idx" "./index/i/Identify[SectionX].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/i/Identify[SectionX].idx" "./index/i/InYerFacePart2[Cosmosis].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/i/InYerFacePart2[Cosmosis].idx" "./index/d/Diffusion[Shidapu].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/d/Diffusion[Shidapu].idx" "./index/p/PsychicPhenomenon[Ultrabeat]2.idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/p/PsychicPhenomenon[Ultrabeat]2.idx" "./index/s/SpacePussy[Hallucinogen]2.idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/s/SpacePussy[Hallucinogen]2.idx" "./index/b/BeneathAnIndianSky[RoyAquarius]2.idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --mix 8 --slide 16 --temposwitch 16 --dumpraw --verbose --normalize   "./index/b/BeneathAnIndianSky[RoyAquarius]2.idx" "./index/t/ListInLove[Legend8].idx"
bplay -s 44100 -S -b 16 mix.raw &
merger --split   --aftermix  --number
for a in *.raw; do sox -r 44100 -s -w -c 2  "$a" "`basename $a .raw`".wav; rm --  "$a"; done
bladeenc -del *.wav