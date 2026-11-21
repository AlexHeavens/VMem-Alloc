CC = gcc
WARNFLAGS = -Wall -Wextra -Werror
# Extra flags, for example EXTRA_CFLAGS="-O1 -fsanitize=address,undefined".
EXTRA_CFLAGS =
CFLAGS = -g $(WARNFLAGS) $(EXTRA_CFLAGS)

BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = lib

LIB_OBJS = $(BUILD_DIR)/vmemalloc.o $(BUILD_DIR)/vmemprint.o $(BUILD_DIR)/vmemtest.o
LIB = vmemalloc
LIB_FILE = $(LIB_DIR)/lib$(LIB).a

TESTS = test1 testSetA testSetB testSetC
# The tests write trace files, so run them in a scratch directory.
RUN_DIR = $(BUILD_DIR)/run

.PHONY: all test clean

all: test1 testSetA testSetB testSetC

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test1 : $(BUILD_DIR)/test1.o $(LIB)
	$(CC) $(BUILD_DIR)/test1.o $(CFLAGS) -o $(BIN_DIR)/test1 -L$(LIB_DIR) -l$(LIB)

testSetA : $(BUILD_DIR)/testSetA.o $(LIB)
	$(CC) $(BUILD_DIR)/testSetA.o $(CFLAGS) -o $(BIN_DIR)/testSetA -L$(LIB_DIR) -l$(LIB)

testSetB : $(BUILD_DIR)/testSetB.o $(LIB)
	$(CC) $(BUILD_DIR)/testSetB.o $(CFLAGS) -o $(BIN_DIR)/testSetB -L$(LIB_DIR) -l$(LIB)

testSetC : $(BUILD_DIR)/testSetC.o $(LIB)
	$(CC) $(BUILD_DIR)/testSetC.o $(CFLAGS) -o $(BIN_DIR)/testSetC -L$(LIB_DIR) -l$(LIB)

$(LIB) : $(LIB_OBJS)
	ar -cvr $(LIB_FILE) $(LIB_OBJS)
	#ranlib $(LIB_FILE) # may be needed on some systems
	ar -t $(LIB_FILE)

test: all
	rm -rf $(RUN_DIR)
	mkdir -p $(RUN_DIR)
	set -e; for t in $(TESTS); do \
		echo "== $$t"; \
		(cd $(RUN_DIR) && ../../$(BIN_DIR)/$$t > $$t.log 2>&1) \
			|| { cat $(RUN_DIR)/$$t.log; echo "$$t FAILED"; exit 1; }; \
	done

clean:
	rm -rf $(BUILD_DIR)/* $(BIN_DIR)/* $(LIB_FILE)
