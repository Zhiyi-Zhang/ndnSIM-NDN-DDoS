#! /bin/bash

cd ../../../..
./waf --run "mixed-ddos --RngRun=2 --frequency=100 --capacity=600 --tolerance=500 --topo=fake-interest-ddos --output=c600-t500"
./waf --run "mixed-ddos --RngRun=2 --frequency=100 --capacity=1500 --tolerance=1000 --topo=fake-interest-ddos --output=c1500-t1000"
./waf --run "mixed-ddos --RngRun=2 --frequency=100 --capacity=1500 --tolerance=500 --topo=fake-interest-ddos --output=c1500-t500"
cd -

Rscript mixed-ddos.R c1500-t1000 1000 1500

Rscript mixed-ddos.R c600-t500 500 600
Rscript mixed-ddos.R c1500-t500 500 1500
