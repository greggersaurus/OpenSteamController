#ifndef COMPOSITION_H
#define COMPOSITION_H

#include <QXmlStreamReader>
#include <QString>

class Composition
{
public:
    Composition(QString filename);
    int parse();
private:
    int parseNote();
    int parsePitch();
    QString filename;
    QXmlStreamReader xml;
};

#endif // COMPOSITION_H
