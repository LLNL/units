FROM  helics/buildenv:clang14-builder as builder

# this is already in a workdir from the builder image

WORKDIR /root/develop

RUN git clone https://github.com/LLNL/units.git units

WORKDIR /root/develop/build

RUN cmake ../units -DUNITS_BUILD_FUZZ_TARGETS=ON -DCMAKE_CXX_STANDARD=17 -DUNITS_INSTALL=OFF -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_COMPILER_FORCED=ON -DCMAKE_CXX_FLAGS="-g -O1 -fsanitize=fuzzer" && make -j2

WORKDIR /root/develop/fuzz_targets

RUN cp ../build/bin/fuzz_measurement . && cp ../units/FuzzTargets/*.txt . && cp ../units/FuzzTargets/*.sh .

FROM ubuntu:22.04

COPY --from=builder /root/develop/fuzz_targets /root/develop/fuzz_targets

RUN export VCS_REF=$(cat /root/develop/Fuzz_targets/commit_hash)

ARG GIT_COMMIT=unspecified

LABEL maintainer="top1@llnl.gov"

LABEL name="units.fuzzer"
LABEL description="Units library fuzzer"
LABEL vcs-ref=$GIT_COMMIT
LABEL vcs-url="https://github.com/LLNL/units"
LABEL version="0.9.1"
LABEL date="2023-09-01"

VOLUME /fuzz

#ENV ASAN_OPTIONS='quarantine_size_mb=16:max_redzone=512:detect_leaks=0:max_malloc_fill_size=1024'

WORKDIR /root/develop/fuzz_targets

CMD ["./fuzz_script.sh"]
