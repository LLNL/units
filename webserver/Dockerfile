FROM opensuse/tumbleweed:latest as builder

RUN zypper mr -d openSUSE-Tumbleweed-Update
RUN zypper install -y make gcc-c++ boost-devel cmake git

WORKDIR /root/develop

RUN git clone https://github.com/LLNL/units.git units
RUN cd units && git checkout webserver

RUN mkdir build && mkdir webserver && cd build && cmake ../units -DUNITS_BUILD_WEBSERVER=ON -DBUILD_TESTING=OFF -DUNITS_CXX_STANDARD=17 -DCMAKE_BUILD_TYPE=Release && make -j2

RUN cp build/webserver/unit_web_server webserver/unit_web_server && cp units/webserver/*.html webserver/

FROM opensuse/tumbleweed:latest

COPY --from=builder /root/develop/webserver /root/develop/webserver

WORKDIR /root/develop/webserver

EXPOSE 80/tcp

CMD ["/root/develop/webserver/unit_web_server", "0.0.0.0", "80"]