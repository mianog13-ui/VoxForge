#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

namespace
{
    // id, name, min, max, CLEAN default, skew
    struct Def { const char* id; const char* name; float lo, hi, def, skew; };

    const Def defs[] = {
        { "inGain",      "Input Gain (dB)",      -12.f, 24.f,   0.f,    1.f },
        { "hpf",         "High-Pass (Hz)",        20.f, 400.f,  90.f,   0.5f },
        { "gateThresh",  "Gate Threshold (dB)",  -80.f, -20.f, -80.f,   1.f },
        { "gateRatio",   "Gate Ratio",             1.f, 20.f,   1.f,    1.f },
        { "gateRelease", "Gate Release (ms)",     10.f, 500.f, 100.f,   1.f },

        { "c1Thresh",    "Comp 1 Threshold",     -50.f, 0.f,  -24.f,    1.f },
        { "c1Ratio",     "Comp 1 Ratio",           1.f, 20.f,   5.f,    1.f },
        { "c1Attack",    "Comp 1 Attack (ms)",    0.1f, 50.f,   4.f,    1.f },
        { "c1Release",   "Comp 1 Release (ms)",   10.f, 500.f,  70.f,   1.f },
        { "c1Makeup",    "Comp 1 Makeup (dB)",     0.f, 18.f,   4.f,    1.f },

        { "c2Thresh",    "Comp 2 Threshold",     -50.f, 0.f,  -16.f,    1.f },
        { "c2Ratio",     "Comp 2 Ratio",           1.f, 20.f,   3.f,    1.f },
        { "c2Attack",    "Comp 2 Attack (ms)",    0.1f, 50.f,  20.f,    1.f },
        { "c2Release",   "Comp 2 Release (ms)",   10.f, 500.f, 180.f,   1.f },
        { "c2Makeup",    "Comp 2 Makeup (dB)",     0.f, 18.f,   2.f,    1.f },

        { "lmFreq",      "Low-Mid Cut Freq (Hz)", 150.f, 600.f, 300.f,  1.f },
        { "lmCut",       "Low-Mid Cut (dB)",      -12.f, 0.f,  -3.f,    1.f },
        { "lowGain",     "Low Shelf 120 Hz (dB)",  -6.f, 9.f,    0.f,    1.f },
        { "midFreq",     "Presence Freq (Hz)",   1000.f, 6000.f, 3000.f, 1.f },
        { "midGain",     "Presence Gain (dB)",    -6.f, 9.f,    2.5f,   1.f },
        { "airGain",     "Air Shelf 10k (dB)",    -6.f, 9.f,    4.f,    1.f },

        { "drive",       "Saturation Drive (dB)",  0.f, 36.f,   6.f,    1.f },
        { "satMix",      "Saturation Mix",         0.f, 1.f,    0.2f,   1.f },
        { "satHP",       "Sat Pre-HP (Hz)",       20.f, 800.f, 200.f,   0.5f },
        { "satTone",     "Sat Tone LP (Hz)",    2000.f, 16000.f, 10000.f, 1.f },
        { "satBias",     "Sat Asymmetry",          0.f, 0.5f,   0.1f,   1.f },

        { "deessThresh", "De-Ess Threshold (dB)", -50.f, -5.f, -28.f,   1.f },
        { "deessAmt",    "De-Ess Amount",          0.f, 1.f,    0.6f,   1.f },

        { "dblMix",      "Doubler Mix",            0.f, 1.f,    0.3f,   1.f },

        { "dlyTime",     "Delay Time (ms)",       20.f, 800.f, 250.f,   1.f },
        { "dlyFb",       "Delay Feedback",         0.f, 0.8f,   0.3f,   1.f },
        { "dlyMix",      "Delay Mix",              0.f, 1.f,    0.1f,   1.f },

        { "rvbSize",     "Reverb Size",            0.f, 1.f,    0.4f,   1.f },
        { "rvbDamp",     "Reverb Damping",         0.f, 1.f,    0.55f,  1.f },
        { "rvbMix",      "Reverb Mix",             0.f, 1.f,    0.16f,  1.f },

        { "outGain",     "Output Gain (dB)",     -24.f, 12.f,   0.f,    1.f },
    };

    // Overrides applied on top of the Clean values for the Scream preset
    struct Ov { const char* id; float value; };
    const Ov screamOverrides[] = {
        { "inGain", 3.f },        { "hpf", 100.f },
        { "gateThresh", -40.f },  { "gateRatio", 10.f },    { "gateRelease", 60.f },
        { "c1Thresh", -28.f },    { "c1Ratio", 10.f },      { "c1Attack", 0.8f },
        { "c1Release", 40.f },    { "c1Makeup", 6.f },
        { "c2Thresh", -20.f },    { "c2Ratio", 5.f },       { "c2Attack", 8.f },
        { "c2Release", 100.f },   { "c2Makeup", 4.f },
        { "lmFreq", 260.f },      { "lmCut", -5.f },        { "lowGain", 2.f },
        { "midFreq", 2600.f },    { "midGain", 4.f },       { "airGain", 3.f },
        { "drive", 20.f },        { "satMix", 0.7f },
        { "satHP", 180.f },       { "satTone", 8500.f },    { "satBias", 0.2f },
        { "deessThresh", -32.f }, { "deessAmt", 0.5f },
        { "dblMix", 0.3f },
        { "dlyTime", 110.f },     { "dlyFb", 0.12f },       { "dlyMix", 0.04f },
        { "rvbSize", 0.2f },      { "rvbDamp", 0.75f },     { "rvbMix", 0.05f },
        { "outGain", -4.f },
    };
}

AudioProcessorValueTreeState::ParameterLayout VoxForgeProcessor::createLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    for (auto& d : defs)
        layout.add (std::make_unique<AudioParameterFloat> (ParameterID { d.id, 1 }, d.name,
                                                           NormalisableRange<float> (d.lo, d.hi, 0.01f, d.skew),
                                                           d.def));
    return layout;
}

VoxForgeProcessor::VoxForgeProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  AudioChannelSet::stereo(), true)
                          .withOutput ("Output", AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
}

bool VoxForgeProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    const auto out = l.getMainOutputChannelSet();
    return (out == AudioChannelSet::mono() || out == AudioChannelSet::stereo())
           && out == l.getMainInputChannelSet();
}

void VoxForgeProcessor::applyPreset (int index)
{
    auto set = [this] (const char* id, float v)
    {
        if (auto* p = apvts.getParameter (id))
            p->setValueNotifyingHost (p->convertTo0to1 (v));
    };

    for (auto& d : defs) set (d.id, d.def);                       // Clean baseline
    if (index == 1)
        for (auto& o : screamOverrides) set (o.id, o.value);      // Scream overrides
}

void VoxForgeProcessor::setCurrentProgram (int index)
{
    currentProgram = jlimit (0, 1, index);
    applyPreset (currentProgram);
}

void VoxForgeProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    dsp::ProcessSpec spec { sampleRate, (uint32) samplesPerBlock, 2 };

    for (auto* f : { &hpf, &lowMidCut, &lowShelf, &midBoost, &airShelf, &deessBand, &satHP, &satLP, &satDC })
        f->prepare (spec);

    gate.prepare (spec);
    comp1.prepare (spec);
    comp2.prepare (spec);
    reverb.prepare (spec);
    limiter.prepare (spec);

    doubler.setMaximumDelayInSamples ((int) (sampleRate * 0.1));
    doubler.prepare (spec);
    echo.setMaximumDelayInSamples ((int) (sampleRate * 1.2));
    echo.prepare (spec);

    sideBuffer.setSize (2, samplesPerBlock);
    wetBuffer.setSize (2, samplesPerBlock);
    deEnv = 0.0f;
    lfoPhase = 0.0f;
    echoLp[0] = echoLp[1] = 0.0f;
}

void VoxForgeProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    const int numCh = jmin (buffer.getNumChannels(), 2);
    const int n = buffer.getNumSamples();
    const float fs = (float) sr;

    for (int c = getTotalNumInputChannels(); c < getTotalNumOutputChannels(); ++c)
        buffer.clear (c, 0, n);

    // ---- input gain
    buffer.applyGain (Decibels::decibelsToGain (get ("inGain")));

    dsp::AudioBlock<float> block (buffer);
    auto sub = block.getSubsetChannelBlock (0, (size_t) numCh);
    dsp::ProcessContextReplacing<float> ctx (sub);

    // ---- high-pass
    *hpf.state = *Coeffs::makeHighPass (sr, get ("hpf"));
    hpf.process (ctx);

    // ---- gate (ratio 1 = effectively off)
    gate.setThreshold (get ("gateThresh"));
    gate.setRatio (get ("gateRatio"));
    gate.setAttack (1.0f);
    gate.setRelease (get ("gateRelease"));
    gate.process (ctx);

    // ---- compressor 1 (fast catcher)
    comp1.setThreshold (get ("c1Thresh"));
    comp1.setRatio (get ("c1Ratio"));
    comp1.setAttack (get ("c1Attack"));
    comp1.setRelease (get ("c1Release"));
    comp1.process (ctx);
    buffer.applyGain (Decibels::decibelsToGain (get ("c1Makeup")));

    // ---- EQ: low-mid carve, presence, air
    *lowMidCut.state = *Coeffs::makePeakFilter (sr, get ("lmFreq"), 1.0f, Decibels::decibelsToGain (get ("lmCut")));
    *midBoost.state  = *Coeffs::makePeakFilter (sr, get ("midFreq"), 0.9f, Decibels::decibelsToGain (get ("midGain")));
    *airShelf.state  = *Coeffs::makeHighShelf  (sr, 10000.0f, 0.7f, Decibels::decibelsToGain (get ("airGain")));
    *lowShelf.state = *Coeffs::makeLowShelf (sr, 120.0f, 0.7f, Decibels::decibelsToGain (get ("lowGain")));
    lowShelf.process (ctx);
    lowMidCut.process (ctx);
    midBoost.process (ctx);
    airShelf.process (ctx);

    // ---- parallel saturation: high-passed wet path (lows stay clean), asymmetric
    //      tanh shaping, tone low-pass, DC block, then blended back in
    {
        const float g = Decibels::decibelsToGain (get ("drive"));
        const float mix = get ("satMix");
        const float bias = get ("satBias");
        const float makeup = std::pow (g, -0.4f);
        const float dcRef = std::tanh (bias);

        if (n > wetBuffer.getNumSamples())
            wetBuffer.setSize (2, n, false, false, true);

        for (int ch = 0; ch < numCh; ++ch)
            wetBuffer.copyFrom (ch, 0, buffer, ch, 0, n);

        dsp::AudioBlock<float> wb (wetBuffer);
        auto wSub = wb.getSubBlock (0, (size_t) n).getSubsetChannelBlock (0, (size_t) numCh);
        dsp::ProcessContextReplacing<float> wctx (wSub);

        *satHP.state = *Coeffs::makeHighPass (sr, get ("satHP"));
        satHP.process (wctx);

        for (int ch = 0; ch < numCh; ++ch)
        {
            auto* w = wetBuffer.getWritePointer (ch);
            for (int s = 0; s < n; ++s)
                w[s] = (std::tanh (w[s] * g + bias) - dcRef) * makeup;
        }

        *satLP.state = *Coeffs::makeLowPass (sr, get ("satTone"));
        satLP.process (wctx);
        *satDC.state = *Coeffs::makeHighPass (sr, 40.0f);
        satDC.process (wctx);

        for (int ch = 0; ch < numCh; ++ch)
            buffer.addFrom (ch, 0, wetBuffer, ch, 0, n, mix);
    }

    // ---- de-esser (band-limited gain reduction)
    {
        if (n > sideBuffer.getNumSamples())
            sideBuffer.setSize (2, n, false, false, true);

        for (int ch = 0; ch < numCh; ++ch)
            sideBuffer.copyFrom (ch, 0, buffer, ch, 0, n);

        *deessBand.state = *Coeffs::makeBandPass (sr, 7000.0f, 1.5f);
        dsp::AudioBlock<float> sb (sideBuffer);
        auto sideSub = sb.getSubBlock (0, (size_t) n).getSubsetChannelBlock (0, (size_t) numCh);
        deessBand.process (dsp::ProcessContextReplacing<float> (sideSub));

        const float thr = Decibels::decibelsToGain (get ("deessThresh"));
        const float amt = get ("deessAmt");
        const float atk = std::exp (-1.0f / (0.001f * fs));
        const float rel = std::exp (-1.0f / (0.050f * fs));

        for (int s = 0; s < n; ++s)
        {
            float peak = 0.0f;
            for (int ch = 0; ch < numCh; ++ch)
                peak = jmax (peak, std::abs (sideBuffer.getSample (ch, s)));

            const float coef = peak > deEnv ? atk : rel;
            deEnv = coef * deEnv + (1.0f - coef) * peak;

            const float gain = deEnv > thr ? std::pow (thr / deEnv, amt) : 1.0f;
            for (int ch = 0; ch < numCh; ++ch)
                buffer.getWritePointer (ch)[s] -= sideBuffer.getSample (ch, s) * (1.0f - gain);
        }
    }

    // ---- compressor 2 (leveler)
    comp2.setThreshold (get ("c2Thresh"));
    comp2.setRatio (get ("c2Ratio"));
    comp2.setAttack (get ("c2Attack"));
    comp2.setRelease (get ("c2Release"));
    comp2.process (ctx);
    buffer.applyGain (Decibels::decibelsToGain (get ("c2Makeup")));

    // ---- doubler + echo (per-sample)
    {
        const float dblMix = get ("dblMix");
        const float dlyMs  = get ("dlyTime");
        const float fb     = get ("dlyFb");
        const float dlyMix = get ("dlyMix");
        const float twoPi  = MathConstants<float>::twoPi;

        for (int s = 0; s < n; ++s)
        {
            const float lfo = std::sin (lfoPhase);
            lfoPhase += twoPi * 0.35f / fs;
            if (lfoPhase > twoPi) lfoPhase -= twoPi;

            for (int ch = 0; ch < numCh; ++ch)
            {
                auto* d = buffer.getWritePointer (ch);
                const float x0 = d[s];
                float x = x0;

                // doubler: two short, slowly modulated delays (L/R different)
                doubler.pushSample (ch, x0);
                const float dMs = (ch == 0 ? 13.0f : 21.0f) + 1.5f * (ch == 0 ? lfo : -lfo);
                x += doubler.popSample (ch, dMs * 0.001f * fs) * dblMix;

                // echo with darkened feedback
                echo.pushSample (ch, x0 + echoLp[ch] * fb);
                const float eMs = dlyMs * (ch == 0 ? 1.0f : 1.25f);
                const float ew = echo.popSample (ch, eMs * 0.001f * fs);
                echoLp[ch] += 0.35f * (ew - echoLp[ch]);
                x += ew * dlyMix;

                d[s] = x;
            }
        }
    }

    // ---- reverb
    {
        dsp::Reverb::Parameters rp;
        rp.roomSize   = get ("rvbSize");
        rp.damping    = get ("rvbDamp");
        rp.wetLevel   = get ("rvbMix");
        rp.dryLevel   = 1.0f;
        rp.width      = 1.0f;
        reverb.setParameters (rp);
        reverb.process (ctx);
    }

    // ---- output + safety limiter
    buffer.applyGain (Decibels::decibelsToGain (get ("outGain")));
    limiter.setThreshold (-1.0f);
    limiter.setRelease (60.0f);
    limiter.process (ctx);
}

AudioProcessorEditor* VoxForgeProcessor::createEditor() { return new VoxForgeEditor (*this); }

void VoxForgeProcessor::getStateInformation (MemoryBlock& dest)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, dest);
}

void VoxForgeProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (ValueTree::fromXml (*xml));
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new VoxForgeProcessor(); }
