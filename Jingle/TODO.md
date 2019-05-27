# TODO

These items relate to improving the experience of customizing Jingles
 for the Steam Controller:

* Clean up, commit and update TODOs regarding haptic.c changes
    * Update firmware.bin stored in Jingle/bin

* Add executables for SCJingleConverter to directory
    * Mac, Linux, Windows
* Exploure ideas to better explain GUI usage
    * Create walkthrough video on customizing Jingles and uploaded to youtube
    * More details in README.md (with pictures?)
    * Tool tips?

* Explore tests and results they produce
    * How well does Open Steam Controller FW mimic official firmware?
    * If issues exist in official FW, explore if/when SCJingleConverter can compensate for them 
        * Maybe design of Note structure simply has these properties...
        * Should count out Note lengths and make sure certain Notes align across channels before download?
        * Should make sure total duration of each channel is same before download?
    * Saving example of Song configuration that brought up these questions as regression test case
        * Bug: Something off with Channels lining up sometimes...
            * Mario Maker 2 with Measure 2 - 6 and Part 1 Voice 1 and Part 1 Voice 5
            * Seems to be a drift over time issue... round seems to have improved it
* Make child SCSerial class for case of not having controller connected?
    * Allows user to select method that writes commands out to file for manual input later??
* Add window to show serial communications?
    * Gives user idea of what we are doing when Jingles are being updated on controller
* Accept compressed musicXML (.mxl) as input?
    * Would mean one less step when using https://musescore.com/sheetmusic (i.e. no .mxl to musicxml conversion)
    * Hooks are in GUI for adding this
    * However, .mxl file format is more complex than just using zlib to decompress...
        * Need functionality to navigate .mxl to find where comprssed .musicxml data exists...
        * See MuseScore source (mscore/importxml.cpp) for example
* Explore idea of different intensity/duty cycle for different Notes
    * Can we pull anything from .musicxml to indicate this?
* Exploure idea of having a trimming method that is more fine grain than by Measure
    * Think through scenario of melody ending in middle of Measure
    * Currently user can use MuseScore to trim a Composition
* Experiment with lowering scserial command delay time
    * Maybe we do need a delay by default since we are waiting for response?
