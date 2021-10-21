KERNEL_VERSION := $(shell uname -r)
MAKE_CMD := make -C /lib/modules/$(KERNEL_VERSION)/build M=$(PWD)
OBJS := sertest.o
BIN := sertest
obj-m += serco.o

all: module sertest
clean: module_clean

distclean: clean
	rm -f *.ko $(BIN)

module:
	$(MAKE_CMD) modules

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJS)

module_clean:
	$(MAKE_CMD) clean
 
.PHONY: distclean clean module_clean
