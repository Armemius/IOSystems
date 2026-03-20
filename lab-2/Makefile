CC      = clang
QEMU    = qemu-system-riscv64

SRC_DIR   = src
INC_DIR   = include
BUILD_DIR = build

# CFLAGS  = -std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib -I$(INC_DIR)
# LDFLAGS = -Wl,-Tkernel.ld -Wl,-Map=$(BUILD_DIR)/kernel.map

# SRCS = $(wildcard $(SRC_DIR)/*.c)
# OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# TARGET = $(BUILD_DIR)/kernel.elf

.PHONY: all run clean dirs install install-ubuntu connect compile-commands gen-ssh-keys

run:
	$(QEMU) \
		-machine virt \
		-nographic \
		-m 5G \
		-smp 1 \
		-bios ./misc/opensbi/fw_jump.bin \
		-kernel ./misc/uboot/uboot.elf \
		-device virtio-rng-pci \
		-drive file=./misc/ubuntu.img,format=raw,if=virtio \
		-device virtio-net-device,netdev=net \
		-netdev user,id=net,hostfwd=tcp::2222-:22

install:
	sudo apt update
	sudo apt install -y \
		clang \
		lld \
		qemu-system-misc \
		opensbi \
		u-boot-qemu \
		make \
		bear

connect-manual:
	ssh ubuntu@localhost -p 2222

connect:
	ssh -i ./.ssh/vm_key ubuntu@localhost -p 2222

setup-ubuntu:
	rm -f ./misc/ubuntu.img
	wget https://cdimage.ubuntu.com/releases/20.04/release/ubuntu-20.04.5-preinstalled-server-riscv64+unmatched.img.xz -O ./misc/ubuntu.img.xz
	xz -vdk ./misc/ubuntu.img.xz
	rm -f ./misc/ubuntu.img.xz
	qemu-img resize -f raw ./misc/ubuntu.img +5G
	echo "Installation complete!"

ssh-keygen:
	rm -rf ./.ssh
	mkdir -p ./.ssh
	ssh-keygen -t ed25519 -N "" -f ./.ssh/vm_key
	ssh-copy-id -i ./.ssh/vm_key -p 2222 ubuntu@localhost
