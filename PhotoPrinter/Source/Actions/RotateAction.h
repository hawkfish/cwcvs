/*
	File:		RotateAction.h

	Contains:	Interface to the rotate item action.

	Written by:	David Dunham and Dav Lion

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

		20 Aug 2001		rmgw	Fix Undo orientation changes.  Bug #339.
		23 jul 2001		dml		179 add RefreshItemOrLayout
		18 Jul 2001		rmgw	Split up ImageActions.
*/

#pragma once

#include "ImageAction.h"

class	RotateAction : public ImageAction
{
public:
						RotateAction(
									PhotoPrintDoc*	inDoc,
									const SInt16	inStringIndex,
									double inRot,
									const Rect* inDest = nil);
						~RotateAction();

protected:
	double		mUndoRot;
	MRect		mUndoDest;

	// LAction
	virtual	void		RedoSelf();
	virtual	void		UndoSelf();
};
