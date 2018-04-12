# Neemscript
I wanted this to have syntax like Batch Files but with a few additions that they lack  
The goal was to make it really fast and `easy to read` or at least `only a single thing happens per line`  
I also want it to be easily embedded in other programs as a scripting language and possibly also able to be on used baremetal systems if possible

You can have a look in your browser with the javascript version (first prototype) or you can compile your own interpreter
There's a makefile that should `just work` by default; it just uses your system gcc... Well, it would be your "cc"

Make sure to check out [the wiki](https://github.com/505e06b2/neemscript/wiki) for the documentation!  

Have a look at [the pseudo-benchmarking script](examples/counting/counting.neem) for an example! 

## [Test Neemscript in your browser](https://505e06b2.github.io/neemscript/) via the power of [Emscripten](http://emscripten.org)!