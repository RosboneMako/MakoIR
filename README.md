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

The IR files this VST works with are:
- A wave file that has a pulse 1024 or 2048 samples in length.
- Created at the sample your will be using the VST at. 
- Mono files.

An IR is a pulse that defines the frequency reponse of a system (speaker). They can be created by
sending a single sample pulse thru a system or by doing a frequency sweep of a system. Once tests
are completed you will have a soft rising pulse that rings (oscillates).

SAMPLE RATE  
IRs are normally saved as WAVE files at a standard sample rate, such as 44.1 kHz. But they can be created
at any frequency. If your using the IR in this VST, you will want the IR sample rate to match the rate your
DAW is running in. Normally, a VST would identify the sample rate and resample the file data to match the
DAW sample rate. This VST does not do that because you have comtrol of the resampling process under the
SIZE adjustment.

FILE SIZE  
The files should be very close 1024 or 2048 samples in length. Sometimes the IR creator will provide a much
larger file. This VST is not designed to handle non-standard sizes. This VST does not provide code to search
thru a giant WAVE file looking for the actual IR pulse. I would suggest using and audio editor such as Goldwave
to edit the IR files before use, or you can add code yourself in the VST!

MIXING  
This VST allows you to load two IRs for each channel of audio (Left, Right). The IRs can be mixed in the VST.
For best results the IR pulses in the files should be aligned in time. It is not necessary, but may provide 
a better mix. This VST does not try to align the pulses. Again, that operation should be done in an audio editor before use.  



<br />

