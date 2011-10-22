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
#ifndef ELMO_H
#define ELMO_H

#include "can.h"

/**
 * Elmo motor controller command mode (unit mode).
 */
enum Mode
{
	MODE_POS = 5,   /** Position mode: Motor is to be controlled with a position command. */
	MODE_TORQUE = 1 /** Torque mode: Motor is to be controlled with a torque command. */
};

/**
 * Tests whether the send and receive functions work properly. With a different kind of a motor
 * controller this test may fail, however, even though the functions were working well.
 * 
 * @param can The TCan pointer of the motor controller.
 * @return 0 on success, <0 otherwise. 
 */
int sendEchoMessage(TCan *can);

/**
 * Commands the motor to drive to the given position. beginMotion() must be called afterwards to
 * make the motor begin the motion.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param pos The absolute position.
 * @return 0 on success, <0 otherwise. 
 */
int setPosition(TCan *can, int pos);

/**
 * Returns the absolute position of the motor.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param pos The pointer to where the position value is stored.
 * @return 0 on success, <0 otherwise. 
 */
int getPosition(TCan *can, int *pos);

/**
 * Commands the motor to drive with the given force. beginMotion() must be called afterwards to
 * make the motor begin the motion.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param force The force.
 * @return 0 on success, <0 otherwise. 
 */
int setForce(TCan *can, float force);

/**
 * Returns the force (in amperes) by which the motor is driven. The return value may be
 * somewhat inaccurate, or at least it was with our test equipment. But I'll bet that the
 * Wave University will fix this.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param force The pointer to where the force value is stored.
 * @return 0 on success, <0 otherwise. 
 */
int getForce(TCan *can, float *force);

/**
 * Sets the motor in a state in which it can be given motion commands.
 * 
 * @param can The TCan pointer of the motor controller.
 * @return 0 on success, <0 otherwise. 
 */
int startMotor(TCan *can);

/**
 * Sets the motor in a state in which it can not be given motion commands.
 * 
 * @param can The TCan pointer of the motor controller.
 * @return 0 on success, <0 otherwise. 
 */
int stopMotor(TCan *can);

/**
 * Commands the motor to begin motion.
 * 
 * @param can The TCan pointer of the motor controller.
 * @return 0 on success, <0 otherwise. 
 */
int beginMotion(TCan *can);

/**
 * Commands the motor to stop.
 * 
 * @param can The TCan pointer of the motor controller.
 * @return 0 on success, <0 otherwise. 
 */
int stop(TCan *can);

/**
 * Sets the unit mode of the motor controller. Roughly speaking the unit mode is MODE_POS
 * when the motor is position commands and MODE_TORQUE if it is given torque commands.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param mode The unit mode.
 * @return 0 on success, <0 otherwise. 
 */
int setUnitMode(TCan *can, enum Mode mode);

/**
 * Sets the speed which the motor will be trying to run when given a position command.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param speed The speed
 * @return 0 on success, <0 otherwise. 
 */
int setSpeed(TCan *can, int speed);

/**
 * Gives the motor controller a position command with an absolute position. The unit mode
 * of the controller must be MODE_POS for this function to work.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param pos The absolute position.
 * @return 0 on success, <0 otherwise. 
 */
int setAbsolutePosition(TCan *can, int pos);

/**
 * Gives the motor controller a position command with a relative position. The unit mode
 * of the controller must be MODE_POS for this function to work.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param pos The relative position.
 * @return 0 on success, <0 otherwise. 
 */
int setRelativePosition(TCan *can, int pos);

/**
 * Gives the motor controller a torque command. The unit mode of the controller must be
 * MODE_TORQUE for this function to work.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param torque The torque (in amperes)
 * @return 0 on success, <0 otherwise. 
 */
int setTorque(TCan *can, float torque);

/**
 * Returns the maximum current that can be used with the motor.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param current The pointer where the maximum current value is stored.
 * @return 0 on success, <0 otherwise. 
 */
int getMaxCurrent(TCan *can, float *current);

/**
 * Sets the speed limits of the motor and the feedback limits of the encoder.
 * 
 * @param can The TCan pointer of the motor controller.
 * @param vmin The minimum speed of the motor (e.g. the negation of vmax).
 * @param vmax The maximum speed of the motor.
 * @param fmin The minimum speed of the feedback (e.g. the negation of fmax).
 * @param fmax The maximum speed of the feedback.
 * @return 0 on success, <0 otherwise. 
 */
int setLimits(TCan *can, int vmin, int vmax, int fmin, int fmax);

#endif /* ELMO_H */
