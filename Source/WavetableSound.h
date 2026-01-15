/*
  ==============================================================================

    WavetableSound.h
    Created: 24 Jul 2025 12:03:54pm
    Author:  D

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct WavetableSound : public juce::SynthesiserSound
{
    WavetableSound();

    bool appliesToNote(int) override;
    bool appliesToChannel(int) override;
};
