FROM helics/buildenv:gcc12-builder as builder

WORKDIR /root/develop

RUN git clone https://github.com/LLNL/units.git units

WORKDIR /root/develop/build

RUN cmake ../units -DUNITS_BUILD_WEBSERVER=ON -DBUILD_TESTING=OFF -DCMAKE_CXX_STANDARD=17 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON && make -j2

WORKDIR /root/develop/webserver

RUN cp ../build/bin/units_webserver . && cp ../units/webserver/*.html . && cp ../units/webserver/*.sh .

FROM ubuntu:22.04

COPY --from=builder /root/develop/webserver /root/develop/webserver

ARG GIT_COMMIT=unspecified

LABEL maintainer="top1@llnl.gov"

LABEL name="units.webserver"
LABEL description="Units library webserver"
LABEL vcs-ref=$GIT_COMMIT
LABEL vcs-url="https://github.com/LLNL/units"
LABEL version="0.9.1"
LABEL date="2023-09-01"

VOLUME /weblog

WORKDIR /root/develop/webserver

EXPOSE 80/tcp

CMD ["./web_script.sh"]
