/*
	File:		RotateController.h

	Contains:	rotation controller for kilt

	Written by:	dav lion

	Copyright:	Copyright �2000 by Electric Fish, Inc.  All Rights reserved.

	Change History (most recent first):

		31 aug 2001		dml		add GetSelectionRectangle
		03 aug 2001		dml		add ApplyCrop
		31 jul 2001		dml		override CalculateHandlesForItem.  
		09 mar 2001		dml		add DoClickItem
		30 Aug 2000		drd		Changed superclass to PhotoController
		15 Aug 2000		dml		Created
*/

#pragma once

#include "PhotoController.h"

class RotateController : public PhotoController {

	protected:
		virtual void ApplyCrop(MRect& ioRect, const PhotoItemRef item);
		virtual void DoClickBoundingLine(ClickEventT& inEvent); 
		virtual void DoClickItem(ClickEventT& inEvent);
		virtual void DoClickHandle(ClickEventT& inEvent);
		virtual void DoRotate(ClickEventT& inEvent);
		virtual void GetSelectionRectangle(const PhotoItemRef item, MRect& rDest) const;
		virtual LAction*	MakeRotateAction(double inRot, const Rect* inDest);
		virtual void CalculateHandlesForItem(PhotoItemRef item, HandlesT& outHandles) const;
	
	public:
		
				RotateController(PhotoPrintView* inView);
		virtual ~RotateController();
			
		virtual void HandleClick(const SMouseDownEvent &inMouseDown, const MRect& inBounds,
									SInt16 inClickCount);
		virtual	void	AdjustCursorSelf(const Point& inViewPt);
		virtual void	Select(PhotoItemList newSelection, bool inRefresh = true);
		
		};//end RotateController