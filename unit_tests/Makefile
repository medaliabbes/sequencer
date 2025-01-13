CC = gcc
OBJCOPY = objcopy 
SEQUENCER_SRC = ../src
SEQUENCER_INC = ../inc 
TEST_FRAMEWORK_PATH = ../acutest/include


PROJECT = SEQ_TEST

SOURCES = tests.c 
SOURCES += $(SEQUENCER_SRC)/sequencer.c 
SOURCES += $(SEQUENCER_SRC)/sequencer_queue.c 
SOURCES += $(SEQUENCER_SRC)/sequencer_utils.c 

CFLAGS = -D ENABLE_UNIT_TEST 
CFLAGS += -I$(SEQUENCER_INC)
CFLAGS += -I$(TEST_FRAMEWORK_PATH)

OBJS = $(SOURCES:.c=.o)

all: $(PROJECT).exe

# compile
$(PROJECT).exe: $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@

test :
	./$(PROJECT).exe
# remove binary files
clean:
	rm -f *.o *.elf *.hex *.bin *.exe

