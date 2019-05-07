#include "composition.h"

#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <cmath>

Composition::Composition(QString filename)
    : filename(filename)
    , xml()
    , parts(0)
    , currDivisions(1)
    , bpm(100)
    , currPart(0)
{
}

int Composition::parse() {
    QFile file(filename);

    parts.clear();
    parts.push_back(Part());
    currPart = 0;

    qDebug() << "parse";

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        //TODO: add error string to be querired on failure
        return -1;
    }

    xml.setDevice(&file);

    while (xml.readNext() != QXmlStreamReader::EndDocument) {

        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("note")) {
                parseNote();
            } else if (xml.name() == QLatin1String("part")) {

            } else if (xml.name() == QLatin1String("backup")) {

            } else if (xml.name() == QLatin1String("measure")) {

            } else if (xml.name() == QLatin1String("per-minute")) {
                xml.readNext();
                bpm = xml.text().toUInt();
            } else if (xml.name() == QLatin1String("divisions")) {
                xml.readNext();
                currDivisions = xml.text().toUInt();
            }
        }
    }

    qDebug() << "parse done";


    return 0;
}

//TODO: will parameters to specify how and what to convert from all possible parts
std::vector<QString> Composition::getSCCommands() {

    std::vector<QString> cmds(0);

    QString cmd;

    int note_cnt = 0;
    for (uint32_t parts_idx = 0; parts_idx < parts.size(); parts_idx++) {
        Part& part = parts[parts_idx];

        for (uint32_t meas_idx = 0; meas_idx < part.measures.size(); meas_idx++) {
            Measure& meas = part.measures[meas_idx];
            for (uint32_t notes_idx = 0; notes_idx < meas.notes.size(); notes_idx++) {
                Note& note = meas.notes[notes_idx];
                cmd = QString("jingle note 0 right ") + QString::number(note_cnt) + QString(" 128 ") +
                        QString::number(note.frequencies[0]) + QString(" ") +
                        QString::number(note.duration * 360 * 1000 / bpm) +
                        QString("\r\n");
                note_cnt++;
                cmds.push_back(cmd);
            }
        }
    }

    cmd = QString("jingle add 0 0 ") + QString::number(note_cnt) + QString("\r\n");
    cmds.insert(cmds.begin(), cmd);
    cmd = QString("jingle clear\r\n");
    cmds.insert(cmds.begin(), cmd);

    return cmds;
}

int Composition::parseNote() {

    float duration = 0.f;
    float frequency = 0.f;
    bool isChord = false;

    // Check that we are actually at the beginning of an XML note token
    if (xml.name() != QLatin1String("note") || xml.tokenType() != QXmlStreamReader::StartElement) {
        return -1;
    }

    // Read all child tokens of note token
    while(1) {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QLatin1String("note")) {
            break;
        }

        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == QLatin1String("pitch")) {
                frequency = parsePitch();
            } else if (xml.name() == QLatin1String("duration")) {
                xml.readNext();
                duration = static_cast<float>(xml.text().toUInt()) / currDivisions;
            } else if (xml.name() == QLatin1String("chord")) {
                isChord = true;
            }
        }

    }

    Part& part = parts[currPart];

    if (part.measures.size() < 1) {
        part.measures.push_back(Measure());
    }

    Measure& meas = part.measures.back();

    if (isChord) {
        if (meas.notes.size() < 1) {
            //TODO: error out if we received a chord and no note exists for the current measure...
        }

        Note& note = meas.notes.back();

        //if (note.duration != duration) {
            // TODO: error if duration does not match for chord
        //}

        note.frequencies.push_back(frequency);
    } else {
        Note note;
        note.frequencies.push_back(frequency);
        note.duration = static_cast<float>(duration) / static_cast<float>(currDivisions);
        meas.notes.push_back(note);
    }

    return 0;
}

float Composition::parsePitch() {
    QChar step = 0;
    int alter = 0;
    int octave = 0;
    double freq = 0;

    if (xml.name() != QLatin1String("pitch") || xml.tokenType() != QXmlStreamReader::StartElement) {
        return -1;
    }

    // Read all child tokens of pitch token
    while(1) {
        xml.readNext();

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
        // TODO: error
    }

    const double twelth_root_of_two = 1.059463094359;
    double factor = 1;
    for (int cnt = 0; cnt < num_half_steps; cnt++) {
        factor *=  twelth_root_of_two;
    }

    const double C_0_FREQ = 16.35;
    freq = C_0_FREQ * factor;

    return static_cast<float>(freq);
}
