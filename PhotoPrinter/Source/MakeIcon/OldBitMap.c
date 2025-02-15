/*
	File:		OldBitMap.c

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
#include "OldBitMap.h"
#include <Quickdraw.h>
#include <Memory.h>

void FreeBitMap(BitMap *inBits)
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Dumps a BitMap created by NewBitMap below.
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
{
	DisposePtr(inBits->baseAddr);
	inBits->baseAddr = nil;
	SetRect(&inBits->bounds, 0, 0, 0, 0);
	inBits->rowBytes = 0;
}

void CalcOffScreen(register Rect *frame, register long *needed, register short *rows)
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Calculates how much memory and rowbytes a bitmap of the size frame would require.
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
{
	*rows = ((((frame->right) - (frame->left)) + 15)/16) * 2;
	*needed = (((long)(*rows)) * (long)((frame->bottom) - (frame->top)));
}
 
void NewBitMap(Rect *frame, BitMap *theMap)
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Creates a new empty bitmap.
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
{
	Size	size;
	short	rbytes;
	
	CalcOffScreen(frame, &size, &rbytes);
	
	theMap->rowBytes = rbytes;
	theMap->bounds = *frame;
	theMap->baseAddr = NewPtrClear(size);
} // NewBitMap


void NewMaskedBitMap(BitMap	*srcBits, BitMap *maskBits, Rect *srcRect)
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Pass a pointer to an existing bitmap, and this creates a bitmap that is an
	equivelent mask.
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
{
	short	rowbytes, height, words;
	long	needed;

	NewBitMap(srcRect, maskBits);
	
	if(MemError()) {
		maskBits->baseAddr = nil;
	 	SetRect(&maskBits->bounds, 0, 0, 0, 0);
	 	maskBits->rowBytes = 0;
	 } else		/*	memory was allocated for Mask BitMap ok */
	 {
	 	CalcOffScreen(srcRect, &needed, &rowbytes);
	 	words = rowbytes / 2;
	 	height = srcRect->bottom - srcRect->top;
	 	CalcMask(srcBits->baseAddr, maskBits->baseAddr, rowbytes, rowbytes, height, words);
	 }
} // NewMaskedBitMap
