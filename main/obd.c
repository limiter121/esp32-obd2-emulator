/* Copyright 2009 Gary Briggs

This file is part of obdgpslogger.

obdgpslogger is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

obdgpslogger is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with obdgpslogger.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
  \brief Functions to convert from values back to OBDII output
*/

// http://en.wikipedia.org/wiki/Table_of_OBD-II_Codes


int obdRevConvert_04    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(255.0f*val/100.0f);
	return 1;
}


int obdRevConvert_05    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(val+40);
	return 1;
}


int obdRevConvert_06_09 (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(128.0f*val/100.0f) + 128;
	return 1;
}


int obdRevConvert_0A    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(val/3.0f);
	return 1;
}


int obdRevConvert_0B    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)val;
	return 1;
}


int obdRevConvert_0C    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	val = val * 4;
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_0D    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)val;
	return 1;
}


int obdRevConvert_0E    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)((val + 64.0f) * 2.0f);
	return 1;
}


int obdRevConvert_0F    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(val+40);
	return 1;
}


int obdRevConvert_10    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	val = val * 100;
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_11    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(255.0f*val/100.0f);
	return 1;
}


int obdRevConvert_14_1B (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(val / 0.005f);
	return 1;
}


int obdRevConvert_1F    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_21    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_22    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	val = val / 0.079f;
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_23    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	val = val / 10.0f;
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_24_2B (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	val = val / 0.0000305f;
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_2C    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(255.0f*val/100.0f);
	return 1;
}


int obdRevConvert_2D    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)((val + 100.0f) / 0.78125f);
	return 1;
}


int obdRevConvert_2E    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(255.0f*val/100.0f);
	return 1;
}


int obdRevConvert_2F    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(255.0f*val/100.0f);
	return 1;
}


int obdRevConvert_30    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)val;
	return 1;
}


int obdRevConvert_31    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_32    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	val = val + 8192.0f;
	val = val * 4.0f;
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_33    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)val;
	return 1;
}


int obdRevConvert_34_3B (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	val = val / 0.0000305f;
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_3C_3F (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	val = val + 40.0f;
	val = val * 10.0f;
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_42    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	val = val * 1000.0f;
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_43    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	val = val * 255.0f / 100.0f;
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_44    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	val = val / 0.0000305f;
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_45    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(255.0f*val/100.0f);
	return 1;
}


int obdRevConvert_46    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(val+40);
	return 1;
}


int obdRevConvert_47_4B (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(255.0f*val/100.0f);
	return 1;
}


int obdRevConvert_4C    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(255.0f*val/100.0f);
	return 1;
}


int obdRevConvert_4D    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_4E    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)((long)val / 256);
	*B = (unsigned int)((long)val % 256);
	return 2;
}


int obdRevConvert_52    (float val, unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D) {
	*A = (unsigned int)(255.0f*val/100.0f);
	return 1;
}


