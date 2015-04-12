/*
    This file is part of lib-bb84.

    lib-bb84 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    lib-bb84 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with lib-bb84.  If not, see <http://www.gnu.org/licenses/>.

    Copyright J. Joel vanBrandwijk, 2015
*/

#include "bb84.h"

int connectSocket(int port);
int connectChannel(int listenerSocket);
BitArray readBitArray(int receiveSock);
int setState(int state, int wconnection, int mconnection);
void sendQuReg(int sendSock, quantum_reg *reg);
quantum_reg receiveQuReg(int receiveSock);
void sendBitArray(int sendSock, BitArray b);
void sendSignal(int sigSocket, int signal);
void waitSignal(int sigSocket, int signal);
float doErrorCheck(int channelSocket, BB84Key k);
