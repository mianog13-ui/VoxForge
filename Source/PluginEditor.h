#pragma once
#include "PluginProcessor.h"

class VoxForgeEditor : public juce::AudioProcessorEditor
{
public:
    explicit VoxForgeEditor (VoxForgeProcessor&);
    void resized() override;

private:
    VoxForgeProcessor& proc;
    juce::TextButton cleanBtn { "CLEAN" }, screamBtn { "SCREAM" };
    juce::GenericAudioProcessorEditor generic;
    juce::Viewport viewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoxForgeEditor)
};
