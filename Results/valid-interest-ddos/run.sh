#! /bin/bash

cd ../../../..
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=400 --frequency=50 --capacity=300 --topo=valid-interest-ddos --output=test-f50-c300"
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=400 --frequency=100 --capacity=300 --topo=valid-interest-ddos --output=test-f100-c300"
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=400 --frequency=50 --capacity=1000 --topo=valid-interest-ddos --output=test-f50-c1000"
./waf --run "valid-interest-ddos --RngRun=2 --maxRange=400 --frequency=100 --capacity=1000 --topo=valid-interest-ddos --output=test-f100-c1000"
cd -
Rscript valid-ddos.R test-f50-c300
Rscript valid-ddos.R test-f100-c300
Rscript valid-ddos.R test-f50-c1000
Rscript valid-ddos.R test-f100-c1000
