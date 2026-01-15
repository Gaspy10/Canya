#include "WavetableVoice.h"
#include "WavetableSound.h"
#include <juce_dsp/juce_dsp.h>

using Coeff = juce::dsp::IIR::Coefficients<double>;

WavetableVoice::WavetableVoice(const juce::AudioSampleBuffer& sine, WaveFormSettings& w)
    : sineTable(sine), waveFormSettings(w), filter{ 101, 48000 } {
    env.setAttackMS(100);
    env.setDecay(500);
    env.setSustain(0.8);
    env.setRelease(100);
}

bool WavetableVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<WavetableSound*> (sound) != nullptr;
}

void WavetableVoice::startNote(int midiNoteNumber, float velocity,
    juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
{
    frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    level = velocity * 0.15;
    filter.setCutoff(waveFormSettings.getCutoffLowFrequency());


    env.trigger = 1; // start attack
}

void WavetableVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    env.trigger = 0; // start release
    if (!allowTailOff)
        clearCurrentNote(); // force cut
}

void WavetableVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer,
    int startSample, int numSamples)
{
    //auto coeffs = Coeff::makeHighPass(sampleRate, cutoffHz, Q);

    if (env.trigger == 0 && envValue < 0.0001)
        return;

    while (--numSamples >= 0)
    {
        envValue = env.adsr(1.0, env.trigger);

		double cutoffLow = waveFormSettings.getCutoffLowFrequency();
		double cutoffHigh = waveFormSettings.getCutoffHighFrequency();
        double sample = filter.processSample(getNextSample());
        sample = amplify(sample * level * envValue);

        for (int i = outputBuffer.getNumChannels(); --i >= 0;)
            outputBuffer.addSample(i, startSample, sample);

        ++startSample;

        if (env.trigger == 0 && envValue < 0.0001)
            clearCurrentNote();
    }
}

double WavetableVoice::amplify(double sample) const
{
	return sample * waveFormSettings.getVelocity();
}

double WavetableVoice::getNextSample()
{
    auto c = waveFormSettings.getSelectedWaveForm();

    switch (c) {
        case WaveFormSettings::WaveForms::sine: {
            return osc.sinewave(frequency);
        }
        case WaveFormSettings::WaveForms::square: {
            return osc.square(frequency);
        }
        case WaveFormSettings::WaveForms::triangle: {
            return osc.triangle(frequency);
        }
        case WaveFormSettings::WaveForms::sawtooth: {
			return osc.saw(frequency);
		}
    }
}
