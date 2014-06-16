TARGET = darkswack.elf
OBJS = darkswack.o

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS)

rm-elf:
	-rm -f $(TARGET)

$(TARGET): $(OBJS)
	kos-c++ -o $(TARGET) $(OBJS) -lgl -lpcx -lkosutils

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)
