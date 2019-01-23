#! /bin/bash

cd ../../../..
# ./waf --run "fake-interest-ddos --RngRun=4 --frequency=100 --tolerance=1000 --withStrategy=true --topo=fake-interest-ddos --output=fake-f100-t1000" 2>&1 | tee src/ndnSIM/Results/legitimate-percentage/fake-f100-t1000

./waf --run "fake-interest-ddos --RngRun=4 --frequency=100 --tolerance=0 --withStrategy=true --topo=fake-interest-ddos --output=fake-f100-t0" 2>&1 | tee src/ndnSIM/Results/legitimate-percentage/fake-f100-t0

cd -
# Rscript fake-ddos.R fake-f100-t1000 1000
Rscript fake-ddos.R fake-f100-t0 0
