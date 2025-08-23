obj-m := msg_board.o
msg_board-objs := src/msg_board.o

# Add custom include directory
EXTRA_CFLAGS += -I$(PWD)/include

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean