FROM opensuse/tumbleweed:latest as builder

RUN zypper install -y build-essential boost-devel cmake git

WORKDIR /root/develop

RUN git clone https://github.com/LLNL/units.git units
cd units && git checkout webserver

RUN mkdir build && cd build && cmake ../units -DUNITS_BUILD_WEBSERVER=ON


