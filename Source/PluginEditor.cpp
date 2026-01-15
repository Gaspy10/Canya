#include "PluginEditor.h"

//==============================================================================
PluginEditor::PluginEditor (JuceSynthPluginAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p),
      keyboardComponent (processor.keyboardState,
                         juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // ================= ADD COMPONENTS =================
    addAndMakeVisible (keyboardComponent);
    addAndMakeVisible (waveForm);
    addAndMakeVisible (decibelSlider);
    addAndMakeVisible (decibelLabel);
    //addAndMakeVisible (analyserComponent);
    addAndMakeVisible (cutoffLowSlider);
    addAndMakeVisible (cutoffHighSlider);

    // ================= LABEL =================
    decibelLabel.setText ("Noise Level in dB", juce::dontSendNotification);

    // ================= SLIDERS =================
    decibelSlider.setRange (-24.0, 24.0);
    decibelSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 100, 20);

    cutoffLowSlider.setRange (20.0, 20000.0);
    cutoffHighSlider.setRange (20.0, 20000.0);

    // ================= WAVEFORM COMBO =================
    waveForm.addItem ("Sine", 1);
    waveForm.addItem ("Square", 2);
    waveForm.addItem ("Triangle", 3);
    waveForm.addItem ("Sawtooth", 4);

    // ================= PARAMETER ATTACHMENTS =================
    auto& apvts = processor.apvts;

    gainAttachment = std::make_unique<SliderAttachment> (
        apvts, "gain", decibelSlider);

    cutoffLowAttachment = std::make_unique<SliderAttachment> (
        apvts, "cutoffLow", cutoffLowSlider);

    cutoffHighAttachment = std::make_unique<SliderAttachment> (
        apvts, "cutoffHigh", cutoffHighSlider);

    waveAttachment = std::make_unique<ComboBoxAttachment> (
        apvts, "wave", waveForm);

    // ================= EDITOR =================
    setSize (600, 160);

    startTimerHz (30); // GUI refresh only (e.g. analyser repaint)
}

//==============================================================================
PluginEditor::~PluginEditor() = default;

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

//==============================================================================
void PluginEditor::resized()
{
    keyboardComponent.setBounds (10, 10,
                                getWidth() - 20,
                                getHeight() / 2);

    waveForm.setBounds (10,
                        getHeight() / 2 + 10,
                        105, 30);

    decibelSlider.setBounds (120,
                             getHeight() / 2 + 10,
                             getWidth() - 130,
                             30);

    //analyserComponent.setBounds (getWidth() * 0.4f, getHeight() / 2 + 40, getWidth() * 0.5f, getHeight() * 0.3f);

    cutoffLowSlider.setBounds (10,
                               getHeight() / 2 + 50,
                               (getWidth() * 0.75f) / 2 - 10,
                               30);

    cutoffHighSlider.setBounds (10 + (getWidth() * 0.75f) / 2,
                                getHeight() / 2 + 50,
                                (getWidth() * 0.75f) / 2 - 10,
                                30);
}

//==============================================================================
void PluginEditor::timerCallback()
{
    // GUI-only updates (no DSP!)
    //analyserComponent.repaint();
}
