#ifndef SCSERIAL_H
#define SCSERIAL_H

#include <QString>
#include <QSerialPortInfo>
#include <QSerialPort>

class SCSerial
{
public:
    SCSerial(QString portName);
    int open();
    int send(QString command);

private:
    QSerialPort serial;
};

#endif // SCSERIAL_H
