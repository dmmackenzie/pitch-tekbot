CC = avr-gcc
EXE = pitch_robot
TARGET = attiny261
CFLAGS = -mmcu=$(TARGET) -Os
PART = t261
PROGRAMMER = usbasp

.PHONY: clean program

$(EXE).hex: pitch_robot
	avr-objcopy -j .text -j .data -O ihex $(EXE) $(EXE).hex

program: $(EXE).hex
	avrdude -p $(PART) -c $(PROGRAMMER) -e -U flash:w:$(EXE).hex:i

$(EXE): pitch_robot.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(EXE) $(EXE).hex
