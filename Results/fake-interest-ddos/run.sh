#! /bin/bash

cd ../../../..
./waf --run "fake-interest-ddos --RngRun=2 --frequency=20 --tolerance=50 --withStrategy=true --output=test-f20-t50"
cd -
Rscript fake-ddos.R test-f20-t50
