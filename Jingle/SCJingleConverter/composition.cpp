/**
 * \file composition.cpp
 * \brief This encapsulates a song composition. This includes the capability to
 *      parse a musicxml file and communicate the Notes (or some of the notes)
 *      to the Steam Controller as a Jingle.
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

#include "composition.h"

#include <QFile>
#include <QMessageBox>
#include <QDebug>

#include <cmath>
#include <algorithm>

#include "zlib.h"

/**
 * @brief Composition::Composition Constructor for class to help parse musicxml
 *      file and communicate Jingle Data to Steam Controller.
 *
 * @param filename musicxml filename containing music data.
 */
Composition::Composition(QString filename)
    : filename(filename)
    , currDivisions(1)
    , bpm(100)
    , noteIntensity(128)
    , octaveAdjust(1.f)
    , measStartIdx(0)
    , measEndIdx(0)
    , voiceStrL(Composition::getNoVoiceStr())
    , voiceStrR(Composition::getNoVoiceStr())
    , chordIdxR(0)
    , chordIdxL(0)
{
}

/**
 * @brief Composition::parse Attempt to parse all the Note related data from the
 *      musicxml file specified via the constructor.
 *
 * @return Composition::ErrorCode
 */
Composition::ErrorCode Composition::parse() {
    QFile file(filename);
    QXmlStreamReader xml;

    if (filename.endsWith(".musicxml")) {
        // Open uncompressed musicxml file and connected with xml reader
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qDebug() << "Failed to open file " << filename;
            return FILE_OPEN;
        }

        xml.setDevice(&file);
    } else if (filename.endsWith(".mxl")) {

        // .mxl file format is more complex than originally thought... Leaving some of this
        //   here in case someone find it useful to adding support later...
        return BAD_FILE_TYPE;
        /*
        // Decomporess data and then add data to xml reader
        const int CHUNK_SIZE = 16384;

        // Based on zlib example https://zlib.net/zlib_how.html
        int ret, flush;
        unsigned have;
        z_stream strm;
        unsigned char in[CHUNK_SIZE];
        unsigned char out[CHUNK_SIZE];

        // Allocate inflate state
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;

        ret = inflateInit2(&strm, -MAX_WBITS);
        if (ret != Z_OK) {
            qDebug() << "inflateInit() failed.";
            return DECOMPRESS_FAILURE;
        }

        FILE* source = fopen(filename.toStdString().c_str(), "rb");
        if (!source) {
            qDebug() << "Failed to open file " << filename;
            return FILE_OPEN;
        }

        // Decompress until deflate stream ends or end of file
        do {
            strm.avail_in = static_cast<uInt>(fread(in, 1, CHUNK_SIZE, source));
            if (ferror(source)) {
                inflateEnd(&strm);
                qDebug() << "Error reading chunk from input file.";
                return DECOMPRESS_FAILURE;
            }

            if (strm.avail_in == 0)
                break;

            strm.next_in = in;

            // Run inflate() on input until output buffer not full
            do {
                strm.avail_out = CHUNK_SIZE;
                strm.next_out = out;

                ret = inflate(&strm, Z_NO_FLUSH);
                if (ret != Z_OK) {
                    inflateEnd(&strm);
                    qDebug() << "inflate() failed (" << ret << "): " << strm.msg;
                    return DECOMPRESS_FAILURE;
                }

                have = CHUNK_SIZE - strm.avail_out;
                for (int idx = 0; idx < have; idx++) {
                    printf("%c", out[idx]);
                }
                //TODO: write decompressed data to byte array
            } while (strm.avail_out == 0);
        } while (ret != Z_STREAM_END);

        inflateEnd(&strm);

        xml.addData();
        */
    } else {
        return BAD_FILE_TYPE;
    }

    measCnt = 0;
    voices.clear();

    voiceStrL = Composition::getNoVoiceStr();
    voiceStrR = Composition::getNoVoiceStr();
    measStartIdx = 0;
    measEndIdx = 0;
    chordIdxR = 0;
    chordIdxL = 0;

    bpm = 0;
    octaveAdjust = 1.f;

    uint32_t part_cnt = 0;

    // Read until end of document or error occurs
    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.name() == QLatin1String("note")) {
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                ErrorCode code = parseXmlNote(xml, "Part " + QString::number(part_cnt));
                if (code != NO_ERROR) {
                    qDebug() << "parseXmlNote() failed. Error: " << getErrorString(code);
                    return code;
                }
            }
        } else if (xml.name() == QLatin1String("measure")) {
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                measCnt++;
            }
        } else if (xml.name() == QLatin1String("per-minute")) {
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                xml.readNext();
                if (xml.text().toUInt() > bpm) {
                    bpm = xml.text().toUInt();
                }
            }
        } else if (xml.name() == QLatin1String("divisions")) {
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                xml.readNext();
                currDivisions = xml.text().toUInt();
            }
        } else if (xml.name() == QLatin1String("part")) {
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                measCnt = 0;
                part_cnt++;
            }
        }
    }

    // Check if we exited loop above due to error
    if (xml.hasError()) {
        qDebug() << "QXmlStreamReader detected error: " << xml.errorString();
        return XML_PARSE;
    }

    // Update default configurations
    measEndIdx = getNumMeasures()-1;

    if (!bpm) {
        qDebug() << "No BPM specification found. Defaulting to 100 bpm.";
        bpm = 100;
    }

    // Make sure all Voices have the same number of measures. It is possible that some Voices
    //  have fewer Measures at this point as they may have not been present for every measure
    //  throughout parsing
    for (std::map<QString, Voice>::iterator itr = voices.begin(); itr != voices.end(); itr++) {
        Voice& voice = itr->second;

        while (voice.measures.size() < measCnt) {
            voice.measures.push_back(Measure());
        }
    }

    // Make sure each measure is the same length (adding appropriate length rests to empty Measures)
    for (uint32_t meas_idx = 0; meas_idx <= measEndIdx; meas_idx++) {

        // First find out what the maximum length is across all Voices for this Measure
        float max_meas_len = 0.f;

        for (std::map<QString, Voice>::iterator itr = voices.begin(); itr != voices.end(); itr++) {
            Voice& voice = itr->second;
            Measure& meas = voice.measures[meas_idx];

            float meas_length = 0.f;
            for (uint32_t note_idx = 0; note_idx < meas.notes.size(); note_idx++) {
                Note& note = meas.notes[note_idx];
                meas_length += note.length;
            }

            if (meas_length > max_meas_len) {
                max_meas_len = meas_length;
            }
        }

        // Now fill empty Measures with rests of the max length, and make sure all non-empty measures
        //  match the max length
        for (std::map<QString, Voice>::iterator itr = voices.begin(); itr != voices.end(); itr++) {
            Voice& voice = itr->second;
            Measure& meas = voice.measures[meas_idx];

            if (!meas.notes.size()) {
                // If Measure is empty, add reset Note that is length of all other Measures
                meas.notes.push_back(Note());
                meas.notes[0].length = max_meas_len;
                meas.notes[0].frequencies.push_back(0.f);
            } else {
                // If Measure is not empty, make sure length matchs all other Measures
                float meas_len = 0.f;

                for (uint32_t note_idx = 0; note_idx < meas.notes.size(); note_idx++) {
                    Note& note = meas.notes[note_idx];
                    meas_len += note.length;

                    // Make sure frequencies are in descending order
                    std::sort(note.frequencies.begin(), note.frequencies.end(), std::greater<float>());
                }

                if (fabs(meas_len- max_meas_len) >= 0.01) {
                    qDebug() << "Measure length of " << meas_len << " detected."
                        << "All measures should have length " << max_meas_len;
                    return XML_PARSE;
                }
            }
        }
    }

    return NO_ERROR;
}

/**
 * @brief Composition::parseXmlNote Parse note token from xml. This assumes xml is at
 *      desired note token to be parsed.
 *
 * @param[inout] xml Used for parsing musicxml file.
 * @param[in] partName Used as first part of creating key to access voices.
 *
 * @return Composition::ErrorCode
 */
Composition::ErrorCode Composition::parseXmlNote(QXmlStreamReader& xml, const QString& partName) {
    float length = 0.f;
    float frequency = 0.f;
    bool isChord = false;
    QString voice_id = " Voice ?"; // Used for second part of key to access voices

    // Check that we are actually at the beginning of an XML note token
    if (xml.name() != QLatin1String("note") || xml.tokenType() != QXmlStreamReader::StartElement) {
        qDebug() << "XML is not at note Start Element. XML Error String:" << xml.errorString();
        return XML_PARSE;
    }

    // Read all child tokens of note token
    while(1) {
        xml.readNext();

        // Exit when all child tokens of note token have been read
        if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QLatin1String("note")) {
            break;
        }

        if (xml.name() == QLatin1String("pitch")) {
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                ErrorCode code = parseXmlPitch(xml, frequency);
                if (code != NO_ERROR) {
                    qDebug() << "parsePitch() failed. Error: " << getErrorString(code);
                    return code;
                }
            }
        } else if (xml.name() == QLatin1String("duration")) {
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                xml.readNext();
                length = static_cast<float>(xml.text().toUInt()) / currDivisions;
            }
        } else if (xml.name() == QLatin1String("voice")) {
            if (xml.tokenType() == QXmlStreamReader::StartElement) {
                xml.readNext();
                voice_id = " Voice " + xml.text();
            }
        } else if (xml.name() == QLatin1String("chord")) {
            isChord = true;
        }
    }

    const QString voice_key = partName + voice_id;

    Voice& voice = voices[voice_key];

    // Add Measures if needed so this Note data ends up in the correct place
    //  Empty measures will be filled in with appropriate rests at the end of the parse
    while (voice.measures.size() < measCnt) {
        voice.measures.push_back(Measure());
    }

    Measure& meas = voice.measures.back();

    if (isChord) {
        if (meas.notes.size() < 1) {
            qDebug() << "Received chord, but no Note exists for the current measure...";
            return XML_PARSE;
        }

        Note& note = meas.notes.back();

        if (static_cast<int>(note.length) != static_cast<int>(length)) {
            qDebug() << "Warning: Length not consistent across notes in chord";
        }

        note.frequencies.push_back(frequency);
    } else {
        Note note;
        note.frequencies.push_back(frequency);
        note.length = length;

        meas.notes.push_back(note);
    }

    return NO_ERROR;
}

/**
 * @brief Composition::parseXmlPitch Parse pitch token from xml. This assumes xml is at
 *      desired pitch token to be parsed.
 *
 * @param[inout] xml Used for parsing musicxml file.
 * @param[out] freq Frequency in Hz calculated from musicxml data.
 *
 * @return Composition::ErrorCode
 */
Composition::ErrorCode Composition::parseXmlPitch(QXmlStreamReader& xml, float& freq) {
    QChar step = 0;
    int alter = 0;
    int octave = 0;

    freq = 0.f;

    if (xml.name() != QLatin1String("pitch") || xml.tokenType() != QXmlStreamReader::StartElement) {
        qDebug() << "XML is not at pitch Start Element. XML Error String:" << xml.errorString();
        return XML_PARSE;
    }

    // Read all child tokens of pitch token
    while(1) {
        xml.readNext();

        // Exit when all child tokens of pitch token have been read
        if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QLatin1String("pitch")) {
            break;
        }

        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("step")) {
                xml.readNext();
                step = xml.text()[0];
            } else if (xml.name() == QLatin1String("alter")) {
                xml.readNext();
                alter = xml.text().toInt();
            } else if (xml.name() == QLatin1String("octave")) {
                xml.readNext();
                octave = xml.text().toInt();
            }
        }
    }

    // See http://pages.mtu.edu/~suits/NoteFreqCalcs.html for details on converting notes to frequencies
    const int HALF_STEPS_PER_OCTAVE = 12;

    int num_half_steps = octave * HALF_STEPS_PER_OCTAVE + alter;

    if (step == 'C') {
        num_half_steps += 0;
    } else if (step == 'D') {
        num_half_steps += 2;
    } else if (step == 'E') {
        num_half_steps += 4;
    } else if (step == 'F') {
        num_half_steps += 5;
    } else if (step == 'G') {
        num_half_steps += 7;
    } else if (step == 'A') {
        num_half_steps += 9;
    } else if (step == 'B') {
        num_half_steps += 11;
    } else {
        qDebug() << "Invalid step specification of " << step << " in XML.";
        return XML_PARSE;
    }

    const double twelth_root_of_two = 1.059463094359;
    double factor = 1;
    for (int cnt = 0; cnt < num_half_steps; cnt++) {
        factor *=  twelth_root_of_two;
    }

    const double C_0_FREQ = 16.35;
    freq = static_cast<float>(C_0_FREQ * factor);

    return NO_ERROR;
}

/**
 * @brief Composition::download Download the Jingle data for each Channel
 *      to the Controller via the providied serial port. This assumes that
 *      the musicxml has been successfully parsed and that the channels
 *      have been configured appropriately.
 *
 * @param[in] serial Allows for communicating with Controller.
 * @param jingleIdx Defines which Jingle index the Jingle Data will exist under.
 *
 * @return Composition::ErrorCode
 */
Composition::ErrorCode Composition::download(SCSerial& serial, uint32_t jingleIdx) {
    SCSerial::ErrorCode serial_err_code = SCSerial::NO_ERROR;
    QString cmd;
    QString resp;

    // Bounds check jingleIdx
    if (jingleIdx >= MAX_NUM_COMPS) {
        qDebug() << "Specified jingleIdx is out of bounds " << jingleIdx;
        return BAD_IDX;
    }

    const uint32_t meas_start_idx = getMeasStartIdx();
    const uint32_t meas_end_idx = getMeasEndIdx();

    const uint32_t num_notes_l = getNumNotes(LEFT);
    const uint32_t num_notes_r = getNumNotes(RIGHT);

    // No notes so do nothing
    if (!num_notes_l && !num_notes_r) {
        qDebug() << "No Notes in Right and Left Channel";
        return NO_NOTES;
    }

    cmd = "jingle add ";
    cmd += QString::number(num_notes_r) + QString(" ");
    cmd += QString::number(num_notes_l) + QString("\n");
    resp = cmd + "\rJingle " + QString::number(jingleIdx) + " added successfully.\n\r";
    serial_err_code = serial.send(cmd, resp);
    if (serial_err_code != SCSerial::NO_ERROR) {
        qDebug() << "serial.send() Error String: " << SCSerial::getErrorString(serial_err_code);
        return CMD_ERR;
    }

    uint32_t note_cnt = 0;

    // Add Notes to Left Channel
    if (voices.count(voiceStrL)) {
        for (uint32_t meas_idx = meas_start_idx; meas_idx <= meas_end_idx; meas_idx++) {
            Measure& meas = voices[voiceStrL].measures[meas_idx];
            for (uint32_t notes_idx = 0; notes_idx < meas.notes.size(); notes_idx++) {
                Note& note = meas.notes[notes_idx];

                cmd = noteToCmd(note, LEFT, jingleIdx, note_cnt, chordIdxL);
                resp = cmd + "\rNote updated successfully.\n\r";
                serial_err_code = serial.send(cmd, resp);
                if (serial_err_code != SCSerial::NO_ERROR) {
                    qDebug() << "serial.send() Error String: " << SCSerial::getErrorString(serial_err_code);
                    return CMD_ERR;
                }
                note_cnt++;
            }
        }
    }

    // Add Notes to Right Channel
    if (voices.count(voiceStrR)) {
        note_cnt = 0;

        for (uint32_t meas_idx = meas_start_idx; meas_idx <= meas_end_idx; meas_idx++) {
            Measure& meas = voices[voiceStrR].measures[meas_idx];
            for (uint32_t notes_idx = 0; notes_idx < meas.notes.size(); notes_idx++) {
                Note& note = meas.notes[notes_idx];

                cmd = noteToCmd(note, RIGHT, jingleIdx, note_cnt, chordIdxR);
                resp = cmd + "\rNote updated successfully.\n\r";
                serial_err_code = serial.send(cmd, resp);
                if (serial_err_code != SCSerial::NO_ERROR) {
                    qDebug() << "serial.send() Error String: " << SCSerial::getErrorString(serial_err_code);
                    return CMD_ERR;
                }
                note_cnt++;
            }
        }
    }

    return NO_ERROR;
}

/**
 * @brief Composition::noteToCmd Convert information into command string to send to
 *      Controller via serial.
 *
 * @param[in] note Contains Note data.
 * @param chan Specificies which channel Note will be played on.
 * @param jingleIdx Defines which Jingle index the Jingle Data will exist under.
 * @param noteIdx Defines which Note in the Jingle we are programming.
 * @param chordIdx Defines which Note in chord to use.
 *
 * @return Command string to send to Controller via serial.
 */
QString Composition::noteToCmd(const Note& note, Channel chan, uint32_t jingleIdx,
        uint32_t noteIdx, uint32_t chordIdx) {

    QString chan_str("right");
    if (chan == LEFT) {
        chan_str = "left";
    }

    const uint32_t duty_cydle = noteIntensity;

    uint32_t frequency = static_cast<uint32_t>(note.frequencies.back() * octaveAdjust);
    if (chordIdx < note.frequencies.size()) {
        frequency = static_cast<uint32_t>(note.frequencies[chordIdx] * octaveAdjust);
    }

    const uint32_t duration_ms = static_cast<uint32_t>(round(note.length * 60 * 1000 / bpm));

    QString cmd = QString("jingle note ") + QString::number(jingleIdx) + QString(" ") +
            chan_str + QString(" ") +
            QString::number(noteIdx) + QString(" ") +
            QString::number(duty_cydle) + QString(" ") +
            QString::number(frequency) + QString(" ") +
            QString::number(duration_ms) + QString("\n");

    return cmd;
}

/**
 * @brief Composition::getVoiceStrs
 *
 * @return A vector of Strings representing the different Voices that a Channel can
 *      be configured to play.
 */
std::vector<QString> Composition::getVoiceStrs() {
    std::vector<QString> voice_strs(0);

    for (std::map<QString, Voice>::iterator itr = voices.begin(); itr != voices.end(); itr++) {
        voice_strs.push_back(itr->first);
    }

    return voice_strs;
}

/**
 * @brief getNumMeasures Returns the number of measures in each of the
 *      parts. This allows a user to know how they can trim the Jingle data.
 *
 * @return The number of measure parsed from the musicxml file.
 */
uint32_t Composition::getNumMeasures() {
    return measCnt;
}

/**
 * @brief Composition::setMeasStartIdx Allows for trimming where Jingle Data
 *      starts in parsed data.
 *
 * @param measStartIdx Defines start point of Jingle Data.
 *
 * @return NO_ERROR on success.
 */
Composition::ErrorCode Composition::setMeasStartIdx(uint32_t measStartIdx) {
    if (!voices.size()) {
        qDebug() << "Cannot setMeasStartIdx if there are no Voices.";
        return BAD_IDX;
    }

    if (measStartIdx >= measCnt) {
        qDebug() << "Invalid range measStartIdx " << measStartIdx << " specified.";
        return BAD_IDX;
    }

    this->measStartIdx = measStartIdx;

    return NO_ERROR;
}

/**
 * @brief Composition::getMeasStartIdx
 *
 * @return Where Jingle data is configured to start in parsed data.
 */
uint32_t Composition::getMeasStartIdx() {
    return measStartIdx;
}

/**
 * @brief Composition::setMeasEndIdx Allows for trimming where Jingle Data
 *      ends in parsed data.
 *
 * @param measStartIdx Defines start point of Jingle Data.
 *
 * @return NO_ERROR on success.
 */
Composition::ErrorCode Composition::setMeasEndIdx(uint32_t measEndIdx) {
    if (!voices.size()) {
        qDebug() << "Cannot setMeasEndIdx if there are no Voices.";
        return BAD_IDX;
    }

    if (measStartIdx >= measCnt) {
        qDebug() << "Invalid range measStartIdx " << measStartIdx << " specified.";
        return BAD_IDX;
    }

    this->measEndIdx = measEndIdx;

    return NO_ERROR;
}

/**
 * @brief Composition::getMeasEndIdx
 *
 * @return Where Jingle data is configured to endin parsed data.
 */
uint32_t Composition::getMeasEndIdx() {
    return measEndIdx;
}

/**
 * @brief Composition::setVoice Configure which Voice is associated with a channel
 *
 * @param chan Defines which channel is being configured.
 * @param voiceStr Indicates which voice to associate with given channel.
 *
 * @return Composition::ErrorCode
 */
Composition::ErrorCode Composition::setVoice(Channel chan, QString voiceStr) {
    if (voiceStr != getNoVoiceStr()) {
        if (1 != voices.count((voiceStr))) {
            qDebug() << "Invalid voiceStr " << voiceStr << " in setVoice.";
            return BAD_IDX;
        }
    }

    switch (chan) {
    case RIGHT:
        voiceStrR = voiceStr;
        break;

    case LEFT:
        voiceStrL = voiceStr;
        break;
    }

    return NO_ERROR;
}

/**
 * @brief Composition::getVoice
 *
 * @param chan Defines which channel we are referring to.
 *
 * @return String that is associated with Voice configured for given channel.
 */
const QString& Composition::getVoice(Channel chan) {
    switch (chan) {
    case RIGHT:
        return voiceStrR;

    case LEFT:
        return voiceStrL;
    }
}

/**
 * @brief getNumChords Given a range for a particular Voice this function
 *      checks for the largest Chord. This is done as a user may want both
 *      channels to use the same Voice, but different Notes from Chords that
 *      might be withing that range.
 *
 * @param voiceStr Defines which Voice we are referring to.
 * @param measStartIdx Defines range of Measures to consider.
 * @param measEndIdx Defines range of Measures to consider.
 *
 * @return The size of the largest chord within the specified drange.
 */
uint32_t Composition::getNumChords(QString voiceStr, uint32_t measStartIdx, uint32_t measEndIdx) {
    if (1 != voices.count(voiceStr)) {
        qDebug() << "Invalid voiceStr " << voiceStr << " with count " << voices.count(voiceStr)
            <<  " specified in CompositionGetNumChords";
        return 0;
    }

    const Voice& voice = voices[voiceStr];

    if (measStartIdx >= voice.measures.size() || measEndIdx >= voice.measures.size()) {
        qDebug() << "Invalid range of " << measStartIdx << " to " << measEndIdx <<
            " specified in CompositiongetNumChords. Max is " << voice.measures.size()-1;
        return 0;
    }

    uint32_t max_chord_size = 0;
    for (uint32_t meas_idx = measStartIdx; meas_idx <= measEndIdx; meas_idx++) {
        const Measure& meas = voice.measures[meas_idx];
        for (uint32_t note_idx = 0; note_idx < meas.notes.size(); note_idx++) {
            const Note& note = meas.notes[note_idx];
            if (note.frequencies.size() > max_chord_size) {
                max_chord_size = static_cast<uint32_t>(note.frequencies.size());
            }
        }
    }

    return max_chord_size;
}

/**
 * @brief Composition::setChordIdx Define which frequency to play in
 *      case of Chord Note.
 *
 * @param chan Channel being referred to.
 * @param chordIdx Defines which frequency to play in case of Chord.
 *
 * @return Composition::ErrorCode
 */
Composition::ErrorCode Composition::setChordIdx(Channel chan, uint32_t chordIdx) {
    uint32_t num_chords = 0;

    switch (chan) {
    case RIGHT:
        num_chords = getNumChords(voiceStrR, getMeasStartIdx(), getMeasEndIdx());
        if (chordIdx >= num_chords) {
            qDebug() << "Bad chordIdx " << chordIdx << " specified for Right Channel";
            return BAD_IDX;
        }
        chordIdxR = chordIdx;
        break;

    case LEFT:
        num_chords = getNumChords(voiceStrL, getMeasStartIdx(), getMeasEndIdx());
        if (chordIdx >= num_chords) {
            qDebug() << "Bad chordIdx " << chordIdx << " specified for Left Channel";
            return BAD_IDX;
        }
        chordIdxL = chordIdx;
        break;
    }

    return NO_ERROR;
}

/**
 * @brief Composition::getChordIdx
 *
 * @param chan Defines which output channel is being referred.
 *
 * @return An index referring to which frequency in chords will be played for this channel.
 */
uint32_t Composition::getChordIdx(Channel chan) {
    switch (chan) {
    case RIGHT:
        return chordIdxR;

    case LEFT:
        return chordIdxL;
    }
}

/**
 * @brief Composition::getMemUsage Function for calculating how the much EEPROM
 *      memory the Jingle data from this composition will take up. This
 *      varies based on configuration and is used to make sure we do not try to
 *      write too much, or invalid, Jingle Data to the EEPROM.
 *
 * @return The number of bytes required to store the Jingle data, as currently
 *      configured, in EEPROM of the Controller.
 */
uint32_t Composition::getMemUsage() {
    static const uint32_t NUM_JINGLE_HDR_BYTES = 4;// Number of bytes required for each
            // Jingle to give data on Jingle (i.e. number of Notes per channel)
    static const uint32_t BYTES_PER_NOTE = 6; // Number of bytes required to store
        // a single Note in EEPROM

    uint32_t byte_cnt = NUM_JINGLE_HDR_BYTES;

    const uint32_t num_notes_l = getNumNotes(LEFT);
    const uint32_t num_notes_r = getNumNotes(RIGHT);

    byte_cnt += num_notes_l * BYTES_PER_NOTE;
    byte_cnt += num_notes_r * BYTES_PER_NOTE;

    return byte_cnt;
}

/**
 * @brief Composition::getNumNotes
 *
 * @param chan Indicates which channel being referred to.
 *
 * @return The number of Notes for the given channel given the currently
 *      configured Measure Start and Stop Indices.
 */
uint32_t Composition::getNumNotes(Channel chan) {
    const uint32_t meas_start_idx = getMeasStartIdx();
    const uint32_t meas_end_idx = getMeasEndIdx();

    uint32_t num_notes = 0;

    Voice* voice = nullptr;
    switch (chan) {
    case RIGHT:
        if (voices.count(voiceStrR)) {
            voice = &voices[voiceStrR];
        }
        break;

    case LEFT:
        if (voices.count(voiceStrL)) {
            voice = &voices[voiceStrL];
        }
        break;
    }

    if (voice) {
        if (meas_start_idx >= voice->measures.size() || meas_end_idx >= voice->measures.size()) {
            qDebug() << "Start or End Measure Index out of bounds for Channel in getNumNotes()";
            return 0;
        }

        for (uint32_t meas_idx = meas_start_idx; meas_idx <= meas_end_idx; meas_idx++) {
            Measure& meas = voice->measures[meas_idx];
            num_notes += meas.notes.size();
        }
    }

    return num_notes;
}
