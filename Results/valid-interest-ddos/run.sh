#! /bin/bash

cd ../../../..
./waf --run "valid-interest-ddos --RngRun=3 --maxRange=9000 --frequency=50 --capacity=600 --withStrategy=true --topo=valid-interest-ddos --output=test-f50-c600"
./waf --run "valid-interest-ddos --RngRun=3 --maxRange=9000 --frequency=100 --capacity=600 --withStrategy=true --topo=valid-interest-ddos --output=test-f100-c600"
./waf --run "valid-interest-ddos --RngRun=3 --maxRange=9000 --frequency=50 --capacity=1500 --withStrategy=true --topo=valid-interest-ddos --output=test-f50-c1500"
./waf --run "valid-interest-ddos --RngRun=3 --maxRange=9000 --frequency=100 --capacity=1500 --withStrategy=true --topo=valid-interest-ddos --output=test-f100-c1500"
./waf --run "valid-interest-ddos --RngRun=3 --maxRange=9000 --frequency=50 --capacity=600 --withStrategy=false --topo=valid-interest-ddos --output=test-n-f50-c600"
./waf --run "valid-interest-ddos --RngRun=3 --maxRange=9000 --frequency=100 --capacity=600 --withStrategy=false --topo=valid-interest-ddos --output=test-n-f100-c600"
./waf --run "valid-interest-ddos --RngRun=3 --maxRange=9000 --frequency=50 --capacity=1500 --withStrategy=false --topo=valid-interest-ddos --output=test-n-f50-c1500"
./waf --run "valid-interest-ddos --RngRun=3 --maxRange=9000 --frequency=100 --capacity=1500 --withStrategy=false --topo=valid-interest-ddos --output=test-n-f100-c1500"
./waf --run "valid-interest-ddos --RngRun=3 --maxRange=9000 --frequency=100 --capacity=1500 --withStrategy=true --topo=cripple-topo --output=cripple-f100-c1500"
cd -
Rscript valid-ddos.R test-f50-c600 600
Rscript valid-ddos.R test-f50-c1500 1500
Rscript valid-ddos.R test-n-f50-c600 600
Rscript valid-ddos.R test-n-f100-c600 600
Rscript valid-ddos.R test-n-f50-c1500 1500
Rscript valid-ddos.R test-n-f100-c1500 1500
Rscript valid-ddos.R cripple-f100-c1500 1500

Rscript valid-ddos.R test-f100-c600 600
Rscript valid-ddos.R test-f100-c1500 1500
