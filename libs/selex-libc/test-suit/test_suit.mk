LIB_TEST_SUIT=libTestSuit

TEST_SUIT_SRCS=test_suit.c

TEST_SUIT_INCLUDES= .

.PHONY: all clean

TEST_SUIT_OBJS = $(TEST_SUIT_SRCS:.c=.o)

all: $(LIB_TEST_SUIT).a

%.o : %.c
	@echo [Building $^ ...]
	$(CC) $(CFLAGS) -c -o $@ $^
	@echo [Done !]

$(LIB_TEST_SUIT).a: $(TEST_SUIT_OBJS)
	@echo [Linking $^ ...]
	$(AR) -rcsv $@  $^

clean:
	rm -f $(TEST_SUIT_OBJS) $(LIB_TEST_SUIT).a
	rm -f *.a