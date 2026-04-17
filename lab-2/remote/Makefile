obj-m = ch_drv.o
PWD = $(shell pwd)

.PHONY: install uninstall

all:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" clean
install:
	sudo insmod ch_drv.ko
uninstall:
	sudo rmmod ch_drv
test:
	echo "abc123" | sudo tee /dev/var4 >/dev/null
	echo "9z8" | sudo tee /dev/var4 >/dev/null
	echo "123zzz456" | sudo tee /dev/var4 >/dev/null
	sudo cat /dev/var4
	echo "Expected: 3 -> 2 -> 6"
