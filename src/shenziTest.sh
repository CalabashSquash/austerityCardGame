#!/bin/bash




##################### NEW CARD TESTS #####################
validNewCard1="newcardY:8:3,61,32,98"
validNewCard2="newcardR:20202020:1231230099,22,2212,3,5"
invalidCard1="newcardG:2:2,1,2,3,4"
invalidCard2=""
invalidCard3="newcardY:300d:1,1,2,3,4"
invalidCard4="newcardhello"

./shenzi 2 1 <<'EOF' 2> shenziTests/out1
newcardY:8:3,61,32,98
newcardR:20202020:1231230099,22,2212,3
newcardG:2:2,1,2,3,4
EOF

if [ "`cmp --silent shenziTests/exp1 shenziTests/out1 && echo "success"`" == "success" ]
then
    echo "Test 1 Passed"
else
    echo "Test 1 Failed"
fi

./shenzi 2 1 <<'EOF' 2> shenziTests/out2
newcardY:8:3,61,32,98
newcardR:20202020:1231230099,22,2212,3
newcardY:8:3,61,32,98
newcardR:20202020:1231230099,22,2212,3
newcardY:8:3,61,32,98
newcardR:20202020:1231230099,22,2212,3
newcardY:8:3,61,32,98
newcardR:20202020:1231230099,22,2212,3
newcardG:2:2,1,2,3,4
EOF

if [ "`cmp --silent shenziTests/exp2 shenziTests/out2 && echo "success"`" == "success" ]
then
    echo "Test 2 Passed"
else
    echo "Test 2 Failed"
fi


./shenzi 2 0 <<'EOF' 
tokens2
newcardP:3:0,1,1,2
newcardY:2:1,0,0,1
newcardR:1:0,0,0,1
newcardB:4:2,1,1,2
newcardR:2:1,0,1,1
newcardY:1:0,1,0,1
newcardP:1:0,1,1,1
dowhat
tookA:1,1,1,0
tookB:0,1,1,1
dowhat
wildB
wildA
dowhat
purchasedA:1:1,0,0,0,1
wildB
dowhat
wildA
purchasedB:1:0,0,0,1,0
dowhat
purchasedA:4:0,1,0,0,1
tookB:1,1,0,1
dowhat
wildA
wildB
dowhat
purchasedA:2:0,0,0,0,1
purchasedB:1:1,1,1,1,1
tookA:1,1,1,0
wildB
purchasedA:1:0,1,0,0,0
purchasedB:0:0,0,0,0,2
eog
EOF



exit
