CC = gcc
CFLAGS = -g -Wall -Wextra
LIBOBJS = vmemalloc.o vmemprint.o vmemtest.o
LIB = vmemalloc
LIBFILE = lib$(LIB).a

all: test1 testSetA testSetB testSetC

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

test1 : test1.o $(LIB)
	$(CC) test1.o $(CFLAGS) -o test1 -L. -l$(LIB)

testSetA : testSetA.o $(LIB)
	$(CC) testSetA.o $(CFLAGS) -o testSetA -L. -l$(LIB)

testSetB : testSetB.o $(LIB)
	$(CC) testSetB.o $(CFLAGS) -o testSetB -L. -l$(LIB)
	
testSetC : testSetC.o $(LIB)
	$(CC) testSetC.o $(CFLAGS) -o testSetC -L. -l$(LIB)

$(LIB) : $(LIBOBJS)
	ar -cvr $(LIBFILE) $(LIBOBJS)
	#ranlib $(LIBFILE) # may be needed on some systems
	ar -t $(LIBFILE)

clean:
	/bin/rm -f *.o test1 testSetA testSetB testSetC $(LIBFILE)
