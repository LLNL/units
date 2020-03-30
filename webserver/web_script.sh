#!/bin/bash

mkdir -p /weblog/logs

# run the webserver
./unit_web_server 0.0.0.0 80 &> "/weblog/logs/fuzz_crash$(date +"%s").log"
