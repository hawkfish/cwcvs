/*
	File:		OldBitMap.h

	Contains:	old-style bitmap utility routines for allocating, de-allocating, and masking.

	Written by: 	

	Copyright:	Copyright � 1992-1999 by Apple Computer, Inc., All Rights Reserved.

				You may incorporate this Apple sample source code into your program(s) without
				restriction. This Apple sample source code has been provided "AS IS" and the
				responsibility for its operation is yours. You are not permitted to redistribute
				this Apple sample source code as "Apple sample source code" after having made
				changes. If you're going to re-distribute the source, we require that you make
				it clear in the source that the code was descended from Apple sample source
				code, but that you've made changes.

	Change History (most recent first):
				7/9/1999	Karl Groethe	Updated for Metrowerks Codewarror Pro 2.1
				

*/
#include <Quickdraw.h>

void FreeBitMap(BitMap *Bits);
void CalcOffScreen(register Rect *frame,register long *needed, register short *rows);
void NewBitMap(Rect *frame,BitMap *theMap);
void NewMaskedBitMap(BitMap	*srcBits, BitMap *maskBits, Rect *srcRect);