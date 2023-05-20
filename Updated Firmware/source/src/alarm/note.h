/**
 * Code contains snippets from the MIT Emergency Ventilator Controller
 * poject: https://github.com/mit-drl/e-vent
 * 
 */

#ifndef UVENT_NOTE_H
#define UVENT_NOTE_H

#include "pitches.h"

// Container for notes elements
struct Note {
    int note;
    int duration;
    int pause;
};

// Notifiation notes
static const Note kNotifyNotes[] = {
        {NOTE_B4, 200, 100},
        {NOTE_B4, 200, 2000}};

// Emergency notes
static const Note kEmergencyNotes[] = {
        {NOTE_G4, 300, 200},
        {NOTE_G4, 300, 200},
        {NOTE_G4, 300, 400},
        {NOTE_G4, 200, 100},
        {NOTE_G5, 200, 1500}};

// Notifiation notes
static const Note kOffNotes[] = {
        {NOTE_G4, 200, 200}};
#endif//UVENT_NOTE_H