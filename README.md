# Neemscript
I wanted this to have syntax like Batch Files but with a few additions that they lack  
The goal was to make it really fast and `easy to read` or at least `only a single thing happens per line`  
I also want it to be easily embedded in other programs as a scripting language and possibly also able to be on used baremetal systems if possible

There's a makefile that should `just work` by default; it just uses your system gcc... Well, it would be your "cc"  
Neemscript is confirmed to compile on `Windows 7 x86_64` (MinGW & Cygwin), `Xubuntu 14.04 x86` (GCC), `Android 6.0 armhf` (Termux Clang) and `the Web` (Emscripten)

To build the executable, run `make` without a target  
If you want a dynamic library, use `make dlib` or if you want static, use `make slib`  
You can build all 3 with `make all`  

Linking against the dynamic library is easy, you can even use a C compiler:  
```Bash
cd neemscript/cpp
gcc -Iinclude src/main.c -L. -lneem
```

Linking against the static lib is a little more work, a basic commandline would look like:  
```Bash
cd neemscript/cpp
g++ -Iinclude src/main.c -L. -lneem -ldl
```
If compiling for Windows, `-ldl` isn't needed

Make sure to check out [the wiki](https://github.com/505e06b2/neemscript/wiki) for the documentation!  

Have a look at [the pseudo-benchmarking script](examples/counting/counting.neem) for an example! 

## [Test Neemscript in your browser](https://505e06b2.github.io/neemscript/) via the power of [Emscripten](http://emscripten.org)!