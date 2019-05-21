/**
 * \file scserial.h
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

#ifndef SCSERIAL_H
#define SCSERIAL_H

#include <QString>
#include <QSerialPortInfo>
#include <QSerialPort>

class SCSerial
{
public:
    enum ErrorCode {
        NO_ERROR,
        SERIAL_OPEN,
        COMMAND_SEND_TIMEOUT,
        RESPONSE_RCV_TIMEOUT,
        RESPONSE_MISMATCH,
    };

    SCSerial(QString portName);

    /**
     * @brief getErrorString Get String description of SCSerial::ErrorCode.
     *
     * @param code Error code to get String description of.
     *
     * @return String describing code and what may have caused the error.
     */
    static QString getErrorString(ErrorCode code) {
        switch (code) {
        case NO_ERROR:
            return "No error.";
        case SERIAL_OPEN:
            return "Call to serial.open() failed. Bad Port Name or Port Already Open?";
        case COMMAND_SEND_TIMEOUT:
            return "Timed out waiting for serial.write() to complete. Incorrect Serial Port?";
        case RESPONSE_RCV_TIMEOUT:
            return "Timed out waiting for response after command was sent. Incorrect Serial Port?";
        case RESPONSE_MISMATCH:
            return "Received response did not match expected.";
        }

        return "Unknown Error";
    }

    ErrorCode open();
    ErrorCode send(QString command, QString response, int fullRespDelay=10);

private:
    QSerialPort serial; // Allows for sending command strings and receiving responses.
};

#endif // SCSERIAL_H
