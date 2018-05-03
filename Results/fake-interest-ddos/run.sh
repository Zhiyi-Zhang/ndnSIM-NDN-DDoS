#! /bin/bash

cd ../../../..
./waf --run "fake-interest-ddos --RngRun=2 --frequency=50 --tolerance=500 --withStrategy=false --topo=fake-interest-ddos --output=test-n-f50-t500"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=50 --tolerance=1000 --withStrategy=false --topo=fake-interest-ddos --output=test-n-f50-t1000"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=500 --withStrategy=false --topo=fake-interest-ddos --output=test-n-f100-t500"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=1000 --withStrategy=false --topo=fake-interest-ddos --output=test-n-f100-t1000"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=50 --tolerance=500 --withStrategy=true --topo=fake-interest-ddos --output=test-f50-t500"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=50 --tolerance=1000 --withStrategy=true --topo=fake-interest-ddos --output=test-f50-t1000"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=500 --withStrategy=true --topo=fake-interest-ddos --output=test-f100-t500"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=1000 --withStrategy=true --topo=fake-interest-ddos --output=test-f100-t1000"

./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=500 --withStrategy=true --topo=fake-interest-ddos --output=new-f100-t500"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=1000 --withStrategy=true --topo=fake-interest-ddos --output=new-f100-t1000"
cd -
Rscript fake-ddos.R test-n-f50-t500 500
Rscript fake-ddos.R test-n-f50-t1000 1000
Rscript fake-ddos.R test-n-f100-t500 500
Rscript fake-ddos.R test-n-f100-t1000 1000
Rscript fake-ddos.R test-f50-t500 500
Rscript fake-ddos.R test-f50-t1000 1000
Rscript fake-ddos.R test-f100-t500 500
Rscript fake-ddos.R test-f100-t1000 1000

Rscript fake-ddos.R test-f100-t500 500
Rscript fake-ddos.R test-f100-t1000 1000
