CC      = clang
QEMU    = qemu-system-riscv32

SRC_DIR   = src
INC_DIR   = include
BUILD_DIR = build

CFLAGS  = -std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib -I$(INC_DIR)
LDFLAGS = -Wl,-Tkernel.ld -Wl,-Map=$(BUILD_DIR)/kernel.map

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TARGET = $(BUILD_DIR)/kernel.elf

.PHONY: all run clean dirs install compile-commands

all: dirs $(TARGET)

dirs:
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	$(QEMU) -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
		-kernel $(TARGET)

clean:
	rm -rf $(BUILD_DIR)

install:
	sudo apt update
	sudo apt install -y \
		clang \
		lld \
		qemu-system-misc \
		opensbi \
		make \
		bear

compile-commands:
	make clean
	mkdir -p $(BUILD_DIR)
	bear --output $(BUILD_DIR)/compile_commands.json -- make all
