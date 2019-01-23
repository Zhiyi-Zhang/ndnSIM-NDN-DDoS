#! /bin/bash

cd ../../../..

./waf --run "two-targets-valid --RngRun=3 --frequency=100 --capacity=750 --tolerance=0 --withStrategy=true --topo=fake-interest-ddos --output=valid-f100-c750" 2>&1 | tee src/ndnSIM/Results/legitimate-percentage/two-valid-f100-c750

cd -
Rscript two-targets.R valid-f100-c750 750
