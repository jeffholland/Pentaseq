/*
 * Pentaseq - a simple pentatonic melody writer/player
 *
 * Jeff Holland's final project for C Programming for Music Technology
 * May 6, 2021
 */

#include <stdio.h>
#include <portaudio.h>
// #include <sndfile.h>
#include <ncurses.h>  // User interface
#include <stdlib.h>   // For atoi()
#include <string.h>   // For memset()
#include <dirent.h>   // For finding txt files in working directory
#include "paUtils.h"
#include "synth.h"
#include "melody.h"

/* Width and height of menu */
#define WIDTH     30
#define HEIGHT    10

/* Initialize ncurses params */
int startx = 0;
int starty = 0;
char* choices[] = {
  "Write melody",
  "Play melody",
  "Exit",
};
const char* mel_choices[MAX_MELS][128];
int n_choices = sizeof(choices) / sizeof(char *);

/* Portaudio callback structure */
typedef struct {
    int num_chan;
    Synth *ps;
    Melody *pm;
    int wav_out; // Sets to 1 if user chooses to record output
    // SNDFILE *sndfile;
} Buf;

/* PortAudio callback function protoype */
static int paCallback(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData );

/* Ncurses display function prototypes */
void display_main_menu(WINDOW *menu_win, int highlight);
void display_write_melody(WINDOW *write_melody_win, Melody* pm,
  int highlight_x, int highlight_y, int choice_x, int choice_y);
void display_read_melody(WINDOW *read_melody_win, int highlight, int counter);

/* Main function */
int main(void)
{
  /* Instantiate synth structure */
  Synth synth;
  Synth *ps = &synth;

  /* Instantiate melody structure */
  Melody melody;
  Melody *pm = &melody;

  /* Instantiate Portaudio structures */
  Buf buf;
  PaStream *stream;

  /* Instantiate Ncurses window structures */
  WINDOW* menu_win;
  WINDOW* write_melody_win;
  WINDOW* read_melody_win;

  /* Instantiate SF_INFO struct for output file  */
  // SF_INFO sfinfo;

  /* Initialize synth params */
  ps->samp_rate = SAMP_RATE;
  ps->samp_count = 0;
  ps->index_count = 0;

  /* Initialize melody params */
  pm->note_duration = 0;
  for (int i = 0; i < NUM_COLS; i++) {
    pm->freqs[i] = 0;
  }

  /* Initialize main function Ncurses params */
  int highlight = 1;
  int choice = 0;
  int c;

  /* Initialize Portaudio buf params */
  buf.num_chan = NUM_CHAN;
  buf.ps = ps;
  buf.pm = pm;
  buf.wav_out = 0;

  /* Start PortAudio */
  stream = startupPa(1, NUM_CHAN,
      SAMP_RATE, FRAMES_PER_BUFFER, paCallback, &buf);

  /* Start ncurses mode */
  initscr();
  cbreak();
  noecho();
  curs_set(0); // Hide cursor

  // Set start points in center of window
  startx = (80 / WIDTH) / 2;
  starty = (24 / HEIGHT) / 2;

  // Create main menu window
  menu_win = newwin(HEIGHT, WIDTH, starty, startx);
  keypad(menu_win, TRUE); // Allow F1 etc. keys
  mvprintw(0,0,"Main menu: Arrow keys to navigate, enter to select an option");
  refresh();

  /* Print main menu */
  display_main_menu(menu_win, highlight);

  /* While loop 1: main menu */
  while(1)
  {
    /* Implement the keyboard functionality */
    c = wgetch(menu_win);
    switch(c)
    {
      case KEY_UP:
        if (highlight==1)
          highlight = n_choices;
        else
          --highlight;
        break;
      case KEY_DOWN:
        if (highlight==n_choices)
          highlight = 1;
        else
          ++highlight;
        break;
      case 10:
        choice = highlight;
        break;
      default:
        mvprintw(24,0,"You typed: %c", c);
        refresh();
        break;
    }

    display_main_menu(menu_win, highlight);
    if (choice != 0)
      break;
    /* End while loop 1 */
  }

  /* Choice 1 : Write melody window */
  if (choice==1) {
    clear();
    refresh();
    echo(); // Turn character echo back on for taking input
    curs_set(2); // Show cursor

    choice = 0; // Reset choice variable
    // Create write melody window
    write_melody_win = newwin(HEIGHT, WIDTH*2, starty, startx);

    // Strings to hold user input
    char* file_str[80], tempo_str[4], scale_str[4], start_str[4];

    int y = 2, x = 2, exit = 0;

    /* Begin melody data entry */

    // Enter melody name
    mvwprintw(write_melody_win, y, x, "Melody name: ");
    mvwscanw(write_melody_win, y, x+15, "%s", file_str);
    strcat(file_str, ".txt"); // Add .txt to create filename
    pm->filename = file_str;
    y++;

    // Enter tempo (BPM)
    mvwprintw(write_melody_win, y, x, "Tempo(bpm): ");
    mvwscanw(write_melody_win, y, x+15, "%s", tempo_str);
    pm->tempo = atoi(tempo_str);
    y++;

    // Enter scale (1 = major pentatonic, 2 = minor pentatonic)
    mvwprintw(write_melody_win, y,x, "Scale (1,2): ");
    mvwscanw(write_melody_win, y, x+15, "%s", scale_str);
    pm->scale = atoi(scale_str);
    y++;

    // Enter starting note (MIDI number)
    mvwprintw(write_melody_win, y,x, "Start note: ");
    mvwscanw(write_melody_win, y, x+15, "%s", start_str);
    pm->start_note = atoi(start_str);

    /* Initialize params for melody grid */
    int highlight_x = 1, highlight_y = 1;
    int choice_x = 0, choice_y = 0;

    /* Zero out the notes array */
    for (int i = 0; i < NUM_COLS; i++)
      pm->notes[i] = 0;

    /* Clear the screen */
    wclear(write_melody_win);
    wrefresh(write_melody_win);

    /* Display the grid */
    display_write_melody(write_melody_win, pm, highlight_x,
      highlight_y, choice_x, choice_y);

    /* While loop 2: Write melody window */
    while(1)
    {
      c = getch();
      switch(c)
      {
        /* For some reason the key codes didn't work here
         * so I just used the integer values.
         */
        case 65: // Up arrow
          if (highlight_y==1)
            highlight_y = NUM_ROWS;
          else
            --highlight_y;
          break;
        case 66: // Down arrow
          if (highlight_y==NUM_ROWS)
            highlight_y = 1;
          else
            ++highlight_y;
          break;
        case 68: // Left arrow
          if (highlight_x==1)
            highlight_x = NUM_COLS;
          else
            --highlight_x;
          break;
        case 67: // Right arrow
          if (highlight_x==NUM_COLS)
            highlight_x = 1;
          else
            ++highlight_x;
          break;
        case 10: // Enter - confirm choice
          choice_x = highlight_x;
          choice_y = highlight_y;
          break;
        case 114: // r - go to read melody window
          choice = 2;
          exit = 1;
        case 113: // q - quit
          exit = 1;
        case 80: // F1 - save melody as .txt
          write_to_txt(pm);
        default:
          refresh();
          break;
      }

      display_write_melody(write_melody_win, pm, highlight_x, highlight_y, choice_x, choice_y);
      if (exit)
        break;
      /* End while loop */
    }

    clear();
    refresh();
    /* End choice 1 */
  }

  /* Choice 2: Read melody window */
  if (choice==2) {
    clear();
    refresh();

    int len, counter = 0, exit = 0;
    highlight = 1;
    choice = 0;
    // Read melody win is taller to show more melody files
    // And wider to fit the full name of the melody
    read_melody_win = newwin(HEIGHT*2, WIDTH*2, starty, startx);

    /* Initialize mel_choices array to null
      (this array will hold the filenames of all the melodies
      in the directory.)
    */
    for (int i = 0; i < MAX_MELS; i++) {
      mel_choices[i][0] = NULL;
    }

    /* Using dirent.h library to find .txt files in
      the current working directory */
    DIR *d;
    struct dirent *dir;
    d = opendir(".");

    if (d) {
      while ((dir = readdir(d)) != NULL) {
        len = strlen(dir->d_name);
        // If name ends in .txt, add to array
        if (strncmp(dir->d_name+len-4, ".txt", 4)==0) {
          strcpy(mel_choices[counter], dir->d_name);
          // Counter keeps track of how many files there are
          counter++;
        }
      }
      closedir(d);
    }

    display_read_melody(read_melody_win, highlight, counter);
    int only_once = 0; // Don't worry about it

    /* While loop 3: Read melody window */
    while(1)
    {
      c = getch();
      switch(c)
      {
        case 65: // Up arrow
          if (highlight==1)
            highlight = counter;
          else
            --highlight;
          break;
        case 66: // Down arrow
          if (highlight==counter)
            highlight = 1;
          else
            ++highlight;
          break;
        case 10: // Enter
          if (counter) // If there are melodies
            choice = highlight;
          else // If no melodies
            exit = 1;
          break;
        case 114: // r - record
          buf.wav_out = 1; // Record!
          only_once = 1;
          break;
        case 113: // q - quit
          exit = 1;
          break;
        default:
          refresh();
          break;
      }

      // if (buf.wav_out && only_once) { // Set up recording
      //   const char* melody_wav_name = pm->filename;
      //   /* Initialize sndfile output params */
      //   memset(&sfinfo, 0, sizeof(sfinfo));
      //   sfinfo.samplerate = SAMP_RATE;
      //   sfinfo.channels = NUM_CHAN;
      //   sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
      //   if ( (buf.sndfile = sf_open ("out.wav", SFM_WRITE, &sfinfo)) == NULL ) {
      //       fprintf (stderr, "Error: could not open output wav file\n");
      //       return -1;
      //   }
      //   only_once = 0;
      // }

      display_read_melody(read_melody_win, highlight, counter);
      if (choice != 0) {
        pm->filename = mel_choices[choice-1];
        read_melody(pm); // Reads melody to melody struct
        make_freqs(pm);  // Converts melody notes to frequencies
      }
      if (exit)
        break;
    /* End while loop 3 */
    }
  /* End choice 2 */
  }

  /* Choice 3: Exit */
  else {
    clear();
    refresh();
  }

  // if (buf.wav_out)
  //     /* close debugging output wav file */
  //     sf_close (buf.sndfile);

  /* Close PortAudio and Ncurses */
  shutdownPa(stream);
  delwin(menu_win);
  endwin();

  return 0;
}

/* Main menu function creates the window for the main menu
  and implements the highlight functionality.
*/
void display_main_menu(WINDOW *menu_win, int highlight)
{
  int x, y, i;

  x = 2;
  y = 2;
  box(menu_win, 0, 0);

  for (i = 0; i < n_choices; i++) {
    if (highlight == i + 1) {
      // Highlighted option
      wattron(menu_win, A_REVERSE);
      mvwprintw(menu_win, y, x, "%s", choices[i]);
      wattroff(menu_win, A_REVERSE);
    }
    else
      // Non-highlighted option
      mvwprintw(menu_win, y, x, "%s", choices[i]);
    ++y;
  }
  wrefresh(menu_win);
}

/* Write melody window function creates the grid
   and implements the highlight in both x and y dimensions,
   allowing the user to write a melody in the grid */
void display_write_melody(WINDOW *write_melody_win, Melody* pm,
  int highlight_x, int highlight_y, int choice_x, int choice_y)
{
  noecho(); // Turn character echo off for grid entry
  curs_set(0); // Hide cursor
  int x, y, i, j;
  char* ch;

  mvwprintw(write_melody_win, 0, 0, "Enter melody %s", pm->filename);
  mvwprintw(write_melody_win, 1, 0, "F1->save, r->play, q->quit");

  /* Make the grid and accept notes as input */
  for (i = 1; i < NUM_COLS+1; i++) {
    for (j = 1; j < NUM_ROWS; j++) {

      // Space Xs one apart horizontally
      x = 2*i;
      // Offset downward by 2
      y = j+2;

      // Add note choice to melody
      if ((choice_x==i) && (choice_y==j))
        pm->notes[i-1] = j;
      // If note in melody, print "o"; otherwise "x"
      if (pm->notes[i-1] == j)
        ch = "o";
      else
        ch = "x";

      // Implement highlight
      if ((highlight_x == i) && (highlight_y == j)) {
        wattron(write_melody_win, A_REVERSE);
        mvwprintw(write_melody_win, y, x, ch);
        wattroff(write_melody_win, A_REVERSE);
      } else {
        mvwprintw(write_melody_win, y, x, ch);
      }
    }
  }
  wrefresh(write_melody_win);
}

/* Read melody function implements highlight if melodies
   are found; or displays "No melodies found." */
void display_read_melody(WINDOW *read_melody_win, int highlight, int counter)
{
  int x, y, i;

  x = 2;
  y = 2;
  box(read_melody_win, 0, 0);
  wrefresh(read_melody_win);

  // If there are melodies
  if(counter) {
    mvwprintw(read_melody_win, y, x, "Enter->select, r->rec, q->quit");
    y++;
    for (i = 0; i < counter; i++) {
      // Implement highlight
      if (highlight == i + 1) {
        wattron(read_melody_win, A_REVERSE);
        mvwprintw(read_melody_win, y, x, "%s", mel_choices[i]);
        wattroff(read_melody_win, A_REVERSE);
      }
      // No highlight
      else
        mvwprintw(read_melody_win, y, x, "%s", mel_choices[i]);
      ++y;
    }
  }
  // If no melodies
  else {
    mvwprintw(read_melody_win, y, x, "No melodies found");
    mvwprintw(read_melody_win, y+1, x, "q or enter to quit");
  }

  wrefresh(read_melody_win);
}

/* Audio callback calls the play_note() and synth_sample() functions,
   which synthesizes one sample of audio at a time
   and uses the melody's freqs array to get the frequencies */
static int paCallback(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
    Buf *pb = (Buf *)userData; /* Cast pointer to data passed through stream */
    Synth *ps = pb->ps;  /* struct Synth */
    Melody *pm = pb->pm;
    float *output = (float *)outputBuffer;
    //float *input = (float *)inputBuffer; /* input not used in this code */

    /* Initialize note at beginning of count */
    if ((ps->index_count == 0) && (ps->samp_count == 0))
      play_note(ps,pm->freqs[ps->index_count]);

    /* Iterate each sample in the buffer */
    for ( int i = 0; i < framesPerBuffer; i++ ) {
      // Synthesize each sample
      output[2*i] = synth_sample(ps);
      output[2*i + 1] = synth_sample(ps);
      // Increment sample count
      ps->samp_count++;

      /* If samp count reaches note duration */
      if (ps->samp_count >= pm->note_duration) {
        // Increment index count
        ps->index_count++;
        // Reset sample count
        ps->samp_count = 0;

        // If there is a new note, play the new note
          // (Otherwise let the previous note ring out)
        if (pm->freqs[ps->index_count])
          play_note(ps,pm->freqs[ps->index_count]);

        if (ps->index_count == 0)
          printf("%2f / ", pm->freqs[ps->index_count]);

        // If end reached, loop melody - start from beginning
        if (ps->index_count >= NUM_COLS) {
          ps->index_count = 0;
        }
      }
    }

  // if (pb->wav_out)
    /* write to output file */
    // sf_writef_float (pb->sndfile, output, framesPerBuffer);

    return 0;
}
