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
#include <stdio.h>
#include <stdlib.h>

#include "can.h"
#include "elmo.h"

/**
 * CAN device interface name.
 */
#define CAN_INTERFACE "can0"

/**
 * Motor controller CANOpen node ID.
 */
#define CANOPEN_ID 127

/**
 * Prints position and force readings from the motor controller.
 * Note: this function will never exit.
 */
void print_info(TCan *can)
{
	int f;
	float a;
	while (1) {
		getPosition(can, &f);
		getForce(can, &a);
		printf("position = %d\tforce = %f\n", f, a);
		usleep(500 * 1000);
	}
}

/**
 * Drive the motor with speed 30000 to position 4000000.
 */
void test_position(TCan *can)
{
	setSpeed(can, 30000);
	setPosition(can, 500000);
	print_info(can);
}

/**
 * Drive the motor using force feedback with 0.6 amperes.
 */
void test_force(TCan *can)
{
	setForce(can, 0.6);
	print_info(can);
}

/**
 * This will test some motor commands. It will test moving the motor to a
 * specified position with a specified speed. After that, it will also test
 * driving the motor with a specified force.
 */
void test(TCan *can)
{
	/**
	 * Set the motor speed limits.
	 */
	setLimits(can, -320000, 320000, -320000, 320000);
	

	/**
	 * Uncomment test_position(can) or test_force(can) to test position
	 * control or force control.
	 */
	test_position(can);
	//test_force(can);

	stop(can);
}

/**
 * A simple test program consisting of the mandatory commands to be given
 * before the motor controller can accept any useful commands and the
 * function calls to be given afterwards. The useful commands are put in
 * the test() function. To be able to run the useful commands one has to
 * remember to use first start.sh and run the useful commands in it.
 */
int main()
{
	printf("CAN test begins\n");

	TCan *can = TCanConstruct(CAN_INTERFACE);
	if (!can) {
		printf("Could not construct can\n");
		return EXIT_FAILURE;
	}
	
	if (TCanOpen(can, CANOPEN_ID) < 0) {
		printf("CanOpen failed\n");
		return EXIT_FAILURE;
	}

	if (sendEchoMessage(can) < 0) {
		printf("Echo failed\n");
		return EXIT_FAILURE;
	}

	test(can);

	if (TCanClose(can) < 0) {
		printf("CanClose failed\n");
		return EXIT_FAILURE;
	}

	TCanDestruct(can);
	
	printf("CAN test end\n");
	return EXIT_SUCCESS;
}
