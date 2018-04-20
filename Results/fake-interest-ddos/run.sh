#! /bin/bash

./waf --run "fake-interest-ddos --RngRun=2 --frequency=10 --tolerance=50 --withStrategy=true --output=test-20-t50"
