#! /bin/bash

cd ../../../..
./waf --run "valid-fine-granularity --RngRun=2 --frequency=100 --capacity=1500 --withStrategy=true --topo=fake-interest-ddos --output=special-f100-c1500" 2>&1 | tee src/ndnSIM/Results/fine-granularity/special-f100-c1500

cd -
