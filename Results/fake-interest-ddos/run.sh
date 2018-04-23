#! /bin/bash

cd ../../../..
./waf --run "fake-interest-ddos --RngRun=2 --frequency=30 --tolerance=10 --withStrategy=true --topo=fake-interest-ddos --output=test-f30-t10"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=30 --tolerance=100 --withStrategy=true --topo=fake-interest-ddos --output=test-f30-t100"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=40 --tolerance=10 --withStrategy=true --topo=fake-interest-ddos --output=test-f40-t10"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=40 --tolerance=100 --withStrategy=true --topo=fake-interest-ddos --output=test-f40-t100"
cd -
Rscript fake-ddos.R test-f30-t10
Rscript fake-ddos.R test-f30-t100
Rscript fake-ddos.R test-f40-t10
Rscript fake-ddos.R test-f40-t100
