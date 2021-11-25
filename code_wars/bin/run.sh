#! /bin/bash

EXE_HOME=$(cd $(dirname $0) && pwd)
RUN_HOME=$(echo ${EXE_HOME} | sed 's/\(.*\/.*\)\/bin/\1/g')

EXEC_BIN="game_play"

if [ "$1" = "" ]; then
    P1_FA=${RUN_HOME}/test_shell/player1/run_fahb.sh
    P1_QIANG=${RUN_HOME}/test_shell/player1/run_qianghb.sh
elif [ "$1" = "js" ]; then
    P1_FA=${RUN_HOME}/test_shell/player1/give.js
    P1_QIANG=${RUN_HOME}/test_shell/player1/grab.js 
else
    echo  "Usage: `basename $0` OR `basename $0` js"
    exit 1
fi

#if [ "$1" = "js" ]; then
 #   P1_FA=${RUN_HOME}/test_shell/player1/give.js
  #  P1_QIANG=${RUN_HOME}/test_shell/player1/grab.js
#else
 #   P1_FA=${RUN_HOME}/test_shell/player1/run_fahb.sh
  #  P1_QIANG=${RUN_HOME}/test_shell/player1/run_qianghb.sh
#fi

P2_FA=${RUN_HOME}/test_shell/player2/run_fahb.sh
P2_QIANG=${RUN_HOME}/test_shell/player2/run_qianghb.sh
P3_FA=${RUN_HOME}/test_shell/player3/run_fahb.sh
P3_QIANG=${RUN_HOME}/test_shell/player3/run_qianghb.sh
P4_FA=${RUN_HOME}/test_shell/player4/run_fahb.sh
P4_QIANG=${RUN_HOME}/test_shell/player4/run_qianghb.sh

echo "">${RUN_HOME}/log/data/display_out.txt 

ulimit -c unlimited


echo "${RUN_HOME}/bin/${EXEC_BIN} ${RUN_HOME}/config ${RUN_HOME}/log \"$P1_FA|$P1_QIANG\" \"$P2_FA|$P2_QIANG\" \"$P3_FA|$P3_QIANG\" \"$P4_FA|$P4_QIANG\""
${RUN_HOME}/bin/${EXEC_BIN} ${RUN_HOME}/config ${RUN_HOME}/log "$P1_FA|$P1_QIANG" "$P2_FA|$P2_QIANG" "$P3_FA|$P3_QIANG" "$P4_FA|$P4_QIANG"

