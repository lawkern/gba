EMU = retroarch -L mgba

ASFLAGS = -mcpu=arm7tdmi
CFLAGS  = -mcpu=arm7tdmi -mthumb -mthumb-interwork -O2 -Wall -fno-strict-aliasing
LDFLAGS = -Wl,-T,src/sys/gba.ld -nostartfiles -specs=nosys.specs

compile:
	mkdir -p build
	arm-none-eabi-as  -o build/crt0.o $(ASFLAGS) src/sys/crt0.s
	arm-none-eabi-gcc -o build/main.o $(CFLAGS) -c src/main.c
	arm-none-eabi-gcc -o build/main.elf build/main.o build/crt0.o $(LDFLAGS)

	arm-none-eabi-objcopy -O binary build/main.elf build/main.gba

	$(CC) -o build/fix src/tools/fix.c
	./build/fix -tMAIN build/main.gba

run:
	$(EMU) build/main.gba
