#ifndef COMPOSITION_H
#define COMPOSITION_H

#include <QXmlStreamReader>
#include <QString>

#include <stdint.h>
#include <vector>

class Composition
{
public:
    struct Note {
        Note()
            : frequencies(0)
            , duration(0.f)
        {}
        std::vector<float> frequencies; // Frequency of note in Hz. Could be multiple frequencies in case of chord
        float duration; // Number of beats that Note/Chord lasts for
    };

    struct Measure {
        Measure()
            : notes(0)
        {}
        std::vector<Note> notes;
    };

    struct Part {
        Part()
            : measures(0)
        {}
        std::vector<Measure> measures;
    };

    Composition(QString filename);
    int parse();
    std::vector<QString> getSCCommands();

private:
    int parseNote();
    float parsePitch();

    QString filename;
    QXmlStreamReader xml;
    std::vector<Part> parts;
    uint32_t currDivisions; // Current conversion factor for Note duration to Number of beats.
    uint32_t bpm; // Beats per minute. Tempo for playback of compostion.
    uint32_t currPart; // Defines which part is currently being built from parsed XML.

};

#endif // COMPOSITION_H
