The Incredibles: Rise of the Underminer (GameCube)
=============

A work-in-progress matching decompilation of *The Incredibles: Rise of the Underminer* for the GameCube (USA,
`GIQE78`), built on Heavy Iron's EvilEngine. Much of the engine is shared with the
[Ratatouille prototype decomp](https://github.com/seilc/rat), which this project ports matched code from.

This repository does **not** contain any game assets or assembly whatsoever. An existing copy of the game is required.

Supported versions:

- `GIQE78` - Rise of the Underminer (USA), retail `main.dol`

Building
========

- Install Python 3 and ninja (on Linux/macOS also [wibo](https://github.com/decompals/wibo) or wine; `configure.py`
  downloads wibo automatically).
- Copy `sys/main.dol` from the disc to `orig/GIQE78/sys/main.dol` (SHA-1 `ef1087df2c1f04bf820b975483adf0d0d3f4d8bf`).
- Run `python configure.py`, then `ninja`.

The project is built with [decomp-toolkit](https://github.com/encounter/decomp-toolkit) on the
[dtk-template](https://github.com/encounter/dtk-template) layout; [objdiff](https://github.com/encounter/objdiff)
reads the generated `objdiff.json`.
