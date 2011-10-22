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
#ifndef ELMO_CAN_H
#define ELMO_CAN_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <net/if.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

/** Protocol family */
#ifndef PF_CAN
#define PF_CAN 29
#endif
 
#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

/**
 * CAN Device information
 */
typedef struct {
	char *iface;              /** can interface device (e.g. "can0") */
	struct sockaddr_can addr; /** socket address structure for CAN address family */
	struct ifreq ifr;         /** interface request structure */
	unsigned int id;          /** CANOpen device node id: 1-127 (e.g. 127) */
	int socket;               /** socket file descriptor */
} TCan;

/**
 * Constructs a new TCan. This function must be called before anything else can be done.
 * 
 * @param iface The name of the CAN interface.
 * @return The pointer to a new TCan.
 */
TCan *TCanConstruct(const char *iface);

/**
 * Destructs a TCan.
 *
 * @param can The pointer to the TCan to be destructed.
 */
void TCanDestruct(TCan *can);

/**
 * Initializes the given TCan for communication and sets the motor controller operational.
 * This function must be called before any useful commands can be given to the motor
 * controller.
 * 
 * @param can The pointer to the TCan to be opened.
 * @param canid The ID of the CAN node in the bus.
 * @return 0 on success, <0 otherwise. 
 */
int TCanOpen(TCan *can, int canid);

/**
 * Closes the communication to the CAN controller.
 * 
 * @param can The pointer to the TCan to be closed.
 * @return 0 on success, <0 otherwise. 
 */
int TCanClose(TCan *can);

/**
 * Sets the motor controller operational.
 * 
 * @param can The TCan pointer of the motor controller.
 * @return 0 on success, <0 otherwise. 
 */
int setOperational(TCan *can);

/**
 * Sets the motor controller in the preoperational state.
 * 
 * @param can The TCan pointer of the motor controller.
 * @return 0 on success, <0 otherwise. 
 */
int setPreOperational(TCan *can);

/**
 * Creates a CAN message frame
 * 
 * @param frame The pointer to the frame to be created.
 * @param id The ID of the CAN node.
 * @param len The length of the message in bytes.
 * @param data The data of the message.
 */
void createFrame(struct can_frame *frame, int id, int len, unsigned char *data);

/**
 * Sends the given frame to the bus.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param frame The pointer to the frame to be sent.
 * @return 0 on success, <0 otherwise. 
 */
int sendFrame(TCan *can, struct can_frame *frame);

/**
 * Sends a PDO2 message to the bus.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param size The size of the message in bytes.
 * @param data The data of the message.
 * @return 0 on success, <0 otherwise. 
 */
int sendPDO2(TCan *can, int size, unsigned char *data);

/**
 * Receives a PDO2 message.
 *
 * @param can The TCan pointer of the motor controller.
 * @param frame The frame pointer where the received messge is written.
 * @return 0 on success, <0 otherwise. 
 */
int receivePDO2(TCan *can, struct can_frame *frame);

/**
 * Sends a PDO2 message to the bus and reads the reply message.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param size The size of the message in bytes.
 * @param data The data of the message.
 * @return 0 on success, <0 otherwise. 
 */
int sendPDO2DiscardReply(TCan *can, int size, unsigned char *data);

/**
 * Writes an integer into the message.
 * 
 * A message with an integer argument must be created in a way that the data is first
 * written without the argument which is added later using this function.
 *
 * @param data The data pointer to where the integer is written.
 * @param i The integer to be written.
 */
void setDataInt(unsigned char *data, int i);

/**
 * Writes a floating point number into the message.
 * 
 * A message with a float argument must be created in a way that the data is first
 * written without the argument which is added later using this function.
 *
 * @param data The data pointer to where the float is written.
 * @param f The float to be written.
 */
void setDataFloat(unsigned char *data, float f);

/**
 * Extracts the floating point number out of a message.
 *
 * @param data The pointer to the data which is to be read.
 * @return The float of the message.
 */
float floatFromData(unsigned char *data);

/**
 * Extracts the integer out of a message.
 *
 * @param data The pointer to the data which is to be read.
 * @return The integer of the message.
 */
int intFromData(unsigned char *data);

#endif /* ELMO_CAN_H */
