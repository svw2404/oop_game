# CLAUDE.md - OOP Game Project Instructions

Before doing any non-trivial work in this repo, read this project context note first:

`C:\Users\user\Documents\Obsidian Vault\03_Projects\OOP Game - Fireboy Watergirl\OOP Game - Fireboy Watergirl - Master Context.md`

This repo is a sensitive Fireboy/Watergirl-style C++ OOP game where small collision or slope changes can easily make gameplay worse.

Core rules:
- Level 1 is the trusted baseline.
- Prefer fixing Level 2 or Level 3 geometry locally before changing shared collision logic.
- Do not casually retune slope tolerances.
- Keep local `SetSize` compatibility implementations unless PTSD truly replaces them.
- Read the current repo state directly before making assumptions.

If the task mentions:
- OOP game
- Fireboy/Watergirl
- LavaGirl/WaterBoy
- Level 1, Level 2, or Level 3
- slopes
- hidden bridge
- hanging platform
- background2
- Block 6

then read the Obsidian note above first and use it as the project memory source.
