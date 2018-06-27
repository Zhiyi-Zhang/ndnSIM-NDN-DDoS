#! /bin/bash

cd ../../../..
./waf --run "fake-interest-ddos --RngRun=2 --frequency=200 --tolerance=500 --withStrategy=true --topo=fake-interest-ddos --output=legit-percent-test" 2>&1 | tee src/ndnSIM/Results/legitimate-percentage/fake.txt
./waf --run "valid-interest-ddos --RngRun=3 --frequency=200 --capacity=1000 --withStrategy=true --topo=valid-interest-ddos --output=legit-percent-test" 2>&1 | tee src/ndnSIM/Results/legitimate-percentage/valid.txt
./waf --run "mixed-ddos --RngRun=2 --frequency=200 --capacity=1000 --tolerance=500 --topo=fake-interest-ddos --output=legit-percent-test" 2>&1 | tee src/ndnSIM/Results/legitimate-percentage/mixed.txt

cd -
Rscript fake-ddos.R test-f100-t500 500
Rscript fake-ddos.R test-f100-t1000 1000
