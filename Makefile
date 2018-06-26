obj-m :=testdev.o
KDIR  :=/usr/src/linux-headers-4.13.0-43-generic
PWD   :=${shell pwd}

all:
	make -C ${KDIR} M=${PWD} modules
clean:
	rm -f *.o *.mod.c .*.*.cmd *.ko
	rm -rf .tmp_versions
