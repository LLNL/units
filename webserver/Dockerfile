FROM opensuse/tumbleweed:latest as builder

RUN zypper install -y build-essential boost-devel cmake git

WORKDIR /root/develop

RUN git clone https://github.com/LLNL/units.git units
cd units && git checkout webserver

RUN mkdir build && mkdir webserver && cd build && cmake ../units -DUNITS_BUILD_WEBSERVER=ON -DUNITS_ENABLE_TESTS=OFF && make -j2

RUN cp build/webserver/unit_web_server webserver/unit_web_server && cp units/webserver/*.html webserver/


FROM opensuse/tumbleweed:latest

COPY --from=builder /root/develop/webserver /root/develop/webserver

WORKDIR /root/develop/webserver

EXPOSE 80/tcp

CMD ["unit_web_server", "0.0.0.0", "80", "."]