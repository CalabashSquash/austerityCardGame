#!/bin/bash

./banzai 2 1 << 'EOF'
tokens2
newcardB:2:1,2,3,4
tookB:1,1,1,0
wildB
wildB
wildB
wildB
wildB
wildB
wildB
dowhat
eog
EOF

#expects to buy with right amount of tokens
