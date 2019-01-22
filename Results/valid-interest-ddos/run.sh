#! /bin/bash

cd ../../../..
./waf --run "valid-interest-ddos --RngRun=2 --frequency=100 --capacity=1500 --withStrategy=true --topo=valid-interest-ddos --output=valid-f100-c1500" 2>&1 | tee src/ndnSIM/Results/legitimate-percentage/valid-f100-c1500

cd -
Rscript valid-ddos.R valid-f100-c1500 1500
