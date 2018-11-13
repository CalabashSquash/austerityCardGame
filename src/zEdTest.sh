#!/bin/bash

#./ed 2 0 << 'EOF'
#tokens2
#newcardY:4:0,1,1,1
#newcardR:2:1,1,2,3
#newcardB:2:1,1,2,0
#dowhat
#tookA:0,1,1,1
#tookB:0,1,1,1
#dowhat
#eog
#EOF


#./ed 3 1 << 'EOF'
#tokens2
#newcardY:4:1,0,0,0
#newcardB:4:0,1,0,0
#tookA:1,0,0,0
#tookC:0,1,0,0
#tookB:1,1,0,0
#dowhat
#eog
#EOF
#expects
#player will purchase card1

./ed 4 1 << 'EOF'
tokens2
newcardY:4:1,0,0,0
newcardB:4:0,1,0,0
newcardR:4:0,0,1,0
tookA:1,0,0,0
tookC:0,1,0,0
dowhat
eog
EOF

#expects taking 0,1,1,1


./ed 4 1 << 'EOF'
tokens1
newcardY:3:2,0,2,2
tookA:1,0,0,0
tookB:0,1,1,1
dowhat
eog
EOF
#expects wild


./ed 4 1 << 'EOF'
tokens1
newcardY:3:1,0,0,1
tookA:1,0,0,1
tookB:1,0,0,1
wildB
dowhat
eog
EOF
#expects purchase0



./ed 4 1 << 'EOF'
tokens2
newcardY:4:1,0,0,1
newcardB:4:0,1,1,1
tookA:1,0,0,1
tookB:1,0,0,1
tookC:1,0,0,1
tookD:0,1,1,1
dowhat
eog
EOF





#expects 
#take0,1,1,1
#wild
#
#
#




