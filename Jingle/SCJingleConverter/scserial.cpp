#include "scserial.h"

#include <QThread>
#include <QDebug>

SCSerial::SCSerial(QString portName)
{
    serial.setPortName(portName);
}

int SCSerial::open() {
    if (!serial.open(QIODevice::ReadWrite)) {
        //TODO: better error reporting (string update?)
        return -1;
    }

    //TODO: use "version" command to verify this is valid serial port

    return 0;
}

int SCSerial::send(QString command) {
    const QByteArray requestData = command.toUtf8();

    serial.write(requestData);

    if (!serial.waitForBytesWritten(1000)) {
        // TODO: better error reporting (string update?)
        return -1;
    }

    // TODO: Check responses (maybe accept parameter for what check string should be?)
    if (serial.waitForReadyRead(1000)) {
        QByteArray responseData = serial.readAll();
        while (serial.waitForReadyRead(10))
            responseData += serial.readAll();

        const QString response = QString::fromUtf8(responseData);
        qDebug() << "response = " << response;
    }

    // Pause after each command to avoid overflowing NXP's faulty USB CDC UART stack
    //TODO: what is the right value for this...
    QThread::msleep(50);

    return 0;
}
