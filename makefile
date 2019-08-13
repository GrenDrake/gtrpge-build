CXXFLAGS= -std=c++11 -g -Wall

BUILD_OBJS=build/build.o build/general.o build/lexer.o \
		   build/parse_main.o build/translate.o build/gamedata.o \
		   build/value.o build/parse_functions.o build/parsestate.o \
		   build/generate.o build/bytestream.o build/dump.o build/opcode.o \
		   build/expression.o
BUILD=./gbuild

RUNNER_OBJS=runner/runner.o runner/gameloop.o runner/gamedata.o \
			runner/formatter.o runner/runfunction.o runner/stack.o \
			runner/loadgame.o runner/dump.o  \
			runner/bytestream.o runner/value.o
RUNNER=./grun

TEST_BYTESTREAM_OBJS=tests/bytestream.o build/bytestream.o
TEST_BYTESTREAM=./test_bytestream

AUTOTESTS_SRC=examples/auto_tests.src
AUTOTESTS=./examples/auto_tests.bin
USERTESTS_SRC=examples/user_tests.src
USERTESTS=../gtrpge-javascript/games/user_tests.bin

all: $(BUILD) $(RUNNER) tests $(AUTOTESTS) $(USERTESTS)

tests: $(TEST_BYTESTREAM)

$(BUILD): $(BUILD_OBJS)
	$(CXX) $(BUILD_OBJS) -o $(BUILD)

$(RUNNER): $(RUNNER_OBJS)
	$(CXX) $(RUNNER_OBJS) -o $(RUNNER)

$(TEST_BYTESTREAM): $(BUILD) $(TEST_BYTESTREAM_OBJS)
	$(CXX) $(TEST_BYTESTREAM_OBJS) -o $(TEST_BYTESTREAM)
	$(TEST_BYTESTREAM)

$(AUTOTESTS): $(BUILD) $(AUTOTESTS_SRC)
	$(BUILD) -data -functions -bytecode -asm -ir $(AUTOTESTS_SRC) -o $(AUTOTESTS)
	cp $(AUTOTESTS) ../gtrpge-javascript/games/
$(USERTESTS): $(BUILD) $(USERTESTS_SRC)
	$(BUILD) -data -functions -bytecode -asm -ir $(USERTESTS_SRC) -o $(USERTESTS)


clean: clean_runner
	$(RM) build/*.o runner/*.o tests/*.o
	$(RM) $(BUILD) $(TEST_BYTESTREAM)

clean_runner:
	$(RM) runner/*.o $(RUNNER)

.PHONY: all clean clean_runner tests