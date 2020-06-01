#!/bin/bash

mkdir -p /fuzz/corpus
mkdir -p /fuzz/logs

# run the first fuzzing sequence
./fuzz_measurement /fuzz/corpus -max_len=512 -dict=/root/develop/fuzz_targets/fuzz_dictionary.txt -rss_limit_mb=512 -max_total_time=1390 -timeout=20 -artifact_prefix=/fuzz/ &> fuzzlog.txt

#if we haven't failed merge the corpus so it is smaller
if [ $? -eq 0 ]
then
  mkdir -p new_corpus
  ./fuzz_measurement new_corpus /fuzz/corpus -merge=1 -max_len=512 -max_total_time=120 &>"/fuzz/logs/fuzz_merge$(date +"%s").log"
  if [ $? -eq 0 ]
  then
    rm /fuzz/corpus/*
    mv new_corpus/* /fuzz/corpus/
  fi
else
  tail --lines=500 fuzzlog.txt >"/fuzz/logs/fuzz_crash$(date +"%s").log"
fi
