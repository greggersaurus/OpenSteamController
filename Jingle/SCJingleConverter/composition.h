/**
 * \file composition.h
 * \brief This encapsulates a song composition. This includes the capability to
 *      parse a musicxml file and communicate the Notes (or some of the notes)
 *      to the Steam Controller.
 *
 * MIT License
 *
 * Copyright (c) 2019 Gregory Gluszek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef COMPOSITION_H
#define COMPOSITION_H

#include <QXmlStreamReader>
#include <QString>

#include <stdint.h>
#include <vector>
#include <map>

#include "scserial.h"

class Composition
{
public:
    enum ErrorCode {
        NO_ERROR,
        FILE_OPEN,
        XML_PARSE,
        CMD_ERR,
        BAD_IDX,
        NO_NOTES,
    };

    /**
     * @brief getErrorString Get String description of Composition::ErrorCode.
     *
     * @param code Error code to get String description of.
     *
     * @return String describing code.
     */
    static QString getErrorString(ErrorCode code) {
        switch (code) {
        case NO_ERROR:
            return "No error.";
        case FILE_OPEN:
            return "Failed to open musicxml file.";
        case XML_PARSE:
            return "Error parsing musicxml file.";
        case CMD_ERR:
            return "Error sending command to controller.";
        case BAD_IDX:
            return "Specified index out of bouds.";
        case NO_NOTES:
            return "No Notes in specified Channel(s).";
        }

        return "Unknown Error";
    }

    enum Channel {
        RIGHT,
        LEFT
    };

    static const uint32_t EEPROM_HDR_NUM_BYTES = 2 + 2 + 1 + 1 + 2 * 14; // See offset
        // default calculation in function addJingle in jingle_data.c in
        // OpenSteamController FW for additional details. The firmware needs
        // a certain number of bytes in the EEPROM Jingle Data memory for header
        // information (i.e. Magic word

    Composition(QString filename);

    /**
     * @brief getFilename
     *
     * @return Filename for musicxml file specified via Constructor.
     */
    const QString& getFilename() {
        return filename;
    }

    ErrorCode parse();
    ErrorCode download(SCSerial& serial, uint32_t jingleIdx);

    std::vector<QString> getVoiceStrs();
    uint32_t getNumMeasures();

    ErrorCode setMeasStartIdx(uint32_t measStartIdx);
    uint32_t getMeasStartIdx();

    ErrorCode setMeasEndIdx(uint32_t measEndIdx);
    uint32_t getMeasEndIdx();

    /**
     * @brief getBpm
     *
     * @return Return Beats per minute setting for composition;
     */
    uint32_t getBpm() {
        return bpm;
    }

    /**
     * @brief setBpm Adjust BPM setting from what was parsed from file.
     *
     * @param bpm Beats per minute
     *
     * @return None.
     */
    void setBpm(uint32_t bpm) {
        this->bpm = bpm;
    }

    /**
     * @brief setOctaveAdjust Allows for manually adjusting all imported Note
     *  frequencies.
     *
     * @param adjust Adjustment factor to be multiplied by each Note frequency.
     *
     * @return None.
     */
    void setOctaveAdjust(float adjust) {
        octaveAdjust = adjust;
    }

    /**
     * @brief getOctaveAdjust
     *
     * @return The adjustment factor used to change all Note frequencies.
     */
    float getOctaveAdjust() {
        return octaveAdjust;
    }

    ErrorCode setVoice(Channel chan, QString voiceStr);
    const QString& getVoice(Channel chan);

    /**
     * @brief getNoVoiceStr
     *
     * @return String used to indicate Channel is configured to not have a Voice
     *      associated with it.
     */
    static const QString& getNoVoiceStr() {
        static QString str("None");
        return str;
    }

    uint32_t getNumChords(QString voiceStr, uint32_t measStartIdx, uint32_t measEndIdx);

    ErrorCode setChordIdx(Channel chan, uint32_t chordIdx);
    uint32_t getChordIdx(Channel chan);

    uint32_t getMemUsage();

private:
    /**
     * @brief The Note struct stores data relating to an
     */
    struct Note {
        Note()
            : frequencies(0)
            , length(0.f)
        {}
        std::vector<float> frequencies; // Frequency of note in Hz. Could be multiple
            // frequencies in case of chord
        float length; // Number of beats that Note/Chord lasts for
    };

    /**
     * @brief The Measure struct is a grouping of a series of Notes. This allows
     *      Parts to be trimmed in a way that maintains time coherency across
     *      parts.
     */
    struct Measure {
        Measure()
            : notes(0)
        {}
        std::vector<Note> notes;
    };

    /**
     * @brief The Voice struct contains music information for a single channel.
     */
    struct Voice {
        Voice()
            : measures(0)
        {}
        std::vector<Measure> measures;
    };

    ErrorCode parseXmlNote(QXmlStreamReader& xml, const QString& partName);
    ErrorCode parseXmlPitch(QXmlStreamReader& xml, float& freq);

    uint32_t getNumNotes(Channel chan);

    QString noteToCmd(const Note& note, Channel chan, uint32_t jingleIdx,
        uint32_t noteIdx, uint32_t chordIdx);

    QString filename; // Filename for musicxml file we are extracting data from

    uint32_t measCnt; // Counts how many measures have been parsed from XML for the
        // current Part
    std::map<QString, Voice> voices; // A series of sequential Measures
        // (containing Notes) that make up data to be played on an output channel.
        // The QString Key is of the Form "Part{n} Voice{m}" where n is the Part{n} is the
        // part id (with {n} changing for different parts) and Voice{m} designates
        // which voice within the part we are referring to.

    uint32_t currDivisions; // Current conversion factor for Note duration to Number of beats.
    uint32_t bpm; // Beats per minute. Tempo for playback of compostion.

    float octaveAdjust; // Control for manually scaling octave of notes in case
        // user wants a different sound relative to what is imported

    uint32_t measStartIdx; // Defines where Jingle data is configured to start
        // in parsed Part data.
    uint32_t measEndIdx; // Defines where Jingle data is configured to ends
        // in parsed Part data.
    QString voiceStrL; // Key for accessing Left Channel Voice.
    QString voiceStrR; // Key for accessing Right Channel Voice.
    uint32_t chordIdxR; // Configuraiton for right channel as to what frequency
        // in chord to play.
    uint32_t chordIdxL; // Configuraiton for left channel as to what frequency
        // in chord to play.
};

#endif // COMPOSITION_H
