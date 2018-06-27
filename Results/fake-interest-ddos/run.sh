#! /bin/bash

cd ../../../..
./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=500 --withStrategy=true --topo=fake-interest-ddos --output=test-f100-t500"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=200 --tolerance=200 --withStrategy=true --topo=fake-interest-ddos --output=test-f200-t200"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=200 --tolerance=500 --withStrategy=true --topo=fake-interest-ddos --output=test-f200-t500"

./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=500 --withStrategy=true --topo=fake-interest-ddos --output=new-f100-t500"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=1000 --withStrategy=true --topo=fake-interest-ddos --output=new-f100-t1000"
cd -
Rscript fake-ddos.R test-f100-t500 500
Rscript fake-ddos.R test-f100-t1000 1000

Rscript fake-ddos.R test-f100-t500 500
Rscript fake-ddos.R test-f100-t1000 1000

Rscript fake-ddos.R test-f200-t200 200
Rscript fake-ddos.R test-f200-t500 500
