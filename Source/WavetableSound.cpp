/*
  ==============================================================================

    WavetableSound.cpp
    Created: 24 Jul 2025 12:03:44pm
    Author:  D

  ==============================================================================
*/

#include "WavetableSound.h"

WavetableSound::WavetableSound() {}

bool WavetableSound::appliesToNote(int) { return true; }
bool WavetableSound::appliesToChannel(int) { return true; }
