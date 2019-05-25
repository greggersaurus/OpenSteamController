# TODO

These items relate to improving the experience of customizing Jingles
 for the Steam Controller:

* Add executables for SCJingleConverter to directory
    * Mac, Linux, Windows
    * External library issues a concern or all statically linked and built into exe?
* More details in README.md on GUI controls and what they are doing
    * Add pictures
* Bug: Something off with Channels lining up sometimes...
    * Mario Maker 2 with Measure 2 - 6 and Part 1 Voice 1 and Part 1 Voice 5
    * Seems to be a drift over time issue... round seems to have improved it
        * Add additional verification step in download?
        * See if it sounds better in with official FW?
* Octave shifting does not seem right...
    * Could be coming back to frequencies not tracking quite right through full range (i.e. use tuner app to verify)
        * Do octave shifted Jingles sound better with official FW?
* Review and see if there are any major TODOs left for GUI functionality
* Comments, comments, comments for SCJingleConverter
    * Take fresh pass through and make sure nothing is too confusing

    
* Create walkthrough video on customizing Jingles and uploaded to youtube
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
