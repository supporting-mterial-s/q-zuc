#include"stdio.h"
#include<iostream>
#include <string.h>
#include"vector"
#include<ctype.h>
#include <fstream>
#include<algorithm>
#include<cstring>

using namespace std;

uint8_t inv_sbox_0[256] = {
0x6, 0x6d, 0xc5, 0x1e, 0x8, 0xe5, 0x4d, 0x90, 0x1c, 0xc, 0x84, 0x94, 0xe7, 0x58, 0xc0, 0xdf,
0x9e, 0xb2, 0xba, 0x86, 0x4b, 0xd9, 0x61, 0x63, 0xa9, 0xfa, 0xf2, 0x11, 0x5f, 0x1b, 0x33, 0x67,
0x3d, 0x4c, 0x85, 0xbf, 0x66, 0xe4, 0xa1, 0xf1, 0x87, 0x2d, 0xc4, 0x95, 0x49, 0x19, 0x6c, 0x7e,
0xe8, 0x53, 0x13, 0x7, 0xfe, 0x98, 0xd2, 0x82, 0x3f, 0x3b, 0x7b, 0x70, 0x8a, 0xda, 0x0, 0xe6,
0xdc, 0x99, 0xb8, 0x35, 0xc9, 0xf8, 0x73, 0x3, 0x2b, 0x5b, 0x60, 0x71, 0x9d, 0x20, 0x21, 0x54,
0xd4, 0x5a, 0x97, 0x1d, 0xa, 0x4e, 0x8f, 0x46, 0x5e, 0xb7, 0x56, 0x2, 0xb5, 0xaf, 0x52, 0x5c,
0xff, 0x9b, 0xd7, 0x3c, 0xb4, 0xef, 0xb3, 0xc7, 0xd5, 0x36, 0x16, 0xe3, 0xcb, 0xe, 0x2e, 0x7d,
0xea, 0xd8, 0x1, 0x34, 0xbc, 0xb9, 0x40, 0xe2, 0xfd, 0x9a, 0xf9, 0x10, 0x88, 0x41, 0x92, 0x55,
0x7c, 0xf4, 0x59, 0xe1, 0x22, 0x2a, 0xc1, 0x79, 0xa3, 0x8b, 0xa4, 0x2c, 0x68, 0xf0, 0xe0, 0x78,
0x1f, 0x27, 0x65, 0xcd, 0xaa, 0xa2, 0x8e, 0x96, 0xb, 0x23, 0x77, 0x4f, 0xbd, 0x15, 0xd6, 0xce,
0xf6, 0xec, 0xca, 0xa6, 0x57, 0x17, 0x9c, 0x43, 0x64, 0xee, 0xbe, 0xc2, 0x2f, 0x6f, 0x6b, 0x14,
0xa5, 0x80, 0x42, 0xf5, 0x81, 0x38, 0x29, 0xeb, 0x18, 0xd, 0x5d, 0x4a, 0xa0, 0x39, 0xc3, 0x91,
0xa8, 0x31, 0xcc, 0x8d, 0xdd, 0x47, 0x37, 0x8c, 0x7f, 0x75, 0x4, 0xf, 0x89, 0x30, 0x25, 0x44,
0xb0, 0x9, 0xd3, 0x6e, 0x3e, 0x62, 0xdb, 0x7a, 0x6a, 0x26, 0x72, 0xf3, 0xb1, 0x28, 0x76, 0xfb,
0x5, 0xac, 0xde, 0x50, 0x24, 0xad, 0xc8, 0xc6, 0xed, 0xae, 0xe9, 0x74, 0xb6, 0x45, 0xfc, 0x51,
0x93, 0xcf, 0xab, 0x48, 0xf7, 0xbb, 0xd0, 0x83, 0x32, 0x12, 0xd1, 0xa7, 0x1a, 0x3a, 0x9f, 0x69};

void check(int *in, int *out)
{
	int t[80] = {0};
	int tmp[8] = {0};

	in[0] = in[0] ^ in[6];
	in[1] = in[1] ^ in[7];
	in[2] = in[2] ^ in[5] ^ 1;
	in[3] = in[3] ^ in[4];

//////////////////////////copy
	tmp[0] = tmp[0] ^ in[6];
	tmp[1] = tmp[1] ^ in[7];
	tmp[2] = tmp[2] ^ in[5];
	tmp[3] = tmp[3] ^ in[4];

	in[5] = in[5] ^ tmp[0];
	in[6] = in[6] ^ tmp[1];
	in[4] = in[4] ^ tmp[2];
	in[7] = in[7] ^ tmp[3];

	tmp[4] = tmp[4] ^ in[5] & tmp[1];
	tmp[5] = tmp[5] ^ in[6] & tmp[3];
	tmp[6] = tmp[6] ^ in[4] & tmp[0];
	tmp[7] = tmp[7] ^ in[7] & tmp[2];

	in[0] = in[0] ^ tmp[4];
	in[1] = in[1] ^ tmp[5];
	in[2] = in[2] ^ tmp[6];
	in[3] = in[3] ^ tmp[7];

	in[5] = in[5] ^ tmp[0];
	in[6] = in[6] ^ tmp[1];
	in[4] = in[4] ^ tmp[2];
	in[7] = in[7] ^ tmp[3];

//////////////////////////reset tmp_0 to tmp_3.
	tmp[0] = tmp[0] ^ in[6];
	tmp[1] = tmp[1] ^ in[7];
	tmp[2] = tmp[2] ^ in[5];
	tmp[3] = tmp[3] ^ in[4];


/// The first layer of the AND depth ------

/*

20 QAND gates, 40 inputs, however, 
only 8 variables are allocated, i.e., in[0]~in[7],
therefore, 40-8 = 32 ancilla qubits are required to copy in[0]~in[7,
which consumes 32 CNOT gates (and 32 CNOT to to reset these 32 qubits), 32 ancilla qubits.

20 QAND gate require 20 0s to store outputs of QAND gates,
20 0s as the ancilla qubits .

Totally, 32 + 20 + 20 = 72 ancilla qubits are required.
88 ancilla qubits are allocated, 4 are not 0.
84 > 72
After this step, 84 - 20 = 64 ancilla qubits are 0.
88 ancilla qubits, tmp_4 ~ tmp_7 ! = 0
          t[0] ~ t[19] ! = 0
          tmp_0 ~ tmp_3 == 0
          t[20] ~ t[79] == 0
*/
t[0] = t[0] ^ in[1]&in[2]; //t[20]
t[1] = t[1] ^ in[0]&in[3]; //t[21]
t[2] = t[2] ^ in[0]&in[2]; //t[22]
t[3] = t[3] ^ in[0]&in[1]; //t[23]
t[4] = t[4] ^ in[2]&in[3]; //t[24]
t[5] = t[5] ^ in[3]&in[5]; //t[25]
t[6] = t[6] ^ in[1]&in[3]; //t[26]
t[7] = t[7] ^ in[0]&in[7]; //t[27]
t[8] = t[8] ^ in[2]&in[6]; //t[28]
t[9] = t[9] ^ in[1]&in[7]; //t[29]
t[10] = t[10] ^ in[0]&in[6]; //t[30]
t[11] = t[11] ^ in[2]&in[5]; //t[31]
t[12] = t[12] ^ in[1]&in[4]; //t[32]
t[13] = t[13] ^ in[3]&in[4]; //t[33]
t[14] = t[14] ^ in[3]&in[7]; //t[34]
t[15] = t[15] ^ in[0]&in[5]; //t[35]
t[16] = t[16] ^ in[0]&in[4]; //t[36]
t[17] = t[17] ^ in[1]&in[6]; //t[37]
t[18] = t[18] ^ in[2]&in[7]; //t[38]
t[19] = t[19] ^ in[3]&in[6]; //t[39]
/*
After this step, 84 - 20 = 64 ancilla qubits are 0.
88 ancilla qubits, tmp_4 ~ tmp_7 ! = 0
          t[0] ~ t[19] ! = 0
          tmp_0 ~ tmp_3 == 0
          t[20] ~ t[79] == 0
*/

/*
T: 0-1-2-3-4-5-6-7-8-9-10-11-12-13-14-15-16-17-18-19
  i.e., t[20-21-22-23 can be reset after being XORed to outpus];
X: 0-1-2-3-4-5-6-7
*/


out[0] = out[0] ^ in[1] ^ in[4];
out[1] = out[1] ^ in[0] ^ in[1] ^ in[3] ^ in[5] ^ in[7];
out[2] = out[2] ^ in[1] ^ in[5];
out[3] = out[3] ^ in[4] ^ in[5];
out[4] = out[4] ^ in[1] ^ in[2] ^ in[4];
out[5] = out[5] ^ in[1] ^ in[2] ^ in[3] ^ in[5];
out[6] = out[6] ^ in[0] ^ in[1] ^ in[2] ^ in[6];
out[7] = out[7] ^ in[0] ^ in[1] ^ in[3] ^ in[7];




/////////////////
out[0] = out[0] ^ t[0] ^ t[2] ^ t[3] ^ t[4] ^ t[7] ^ t[8] ^ t[9] ^ t[10] ^ t[12] ^ t[16] ^ t[18] ^ t[19];
out[1] = out[1] ^ t[0] ^ t[1] ^ t[2] ^ t[3] ^ t[5] ^ t[6] ^ t[8] ^ t[9] ^ t[11] ^ t[14] ^ t[17] ^ t[18] ^ t[19];
out[2] = out[2] ^ t[0] ^ t[1] ^ t[3] ^ t[5] ^ t[6] ^ t[12] ^ t[13] ^ t[14] ^ t[15] ^ t[16];
out[3] = out[3] ^ t[0] ^ t[2] ^ t[3] ^ t[8] ^ t[11] ^ t[13] ^ t[16] ^ t[17];
out[4] = out[4] ^ t[1] ^ t[2] ^ t[3];
out[5] = out[5] ^ t[0] ^ t[1] ^ t[4];
out[6] = out[6] ^ t[1] ^ t[2] ^ t[3] ^ t[4] ^ t[6];
out[7] = out[7] ^ t[0] ^ t[1] ^ t[2] ^ t[3];
///////////////////////////

t[24] = t[24] ^ in[5] ^ in[6] ^ t[0] ^ t[2] ^ t[3] ^ t[6];
t[6] = t[6] ^ in[6] ^ t[1] ^ t[2] ^ t[3] ^ t[4];
t[16] = t[16] ^ in[0] ^ in[6] ^ t[7];
t[15] = t[15] ^ in[0] ^ in[5] ^ in[6] ^ in[7] ^ t[5] ^ t[7] ^ t[10];
t[17] = t[17] ^ in[6] ^ in[7] ^ t[9];
t[14] = t[14] ^ in[3] ^ in[4] ^ in[5] ^ in[7] ^ t[5] ^ t[7] ^ t[13];
t[18] = t[18] ^ in[1] ^ in[4] ^ in[5] ^ t[9];
t[25] = t[25] ^ in[4] ^ t[2] ^ t[3] ^ t[4];
t[12] = t[12] ^ in[5] ^ in[6] ^ t[8] ^ t[11];
t[19] = t[19] ^ in[0] ^ in[3] ^ t[5];
t[8] = t[8] ^ in[2];
t[26] = t[26] ^ in[7] ^ in[4];
t[27] = t[27] ^ t[3] ^ t[2];
t[28] = t[28] ^ in[5] ^ in[6] ^ in[7];
t[29] = t[29] ^ in[5];
t[30] = t[30] ^ in[6];
in[6] = in[6] ^ in[5];
in[5] = in[5] ^ in[4];


/* 23 QAND gates
23*2 = 46 inputs, 28 have been allocated.
23 0s to store QAND gate output
23 0s as ancilla qubits, 
46-28 = 18 ancilla qubits required to copy the remaining 18 inputs.
totally, 23*2 +18 = 64 ancilla qubits are required in this layer.


After the previous step, 84 - 20 = 64 ancilla qubits are 0.
88 ancilla qubits, tmp_4 ~ tmp_7 ! = 0
          t[0] ~ t[19] ! = 0
          tmp_0 ~ tmp_3 == 0
          t[20] ~ t[79] == 0

64 (left) == 64 (needed)
T: 0-1-4-5-6-8-12-14-15-16-17-18-19-24-25-26-27-28-29-30
  6-6-1-1-1-1-1 -1 - 1-1 -1 - 1-1 -1 -1 -1 -2 -1 -3 -1
X: 0-1-2-3-4-5-6-7
  1-3-2-1-2-1-1-2
 */
t[20] = t[20] ^ in[1]&t[29]; 
t[21] = t[21] ^ in[2]&in[4]; 
t[22] = t[22] ^ t[29]&t[30]; 
t[23] = t[23] ^ t[29]&in[7];
t[31] = t[31] ^ t[26]&t[6];
t[32] = t[32] ^ t[0]&t[16];
t[33] = t[33] ^ t[1]&t[8];
t[34] = t[34] ^ t[0]&t[15];
t[35] = t[35] ^ t[1]&t[17];
t[36] = t[36] ^ t[4]&t[28];
t[37] = t[37] ^ in[1]&t[5];
t[38] = t[38] ^ t[0]&t[14];
t[39] = t[39] ^ t[1]&t[18];
t[40] = t[40] ^ in[7]&t[25];
t[41] = t[41] ^ t[27]&in[5];
t[42] = t[42] ^ in[4]&t[24];
t[43] = t[43] ^ t[1]&t[12];
t[44] = t[44] ^ t[0]&t[19];
t[45] = t[45] ^ t[27]&in[6];
t[46] = t[46] ^ t[0]&in[3];
t[47] = t[47] ^ in[0]&t[0];
t[48] = t[48] ^ t[1]&in[2];
t[49] = t[49] ^ t[1]&in[1];

out[0] = out[0] ^ t[21] ^ t[31] ^ t[32] ^ t[33];
out[1] = out[1] ^ t[22] ^ t[23] ^ t[34] ^ t[35] ^ t[36] ^ t[37];
out[2] = out[2] ^ t[20] ^ t[23] ^ t[38] ^ t[39] ^ t[40] ^ t[41];
out[3] = out[3] ^ t[20] ^ t[42] ^ t[43] ^ t[44] ^ t[45];
out[4] = out[4] ^ t[48] ^ t[46] ^ 1;
out[5] = out[5] ^ t[49] ^ t[47];
out[6] = out[6] ^ t[47];
out[7] = out[7] ^ t[46];
///////////////////////////reset all the ancilla qubits
t[49] = t[49] ^ t[1]&in[1];
t[48] = t[48] ^ t[1]&in[2];
t[47] = t[47] ^ in[0]&t[0];
t[46] = t[46] ^ t[0]&in[3];
t[45] = t[45] ^ t[27]&in[6];
t[44] = t[44] ^ t[0]&t[19];
t[43] = t[43] ^ t[1]&t[12];
t[42] = t[42] ^ in[4]&t[24];
t[41] = t[41] ^ t[27]&in[5];
t[40] = t[40] ^ in[7]&t[25];
t[39] = t[39] ^ t[1]&t[18];
t[38] = t[38] ^ t[0]&t[14];
t[37] = t[37] ^ in[1]&t[5];
t[36] = t[36] ^ t[4]&t[28];
t[35] = t[35] ^ t[1]&t[17];
t[34] = t[34] ^ t[0]&t[15];
t[33] = t[33] ^ t[1]&t[8];
t[32] = t[32] ^ t[0]&t[16];
t[31] = t[31] ^ t[26]&t[6];
t[23] = t[23] ^ t[29]&in[7];
t[22] = t[22] ^ t[29]&t[30];
t[21] = t[21] ^ in[2]&in[4];
t[20] = t[20] ^ in[1]&t[29];
in[5] = in[5] ^ in[4];
in[6] = in[6] ^ in[5];
t[30] = t[30] ^ in[6];
t[29] = t[29] ^ in[5];
t[28] = t[28] ^ in[5] ^ in[6] ^ in[7];
t[27] = t[27] ^ t[3] ^ t[2];
t[26] = t[26] ^ in[7] ^ in[4];
t[8] = t[8] ^ in[2];
t[19] = t[19] ^ in[0] ^ in[3] ^ t[5];
t[12] = t[12] ^ in[5] ^ in[6] ^ t[8] ^ t[11];
t[25] = t[25] ^ in[4] ^ t[2] ^ t[3] ^ t[4];
t[18] = t[18] ^ in[1] ^ in[4] ^ in[5] ^ t[9];
t[14] = t[14] ^ in[3] ^ in[4] ^ in[5] ^ in[7] ^ t[5] ^ t[7] ^ t[13];
t[17] = t[17] ^ in[6] ^ in[7] ^ t[9];
t[15] = t[15] ^ in[0] ^ in[5] ^ in[6] ^ in[7] ^ t[5] ^ t[7] ^ t[10];
t[16] = t[16] ^ in[0] ^ in[6] ^ t[7];
t[6] = t[6] ^ in[6] ^ t[1] ^ t[2] ^ t[3] ^ t[4];
t[24] = t[24] ^ in[5] ^ in[6] ^ t[0] ^ t[2] ^ t[3] ^ t[6];
t[19] = t[19] ^ in[3]&in[6]; //t[39]
t[18] = t[18] ^ in[2]&in[7]; //t[38]
t[17] = t[17] ^ in[1]&in[6]; //t[37]
t[16] = t[16] ^ in[0]&in[4]; //t[36]
t[15] = t[15] ^ in[0]&in[5]; //t[35]
t[14] = t[14] ^ in[3]&in[7]; //t[34]
t[13] = t[13] ^ in[3]&in[4]; //t[33]
t[12] = t[12] ^ in[1]&in[4]; //t[32]
t[11] = t[11] ^ in[2]&in[5]; //t[31]
t[10] = t[10] ^ in[0]&in[6]; //t[30]
t[9] = t[9] ^ in[1]&in[7]; //t[29]
t[8] = t[8] ^ in[2]&in[6]; //t[28]
t[7] = t[7] ^ in[0]&in[7]; //t[27]
t[6] = t[6] ^ in[1]&in[3]; //t[26]
t[5] = t[5] ^ in[3]&in[5]; //t[25]
t[4] = t[4] ^ in[2]&in[3]; //t[24]
t[3] = t[3] ^ in[0]&in[1]; //t[23]
t[2] = t[2] ^ in[0]&in[2]; //t[22]
t[1] = t[1] ^ in[0]&in[3]; //t[21]
t[0] = t[0] ^ in[1]&in[2]; //t[20]
   tmp[3] = tmp[3] ^ in[4];
   tmp[2] = tmp[2] ^ in[5];
   tmp[1] = tmp[1] ^ in[7];
   tmp[0] = tmp[0] ^ in[6];
   in[7] = in[7] ^ tmp[3];
   in[4] = in[4] ^ tmp[2];
   in[6] = in[6] ^ tmp[1];
   in[5] = in[5] ^ tmp[0];
   in[3] = in[3] ^ tmp[7];
   in[2] = in[2] ^ tmp[6];
   in[1] = in[1] ^ tmp[5];
   in[0] = in[0] ^ tmp[4];
   tmp[7] = tmp[7] ^ in[7] & tmp[2];
   tmp[6] = tmp[6] ^ in[4] & tmp[0];
   tmp[5] = tmp[5] ^ in[6] & tmp[3];
   tmp[4] = tmp[4] ^ in[5] & tmp[1];
   in[7] = in[7] ^ tmp[3];
   in[4] = in[4] ^ tmp[2];
   in[6] = in[6] ^ tmp[1];
   in[5] = in[5] ^ tmp[0];
   tmp[3] = tmp[3] ^ in[4];
   tmp[2] = tmp[2] ^ in[5];
   tmp[1] = tmp[1] ^ in[7];
   tmp[0] = tmp[0] ^ in[6];
   in[3] = in[3] ^ in[4];
   in[2] = in[2] ^ in[5] ^ 1;
   in[1] = in[1] ^ in[7];
in[0] = in[0] ^ in[6];

//check
int summ=0;
for(int i = 0; i < 80;i++)
{
	summ += t[i];
}

for(int i = 0; i < 8;i++)
{
	summ += tmp[i];
}
cout<<summ;
}

int main()
{
	for(int i = 0; i < 256; i++)
	{
		int x[8] = {0};
		int in[8] = {0};
		int out[8] = {0};

		for(int j = 0; j < 8; j++)
		{
			in[j] = ((i>>(7 - j))&1);
		}
		for(int j = 0; j < 8; j++)
		{
			x[j] = in[(j + 3)%8];
		}
		check(x, out);
	
		int out_0 = 128 * out[0] + 64*out[1] ^ 32 * out[2] + 16 * out[3] + 8 * out[4] + 4*out[5] ^ 2*out[6] + out[7];
		

		if((i != 0) && ((i%16) == 0))
		{
			cout<<endl;
		}

		cout<<(out_0 ^ inv_sbox_0[i])<<" ";
	}
	cout<<endl;
	return 1;
}


/*
Pauli-X: 3;
QAND   : 47;
QAND_C : 47;
CNOT   : 275
T-depth: 3

the 2nd layer: 32 CNOT gates to copy 32 variables,
               32 CNOT gates to reset them;
the 3rd layer: 18 CNOT gates to copy 18 variables,
               18 CNOT gates to reset them;              

ancilla qubits: 8(S-box outputs)+88
1 QAND = 3 1-qubit Clifford gates 
       + 4 T gates
       + 8 CNOT gates
1 QAND_C = 4 1-qubit Clifford gates
         + 1 CNOT gate
         + 1 Measure

Totally, 
#Measure : 47;
#1-qubit : 3 + 47*3 + 47*4 = 332;
#CNOT    : 275 + 32*2 + 18*2+ 47*8+47*1 = 798;
#T       : 47*4 = 188;
T-depth  : 3
*/
