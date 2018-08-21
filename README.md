# towave-j: Program to record channels from chiptune files

`towave-j` is a fork of `towave rel1` by icesoldier (source code of rel2 and rel3 were never uploaded).

## Changes

- Bugfix: If track 1 begins in silence, its silence was skipped. (Also found in binary-only `towave rel3`)
- CMakeLists.txt with static linking to <https://bitbucket.org/mpyne/game-music-emu>

## Planned
- Command-line arguments for time (optionally track number)
- Fadeout (5 seconds, can be changed via command line)

## Usage

towave-j will be used in <https://github.com/corrscope/corrscope>, a multi-channel oscilloscope program with "intelligent" correlation triggering and real-time playback.
