function abs(a)
{
    if (a < 0)
	return -a
    else
	return a
}

function choose(drie,zes,f,g,h,i,j) 
{
    a=abs(drie*f-zes)
    b=abs(drie*g-zes)
    c=abs(drie*h-zes)
    d=abs(drie*i-zes)
    e=abs(drie*j-zes)
    if (a < b && a < c && a < d && a < e)
	{ A++ ; return f}
    if (b < a && b < c && b < d && b < e)
	{ B++; return g}
    if (c < a && c < b && c < d && c < e)
	{ C++; return h}
    if (d < a && d < b && d < c && d < e)
	{ D++; return i}
    if (e < a && e < b && e < c && e < d)
	{ E++; return j}
    return "KRASH " a " " b " " c " " d " " e
}

function bpm(p)
{
    return 60*4*11025/p
}

{
    split ($10,AR,":")
    harmonische = choose($3,$6,1,5/4,6/4,7/4,8/4)
    measure = $6 / harmonische
    bpm_actual = bpm($3)
    bpm_measure = bpm(measure)
#    harmonische "\t" bpm_actual "\t" bpm_measure "\t" bpm($6) "\t"\
    diff = abs(bpm_actual-bpm_measure)
    sum += diff
    if (diff < 0.5)
	{ 
	  if (diff > max) max = diff
	  internsum+=diff
	  intersumcount++
#	    variantie+=(diff - 0.0340193)*(diff - 0.0340193)
	  variantie+=diff*diff
	}
    speed = (AR[1]*60+AR[2])/$8
    speedsum += speed
    count ++
    print speed "\t " diff "     \t" $12 
}

END {
print "----------------------"
print "mean speed = " speedsum/count
print "mean bpm measure error = " sum / count
print "mean bpm measure error without outliers = " internsum / intersumcount
print "number of outliers = " count-intersumcount
print "harmonishce problems"
print "   5/4 = " B
print "   6/4 = " C
print "   7/4 = " D
print "   8/4 = " E
print "   1   = " A
print " max below 0.5 is " max
print "standaard afwijking " sqrt(variantie/intersumcount)
}
