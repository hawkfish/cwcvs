#include "StPtr.h"

#include <MacMemory.h>

// ---------------------------------------------------------------------------
//		� StPtr
// ---------------------------------------------------------------------------

StPtr::StPtr (

	Ptr	inP)
	
	: mP (inP)
	
	{ // begin StPtr
		
	} // end StPtr

// ---------------------------------------------------------------------------
//		� ~StPtr
// ---------------------------------------------------------------------------

StPtr::~StPtr (void)
	
	{ // begin ~StPtr
		
		if (mP) ::DisposePtr (mP);
		mP = nil;
		
	} // end ~StPtr
