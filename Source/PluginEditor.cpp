#include "PluginEditor.h"

VoxForgeEditor::VoxForgeEditor (VoxForgeProcessor& p)
    : AudioProcessorEditor (&p), proc (p), generic (p)
{
    addAndMakeVisible (cleanBtn);
    addAndMakeVisible (screamBtn);

    cleanBtn.onClick  = [this] { proc.setCurrentProgram (0); proc.updateHostDisplay(); };
    screamBtn.onClick = [this] { proc.setCurrentProgram (1); proc.updateHostDisplay(); };

    viewport.setViewedComponent (&generic, false);
    addAndMakeVisible (viewport);

    setSize (480, 640);
}

void VoxForgeEditor::resized()
{
    auto r = getLocalBounds();
    auto top = r.removeFromTop (44).reduced (6);
    cleanBtn.setBounds (top.removeFromLeft (top.getWidth() / 2).reduced (2));
    screamBtn.setBounds (top.reduced (2));
    viewport.setBounds (r);
}
