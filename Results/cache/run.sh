#! /bin/bash

./waf --run "cache --RngRun=2 --cacheSize=200 --maxRange=500 --frequency=100 --output=max500-c200"
./waf --run "cache --RngRun=2 --cacheSize=400 --maxRange=500 --frequency=100 --output=max500-c400"
./waf --run "cache --RngRun=2 --cacheSize=200 --maxRange=1000 --frequency=100 --output=max1000-c200"
./waf --run "cache --RngRun=2 --cacheSize=400 --maxRange=1000 --frequency=100 --output=max1000-c400"
