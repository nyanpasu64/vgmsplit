# towave-j: Program to record channels from chiptune files

`towave-j` is an improved fork of `towave rel1` by icesoldier (source code of rel2 and rel3 were never uploaded).

## Changes

- Bugfix: If track 1 begins in silence, its silence was skipped. (Also found in binary-only `towave rel3`)
- CMakeLists.txt with static linking to <https://bitbucket.org/mpyne/game-music-emu> (more accurate YM2612 emulation)
- Command-line arguments for duration and track number (both optional)
- Fadeout (8 seconds, hard-coded by libgme)

## Planned

- Ability to accurately dump NSF files with nonlinear mixing
- More accurate NSF emulation (using nsfplay core?)

## Usage

towave-j will be used in <https://github.com/corrscope/corrscope>, a multi-channel oscilloscope program with "intelligent" correlation triggering and real-time playback.
