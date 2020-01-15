FROM ubuntu:18.04 as builder

RUN apt-get update && apt-get install -y clang cmake git make 

WORKDIR /root/develop

RUN git clone https://github.com/LLNL/units.git units

WORKDIR /root/develop/build
RUN cmake ../units -DUNITS_BUILD_FUZZ_TARGETS=ON -DUNITS_CXX_STANDARD=17 -DUNITS_INSTALL=OFF -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_COMPILER_FORCED=ON -DCMAKE_CXX_FLAGS="-g -O1 -fsanitize=fuzzer,undefined,address" && make -j2

WORKDIR /root/develop/fuzz_targets

RUN cp ../build/FuzzTargets/fuzz_from_string . && cp ../units/FuzzTargets/*.txt . && cp ../units/FuzzTargets/*.sh .

FROM ubuntu:18.04

COPY --from=builder /root/develop/fuzz_targets /root/develop/fuzz_targets
VOLUME /fuzz

WORKDIR /root/develop/fuzz_targets

CMD ["fuzz_from_string" "-max_len=512" "-dict=/root/develop/fuzz_targets/fuzz_dictionary.txt"]