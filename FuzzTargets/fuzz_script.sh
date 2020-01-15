#!/bin/bash

mkdir -p /fuzz/corpus

./fuzz_from_string /fuzz/corpus -max_len=512 -dict=/root/develop/fuzz_targets/fuzz_dictionary.txt -rss_limit_mb=512 -max_total_time=1200 -timeout=10 -artifact_prefix=/fuzz/