#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class VoxForgeProcessor : public juce::AudioProcessor
{
public:
    VoxForgeProcessor();
    ~VoxForgeProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "VoxForge"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 3.0; }

    // Presets are exposed as host "programs": 0 = Clean, 1 = Scream
    int getNumPrograms() override { return 2; }
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override { return index == 0 ? "Clean" : "Scream"; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    void applyPreset (int index);
    float get (const char* id) const { return apvts.getRawParameterValue (id)->load(); }

    using Filter = juce::dsp::IIR::Filter<float>;
    using Coeffs = juce::dsp::IIR::Coefficients<float>;
    using Duplicator = juce::dsp::ProcessorDuplicator<Filter, Coeffs>;
    using Delay = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>;

    int currentProgram = 0;
    double sr = 44100.0;

    Duplicator hpf, lowMidCut, lowShelf, midBoost, airShelf, deessBand, satHP, satLP, satDC;
    juce::dsp::NoiseGate<float> gate;
    juce::dsp::Compressor<float> comp1, comp2;
    juce::dsp::Reverb reverb;
    juce::dsp::Limiter<float> limiter;
    Delay doubler { 96000 }, echo { 192000 };

    juce::AudioBuffer<float> sideBuffer, wetBuffer;
    float deEnv = 0.0f;
    float lfoPhase = 0.0f;
    float echoLp[2] { 0.0f, 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoxForgeProcessor)
};
