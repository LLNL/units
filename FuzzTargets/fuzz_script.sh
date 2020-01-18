#!/bin/bash

mkdir -p /fuzz/corpus

# run the first fuzzing sequence
./fuzz_from_string /fuzz/corpus -max_len=512 -dict=/root/develop/fuzz_targets/fuzz_dictionary.txt -rss_limit_mb=600 -max_total_time=1200 -timeout=5 -artifact_prefix=/fuzz/

#if we haven't failed merge the corpus so it is smaller
if [ $? -eq 0 ]
then
  mkdir -p new_corpus
  ./fuzz_from_string new_corpus /fuzz/corpus -merge=1 -max_len=512 -max_total_time=120
  if [ $? -eq 0 ]
  then
    rm /fuzz/corpus/*
    mv new_corpus/* /fuzz/corpus/
  fi
fi
