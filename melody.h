#ifndef _MELODY_H_
#define _MELODY_H_

#include "synth.h"

#define NUM_COLS   16   // Number of columns = number of notes in melody
#define NUM_ROWS   7    // Number of rows = number of possible notes to play
                        // (including no note)
#define MAX_MELS   16   // Max number of melodies to load in read melody window

/* Melody struct */
typedef struct {
  const char* filename;        // melody filename
  int tempo;                   // in BPM
  int scale;                   // 0 is major pentatonic, 1 is minor pentatonic
  int start_note;              // MIDI number, ex. 48 is middle C
  int notes[NUM_COLS];         // array of notes 0 to 6
  double freqs[NUM_COLS];      // array of corresponding frequencies
  int note_duration;           // in samples
} Melody;

/* melody.c function prototypes */
int read_melody(Melody *pm);
void make_freqs(Melody *pm);
double convert_to_freq(int note);
int write_to_txt(Melody* pm);

#endif
