# Neemscript
I wanted this to have syntax like Batch Files but with a few additions that they lack  
The goal was to make it really fast and `easy to read` or at least `only a single thing happens per line`  
I also want it to be easily embedded in other programs as a scripting language and possibly also able to be on used baremetal systems if possible

There's a makefile that should `just work` by default; it just uses your system gcc but if you modify the `mingw` lines, you can specify an absolute path to another compiler  