#! /bin/bash

./waf --run "valid-interest-aggregation --RngRun=3 --maxRange=200 --frequency=20 --topo=meshed-bad --output=200-20-bad-1"

./waf --run "valid-interest-aggregation --RngRun=3 --maxRange=200 --frequency=20 --topo=meshed-bad --output=200-20-bad-2"

./waf --run "valid-interest-aggregation --RngRun=3 --maxRange=10000 --frequency=20 --topo=meshed-bad --output=10000-20-bad"

./waf --run "valid-interest-aggregation --RngRun=3 --maxRange=10000 --frequency=50 --topo=meshed-bad --output=10000-50-bad"
