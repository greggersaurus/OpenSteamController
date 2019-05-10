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

#include "scserial.h"

class Composition
{
public:
    enum ErrorCode {
        NO_ERROR,
        FILE_OPEN,
        XML_PARSE,
        CMD_ERR,
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
        }

        return "Unknown Error";
    }

    enum Channel {
        RIGHT,
        LEFT
    };

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

    /**
     * @brief getNumParts Get information on how many parts a user can select
     *      from when configuring a channel.
     *
     * @return Number of parts parsed from XML.
     */
    uint32_t getNumParts() {
        return static_cast<uint32_t>(parts.size());
    }

    int getNumMeasures(); //TODO: should be constant across all parts, right?
    int getNumChords(int partIdx);

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

    int getOctaveAdjust();
    ErrorCode setOctaveAdjust(int adjust);

    ErrorCode configureChan(Channel chan, int partIdx, int measStartIdx, int measEndIdx);

    uint32_t getMemUsage();

private:
    /**
     * @brief The Note struct stores data relating to an
     */
    struct Note {
        Note()
            : frequencies(0)
            , duration(0.f)
        {}
        std::vector<float> frequencies; // Frequency of note in Hz. Could be multiple
            // frequencies in case of chord
        float duration; // Number of beats that Note/Chord lasts for
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
     * @brief The Part struct contains music information for a single channel.
     */
    struct Part {
        Part()
            : measures(0)
        {}
        std::vector<Measure> measures;
    };

    ErrorCode parseXmlNote();
    ErrorCode parseXmlPitch(float& freq);
    QString noteToCmd(const Note& note, Channel chan, uint32_t jingleIdx,
        uint32_t noteIdx, uint32_t chordIdx);

    QString filename; // Filename for musicxml file we are extracting data from
    QXmlStreamReader xml; // Used to parse musixcml file
    std::vector<Part> parts; // Data parsed from musicxml file.

    uint32_t currDivisions; // Current conversion factor for Note duration to Number of beats.
    uint32_t bpm; // Beats per minute. Tempo for playback of compostion.
    uint32_t currPart; // Defines which part is currently being built from parsed XML.
};

#endif // COMPOSITION_H
