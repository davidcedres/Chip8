# Chip 8 emulator made with modern C++

This is my first emulation project, it works correctly but it's got some
performance issues.

# Installation
* Install libsfml* from your distro's repo.
* Install make and a c++11 compiler, clang or g++ will do.
* Clone this repo
* Edit the makefile to match your compiler, it uses clang by default.
* Run `make` to compile the emulator.
* Run `./app roms/INVADERS`.
* Play with the keys 1 2 3 4 Q W E R A S D F Z X C V

# DevLog

## v1:
My first attemp was to make it with python, I struggled to get it right,
and when it was *finally* done I realized it was really slow,
I blamed python for being an interpreted language and moved to V2.

time invested: a lot.
pros: I learned about bitwise operations and hexadecimal representation.
crazy bug: *damn you 00EE* I wasn't moving to the next instruction when
 returning from a subroutine, calling it again and again and again.

## v2:
I'm more confident with c++ than python, so I migrated my work.
guess what? It's still slow.

time invested: one day

## v3:
Working on it
