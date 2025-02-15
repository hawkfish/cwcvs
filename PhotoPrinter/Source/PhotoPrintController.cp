/*
	File:		PhotoPrintController.cp

	Contains:	controller for items

	Written by:	Dav Lion and David Dunham

	Copyright:	Copyright �2000 by Electric Fish, Inc.  All Rights reserved.

	Change History (most recent first):

	11 Aug 2000		drd		curs_ now in PhotoPrintResources.h, tool_ now in PhotoPrintCommands.h
	03 aug 2000		dml		Select is now list based
	20 Jul 2000		drd		AdjustCursor (initial version)
	11 jul 2000		dml		begin rewrite to use ImageRect instead of DestRect
	26 Jun 2000		drd		Use double, not float
	19 june 2000	dml		added DoCrop method, fixed bugs in PointLineDistance (degenerate cases)
*/

#include "PhotoPrintController.h"

#include <math.h>
#include "PhotoPrintApp.h"
#include "PhotoPrintCommands.h"
#include "PhotoPrintResources.h"
#include "PhotoPrintView.h"
#include <stdio.h>
#include <UDebugging.h>
#include <UNavServicesDialogs.h>

const double kRad2Degrees = 57.2958;	// !!! move to a header



//--------------------------------------------------------
// PhotoPrintController
//--------------------------------------------------------
PhotoPrintController::PhotoPrintController(PhotoPrintView* inView, PhotoPrintModel* inModel)
	: mView (inView)
	, mModel (inModel)
{
	MRect	emptyRect;
	RecalcHandles (emptyRect); // start handles out as empty so they won't draw
	}//end ct
	
	
//--------------------------------------------------------
// ~PhotoPrintController
//--------------------------------------------------------
PhotoPrintController::~PhotoPrintController() {
	}//end dt


//--------------------------------------------------------
// Selection
//--------------------------------------------------------
PhotoItemRef
PhotoPrintController::Selection()
{
	return mView->GetPrimarySelection();
	}//end Selection

//------------------------------------------------------------------
// AdjustCursor
//------------------------------------------------------------------
void
PhotoPrintController::AdjustCursor(const Point& inPortPt)
{
	PhotoItemRef			primarySelection (Selection());
	
	this->SetupHandlesForNewSelection(primarySelection); // if there is a selection, setup the handles for it
														// so that we can determine which one (might be) hit.

	OSType				curTool = PhotoPrintApp::GetCurrentTool();
	ResIDT				desired = 0;		// 0 means arrow
	HandleType			whichHandle = kFnordHandle;
	BoundingLineType	whichLine = kFnordLine;
	switch (this->OperationFromClick(inPortPt, whichHandle, whichLine, primarySelection)) {
		case kResizeOperation:
			if (curTool == tool_Crop)
				desired = curs_Crop;
			break;

		case kCropOperation:
		case kMoveOperation:
			if (curTool == tool_Crop || curTool == tool_Zoom)
				desired = crossCursor;
			break;
	}

	UCursor::SetTheCursor(desired);

} // AdjustCursor

//------------------------------------------------------------------
// HandleClick
//------------------------------------------------------------------
void		
PhotoPrintController::HandleClick(const SMouseDownEvent &inMouseDown,
									const MRect& inBounds)
{
	if (!mModel) // make sure we are connected to something!
		return;
		
	mBounds = inBounds;
	PhotoItemRef			primarySelection (Selection());

	SetupHandlesForNewSelection(primarySelection); // if there is a selection, setup the handles for it
											// so that we can determine which one (might be) hit.

	Point	starting (inMouseDown.whereLocal);
	HandleType	whichHandle (kFnordHandle);
	BoundingLineType	whichLine (kFnordLine);

	bool handled (true);
	switch (OperationFromClick(starting, whichHandle, whichLine, primarySelection)) {
		case kSelectOperation:
			DoSelect(primarySelection);
			break;
		case kPlaceOperation:
			DoPlace(starting);
			break;
		case kRotateOperation:
			DoRotate(starting, whichLine);
			break;
		case kSkewOperation:
			DoSkew(starting);
			break;
		case kResizeOperation:
			DoResize(starting, whichHandle);
			break;
		case kMoveOperation: 
			DoMove(starting);
			break;
		case kCropOperation:
			break;
		default:
			handled = false;
			return;
		}//end switch

	if (handled)
		mModel->SetDirty();	
}//end HandleClick


//------------------------------------------------------------------
//Select (utility routine)
//------------------------------------------------------------------
void
PhotoPrintController::Select(PhotoItemList newSelection, bool inRefresh) {
	for (PhotoIterator i = newSelection.begin(); i != newSelection.end(); ++i) {
		SetupHandlesForNewSelection(*i);
		if (inRefresh)
			DrawHandles();	
		}//for
	}//end Select


//------------------------------------------------------------------
//OperationFromClick
//------------------------------------------------------------------
PhotoPrintController::OperationType
PhotoPrintController::OperationFromClick(const Point& inPortPt, 
								HandleType& outHandle,
								BoundingLineType& outLine,
								PhotoItemRef& outSelect) {

	OperationType ret (kNullOperation);

	outHandle = kFnordHandle; // start off pessimistic

	// do tests on starting to see if it's resizing, rotating, skewing, etc
	do {
		// first see if it's on one of the handles
		for (int i = 0; i < kFnordHandle; ++i) {
			MRect rHandle(handles[i], handles[i]);
			rHandle.Inset(-5,-5);
			if (::PtInRect(inPortPt, &rHandle)) {
				outHandle = (HandleType)i;
				if (i == kMidMid) // middle handle is just a move
					return kMoveOperation;
				else
					return kResizeOperation; // rest are resize events
				}//endif found it
			}//for
		
		// next, see if it's on a line between handles (rotate)
		if (FindClosestLine(inPortPt, outLine) < 5.0){
			return kRotateOperation;
			}//endif topline rotate

		// is it inside the (transformed) bounding polygon
//		if (PointInsideItem(inPortPt, mView->GetSelection())) {
//			return kCropOperation; // was kMoveOperation
//			}//endif inside (but not on a handle)
			
		// nothing yet.  check the point against all known items (new selection?)
		for (PhotoIterator i = mModel->begin(); i != mModel->end(); ++i) {
			if (PointInsideItem(inPortPt, *i)) {
				outSelect = *i;
				return kSelectOperation;
				}//endif found it
			}//for all known items
			
		//else, last resort
//		ret = kPlaceOperation;
		ret = kCropOperation;
					
		} while (false);
		
	return ret;
	}//end OperationFromClick


#pragma mark -
//------------------------------------------------------------------
// DoCrop
//------------------------------------------------------------------
void
PhotoPrintController::DoCrop(const Point& /*start*/) {
#ifdef WRONG
	MRect	rSelection;
	StColorPenState	penState;
	penState.Normalize ();
	UMarchingAnts::UseAntsPattern ();
	::PenMode (srcXor);
	
	PhotoItemRef	selection (Selection());
	MatrixRecord	mat;
	double			rot (selection->GetRotation()); 
	double			skew (selection->GetSkew());
	
	while (::StillDown ()) {
		Point	dragged;
		::GetMouse (&dragged);
		
		MRect	rDragged (start, dragged);
		rDragged *= mBounds; // clamp to pane
		if (rDragged == rSelection) continue;
		
		SetupDestMatrix(&mat, rot, skew, rSelection.MidPoint(), true);
		RecalcHandles(rSelection, &mat);
		DrawHandles();
		rSelection = rDragged;
		SetupDestMatrix(&mat, rot, skew, rSelection.MidPoint(), true);
		RecalcHandles(rSelection, &mat);
		DrawHandles();
		} // while stilldown
	
	rSelection.Frame ();
	
	selection->SetCrop(rSelection);	
#endif
	}//end DoCrop

//------------------------------------------------------------------
// DoSelect
//------------------------------------------------------------------
void
PhotoPrintController::DoSelect(PhotoItemRef item) {
	PhotoItemList newList;
	newList.insert(newList.end(), item);
	mView->Select(newList);
	Select(newList);
	}//end DoSelect


//------------------------------------------------------------------
// DoPlace
//------------------------------------------------------------------
void
PhotoPrintController::DoPlace(const Point& start) {
	MRect	rSelection;
	StColorPenState	penState;
	penState.Normalize ();
	UMarchingAnts::UseAntsPattern ();
	::PenMode (srcXor);
	
	MatrixRecord	mat;
	double	rot (0.0); // no selection yet
	double	skew (0.0); // not yet
	
	while (::StillDown ()) {
		Point	dragged;
		::GetMouse (&dragged);
		
		MRect	rDragged (start, dragged);
		rDragged *= mBounds; // clamp to pane
		if (rDragged == rSelection) continue;
		
		SetupDestMatrix(&mat, rot, skew, rSelection.MidPoint(), true);
		RecalcHandles(rSelection, &mat);
		DrawHandles();
		rSelection = rDragged;
		SetupDestMatrix(&mat, rot, skew, rSelection.MidPoint(), true);
		RecalcHandles(rSelection, &mat);
		DrawHandles();
		} // while stilldown
	
	rSelection.Frame ();
	
	OSType	jpegType ('JPEG');
	MFileSpec spec;
	if (UNavServicesDialogs::AskChooseOneFile(jpegType,	spec)) {
		PhotoItemRef newItem = new PhotoPrintItem(spec);
		newItem->SetDest(rSelection);
		mModel->AdoptNewItem(newItem);
		PhotoItemList newList;
		newList.insert(newList.end(), newItem);
		mView->Select(newList);
		}//endif happily chose a jpeg
	
}//end DoPlace


//------------------------------------------------------------------
//DoRotate
//------------------------------------------------------------------
void
PhotoPrintController::DoRotate(const Point& start, BoundingLineType whichLine) {

	StColorPenState	penState;
	penState.Normalize ();
	UMarchingAnts::UseAntsPattern ();
	::PenMode (srcXor);
	
	MatrixRecord	mat;
	double	startingRot (Selection()->GetRotation());
	double	skew (Selection()->GetSkew());
	Point	last (start);
	Point	oldMid (Selection()->GetDestRect().MidPoint());
	MRect	dest (Selection()->GetDestRect());

	// if we're inside the midline (either one) then movement towards interior of bounds
	// is unchanged (negative == ccw rotation).  if however, we're on RHand portion of boundingline
	// (as determined by "outside" midline, then movement towards interior is positive == CW rot
	double rotationMultiplier (PointInsideMidline(start, whichLine) ? 1.0 : -1.0);

	Point startPoint;
	Point endPoint;
	bool inside (true);
	GetRotationSegment(whichLine, startPoint, endPoint);

	double	rot (0);
	while (::StillDown ()) {
		Point	dragged;
		::GetMouse (&dragged);
		if (::EqualPt(last, dragged))
			continue;

		double howFar = PointLineDistance(dragged, startPoint, endPoint, inside);

		SetupDestMatrix(&mat, rot + startingRot, skew, oldMid, true);
		RecalcHandles(dest, &mat);
		DrawHandles();

		rot = RotFromPointLine(dragged, startPoint, endPoint);
		rot *= rotationMultiplier;

		mView->AdjustTransforms(rot, skew, dest, Selection());

		SetupDestMatrix(&mat, rot + startingRot, skew, oldMid, true);
		RecalcHandles(dest, &mat);
		DrawHandles();
		last = dragged;
		}//end while stilldown
	
	UpdateModelSelection(rot + startingRot, skew, dest);
	}//end DoRotate


//------------------------------------------------------------------
//DoSkew
//------------------------------------------------------------------
void
PhotoPrintController::DoSkew(const Point& /*start*/) {
	DebugStr("\pSkew");
	}//end DoSkew


//------------------------------------------------------------------
//DoResize
//------------------------------------------------------------------
void
PhotoPrintController::DoResize(const Point& start, HandleType whichHandle) {

	StColorPenState	penState;
	penState.Normalize ();
	UMarchingAnts::UseAntsPattern ();
	::PenMode (srcXor);
	
	
	MatrixRecord	mat;

	Point			last (start);
	Point			oldMid (Selection()->GetDestRect().MidPoint());
	MatrixRecord 	inverseMat;
	bool 		bInverse (false);
	double 			rot (Selection()->GetRotation());
	MRect			dest (Selection()->GetDestRect());
	double			skew (Selection()->GetSkew());

	SetupDestMatrix(&mat, rot, skew, oldMid, true);
	if (::InverseMatrix(&mat, &inverseMat) == true) {
		bInverse = true;
		}//endif able to invert current xform matrix
	else {
		DebugStr("\pCouldn't make an inverseMatrix");
		}//else


	while (::StillDown ()) {
		Point	dragged;
		::GetMouse (&dragged);
		if (::EqualPt(last, dragged))
			continue;

		SetupDestMatrix(&mat, rot, skew, oldMid, true);
		RecalcHandles(dest, &mat);
		DrawHandles();

		// we have the point of the mouse click, but it's in xform space
		// and we want to modify non-xformed space
		// so transform it by the (current) inverse matrix
		Point vanilla (dragged);
		if (bInverse) {
			::TransformPoints(&inverseMat, &dragged, 1);
			}//endif able to invert current xform matrix

		switch (whichHandle) {
			case kTopLeft:
				if (dragged.v < dest.bottom)
					dest.top = dragged.v;
				if (dragged.h < dest.right)
					dest.left = dragged.h;
				break;
			case kTopMid:
				if (dragged.v < dest.bottom)
					dest.top = dragged.v;
				break;
			case kTopRight:
				if (dragged.v < dest.bottom)
					dest.top = dragged.v;
				if (dragged.h > dest.left)
					dest.right = dragged.h;
				break;
			case kMidLeft:
				if (dragged.h < dest.right)
					dest.left = dragged.h;
				break;
			case kMidMid:
				break;
			case kMidRight:
				if (dragged.h > dest.left)
					dest.right = dragged.h;
				break;
			case kBotLeft:
				if (dragged.v > dest.top)
					dest.bottom = dragged.v;
				if (dragged.h < dest.right)
					dest.left = dragged.h;
				break;
			case kBotMid:
				if (dragged.v > dest.top)
					dest.bottom = dragged.v;
				break;
			case kBotRight:
				if (dragged.v > dest.top)
					dest.bottom = dragged.v;
				if (dragged.h > dest.left)
					dest.right = dragged.h;
				break;		
			}//switch

		mView->AdjustTransforms(rot, skew, dest, Selection());

		SetupDestMatrix(&mat, rot, skew, oldMid, true);
		RecalcHandles(dest, &mat);
		DrawHandles();
		last = dragged;
		}//end while

	
	// if we are rotated, we must fixup
	// reconcile the difference between oldMid and newMid;
	DeconstructDestIntoComponents(dest, rot, skew);
	
	UpdateModelSelection(rot, skew, dest);
				
}// end DoResize



//------------------------------------------------------------------
//DoMove
//------------------------------------------------------------------
void
PhotoPrintController::DoMove(const Point& starting) {
	
	StColorPenState	penState;
	penState.Normalize ();
	UMarchingAnts::UseAntsPattern ();
	::PenMode (srcXor);
	
	MatrixRecord	mat;
	double 			rot (Selection()->GetRotation());
	double			skew (Selection()->GetSkew());
	MRect			dest (Selection()->GetDestRect());

	Point last (starting);
	while (::StillDown ()) {
		Point	dragged;
		::GetMouse (&dragged);
		if (::EqualPt(dragged,last))
			continue;

		SetupDestMatrix(&mat, rot, skew, dest.MidPoint(), true);
		RecalcHandles(dest, &mat);
		DrawHandles();

		SInt16 dH (dragged.h - last.h);
		SInt16 dV (dragged.v - last.v);
		
		dest.Offset(dH, dV);

		// clamp inside the frame
		if (dest.left < mBounds.left)
			dest.Offset(mBounds.left - dest.left, 0);
		if (dest.left > mBounds.right - dest.Width())
			dest.Offset(mBounds.right - dest.Width() - dest.left,0);
		if (dest.top < mBounds.top)
			dest.Offset(0, mBounds.top - dest.top);
		if (dest.top > mBounds.bottom - dest.Height())
			dest.Offset(0, mBounds.bottom - dest.Height() - dest.top);
		// don't allow them to drag off

		SetupDestMatrix(&mat, rot, skew, dest.MidPoint(), true);
		RecalcHandles(dest, &mat);
		DrawHandles();
		last = dragged;
		}//while
		
	UpdateModelSelection(rot, skew, dest);
		
	}//end DoMove


#pragma mark -
//------------------------------------------------------------------
// PointLineDistance
//		inside is defined as follows:  looking from startPoint to endPoint, inside is on right.
//------------------------------------------------------------------
double
PhotoPrintController::PointLineDistance(const Point p, const Point l1, const Point l2, bool& inside) {

	double distance (0.0);

	do {
		// degenerate cases first
		
		// vertical line
		if (l2.h == l1.h) {
			if (((l1.v >= p.v) && (p.v >= l2.v)) ||
				((l1.v <= p.v) && (p.v <= l2.v))) {
				distance = fabs((double)(p.h - l2.h));
				}//endif in between endpoints of vertical line, so distance is dh
			else {
				double d1 = (sqrt(((p.v - l1.v) * (p.v - l1.v)) + ((p.h - l1.h) * (p.h - l1.h))));
				double d2 = (sqrt(((p.v - l2.v) * (p.v - l2.v)) + ((p.h - l2.h) * (p.h - l2.h))));
				distance = min(d1,d2);
				}//else distance is min of distances from endpoints
			
			if (l2.v > l1.v)
				inside = p.h < l2.h ? true : false;
			else
				inside = p.h > l2.h ? true : false;
			continue;
			}//endif a vertical line

		if (l2.v == l1.v) {
			if (((l1.h >= p.h) && (p.h >= l2.h)) ||
				((l1.h <= p.h) && (p.h <= l2.h))) {
				distance = fabs((double)(p.v - l2.v));
				}//endif point is between endpoints, so distance is dv
			else {
				double d1 = (sqrt(((p.v - l1.v) * (p.v - l1.v)) + ((p.h - l1.h) * (p.h - l1.h))));
				double d2 = (sqrt(((p.v - l2.v) * (p.v - l2.v)) + ((p.h - l2.h) * (p.h - l2.h))));
				distance = min(d1,d2);				
				}//else distance is min of distances from endpoints
			
			if (l2.h > l1.h)
				inside = p.v > l2.v ? true : false;
			else
				inside = p.v < l2.v ? true : false;
			continue;
			}//endif it's a horizontal line

		//otherwise,  non-orthagonal line.
		
		double dx (l2.h - l1.h);
		double dy (l2.v - l1.v);
		double m = dy / dx;
		double b = (double)l1.v - (double)l1.h * m;

		// find negative recprocal slope of l1-l2
		double mPrime = -1.0 / m;
		// solve y = mPrimeX + b using p:  
		double bPrime;
		bPrime = (double)p.v - (double)p.h * mPrime;
		
		// compute intersection of lines.
		double x = (bPrime - b) / (m - mPrime);
		double y = (m * x) + b;

		distance = (sqrt(((p.v - y) * (p.v - y)) + ((p.h - x) * (p.h - x))));

		// we already have most of the coefficients for this test (though there is a function)
		double F ((dy * p.h) - (dx * p.v) + (b * dx));
		inside = F < 0.0 ? true : false;

		} while (false);
		

	// find distance from p to that intersection	
	return distance;
}//end PointLineDistance


//------------------------------------------------------------------
//DistanceFromBoundary
//------------------------------------------------------------------
/**
* Note all lines are Right handed (CW traversal define the bounding box)
*/
double
PhotoPrintController::DistanceFromBoundary(const Point& point, BoundingLineType whichLine, bool& inside) {
	switch (whichLine) {
		case kTopLine:
			return (PointLineDistance(point, handles[kTopLeft], handles[kTopRight], inside));
			break;
		case kRightLine:
			return (PointLineDistance(point, handles[kTopRight], handles[kBotRight], inside));
			break;
		case kBotLine:
			return (PointLineDistance(point, handles[kBotRight], handles[kBotLeft], inside));
			break;
		case kLeftLine:
			return (PointLineDistance(point, handles[kBotLeft], handles[kTopLeft], inside));
			break;
		case kVerticalMidline:
			return (PointLineDistance(point, handles[kTopMid], handles[kBotMid], inside));
			break;
		case kHorizontalMidline:
			return (PointLineDistance(point, handles[kMidLeft], handles[kMidRight], inside));
			break;
		default:
			DebugStr("\pBogus Argument to DistanceToBoundary");
		}//end switch	

	return (0.0);
	}//end DistanceFromBoundary


//------------------------------------------------------------------
//DeconstructDestIntoComponents
//------------------------------------------------------------------
/**
* we've just finished a resize operation
* the handles are correct, but dest isn't quite right
* because the rotation (in order to rubberband/draw the handles correctly)
* has ocurred around some point other than dest.MidPoint()
* So, based on the handles and dest, we reconstruct dest + associated placement values
* SKEW NOT YET IMPLEMENTED
*/
void
PhotoPrintController::DeconstructDestIntoComponents(MRect& dest, double rot, double /*skew*/) {
	long height;
	long width;
	
	height = dest.Height();
	width = dest.Width();


	Point newMiddle = dest.MidPoint();
	Point newTopLeft = handles[kTopLeft];
	MatrixRecord unRotate;
	::SetIdentityMatrix(&unRotate);
	::RotateMatrix(&unRotate, ::Long2Fix(-rot), 
					::Long2Fix(handles[kMidMid].h), ::Long2Fix(handles[kMidMid].v));
	::TransformPoints(&unRotate, &newTopLeft, 1);
	
	dest.Offset(newTopLeft.h - dest.left, newTopLeft.v - dest.top);
	
	}//end DeconstructDestIntoComponents()


//------------------------------------------------------------------
//FindClosestLine
//------------------------------------------------------------------
double
PhotoPrintController::FindClosestLine(const Point& starting, BoundingLineType& outLine) {	
	double shortest (0.0);
	double temp;
	bool inside;
	
	shortest = DistanceFromBoundary(starting, kTopLine, inside);
	outLine = kTopLine;
	
	temp = DistanceFromBoundary(starting, kBotLine, inside);
	if (temp < shortest) {
		outLine = kBotLine;
		shortest = temp;
		}//endif 
		
	
	temp = DistanceFromBoundary(starting, kLeftLine, inside);
	if (temp < shortest) {
		outLine = kLeftLine;
		shortest = temp;
		}//endif 
	
	temp = DistanceFromBoundary(starting, kRightLine, inside);
	if (temp < shortest) {
		outLine = kRightLine;
		shortest = temp;
		}//endif 

	return shortest;
}//end FindClosesLine



//------------------------------------------------------------------
//RotFromPointLine
// give a point and the line, find the angle between line from line's start to point
// and line
//------------------------------------------------------------------
double
PhotoPrintController::RotFromPointLine(const Point& start, const Point& startPoint,
							const Point& endPoint) {	
	// dot product is |a||b|cos theta
	// but we more easily can determine sin, since we have fund to determine perp distance.
	
	bool inside;
	double b = PointLineDistance(start, startPoint, endPoint, inside);

	double dx = (start.h - startPoint.h);
	double dy = (start.v - startPoint.v);
	double a (sqrt((dx * dx) + (dy * dy)));
	
	double sinTheta (b / a);
	double rotation ((asin(sinTheta) * kRad2Degrees));
	if (!inside)
		rotation *= -1.0;

	return rotation;

}//end RotationFromPointLine


//------------------------------------------------------------------
//GetRotationSegment
//------------------------------------------------------------------
void	
PhotoPrintController::GetRotationSegment(const BoundingLineType& whichLine, 
								Point& startPoint, Point& endPoint) {

	switch (whichLine) {
		case kTopLine:
				startPoint = handles[kTopLeft];
				endPoint = handles[kTopRight];
			break;
		case kRightLine:
				startPoint = handles[kTopRight];
				endPoint = handles[kBotRight];
			break;
		case kBotLine:
				startPoint = handles[kBotRight];
				endPoint = handles[kBotLeft];
			break;
		case kLeftLine:
				startPoint = handles[kBotLeft];
				endPoint = handles[kTopLeft];
			break;
		}//end switch

}//end GetRotationSegment


//------------------------------------------------------------------
//PointInsideItem
//------------------------------------------------------------------
bool	
PhotoPrintController::PointInsideItem(const Point& p, PhotoItemRef item)
{
	Point startPoint;
	Point endPoint;
	bool inside (true);
	
	if (item) {
		SetupHandlesForNewSelection(item);
		do {
			GetRotationSegment(kTopLine, startPoint, endPoint);
			PointLineDistance(p, startPoint, endPoint, inside);
			if (!inside)
				continue;
			GetRotationSegment(kRightLine, startPoint, endPoint);
			PointLineDistance(p, startPoint, endPoint, inside);
			if (!inside)
				continue;
			GetRotationSegment(kBotLine, startPoint, endPoint);
			PointLineDistance(p, startPoint, endPoint, inside);
			if (!inside)
				continue;
			GetRotationSegment(kLeftLine, startPoint, endPoint);
			PointLineDistance(p, startPoint, endPoint, inside);
			if (!inside)
				continue;
			} while (false);
		}//endif sane		
	else
		inside = false;	

	return inside;
	}//end PointInsideTransformedRect


//------------------------------------------------------------------
//PointInsideMidline
//------------------------------------------------------------------
bool
PhotoPrintController::PointInsideMidline(const Point& p, BoundingLineType whichLine) 
{
	// so, we're rotating.  figure out which side of midline we've grabbed.
	// inside is typically defined as "right side of", so we flip in 2 cases
	bool insideMidline;
	switch (whichLine) {
		case kTopLine:
			DistanceFromBoundary(p, kVerticalMidline, insideMidline);
			insideMidline = !insideMidline;// flip sense since reflected case
			break;
		case kBotLine:
			DistanceFromBoundary(p, kVerticalMidline, insideMidline);
			break;
		case kLeftLine:
			DistanceFromBoundary(p, kHorizontalMidline, insideMidline);
			insideMidline = !insideMidline;// flip sense since reflected case
			break;
		case kRightLine:
			DistanceFromBoundary(p, kHorizontalMidline, insideMidline);
			break;
		}//end switch	

	return insideMidline;
	}//end PointInsideMidline


#pragma mark -
//------------------------------------------------------------------
//SetupHandlesForNewSelection
//------------------------------------------------------------------
void
PhotoPrintController::SetupHandlesForNewSelection(PhotoItemRef selection) {
	if (selection) {
		MatrixRecord mat;
		selection->GetMatrix(&mat);
		SetupDestMatrix(&mat, selection->GetRotation(), selection->GetSkew(), 
						selection->GetImageRect().MidPoint(), true);
		RecalcHandles(selection->GetDestRect(), &mat);
		}//endif sane
	else {
		MRect	emptyRect;
		RecalcHandles (emptyRect); // start handles out as empty so they won't draw		
		}//else must be choosing no selection
	}//end SetupHandlesForNewSelection


//------------------------------------------------------------------
//SetupDestMatrix
//------------------------------------------------------------------
void
PhotoPrintController::SetupDestMatrix(MatrixRecord* pMatrix, double inRot, double /*skew*/,
										const Point& center,
										bool bInitialize) {
	if (bInitialize)
		::SetIdentityMatrix(pMatrix);
	::RotateMatrix (pMatrix, Long2Fix(inRot), Long2Fix(center.h), Long2Fix(center.v));
	}//end
	

//------------------------------------------------------------------
//RecalcHandles
//------------------------------------------------------------------
void
PhotoPrintController::RecalcHandles(const MRect& rDest, const MatrixRecord* pMatrix) {
	// MRect built-ins
	handles[kTopLeft] = rDest.TopLeft();
	handles[kBotRight] = rDest.BotRight();
	handles[kMidMid] = rDest.MidPoint();

	// derived
	handles[kTopMid].h = handles[kMidMid].h;
	handles[kTopMid].v = handles[kTopLeft].v;	
	
	handles[kTopRight].h = handles[kBotRight].h;
	handles[kTopRight].v = handles[kTopLeft].v;	
	
	handles[kMidLeft].h = handles[kTopLeft].h;
	handles[kMidLeft].v = handles[kMidMid].v;	

	handles[kMidRight].h = handles[kTopRight].h;
	handles[kMidRight].v = handles[kMidMid].v;	

	handles[kBotLeft].h = handles[kTopLeft].h;
	handles[kBotLeft].v = handles[kBotRight].v;
	
	handles[kBotMid].h = handles[kMidMid].h;
	handles[kBotMid].v = handles[kBotRight].v;

	if (pMatrix)
		TransformPoints (pMatrix, handles, kFnordHandle); 
	}//end RecalcHandles


//------------------------------------------------------------------
//DrawHandles
//------------------------------------------------------------------
void
PhotoPrintController::DrawHandles() {

	bool realData (false);
	Point	emptyPoint;
	emptyPoint.v = emptyPoint.h = 0;
	
	for (int i = 0; i < kFnordHandle; ++i) {
		MRect rHandle(handles[i], handles[i]);
		if (!::EqualPt(handles[i], emptyPoint)) { // make sure some handle is real
			realData = true;
			rHandle.Inset(-5,-5);
			rHandle.Frame();
			}//endif sane
		}//for all handles

	if (realData) {		
		// draw the lines between the handles
		::MoveTo(handles[kTopLeft].h, handles[kTopLeft].v);
		::LineTo(handles[kTopRight].h, handles[kTopRight].v);

		::MoveTo(handles[kTopRight].h, handles[kTopRight].v);
		::LineTo(handles[kBotRight].h, handles[kBotRight].v);

		::MoveTo(handles[kBotRight].h, handles[kBotRight].v);
		::LineTo(handles[kBotLeft].h, handles[kBotLeft].v);

		::MoveTo(handles[kBotLeft].h, handles[kBotLeft].v);
		::LineTo(handles[kTopLeft].h, handles[kTopLeft].v);
		}//endif there is something to draw
		
	}//end DrawHandles;


//------------------------------------------------------------------
//UpdateModelSelection
//------------------------------------------------------------------
void
PhotoPrintController::UpdateModelSelection(double rot, double skew, const MRect& r)
{
	PhotoItemRef	selection (Selection());
	if (selection) {
		selection->SetDest(r);
		selection->SetRotation(rot);
		selection->SetSkew(skew);
		}//endif sane

}//end UpdateModelSelection


