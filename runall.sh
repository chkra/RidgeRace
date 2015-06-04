#!/bin/bash

pref=/home/sjanssen/RR_Test/;
base=/home/sjanssen/Desktop/DZIF/RidgeRace/RidgeRace/;

conf=$1;
confname=$(basename $conf | cut -d "." -f 1);
confpath=$(readlink -f $base/ExampleRuns/Configs/$conf);

cur=$(pwd);
mkdir -p $base/ExampleRuns/PrecomputedResults/$confname;
mkdir -p $base/ExampleRuns/PrecomputedResults/$confname/ExampleRuns/log;
echo "cd $base/ExampleRuns/PrecomputedResults/$confname; $pref/bin/RidgeRace $confpath 2> $base/ExampleRuns/PrecomputedResults/$confname/err > $base/ExampleRuns/PrecomputedResults/$confname/out &";

