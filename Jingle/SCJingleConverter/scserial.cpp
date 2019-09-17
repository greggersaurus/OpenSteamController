/**
 * \file scserial.cpp
 * \brief This encapsulates the ability to communicate with the Open Steam
 *      Controller Firmware Dev Kit via the USB CDC Serial Port.
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

#include "scserial.h"

#include <QThread>
#include <QDebug>

/**
 * @brief SCSerial::SCSerial Constructor for class to communicate with Open
 *      Steam Controller Dev Kit FW.
 *
 * @param portName Defines which serial port is associated with this class. Use
 *      QSerialPortInfo::availablePorts() to get a list of available port names.
 */
SCSerial::SCSerial(QString portName) {
    serial.setPortName(portName);
}

/**
 * @brief SCSerial::open Attempt to establish connection with Open Steam Controller
 *      Dev Kit FW via this serial port.
 *
 * @return SCSerial::ErrorCode
 */
SCSerial::ErrorCode SCSerial::open() {
    if (!serial.open(QIODevice::ReadWrite)) {
        qDebug() << "serial.open() error: " << serial.error();
        return SERIAL_OPEN;
    }

    // Verify we can communicate with the controller
    const QString ver_cmd = "version\n";
    const QString ver_resp = ver_cmd + "\rOpenSteamController Ver 1.1.\n\r";
    ErrorCode err_code = send(ver_cmd, ver_resp);
    if (err_code != NO_ERROR) {
        // Try once more, just in case there was a partial command in the
        //  Controller FW command buffer or data being output by controller
        //  upon connection
        err_code = send(ver_cmd, ver_resp);
    }

    return err_code;
}

/**
 * @brief SCSerial::send Send a command to the Steam Controller.
 *
 * @param command Command to send to Steam Controller.
 * @param response Expected response from Steam Controller after sending command.
 * @param fullRespDelay Number of ms after initial response to wait for all response
 *      values to be received. Default 10 ms, but may need to extend if action takes
 *      longer than usual (i.e. EEPROM save).
 *
 * @return SCSerial::ErrorCode
 */
SCSerial::ErrorCode SCSerial::send(QString command, QString response, int fullRespDelay) {
    const QByteArray request_data = command.toUtf8();

    serial.write(request_data);

    if (!serial.waitForBytesWritten(250)) {
        qDebug() << "serial.waitForBytesWritten() error: " << serial.error();
        return COMMAND_SEND_TIMEOUT;
    }

    if (!serial.waitForReadyRead(250)) {
        qDebug() << "serial.waitForReadyRead() error: " << serial.error();
        return RESPONSE_RCV_TIMEOUT;
    }

    QByteArray response_data = serial.readAll();
    while (serial.waitForReadyRead(fullRespDelay)) {
        response_data += serial.readAll();
    }

    const QString rcvd_response = QString::fromUtf8(response_data);
    if (rcvd_response != response) {
        qDebug() << "expected response = " << response;
        qDebug() << "rcvd_response = " << rcvd_response;
        return RESPONSE_MISMATCH;
    }

    qDebug() << command;

    // Pause after each command to avoid overflowing NXP's faulty USB CDC UART stack
    // This can be removed if USB CDC UART stack issues are ever fixed
//TODO: how much can we decrease this?
    QThread::msleep(5);

    return NO_ERROR;
}
