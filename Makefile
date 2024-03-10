
CXX=clang++
CXXFLAGS+=-std=c++20 -O3 -march=x86-64-v3

all: bench test

bench: bench.cpp
	${CXX} ${CXXFLAGS} bench.cpp -obench -lbenchmark

test: test.cpp
	${CXX} ${CXXFLAGS} `pkg-config gtest --cflags` -DENABLE_GTEST test.cpp -otest `pkg-config gtest --libs`

.PHONY: clean
clean:
	-rm -rf bench test
