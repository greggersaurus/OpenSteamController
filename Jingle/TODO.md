# [Jingle](./) TODO

The functionality for the customizing Jingles is in a good place at this point. 
 The priorities I have for this project fall more under the category of niceties.

1. Having the SCJingleConverter take compressed MusicXML files would be nice. Especially since musescore.com allows you to download this format directly.
2. Port the SCJingleConverter application to Python. This could lead to an easier path of creating a distributable executables for all OS platforms, while giving me more experience with Python.
3. Explore the possibility of other ways to get Jingle Data into EEPROM. Maybe there is an unknown USB command in the official firmware that could be leveraged? The hope is to simplify the customization process so that users do not need to change the Steam Controller firmware. 


# Notes

Below are less verbose notes on items I would like to dig into. These are a bit
 terse and may not be worded well. Please ask if you have any questions:

* Explore ideas to better explain GUI usage
    * More details in README.md (with pictures?)
    * Tool tips in GUI itself?

* Port SCJingleConverter to Python 
    * Easier to install/distribute?
    * Access to more libraries, etc. which might make adding new features easier (i.e. compressed MusicXML?)
* Create standalone executable for all platforms? (worth the trouble?)
    * macOS: Look into https://doc.qt.io/archives/qt-4.8/deployment-mac.html
    * Windows: Look into https://wiki.qt.io/Build_Standalone_Qt_Application_for_Windows
    * Linux: This help any https://stackoverflow.com/questions/45423815/building-standalone-qt-linux-executable?
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
        * Need functionality to navigate .mxl to find where compressed .musicxml data exists...
        * See MuseScore source (mscore/importxml.cpp) for example
* Explore idea of different intensity/duty cycle for different Notes
    * Can we pull anything from .musicxml to indicate this?
* Explore idea of having a trimming method that is more fine grain than by Measure
    * Think through scenario of melody ending in middle of Measure
    * Currently user can use MuseScore to trim a Composition
* Experiment with lowering scserial command delay time
    * Maybe we do need a delay by default since we are waiting for response?
