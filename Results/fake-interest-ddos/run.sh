#! /bin/bash

cd ../../../..
./waf --run "fake-interest-ddos --RngRun=2 --frequency=50 --tolerance=300 --withStrategy=false --topo=fake-interest-ddos --output=test-n-f50-t300"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=50 --tolerance=600 --withStrategy=false --topo=fake-interest-ddos --output=test-n-f50-t600"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=300 --withStrategy=false --topo=fake-interest-ddos --output=test-n-f100-t300"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=600 --withStrategy=false --topo=fake-interest-ddos --output=test-n-f100-t600"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=50 --tolerance=300 --withStrategy=true --topo=fake-interest-ddos --output=test-f50-t300"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=50 --tolerance=600 --withStrategy=true --topo=fake-interest-ddos --output=test-f50-t600"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=300 --withStrategy=true --topo=fake-interest-ddos --output=test-f100-t300"
./waf --run "fake-interest-ddos --RngRun=2 --frequency=100 --tolerance=600 --withStrategy=true --topo=fake-interest-ddos --output=test-f100-t600"
cd -
Rscript fake-ddos.R test-n-f50-t300
Rscript fake-ddos.R test-n-f50-t600
Rscript fake-ddos.R test-n-f100-t300
Rscript fake-ddos.R test-n-f100-t600
Rscript fake-ddos.R test-f50-t300
Rscript fake-ddos.R test-f50-t600
Rscript fake-ddos.R test-f100-t300
Rscript fake-ddos.R test-f100-t600
