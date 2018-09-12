# towave-j: Program to record channels from chiptune files

`towave-j` is an improved fork of `towave rel1` by icesoldier (source code of rel2 and rel3 were never uploaded).

## Bugfixes

- No longer erroneously skips silence in track 1. (Also fixed in binary-only `towave rel3`)

## Features

- Automatically rips master audio (all channels together)
- Command-line arguments for duration and track number (both optional)
- Fadeout (8 seconds, hard-coded by libgme)
- More accurate YM2612 emulation
    - CMakeLists.txt with static linking to <https://bitbucket.org/mpyne/game-music-emu>

## Planned

- Ability to accurately dump NSF files with nonlinear mixing (by emulating dpcm, dpcm+tri, dpcm+tri+noise)
- More accurate NSF emulation (using nsfplay core?)

## Usage

towave-j will be used in <https://github.com/corrscope/corrscope>, a multi-channel oscilloscope program with "intelligent" correlation triggering and real-time playback.
