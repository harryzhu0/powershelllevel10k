# PowerShellLevel10K

If you don't know, I like ricing. You can customise your desktop, make stuff pretty, and have something you can stand looking at for twenty minutes straght while trying to debug what the hell is wrong with your computer. Unfortunately to all the windows users out there (me included), we have to look at this tragic piece of verbosity we know as Powershell. Hell, the only interpretation of "Power" that might be accurate is that said piece of verbosity is backed up by Microsoft.

As a ricer, I far prefer it when my terminal looks like a terminal and not a job application. So I built **PowerShellLevel10K** - if I'm staring at my mistakes for an hour then i might as well stare at it while keeping my eyes partially intact. 

All "jokes" aside, this software (hopefully) runs on both windows and linux, although as of now it only works in my virtual machine since windows include paths are almost as abominal as the powershell default prompt. This is still a work in progress, as of now configuring it does nothing besides install it.

## Current Known Issues
1. ANSI escape codes don't work well in older versions of Powershell. This means that currently, this only works in powershell 7.
2. ncurses does not render on some devices, including the codespaces. PDcurses include paths are broken on Windows, so it's very difficult to get that working
3. My motivation
4. The weird parser thingy doesn't work, so I'm reverting back to pure C
5. It only works on Linux because the windows function does nothing

## To be done
1. Add in automatic terminal colour mode detection
2. Fix the parser thingy
3. Fix the powershell code
4. Add in configurable prompt beyond this
5. Some more status stuff
