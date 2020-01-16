#!/bin/bash

mkdir -p /fuzz/corpus

./fuzz_from_string /fuzz/corpus -max_len=512 -dict=/root/develop/fuzz_targets/fuzz_dictionary.txt -rss_limit_mb=600 -max_total_time=600 -timeout=10 -artifact_prefix=/fuzz/
./fuzz_from_string /fuzz/corpus -max_len=512 -dict=/root/develop/fuzz_targets/fuzz_dictionary.txt -rss_limit_mb=600 -max_total_time=600 -timeout=10 -artifact_prefix=/fuzz/

mkdir -p new_corpus
./fuzz_from_string new_corpus /fuzz/corpus -merge=1

rm /fuzz/corpus/*
mv new_corpus/* /fuzz/corpus/
