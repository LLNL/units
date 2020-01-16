FROM opensuse/tumbleweed:latest as builder

RUN zypper mr -d openSUSE-Tumbleweed-Update
RUN zypper install -y make gcc-c++ boost-devel cmake git

WORKDIR /root/develop

RUN git clone https://github.com/LLNL/units.git units

WORKDIR /root/develop/build
RUN cmake ../units -DUNITS_BUILD_WEBSERVER=ON -DBUILD_TESTING=OFF -DUNITS_CXX_STANDARD=17 -DCMAKE_BUILD_TYPE=Release && make -j2

WORKDIR /root/develop/webserver

RUN cp ../build/webserver/unit_web_server . && cp ../units/webserver/*.html .

FROM opensuse/tumbleweed:latest

COPY --from=builder /root/develop/webserver /root/develop/webserver

MAINTAINER top1@llnl.gov

WORKDIR /root/develop/webserver

EXPOSE 80/tcp

CMD ["./unit_web_server", "0.0.0.0", "80"]
