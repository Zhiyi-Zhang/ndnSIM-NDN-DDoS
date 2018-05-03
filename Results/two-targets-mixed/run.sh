#! /bin/bash

cd ../../../..

./waf --run "two-targets-mixed --RngRun=3 --maxRange=400 --frequency=100 --topo=fake-interest-ddos --output=two-mixed-t500-c1000"

cd -
Rscript two-targets.R two-mixed-t500-c1000 500 1000
