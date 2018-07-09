CC = gcc
CFLAGS = -g -Wall -Wextra

BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = lib

LIB_OBJS = $(BUILD_DIR)/vmemalloc.o $(BUILD_DIR)/vmemprint.o $(BUILD_DIR)/vmemtest.o
LIB = vmemalloc
LIB_FILE = $(LIB_DIR)/lib$(LIB).a

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

clean:
	rm -f $(BUILD_DIR)/* $(BIN_DIR)/* $(LIB_FILE)
