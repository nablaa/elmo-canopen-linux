/*
 * Copyright (C) 2009 Miika-Petteri Matikainen, Tuomas Miettinen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "elmo.h"

int sendEchoMessage(TCan *can)
{
	struct can_frame frame;
	unsigned char data[8] = { 0x53, 0x4e, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
	if (sendPDO2(can, 4, data) < 0) {
		return -1;
	}

	if (receivePDO2(can, &frame) < 0) {
		return -2;
	}

	if (frame.can_id != (can->id | (5 << 7))) return -4; /* RPDO COB-ID: 0x281-0x2ff */
	if (frame.can_dlc != 8) return -5;
	if (frame.data[0] != 0x53) return -6;
	if (frame.data[1] != 0x4e) return -6;
	if (frame.data[2] != 0x02) return -6;
	if (frame.data[3] != 0x00) return -6;
	if (frame.data[4] != 0x2a) return -6;
	if (frame.data[5] != 0x01) return -6;
	if (frame.data[6] != 0x03) return -6;
	if (frame.data[7] != 0x00) return -6;

	return 0;
}

int setPosition(TCan *can, int pos)
{
	int rval;
	stopMotor(can); /* The motor must be stopped before any changes in the unit mode can be made. */ 
	rval = setUnitMode(can, MODE_POS);
	rval |= startMotor(can);
	rval |= setAbsolutePosition(can, pos);
	rval |= beginMotion(can);
	return rval;
}

int getPosition(TCan *can, int *pos)
{
	struct can_frame frame;
	unsigned char data[8] = { 0x50, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* PX */
	
	if (sendPDO2(can, 4, data) < 0) {
		return -1;
	}

	if (receivePDO2(can, &frame)) {
		return -2;
	}

	*pos = intFromData(frame.data);
	return 0;
}

int setForce(TCan *can, float force)
{
	int rval;
	stopMotor(can); /* The motor must be stopped before any changes in the unit mode can be made. */ 
	rval = setUnitMode(can, MODE_TORQUE);
	rval |= startMotor(can);
	rval |= setTorque(can, force);
	return rval;
}

int getForce(TCan *can, float *pos)
{
	struct can_frame frame;
	unsigned char data[8] = { 0x49, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* IQ */
	
	if (sendPDO2(can, 4, data)) {
		return -1;
	}

	if (receivePDO2(can, &frame)) {
		return -1;
	}

	*pos = floatFromData(frame.data);
	return 0;
}

int startMotor(TCan *can)
{
	unsigned char data[8] = { 0x4d, 0x4f, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 }; /* MO=1 */
	int rval = sendPDO2DiscardReply(can, 8, data);
	usleep(50 * 1000); /* >10 ms delay required after MO=1 */
	return rval;
}

int stopMotor(TCan *can)
{
	unsigned char data[8] = { 0x4d, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* MO=0 */
	int rval = sendPDO2DiscardReply(can, 8, data);
	usleep(50 * 1000); /* >10 ms delay required after MO=0 */
	return rval;
}

int beginMotion(TCan *can)
{
	unsigned char data[8] = { 0x42, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* BG */
	return sendPDO2DiscardReply(can, 4, data);
}

int stop(TCan *can)
{
	stopMotor(can);
	setUnitMode(can, MODE_POS); /* UnitMode must be MODE_POS for ST to work. */
	unsigned char data[8] = { 0x53, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* ST */
	return sendPDO2DiscardReply(can, 4, data);
}

int setUnitMode(TCan *can, enum Mode mode)
{
	stopMotor(can);
	unsigned char data[8] = { 0x55, 0x4d, 0x00, 0x00, mode, 0x00, 0x00, 0x00 }; /* UM=mode */
	return sendPDO2DiscardReply(can, 8, data);
}

int setSpeed(TCan *can, int speed)
{
	unsigned char data[8] = { 0x53, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* SP */
	setDataInt(data, speed);
	return sendPDO2DiscardReply(can, 8, data);
}

int setAbsolutePosition(TCan *can, int pos)
{
	unsigned char data[8] = { 0x50, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* PA */
	setDataInt(data, pos);
	return sendPDO2DiscardReply(can, 8, data);
}

int setRelativePosition(TCan *can, int pos)
{
	unsigned char data[8] = { 0x50, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* PR */
	setDataInt(data, pos);
	return sendPDO2DiscardReply(can, 8, data);;
}

int setTorque(TCan *can, float torque)
{
	unsigned char data[8] = { 0x54, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* TC */
	setDataFloat(data, torque);
	return sendPDO2DiscardReply(can, 8, data);
}

int getMaxCurrent(TCan *can, float *current)
{
	struct can_frame frame;
	unsigned char data[8] = { 0x4d, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* MC */
	int rval;
	
	rval = sendPDO2(can, 4, data);
	if (rval < 0) {
		return rval;
	}

	rval = receivePDO2(can, &frame);
	if (rval < 0) {
		return rval;
	}

	*current = floatFromData(frame.data);
	return 0;
}

int setLimits(TCan *can, int vmin, int vmax, int fmin, int fmax)
{
	unsigned char data1[8] = { 0x56, 0x4c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* VL[2] */
	unsigned char data2[8] = { 0x56, 0x48, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* VH[2] */
	unsigned char data3[8] = { 0x4c, 0x4c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* LL[2] */
	unsigned char data4[8] = { 0x48, 0x4c, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00 }; /* HL[2] */
	
	setDataInt(data1, vmin);
	setDataInt(data2, vmax);
	setDataInt(data3, fmin);
	setDataInt(data4, fmax);

	stopMotor(can); /* The motor must be stopped before any changes in the unit mode can be made. */ 
	setUnitMode(can, MODE_POS); /* Again the damn LL and HL commands work only in MODE_POS. */
	sendPDO2DiscardReply(can, 8, data1);
	sendPDO2DiscardReply(can, 8, data2);
	sendPDO2DiscardReply(can, 8, data3);
	sendPDO2DiscardReply(can, 8, data4);
	return 0;
}

