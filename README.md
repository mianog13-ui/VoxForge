# VoxForge

A metalcore vocal channel strip (VST3 + Standalone) with two presets: **Clean** and **Scream**.
Inspired by the tight, upfront, heavily-processed modern metalcore vocal sound (Architects-era polish, Buster Odeholm-style aggression). These are starting points, not exact copies of anyone's chain.

## Signal chain

Input gain → High-pass → Gate → Comp 1 (fast) → EQ (low-mid carve, presence, air) → Parallel saturation → De-esser → Comp 2 (leveler) → Doubler → Echo → Reverb → Output gain → Safety limiter

## Presets

| | Clean | Scream |
|---|---|---|
| HPF | 90 Hz | 100 Hz |
| Gate | off | -40 dB, 10:1 |
| Comp 1 | 5:1, 4 ms | 10:1, 0.8 ms |
| Comp 2 | 3:1 | 5:1 |
| Low-mid cut | -3 dB @ 300 Hz | -5 dB @ 260 Hz |
| Low shelf | 0 dB | +2 dB @ 120 Hz |
| Presence | +2.5 dB @ 3 kHz | +4 dB @ 2.6 kHz |
| Air | +4 dB | +3 dB |
| Saturation | 6 dB drive, 20% | 20 dB drive, 70% |
| Sat tone LP | 10 kHz | 8.5 kHz |
| Doubler | 30% | 30% |
| Delay | 250 ms, 10% | 110 ms, 4% |
| Reverb | medium, 16% | short, 5% |

Saturation runs as a parallel path: it is high-passed before distortion (so the low end stays clean and tight), shaped by an asymmetric tanh curve, low-passed to tame fizz, and blended back in.

## Build with GitHub (no local setup needed)

1. Create an empty repository on github.com (e.g. `VoxForge`), no README.
2. In this folder:
   ```bash
   git init
   git add .
   git commit -m "Initial commit"
   git branch -M main
   git remote add origin https://github.com/<your-username>/VoxForge.git
   git push -u origin main
   ```
3. Open the **Actions** tab. The "Build VoxForge" workflow starts automatically (first run takes ~10 to 15 min because it downloads JUCE).
4. When it finishes, open the run and download the artifacts at the bottom: `VoxForge-Windows`, `VoxForge-macOS`, `VoxForge-Linux`.
5. Unzip and copy `VoxForge.vst3` to:
   - **Windows:** `C:\Program Files\Common Files\VST3`
   - **macOS:** `~/Library/Audio/Plug-Ins/VST3` (unsigned builds: run `xattr -cr VoxForge.vst3` in Terminal first)
   - **Linux:** `~/.vst3`
6. Rescan plugins in your DAW.

If a build fails, open the failed job log and paste the error to me and I'll fix it. This code was written without being compiled in my environment, so a small fix on the first run is possible.

## Using it

- **Gain staging:** peak around -12 dBFS going in. Aim for roughly 4 to 8 dB of gain reduction on Comp 1 for screams and 3 to 5 dB for cleans.
- **Scream gate:** raise the threshold until breaths and bleed vanish, but sustained screams don't chatter.
- **Layering (big part of the modern sound):** track the scream 3 to 4 times, pan two hard L/R, keep one center. Add a low layer (a separate pass, pitched or sung down) mixed under the main. Put VoxForge on each, with the Doubler lower on the stacked tracks.
- **Cleans:** double-track and pan the doubles; use Delay/Reverb sends in your DAW if you want more space.
- Use a parallel bus (heavy compression + saturation, blended low) for extra density.

## Licensing

JUCE is dual-licensed (AGPLv3 / commercial). Releasing this publicly means your repo must be AGPLv3-compatible; selling closed-source plugins requires a JUCE commercial license. See https://juce.com/juce-8-licence/
