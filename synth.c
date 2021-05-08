#include <stdio.h>
#include <stdlib.h> //for exit()
#include <math.h>   //for sin()
#include "synth.h"
#include "melody.h"

void play_note(Synth *ps, double freq)
{
  /* This function sets the synth parameters for a new incoming
     frequency. */
  double fs;
  Tone *pt = &ps->tone;

  fs = ps->samp_rate;

  pt->f0 = freq;
  pt->phase_inc = 2*PI*freq/fs;
  pt->phase = 0.0;
  pt->attack_factor = ATTACK_FACTOR;
  pt->decay_factor = DECAY_FACTOR;
  /* Comment above and uncomment below to make the note decay according to note length
     This removes clicks but also makes the notes very short */
  //pt->decay_factor = note_duration_decay_factor;
  pt->attack_amp = 1.0;
  pt->decay_amp = 1.0;
}

double synth_sample(Synth *ps)
{
  /* This function synthesizes one sample of audio. */
    Tone *pt = &ps->tone;
    double v;

    // Initialize output value to 0
    v = 0;

    if ( pt->phase_inc > -1 ) {
      // Compute sample value
      v += FS_AMPL * sin(pt->phase);

      // Implement attack and decay
      v *= (1 - pt->attack_amp);
      v *= pt->decay_amp;
      pt->attack_amp *= pt->attack_factor;
      pt->decay_amp *= pt->decay_factor;
      // Increment phase
      pt->phase += pt->phase_inc;
    }

    // Stop playout if below drop level
    if ( pt->decay_amp < DROP_LEVEL ) {
      pt->phase_inc = -1;
    }

    return v;
}
