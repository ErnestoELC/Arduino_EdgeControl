/*
  This file is part of the Arduino Outdoor Carrier library.
  Copyright (C) 2017  Arduino AG (http://www.arduino.cc/)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "OutdoorCarrier_SolidStateRelay.h"

OutdoorCarrier_SolidStateRelayClass::OutdoorCarrier_SolidStateRelayClass()
    : _relaySPI(P1_4, digitalPinToPinName(CMD_TRIAC_1), P1_5)
    , _relayPkt()
{
}

void OutdoorCarrier_SolidStateRelayClass::begin()
{
    _relaySPI.begin();

    pin_size_t channels[] { RELAY_CH01, RELAY_CH02, RELAY_CH03, RELAY_CH04 };

    for (auto ch : channels)
        off(ch);
}

bool OutdoorCarrier_SolidStateRelayClass::on(pin_size_t address)
{
    return doRelayCommand(address, TS13102_COMMAND_ON_IMMEDIATE_DITHERING);
}

bool OutdoorCarrier_SolidStateRelayClass::off(pin_size_t address)
{
    return doRelayCommand(address, TS13102_COMMAND_OFF_IMMEDIATE);
}

bool OutdoorCarrier_SolidStateRelayClass::poll(pin_size_t address)
{
    return doRelayCommand(address, TS13102_COMMAND_POLL);
}

bool OutdoorCarrier_SolidStateRelayClass::doRelayCommand(pin_size_t address, TS13102_COMMANDS command)
{
    switch (address) {
    case RELAY_CH01:
    case RELAY_CH02:
    case RELAY_CH03:
    case RELAY_CH04:
        _relayPkt.setAddress(address);
        _relayPkt.setCommand(command);
        doSPITransfer();
        break;
    default:
        return false;
    }

    return true;
}

void OutdoorCarrier_SolidStateRelayClass::doSPITransfer()
{
    uint8_t data[sizeof(TS13102Packet)] { 0 };
    auto len = _relayPkt.getBytes(data);

    _relaySPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    _relaySPI.transfer((void*)data, len);
    _relaySPI.endTransaction();
}

OutdoorCarrier_SolidStateRelayClass Relay {};