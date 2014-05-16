CC = avr-gcc
EXE = pitch_robot
OBJS = pitch_robot.o pitch_analyzer.o
TARGET = attiny261
CFLAGS = -mmcu=$(TARGET) -Os
PART = t261
PROGRAMMER = usbasp

.PHONY: clean program

$(EXE).hex: pitch_robot
	avr-objcopy -j .text -j .data -O ihex $(EXE) $(EXE).hex

program: $(EXE).hex
	avrdude -p $(PART) -c $(PROGRAMMER) -e -U flash:w:$(EXE).hex:i

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

pitch_robot.o: pitch_robot.c pitch_robot.h pitch_analyzer.h
	$(CC) $(CFLAGS) -o $@ -c $<

pitch_analyzer.o: pitch_analyzer.c pitch_analyzer.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm $(OBJS) $(EXE) $(EXE).hex
