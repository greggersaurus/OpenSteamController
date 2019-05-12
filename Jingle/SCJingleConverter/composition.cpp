/**
 * \file composition.cpp
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

#include "composition.h"

#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <cmath>

/**
 * @brief Composition::Composition Constructor for class to help parse musicxml
 *      file and communicate Jingle Data to Steam Controller.
 *
 * @param filename musicxml filename containing music data.
 */
Composition::Composition(QString filename)
    : filename(filename)
    , xml()
    , parts(0)
    , currDivisions(1)
    , bpm(100)
    , currPart(0)
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

    parts.clear();
    currPart = 0;

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        //TODO: add error string to be querired on failure
        return FILE_OPEN;
    }

    xml.setDevice(&file);

    while (xml.readNext() != QXmlStreamReader::EndDocument) {

        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("note")) {
                ErrorCode code = parseXmlNote();
                if (code != NO_ERROR) {
                    qDebug() << "parseXmlNote() failed. Error: " << getErrorString(code);
                    return code;
                }
            } else if (xml.name() == QLatin1String("backup")) {
                ErrorCode code = parseXmlBackup();
                if (code != NO_ERROR) {
                    qDebug() << "parseXmlBackup() failed. Error: " << getErrorString(code);
                    return code;
                }
            } else if (xml.name() == QLatin1String("measure")) {
                // Double check that all specified backups were seen through
                while(backups.size()) {
                    if (backups.top() != 0) {
                        qDebug() << "Reached beginning of measure with " << backups.size() << " backups "
                            "and top having value of " << backups.top();
                        return XML_PARSE;
                    }
                    backups.pop();
                    currPart = prevParts.top();
                    prevParts.pop();
                }

                // Add new measures for all parts below current
                for (uint32_t parts_idx = currPart; parts_idx < parts.size(); parts_idx++) {
                    Part& part = parts[parts_idx];

                    part.measures.push_back(Measure());
                }

            } else if (xml.name() == QLatin1String("per-minute")) {
                xml.readNext();
                bpm = xml.text().toUInt();
            } else if (xml.name() == QLatin1String("divisions")) {
                xml.readNext();
                currDivisions = xml.text().toUInt();
            }
        } else if (xml.tokenType() == QXmlStreamReader::EndElement) {
            if (xml.name() == QLatin1String("part")) {
                // Double check that all specified backups were seen through
                while(backups.size()) {
                    if (backups.top() != 0) {
                        qDebug() << "Reached end of part with " << backups.size() << " backups "
                            "and top having value of " << backups.top();
                        return XML_PARSE;
                    }
                    backups.pop();
                    currPart = prevParts.top();
                    prevParts.pop();
                }

                currPart++;
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

    uint32_t duty_cydle = 128;
    uint32_t frequency = 0;
    if (chordIdx < note.frequencies.size()) {
        frequency = static_cast<uint32_t>(note.frequencies[chordIdx]);
    } else {
        qDebug() << "warning: chordIdx " << chordIdx << " out of range for note.frequencies.size() = " << note.frequencies.size();
    }

    //TODO: adjust frequency based on octave adjust setting

    uint32_t duration_ms = static_cast<uint32_t>(note.length * 60 * 1000 / bpm);

    QString cmd = QString("jingle note ") + QString::number(jingleIdx) + QString(" ") +
            chan_str + QString(" ") +
            QString::number(noteIdx) + QString(" ") +
            QString::number(duty_cydle) + QString(" ") +
            QString::number(frequency) + QString(" ") +
            QString::number(duration_ms) + QString("\n");

    return cmd;
}

/**
 * @brief Composition::download Download the Jingle data for each channel
 *      to the Controller via the providied serial port. This assumes that
 *      the musicxml has been successfully parsed and that the channels
 *      have been configured appropriately.
 *
 * @param serial Allows for communicating with Controller.
 * @param jingleIdx Defines which Jingle index the Jingle Data will exist under.
 *
 * @return Composition::ErrorCode
 */
Composition::ErrorCode Composition::download(SCSerial& serial, uint32_t jingleIdx) {
    // TODO: bounds check jingleIdx

    SCSerial::ErrorCode serial_err_code = SCSerial::NO_ERROR;
    QString cmd;
    QString resp;

    cmd = "jingle clear\n";
    resp = cmd + "\rJingle data cleared successfully.\n\r";
    serial_err_code = serial.send(cmd, resp);
    if (serial_err_code != SCSerial::NO_ERROR) {
        qDebug() << "serial.send() Error String: " << SCSerial::getErrorString(serial_err_code);
        return CMD_ERR;
    }

    // TODO: part selection and measure range should be based on configuration settings...
    const int parts_idx = 0;

    int num_notes = 0;

    if (parts_idx >= parts.size()) {
        qDebug() << "Not enough parts";
        return NO_ERROR;
    }

    Part& part = parts[parts_idx];

    for (uint32_t meas_idx = 0; meas_idx < part.measures.size(); meas_idx++) {
        Measure& meas = part.measures[meas_idx];
        num_notes += meas.notes.size();
    }

    cmd = "jingle add ";
    cmd += QString::number(num_notes) + QString(" ");
    cmd += QString::number(num_notes) + QString("\n");
    resp = cmd + "\rJingle added successfully.\n\r";
    serial_err_code = serial.send(cmd, resp);
    if (serial_err_code != SCSerial::NO_ERROR) {
        qDebug() << "serial.send() Error String: " << SCSerial::getErrorString(serial_err_code);
        return CMD_ERR;
    }

    uint32_t note_cnt = 0;

    for (uint32_t meas_idx = 0; meas_idx < part.measures.size(); meas_idx++) {
        Measure& meas = part.measures[meas_idx];
        for (uint32_t notes_idx = 0; notes_idx < meas.notes.size(); notes_idx++) {
            //TODO: note should be pulled based on channel...
            Note& note = meas.notes[notes_idx];

            cmd = noteToCmd(note, RIGHT, jingleIdx, note_cnt, 0);
            resp = cmd + "\rNote updated successfully.\n\r";
            serial_err_code = serial.send(cmd, resp);
            if (serial_err_code != SCSerial::NO_ERROR) {
                qDebug() << "serial.send() Error String: " << SCSerial::getErrorString(serial_err_code);
                return CMD_ERR;
            }
            cmd = noteToCmd(note, LEFT, jingleIdx, note_cnt, 0);
            resp = cmd + "\rNote updated successfully.\n\r";
            serial_err_code = serial.send(cmd, resp);
            if (serial_err_code != SCSerial::NO_ERROR) {
                qDebug() << "serial.send() Error String: " << SCSerial::getErrorString(serial_err_code);
                return CMD_ERR;
            }
            note_cnt++;
        }
    }

    return NO_ERROR;
}

/**
 * @brief Composition::parseXmlBackup Parse backup token from xml. This assumes xml is at
 *      desired backup token to be parsed.
 *
 * @return Composition::ErrorCode
 */
Composition::ErrorCode Composition::parseXmlBackup() {
    uint32_t duration = 0;

    // Check that we are actually at the beginning of an XML backup token
    if (xml.name() != QLatin1String("backup") || xml.tokenType() != QXmlStreamReader::StartElement) {
        qDebug() << "XML is not at backup Start Element. XML Error String:" << xml.errorString();
        return XML_PARSE;
    }

    // Read all child tokens of backup token
    while(1) {
        xml.readNext();

        // Exit when all child tokens of backup token have been read
        if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QLatin1String("backup")) {
            break;
        }

        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("duration")) {
                xml.readNext();
                duration = static_cast<float>(xml.text().toUInt());
            }
        }
    }

    if (duration == 0) {
        qDebug() << "0 valued duration within backup token encountered";
        return XML_PARSE;
    }

    backups.push(duration);
    prevParts.push(currPart);
    currPart++;

    return NO_ERROR;
}

/**
 * @brief Composition::parseXmlNote Parse note token from xml. This assumes xml is at
 *      desired note token to be parsed.
 *
 * @return Composition::ErrorCode
 */
Composition::ErrorCode Composition::parseXmlNote() {
    uint32_t raw_xml_duration = 0;
    float length = 0.f;
    float frequency = 0.f;
    bool isChord = false;

    // Check that we are actually at the beginning of an XML note token
    if (xml.name() != QLatin1String("note") || xml.tokenType() != QXmlStreamReader::StartElement) {
        qDebug() << "XML is not at note Start Element. XML Error String:" << xml.errorString();
        return XML_PARSE;
    }

    // Check if we should pop back up to part because we have covered duration we backed up via this note
    if (backups.size()) {
        if (backups.top() == 0) {
            backups.pop();
            currPart = prevParts.top();
            prevParts.pop();
        }
    }

    // Read all child tokens of note token
    while(1) {
        xml.readNext();

        // Exit when all child tokens of note token have been read
        if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QLatin1String("note")) {
            break;
        }

        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("pitch")) {
                ErrorCode code = parseXmlPitch(frequency);
                if (code != NO_ERROR) {
                    qDebug() << "parsePitch() failed. Error: " << getErrorString(code);
                    return code;
                }
            } else if (xml.name() == QLatin1String("duration")) {
                xml.readNext();
                length = static_cast<float>(xml.text().toUInt()) / currDivisions;
                raw_xml_duration = static_cast<float>(xml.text().toUInt());
            } else if (xml.name() == QLatin1String("chord")) {
                isChord = true;
            }
        }

    }

    if (currPart >= parts.size()) {
        parts.push_back(Part());
    }

    Part& part = parts[currPart];

    if (part.measures.size() < 1) {
        part.measures.push_back(Measure());
    }

    Measure& meas = part.measures.back();

    if (isChord) {
        if (meas.notes.size() < 1) {
            qDebug() << "Received chord, but no note exists for the current measure...";
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
        note.length = static_cast<float>(length);
        meas.notes.push_back(note);
        meas.xmlDurationSum += raw_xml_duration;

        if (backups.size()) {
            uint32_t backup_dur = backups.top();
            if (raw_xml_duration > backup_dur) {
                qDebug() << "Remaining backup duration (" << backup_dur << ") is less than"
                    " current Note duration (" << raw_xml_duration << ")";
                return XML_PARSE;
            }

            // Update backup duration counter
            backups.pop();
            backups.push(backup_dur - raw_xml_duration);
        }
    }

    return NO_ERROR;
}

/**
 * @brief Composition::parseXmlPitch Parse pitch token from xml. This assumes xml is at
 *      desired pitch token to be parsed.
 *
 * @param[out] freq Frequency in Hz calculated from musicxml data.
 *
 * @return Composition::ErrorCode
 */
Composition::ErrorCode Composition::parseXmlPitch(float& freq) {
    QChar step = 0;
    int alter = 0;
    int octave = 0;

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
