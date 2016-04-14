# Pawpy

Threaded Python utility plugin for SA:MP - unifying two of my favourite languages! Run threaded Python scripts from within a SA:MP script.

## What?

Run Python modules from within a Pawn script on a separate thread. Crunch numbers, do networking, large file operations and much more without halting the Pawn AMX.

This isn't a full SA:MP API in Python, that would require upkeep and I find those projects quite pointless to build on.

## Why?

Python is a great language. Flexible, quick to write and very fast. Some jobs can be written, tested and executed before you've hit your first Pawn compile!

Utilise Python sockets for networking (goodbye sockets.inc), SQL connections for databases (goodbye MySQL.dll), email and file transfer, machine learning and of course not forgetting the massive library of Python modules on the internet.

## How?

When called, a new thread is created to run the module which then drops the result onto a stack when it's finished. ProcessTick grabs the stack data and calls the correct AMX callback. It's not threadsafe yet since there are no mutexes but the underlying framework works. The first few code commits can actually be used to build any threaded SA:MP plugin since the Python stuff wasn't added until later.

It's a pretty basic plugin and could be very easily adapted to call scripts in any language (or just system calls) including JavaScript, Ruby, Perl, etc.

### Talking of system calls, why not just use exec?

The use of python.h and integration instead of a simple system call is so that more detailed information about the module can be get and set via the plugin. It's also slightly faster and threaded execution can be controlled more.

## When?

I wrote the first commit in a morning. Shouldn't take long to finsh the rest of the project and test it. The main objectives now are thread safety, Linux compatibility and integrating Python features.
