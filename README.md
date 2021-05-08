# Pentaseq
Pentatonic melody writer/player. Final project for Dr. Schuyler Quackenbush's C Programming for Music Technology class.

Comes with several pre-written melodies which of course can be deleted if desired.

To run: ./build.sh, ./pentaseq.

How to write a melody:
- Name: Enter a name that will be valid as a filename (the program will automatically generate a .txt file with this name).
- Tempo: This is in Beats Per Minute (BPM). I recommend using tempos below 100 BPM. Your melody will be one bar of 16th notes, but you can also cut the desired tempo in half and consider it two bars of 8th notes, or again to make four bars of quarter notes.
- Scale: 1 for major pentatonic, 2 for minor pentatonic. If you choose any other value your melody will not work.
- Starting note: This is a MIDI number. 48 is middle C for example.

Then you will see a grid of Xs which you can maneuver with the arrow keys. The top row is the root note and the following rows are the following notes of the pentatonic scale. The bottom row is an octave above the root. Press F1 to save your melody, r to go to the play melody window, or q to quit.

Playing melodies is straightforward. Just select a melody with the arrow keys and press enter to play it. Press r to start recording output to a WAV file. Press q to quit the program and stop recording. The WAV file will be called out.wav and will be overwritten unless you rename it before recording again.
