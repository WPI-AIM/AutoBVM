
/**
 * Code contains snippets from the MIT Emergency Ventilator Controller
 * poject: https://github.com/mit-drl/e-vent
 * 
 */

#ifndef UVENT_TONE_H
#define UVENT_TONE_H

#include <Arduino.h>
#include "tone_driver.h"
#include "note.h"

/**
 * A Tone is a sequence of notes.
 * Each Note has a note, duration and pause.
 */
class Tone {
public:
    Tone() : length_(0) {}

    Tone(const Note notes[], const int& notes_length, const int* pin)
        : notes_(notes), length_(notes_length), pin_(pin), tone_step_(length_) {}

    // Play the tone, if any
    void play()
    {
        if (length_ == 0) {
            return;
        }
        if (!playing_) {// Do once when tone starts
            tone_timer_ = millis();
            tone_step_ = 0;
            playing_ = true;
        }
        tone_step_ %= length_;// Start again if tone finished
        if (millis() > tone_timer_) {
            tone_rrb(*pin_, notes_[tone_step_].note, notes_[tone_step_].duration);
            tone_timer_ += notes_[tone_step_].duration + notes_[tone_step_].pause;
            tone_step_++;
        }
    }

    // Stop playing
    inline void stop() { playing_ = false; }

private:
    const Note* notes_;           // Pointer to the notes to play
    int length_;                  // Length of the tone sequence
    const int* pin_;              // The pin to play the note on
    bool playing_ = false;        // Flag to initialize the tone timer once
    int tone_step_;               // Time to pause between each note in a tone sequence
    unsigned long tone_timer_ = 0;// Timer to keep track of each note in a tone sequence
};

#endif//UVENT_TONE_H