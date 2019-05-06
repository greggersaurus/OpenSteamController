#include "composition.h"

#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <cmath>

Composition::Composition(QString filename)
    :filename(filename)
{

}

int Composition::parse() {
    QFile file(filename);

    qDebug() << "parse\n";

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        //TODO: add error string to be querired on failure
        return -1;
    }

    xml.setDevice(&file);


    while (xml.readNext() != QXmlStreamReader::EndDocument) {

        if (xml.name() == QLatin1String("note")) {
            parseNote();
        }
    }

    // Handle Part
        // Handle Measure
                // Handle Note

    return 0;
}

int Composition::parseNote() {

    int duration = 0;
    int frequency = 0;

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
                duration = xml.text().toInt();
            } else if (xml.name() == QLatin1String("chord")) {

            }
        }

    }

    qDebug() << "Note: freq = " << frequency << " dur = " << duration;

    return 0;
}

int Composition::parsePitch() {
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

    return static_cast<int>(round(freq));
}
