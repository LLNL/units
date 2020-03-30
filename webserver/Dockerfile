FROM helics/buildenv:tumbleweed-builder as builder

# should already be in /root/develop from the builder

RUN git clone https://github.com/LLNL/units.git units

WORKDIR /root/develop/build
RUN cmake ../units -DUNITS_BUILD_WEBSERVER=ON -DBUILD_TESTING=OFF -DCMAKE_CXX_STANDARD=17 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON && make -j2

WORKDIR /root/develop/webserver

RUN cp ../build/webserver/unit_web_server . && cp ../units/webserver/*.html && cp ../units/webserver/*.sh .

FROM opensuse/tumbleweed:latest

COPY --from=builder /root/develop/webserver /root/develop/webserver

LABEL maintainer="top1@llnl.gov"

VOLUME /weblog

WORKDIR /root/develop/webserver

EXPOSE 80/tcp

CMD ["./web_script.sh"]
