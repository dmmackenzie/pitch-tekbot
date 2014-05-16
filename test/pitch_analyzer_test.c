#include <stdio.h>
#include "../src/pitch_analyzer.h"


int test_pitch(FILE* fp) {
    int i, period, bytes;
    PitchContext c;
    PitchContext* cx = &c;
    Sample buf[100];

    pitch_init(cx);

    while ((bytes = fread(buf, 1, 100, fp)) > 0) {
        for (i = 0; i < bytes; ++i) {
            period = pitch_sample(buf[i], cx);
            if (period > 0) {
                printf("%d Hz\n", PITCH_GET_FREQ(period, 44100));
            } else if(period < 0) {
                printf("ERROR: %d\n", period);
            }
        }
    }

    fclose(fp);

    return 0;

}

int main(int argc, char** argv) {
    int i;

    for (i = 1; i < argc; ++i) {
        printf("Running test %s\n", argv[i]);
        printf("===========================\n");
        test_pitch(fopen(argv[i], "r"));
    }

    return 0;
}
