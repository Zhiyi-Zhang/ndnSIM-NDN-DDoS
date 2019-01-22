#! /bin/bash

cd ../../../..

./waf --run "two-targets-fake --RngRun=3 --frequency=100 --tolerance=500 --withStrategy=true --topo=fake-interest-ddos --output=fake-f100-c1000-t500" 2>&1 | tee src/ndnSIM/Results/legitimate-percentage/two-fake-f100-t500

cd -
Rscript two-targets.R fake-f100-c1000-t500 500
