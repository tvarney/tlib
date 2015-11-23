
INC  := ./inc
SRC  := ./src
TEST := ./test
BIN  := ./build

COMPILE := clang++ -c
LINK    := clang++

COMPILE_FLAGS := -I${INC} -Wall --std=c++11

LIBS := -lboost_locale

TESTLIBS := ${LIBS} -lpthread -lgtest_main -lgtest
TESTOBJS := $(patsubst ${TEST}/%.cpp, ${BIN}/%.o, $(wildcard ${TEST}/*.cpp))

.PHONEY: all clean tests

all: ${BIN}

clean:
	rm -rf ${BIN}/*
tests: ${BIN} ${TESTOBJS}
	${LINK} -o ${BIN}/tests ${TESTOBJS} ${TESTLIBS}

${BIN}:
	mkdir ${BIN}

${BIN}/%.o: ${TEST}/%.cpp
	${COMPILE} ${COMPILE_FLAGS} -o $@ $<
