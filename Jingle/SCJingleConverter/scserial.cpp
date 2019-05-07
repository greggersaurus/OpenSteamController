#include "scserial.h"

#include <QThread>

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

    // Pause after each command to avoid overflowing NXP's faulty USB CDC UART stack
    QThread::msleep(100);

    return 0;
}
