#!/bin/bash
MAINSIMDIR=../../src/
MAINSIMEXEC=swim

RUNS=""
if [ "$2" != "" ] && [ "$2" != "all" ]; then
    RUNS="-r $2"
fi

INIFILE="swim.ini"
if [ "$3" != "" ]; then
    INIFILE="$3"
fi

opp_runall -j1 $MAINSIMDIR/$MAINSIMEXEC $INIFILE -u Cmdenv -c $1 -n ..:$MAINSIMDIR:../../../queueinglib:../../src -lqueueinglib $RUNS

RESULTDIR=`grep result-dir $INIFILE | cut -d= -f2 | sed -e 's/^[[:space:]]*//'`
pushd $RESULTDIR
scavetool index $1*.vec
popd

#paplay /usr/share/sounds/gnome/default/alerts/bark.ogg
paplay /usr/share/sounds/freedesktop/stereo/dialog-information.oga
