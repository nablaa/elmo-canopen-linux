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
#include "can.h"

TCan *TCanConstruct(const char *iface)
{
	TCan *can = (TCan *)malloc(sizeof(TCan));
	if (!can) {
		return NULL;
	}

	can->iface = (char *)malloc(strlen(iface) + 1);
	if (!can->iface) {
		free(can);
		return NULL;
	}

	strcpy(can->iface, iface);
	return can;
}

void TCanDestruct(TCan *can)
{
	free(can->iface);
	free(can);
}

int TCanOpen(TCan *can, int canid)
{
	can->id = canid;

	if ((can->socket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket error");
		return -1;
	}

	can->addr.can_family = AF_CAN;
	strcpy(can->ifr.ifr_name, can->iface);

	if (ioctl(can->socket, SIOCGIFINDEX, &can->ifr) < 0) {
		perror("SIOCGIFINDEX error");
		return -2;
	}

	can->addr.can_ifindex = can->ifr.ifr_ifindex;

        if (bind(can->socket, (struct sockaddr *)&can->addr, sizeof(can->addr)) < 0) {
                perror("bind error");
                return -3;
        }

	return setOperational(can);
}

int TCanClose(TCan *can)
{
	return close(can->socket);
}

int setOperational(TCan *can)
{
	struct can_frame frame;
	unsigned char data[8] = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	createFrame(&frame, can->id, 2, data);
	return sendFrame(can, &frame);
}

int setPreOperational(TCan *can)
{
	struct can_frame frame;
	unsigned char data[8] = { 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	createFrame(&frame, can->id, 2, data);
	return sendFrame(can, &frame);
}

void createFrame(struct can_frame *frame, int id, int len, unsigned char *data)
{
	int i;

	frame->can_id = id;
	frame->can_dlc = len;

	for (i = 0; i < 8; i++) {
		frame->data[i] = data[i];
	}
}

int sendFrame(TCan *can, struct can_frame *frame)
{
	int bytes;
	if ((bytes = write(can->socket, frame, sizeof(*frame))) != sizeof(*frame)) {
		perror("write");
		return -1;
	}
	return 0;
}

int sendPDO2(TCan *can, int size, unsigned char *data)
{
	struct can_frame frame;
	createFrame(&frame, can->id | (6 << 7), size, data); /* TPDO2 COB-ID: 0x301-0x37f */

	if (sendFrame(can, &frame) < 0) {
		return -1;
	}

	return 0;
}

int receivePDO2(TCan *can, struct can_frame *frame)
{
	int bytes;
	for (;;) {
		bytes = read(can->socket, frame, sizeof(*frame));
		if (bytes < 0) {
			perror("read error");
			return -1;
		}

		if ((unsigned int)bytes < sizeof(struct can_frame)) {
			return -2;
		}

		/*
		 * Read CAN-messages until we get a message from our
		 * own can device with RPDO2 COB-ID (0x281-0x2ff)
		 */
		if (frame->can_id == (can->id | (5 << 7))) {
			return 0;
		}
	}

	return 0;
}

int sendPDO2DiscardReply(TCan *can, int size, unsigned char *data)
{
	int rval = sendPDO2(can, size, data);
	struct can_frame frame;
	receivePDO2(can, &frame);
	return rval;
}

void setDataInt(unsigned char *data, int i)
{
	data[7] = (i >> 24);
	data[6] = ((i << 8) >> 24);
	data[5] = ((i << 16) >> 24);
	data[4] = ((i << 24) >> 24);
}

void setDataFloat(unsigned char *data, float f)
{
	int i = *(int *)&f;
	data[7] = (i >> 24);
	data[6] = ((i << 8) >> 24);
	data[5] = ((i << 16) >> 24);
	data[4] = ((i << 24) >> 24);
	data[3] |= 1 << 7; /* float mode ON */
}

float floatFromData(unsigned char *data)
{
	int i = 0x00;
	i |= (data[7] << 24);
	i |= (data[6] << 16);
	i |= (data[5] << 8);
	i |= (data[4]);
	return *(float *)&i;
}

int intFromData(unsigned char *data)
{
	int i = 0x00;
	i |= (data[7] << 24);
	i |= (data[6] << 16);
	i |= (data[5] << 8);
	i |= (data[4]);
	return i;
}

