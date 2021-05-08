#include <stdio.h>
#include <stdlib.h> // for atoi() and malloc()
#include <math.h>   // for pow()
#include "melody.h"
#include "synth.h"

#define TEMPO_STR_LEN         4
#define SCALE_STR_LEN         2
#define START_NOTE_STR_LEN    3
#define SCALE_MAJ             1
#define SCALE_MIN             2

int read_melody(Melody* pm)
{
  /* Reads melody from properly formatted txt file
     to Melody struct. */
  char tempo_str[TEMPO_STR_LEN];
  char scale_str[SCALE_STR_LEN];
  char start_note_str[START_NOTE_STR_LEN];
  char c;
  int k, cnt;

  FILE *fp = fopen(pm->filename, "r");
  if (!fp) {
    fprintf(stderr, "File open failed");
    return -1;
  }

  /* read tempo */
  cnt = 0;
  while ((c = fgetc(fp)) != '\n') {
    // Check to make sure the read worked
    if (!c) {
      fprintf(stderr, "File read failed");
      return -1;
    }
    tempo_str[cnt] = c;
    cnt++;
  }
  tempo_str[cnt] = '\0';
  pm->tempo = atoi(tempo_str);
  // Set note duration based on tempo
  pm->note_duration = SAMP_RATE/((pm->tempo*4)/60);

  /* read scale */
  cnt = 0;
  while ((c = fgetc(fp)) != '\n') {
    scale_str[cnt] = c;
    cnt++;
  }
  scale_str[cnt] = '\0';
  pm->scale = atoi(scale_str);

  /* read start note */
  cnt = 0;
  while ((c = fgetc(fp)) != '\n') {
    start_note_str[cnt] = c;
    cnt++;
  }
  start_note_str[cnt] = '\0';
  pm->start_note = atoi(start_note_str);

  /* read note values */
  for (int i = 0; i < NUM_COLS; i++) {
    c = fgetc(fp);
    if (c == ',') {
      i--;
      continue;
    } else {
      // Convert to int
      k = c - '0';
      pm->notes[i] = k;
    }
  }
  fclose(fp);
  return 0;
}

void make_freqs(Melody *pm) {
  /* Takes in a newly read melody,
     and outputs an array of 16 frequencies. */
  int note_in_mel, start_note, scale, note_num;
  double freq;

  start_note = pm->start_note;
  scale = pm->scale;

  /* Convert melody note array to freq array */
  for (int i = 0; i < NUM_COLS; i++) {
    note_in_mel = pm->notes[i];

    // Fit all notes to the pentatonic scale
    switch (note_in_mel) {
      case 0 :
        note_num = 0;
        break;
      case 1 :
        note_num = start_note;
        break;
      case 2 :
        if (scale == SCALE_MAJ) note_num = start_note + 2;
        else note_num = start_note + 3;
        break;
      case 3 :
        if (scale == SCALE_MAJ) note_num = start_note + 4;
        else note_num = start_note + 5;
        break;
      case 4 :
        note_num = start_note + 7;
        break;
      case 5 :
        if (scale == SCALE_MAJ) note_num = start_note + 9;
        else note_num = start_note + 10;
        break;
      case 6 :
        note_num = start_note + 12;
        break;
      default :
        fprintf(stderr, "ERROR: Melody notes out of range.\n");
        break;
    }

    freq = convert_to_freq(note_num);
    pm->freqs[i] = freq;
  }
}

double convert_to_freq(int note) {
  /* Converts note number to frequency
     using A3 = 57 = 220 as a base note. */
  if (note == 0) {
    return 0.0;
  }
  double freq, diff_double;
  int diff = note - 57;

  diff_double = (double)diff;
  freq = 220.0*pow(2.0,diff_double/12.0);
  return freq;
}

int write_to_txt(Melody* pm) {
  /* Writes the melody to a .txt file. */

  // Open file stream
  FILE *fp = fopen(pm->filename, "w");
  if (!fp) {
    fprintf(stderr, "ERROR: File open failed");
    return -1;
  }

  // Write to file stream
  fprintf(fp, "%i\n", pm->tempo);
  fprintf(fp, "%i\n", pm->scale);
  fprintf(fp, "%i\n", pm->start_note);
  for (int i = 0; i < NUM_COLS; i++) {
    fprintf(fp, "%d,", pm->notes[i]);
  }

  // Close file stream
  fclose(fp);
  return 0;
}
