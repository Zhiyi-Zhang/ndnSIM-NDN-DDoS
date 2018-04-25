#! /bin/bash

cd ../../../..
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=9000 --frequency=50 --capacity=300 --withStrategy=true --topo=valid-interest-ddos --output=test-f50-c300"
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=9000 --frequency=100 --capacity=300 --withStrategy=true --topo=valid-interest-ddos --output=test-f100-c300"
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=9000 --frequency=50 --capacity=1000 --withStrategy=true --topo=valid-interest-ddos --output=test-f50-c1000"
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=9000 --frequency=100 --capacity=1000 --withStrategy=true --topo=valid-interest-ddos --output=test-f100-c1000"
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=9000 --frequency=50 --capacity=300 --withStrategy=false --topo=valid-interest-ddos --output=test-n-f50-c300"
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=9000 --frequency=100 --capacity=300 --withStrategy=false --topo=valid-interest-ddos --output=test-n-f100-c300"
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=9000 --frequency=50 --capacity=1000 --withStrategy=false --topo=valid-interest-ddos --output=test-n-f50-c1000"
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=9000 --frequency=100 --capacity=1000 --withStrategy=false --topo=valid-interest-ddos --output=test-n-f100-c1000"
cd -
Rscript valid-ddos.R test-f50-c300
Rscript valid-ddos.R test-f100-c300
Rscript valid-ddos.R test-f50-c1000
Rscript valid-ddos.R test-f100-c1000
Rscript valid-ddos.R test-n-f50-c300
Rscript valid-ddos.R test-n-f100-c300
Rscript valid-ddos.R test-n-f50-c1000
Rscript valid-ddos.R test-n-f100-c1000
