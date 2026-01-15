#pragma once
#include <JuceHeader.h>
#include <vector>
#include <cmath>

class FIRFilter
{
public:
    FIRFilter(int numTaps, double sampleRate)
        : taps(numTaps),
        fs(sampleRate),
        buffer(numTaps, 0.0f),
        coeffs(numTaps, 0.0f)
    {
        jassert(numTaps >= 3);
        setCutoff(1000.0f); // default until user sets
    }

    // ============================================================
    //      Change cutoff frequency dynamically
    // ============================================================
    void setCutoff(float cutoffHz)
    {
        cutoff = cutoffHz;
        generateCoefficients(cutoffHz);
    }

    // ============================================================
    //      Process single sample (real-time safe)
    // ============================================================
    float processSample(float x) noexcept
    {
        buffer[index] = x;

        float y = 0.0f;
        int bufIdx = index;

        // Manual circular buffer convolution
        for (int i = 0; i < taps; i++)
        {
            y += coeffs[i] * buffer[bufIdx];
            bufIdx = (bufIdx == 0 ? taps - 1 : bufIdx - 1);
        }

        index = (index + 1) % taps;
        return y;
    }

    // ============================================================
    //      Process JUCE AudioBuffer
    // ============================================================
    void processBlock(juce::AudioBuffer<float>& bufferToProcess)
    {
        const int numSamples = bufferToProcess.getNumSamples();
        const int numChannels = bufferToProcess.getNumChannels();

        for (int ch = 0; ch < numChannels; ch++)
        {
            float* data = bufferToProcess.getWritePointer(ch);

            for (int i = 0; i < numSamples; i++)
                data[i] = processSample(data[i]);
        }
    }

private:
    int taps;
    double fs;
    float cutoff = 1000.0f;

    std::vector<float> coeffs;
    std::vector<float> buffer;
    int index = 0;

    // ============================================================
    //      Recompute coefficients using Hamming-windowed sinc
    // ============================================================
    void generateCoefficients(float cutoffHz)
    {
        const float fc = cutoffHz / fs;  // normalized 0..0.5
        const int M = taps - 1;

        for (int n = 0; n < taps; n++)
        {
            int k = n - M / 2;

            // Ideal sinc low-pass
            float sinc = (k == 0)
                ? 2.0f * fc
                : std::sin(2.0f * juce::MathConstants<float>::pi * fc * k)
                / (juce::MathConstants<float>::pi * k);

            // Hamming window
            float w = 0.54f - 0.46f * std::cos(2.0f * juce::MathConstants<float>::pi * n / M);

            coeffs[n] = sinc * w;
        }
    }
};
