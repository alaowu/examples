PWD = $(shell pwd)
KDIR = /lib/modules/$(shell uname -r)/build
obj-m := rdwr_test.o
EXTRA_CFLAGS += -g -O2
all:
	@echo "KDIR=$(KDIR)"
	@echo "EXTRA_CFLAGS=$(EXTRA_CFLAGS)"
	$(MAKE) -C $(KDIR) M=`pwd` modules
clean:
	$(MAKE) -C $(KDIR) M=`pwd` clean
	@rm -rf *.o *.ko .depend *.mod.o*.mod.c Module.* modules.* 

