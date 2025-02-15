/*
	File:		CropAction.cp

	Contains:	Implementation of the crop action.

	Written by:	David Dunham and Dav Lion

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

		31 aug 2001		dml		275, 282.  also, move CalcCropValuesAsPercentages to PhotoUtility
		21 aug 2001		dml		don't clamp to bounds in CalcCropValues call 
		07 aug 2001		dml		add clampToBounds arg to CalculateCropValuesAsPercentages
		24 Jul 2001		rmgw	Refresh the image.  Bug #220.
		24 Jul 2001		rmgw	Undo dirty state correctly.
		18 Jul 2001		rmgw	Provide accessors for MVC values.
		18 Jul 2001		rmgw	Split up ImageActions.
*/

#include "CropAction.h"

#include "PhotoPrintDoc.h"
#include "PhotoPrintModel.h"

#include "ERect32.h"

/*
CropAction
*/
CropAction::CropAction(
	PhotoPrintDoc*	inDoc,
	const SInt16	inStringIndex,
	const MRect&	inNewCrop,
	const double	inTopOffset,
	const double	inLeftOffset)
	: ImageAction(inDoc, inStringIndex)
	, mNewTopOffset (inTopOffset)
	, mNewLeftOffset (inLeftOffset)
{
	mImage->GetCrop(mOldTopCrop, mOldLeftCrop, mOldBottomCrop, mOldRightCrop);
	mImage->GetCropZoomOffset(mOldTopOffset, mOldLeftOffset);
	ERect32	newCrop32		(inNewCrop);
	ERect32 image			(mImage->GetImageRect());
	
	PhotoUtility::CalcCropValuesAsPercentages(newCrop32, image, mNewTopCrop, mNewLeftCrop, 
												mNewBottomCrop, mNewRightCrop, kDontClampToBounds);	
} // CropAction


CropAction::~CropAction()
{
} // ~CropAction




/*
RedoSelf {OVERRIDE}
*/
void
CropAction::RedoSelf()
{
		//	Get the new undo state
	bool				mRedoDirty (GetCurrentDirty ());

		//	Swap the values
	mImage->SetCrop(mNewTopCrop, mNewLeftCrop, mNewBottomCrop, mNewRightCrop);
	mImage->SetCropZoomOffset(mNewTopOffset, mNewLeftOffset);

		//	Redraw it
	RefreshImage ();

		//	Restore the dirty flag
	GetDocument ()->SetDirty (mUndoDirty);
	
		//	Swap the state
	mUndoDirty = mRedoDirty;

} // RedoSelf


/*
UndoSelf {OVERRIDE}
*/
void
CropAction::UndoSelf()
{
		//	Get the new undo state
	bool				mRedoDirty (GetCurrentDirty ());

		//	Swap the values
	mImage->SetCrop(mOldTopCrop, mOldLeftCrop, mOldBottomCrop, mOldRightCrop);
	mImage->SetCropZoomOffset(mOldTopOffset, mOldLeftOffset);

		//	Redraw it
	RefreshImage ();

		//	Restore the dirty flag
	GetDocument ()->SetDirty (mUndoDirty);
	
		//	Swap the state
	mUndoDirty = mRedoDirty;

} // UndoSelf

