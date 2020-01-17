#!/bin/bash

mkdir -p /fuzz/corpus

# run the first fuzzing sequence
./fuzz_from_string /fuzz/corpus -max_len=512 -dict=/root/develop/fuzz_targets/fuzz_dictionary.txt -rss_limit_mb=675 -max_total_time=300 -timeout=7 -artifact_prefix=/fuzz/

#if we haven't failed run another
if [ $? -eq 0 ]
then
   ./fuzz_from_string /fuzz/corpus -max_len=512 -dict=/root/develop/fuzz_targets/fuzz_dictionary.txt -rss_limit_mb=675 -max_total_time=300 -timeout=7 -artifact_prefix=/fuzz/
else
  exit 1
fi 

#if we haven't failed run another 
if [ $? -eq 0 ]
then
   ./fuzz_from_string /fuzz/corpus -max_len=512 -dict=/root/develop/fuzz_targets/fuzz_dictionary.txt -rss_limit_mb=675 -max_total_time=300 -timeout=7 -artifact_prefix=/fuzz/
else
  exit 1
fi 

#if we haven't failed run another 
if [ $? -eq 0 ]
then
   ./fuzz_from_string /fuzz/corpus -max_len=512 -dict=/root/develop/fuzz_targets/fuzz_dictionary.txt -rss_limit_mb=675 -max_total_time=300 -timeout=7 -artifact_prefix=/fuzz/
else
  exit 1
fi 

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
