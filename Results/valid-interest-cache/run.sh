#! /bin/bash

./waf --run "valid-interest-cache --RngRun=2 --cacheSize=0 --maxRange=200 --frequency=20 --topo=meshed-bad --output=0-200-20-bad"
./waf --run "valid-interest-cache --RngRun=2 --cacheSize=100 --maxRange=200 --frequency=20 --topo=meshed-bad --output=100-200-20-bad"
./waf --run "valid-interest-cache --RngRun=2 --cacheSize=200 --maxRange=200 --frequency=20 --topo=meshed-bad --output=200-200-20-bad"
./waf --run "valid-interest-cache --RngRun=2 --cacheSize=300 --maxRange=200 --frequency=20 --topo=meshed-bad --output=300-200-20-bad"
