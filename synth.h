#ifndef _SYNTH_H_
#define _SYNTH_H_

#include "melody.h"

/* write output to wav file for debugging */
//#define DB_WAV_OUT          1

/* other defines
   (These are all taken from PS08) */
#define SAMP_RATE           48000
#define NUM_CHAN	          2
#define FRAMES_PER_BUFFER   1024
#define FS_AMPL             0.5 /* -6 dB FS */
#define ATTACK_FACTOR       0.998562 /* attack time constant of 100 ms */
//#define ATTACK_FACTOR     0.997126 /* attack time constant of 50 ms */
//#define ATTACK_FACTOR       0.985712 /* attack time constant of 10 ms */
#define DECAY_FACTOR        0.9998 /* decay time constant of 1.0 sec */
#define DROP_LEVEL          0.001  /* -60 dBFS */
#define PI                  3.14159265358979323846

typedef struct {
    double f0; /* frequency associated with key */
    double phase_inc; /* phase increment per sample to realize freq */
    double phase; /* save phase value for next sample */
    double attack_factor;
    double decay_factor;
    double attack_amp; /* save attack amplitude for next sample */
    double decay_amp; /* save decay amplitude for next sample */
} Tone;

typedef struct {
    int samp_rate;   // sampling rate of output
    int samp_count;  // count samples, reset every note
    int index_count; // count indexes i.e. notes
    Tone tone;       // tone that plays the notes
    double output[FRAMES_PER_BUFFER];
} Synth;

/* function prototypes */
void play_note(Synth *ps, double freq);
double synth_sample(Synth *ps);

#endif
