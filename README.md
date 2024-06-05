# Mako IR
* JUCE VST3 Guitar Speaker Sim.
* Tested on Windows only.
* Written in Visual C++ 2022.
* Written for new programmers, not complicated.
* Version: 1.00
* Posted: June 5, 2024

VERSION
------------------------------------------------------------------
1.00 - Initial release.

DISCLAIMER
------------------------------------------------------------------  
This VST was written in a very simple way. No object defs, etc. 
Just simple code for people who are not programmers but want to 
dabble in the magic of JUCE VSTs.

If you are new to JUCE code, the Mako Thump VST may be a better
starting place. Or be brave and try this.
       
SUMMARY
------------------------------------------------------------------
A Juce/C++ VST3 written to simulate a speaker cabinet. 

![Demo Image](docs/assets/makoir_demo_01.png)

# THEORY OF OPERATION<br />
This VST is designed to simulate a speaker cabinet. It uses standard Impulse Response (IR) files.
An IR is a pulse that defines the frequency reponse of a system (speaker).

The IR files this VST works with are:
- A wave file that has a pulse 1024 or 2048 samples in length.
- Mono files.



<br />

