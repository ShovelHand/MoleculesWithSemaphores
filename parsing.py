import re  #import stuff for regular expressions

#this file is the work of Alexander Carmichael
f = open('out.txt')
h = 0
c = 0
cStarve = 0
hStarve = 0
starvation = 0
mols = 0;
line = f.readline()

#this loop parses every line in make_moleculs output
while line: 
    hyd = re.search('h[0-9]+' ,line)
    if hyd:
        h +=1
	hStarve +=1
    car = re.search('c[0-9]+' ,line)
    if car:
        c +=1
	cStarve+=1
    ethC = re.search('ethynyl radical number [0-9]* was made by actions of c[0-9]*', line)
    if ethC:
	mols +=1
        c -=1  #don't count the atoms that appear on "was created" lines
    #if there was already two carbons and a hydrogen ready, the radical should
    #already have been made
    if cStarve >= 3 & h >=1:
        print "AVAST!  starvations has occurred"
    cStarve -=2;
    hStarve -=1;
    #if already the material was in place, why on this hydrogen?
    ethH = re.search('ethynyl radical number [0-9]* was made by actions of h[0-9]*', line)
    if cStarve >= 2 & h >=1:
        print "AVAST!  starvations has occurred"
    cStarve -=2;
    hStarve -=1;
    if ethH:
        mols +=1
        h -=1  #don't count the atoms that appear on "was created" lines
    print line
    line = f.readline()
#end of parsing loop
    
#return the results 
print "\n\n*****PYTHON PARSING RESULTS****"
print "hydrogens parsed: " + str(h)
print "carbons parsed:   " + str(c)
print "molecules made:   " + str(mols)
if starvation > 0:
    print "starvation appears to have occurred"
else:
        print "no evidence of starvation.  Hooray!"

if c + h != 50:
    print "not all atoms parsed. Something went wrong!"
Cmols = (c/2)
if h > Cmols:
    print "should have made " + str(Cmols)+ " \nmolecules (based on threads parsed), or starvation has occurred"
else:
    print "should have made " + str(h)+ " molecules (based on threads parsed), \nor starvation has occurred"
f.close()

