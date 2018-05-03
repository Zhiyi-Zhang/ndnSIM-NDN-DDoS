#! /bin/bash

cd ../../../..

./waf --run "two-targets-fake --RngRun=2 --maxRange=400 --frequency=100 --topo=fake-interest-ddos --output=two-fake-t500"

cd -
Rscript two-targets.R two-fake-t500 500
