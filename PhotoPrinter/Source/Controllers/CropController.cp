/*
	File:		CropController.cp

	Contains:	Crop tool controller for Kilt

	Written by:	David Dunham and Dav Lion

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

		21 aug 2001		dml		275, 282.  crop-zoom rewrite.  ClickItem must calc crop-zoom-rect, since might be expanded
		03 aug 2001		dml		ability to expand crop handles!  also, handles drawn only around CropRect
		02 aug 2001		dml		272.  not "this->MakeCropAction, but "CropController::MakeCropAction"
		24 Jul 2001		drd		127 DoClickHandle erases old handles (and uses antsy)
		23 Jul 2001		rmgw	Get document from view.
		18 Jul 2001		rmgw	Split up ImageActions.
		05 jul 2001		dml		68.  fix crop on drag
		05 jul 2001		dml		120.  don't install an empty-crop region even if rect was valid
		02 jul 2001		dml		57.  Set resolution of DrawProperties in DoClickItem loop
		28 mar 2001		dml		use GetBodyToScreenMatrix
		02 mar 2001		dml		move inversematrix out of loop
		26 feb 2001		dml		cleanup for refactoring, make GetMatrix use explicit desires
		05 Oct 2000		drd		Added using for min, max
		22 Sep 2000		drd		DrawXformedRect is now in PhotoUtility
		18 Sep 2000		drd		Avoid warning by including ESpinCursor.h (why?)
		31 aug 2000		dml		cropping via handle must preserve offsets!
		30 Aug 2000		drd		Changed superclass to PhotoController
		25 Aug 2000		drd		ClickEventT now derived from SMouseDownEvent
		23 aug 2000		dml		move DrawXFormedRect to PhotoController
		23 aug 2000		dml		remember to crop the draw when handing
		23 aug 2000		dml		crop-hand-drag works
		23 Aug 2000		drd		AdjustCursorSelf: removed extra InterpretClick, only show hand
								if we are cropped
		22 aug 2000		dml		allow multiple crops to work
		21 aug 2000		dml		make work with rotation (based on old PhotoPrintController)
		21 aug 2000		dml		handle scrolled view
		15 Aug 2000		drd		Added factory method MakeCropAction; clicking selects
		14 Aug 2000		drd		First cut at cropping
		11 Aug 2000		drd		Created
*/

#include "CropController.h"

#include "CropAction.h"
#include "PhotoPrintDoc.h"
#include "PhotoPrintResources.h"
#include "PhotoPrintView.h"
#include "PhotoUtility.h"
#include "MNewRegion.h"
#include "PhotoPrintCommands.h"

using std::max;
using std::min;

/*
CropController
*/
CropController::CropController(PhotoPrintView* inView)
	: PhotoController(inView)
{
}//end ct

/*
~CropController
*/
CropController::~CropController()
{
}//end dt

/*
AdjustCursorSelf {OVERRIDE}
*/
void	
CropController::AdjustCursorSelf(const Point& inViewPt)
{
	// See if we're over an image
	ClickEventT		clickEvent;
	clickEvent.whereLocal = inViewPt;
	// Analyze what the mouse position represents
	this->InterpretClick(clickEvent);
	if (clickEvent.type == kClickInsideItem) {
		if (clickEvent.target.item->HasCrop())
			UCursor::SetTheCursor(curs_Hand);
		else
			::InitCursor();
	} else if (clickEvent.type == kClickOnHandle)
		UCursor::SetTheCursor(curs_Crop);
	else
		::InitCursor();
} // AdjustCursorSelf



/*
* CalculateHandlesForItem OVERRIDE
* override to make handles around image rect only
*/
void 
CropController::CalculateHandlesForItem(PhotoItemRef item, HandlesT& outHandles) const {
	MRect	rDest;
	
	item->DeriveCropRect(rDest);

	CalculateHandlesForRect(rDest, outHandles);

	MatrixRecord mat;
	item->GetMatrix(&mat, kIgnoreScale, kDoRotation);
	MatrixRecord paperToScreen;
	mView->GetBodyToScreenMatrix(paperToScreen);
	::ConcatMatrix(&paperToScreen, &mat);
	TransformPoints (&mat, outHandles, kFnordHandle); 

	}//end CalculateHandlesForItem


void
CropController::CalculateNewOffsets(const MRect& oldCrop, const MRect& newCrop, 
									const ClickEventT& inEvent, 
									double& newTopOffset, double& newLeftOffset) {

	PhotoItemRef	image (inEvent.target.item);

	// calc new offsets IFF either crop dimension is bigger than before
	// bounds are the new cropRect, in normalized coords
	// we know the old offsets, which are %ages based on the old dimensions.
	// recalculate them as %ages based off the new dimensions


	double oldTopOffset;
	double oldLeftOffset;
	image->GetCropZoomOffset(oldTopOffset, oldLeftOffset);
	newTopOffset = oldTopOffset;
	newLeftOffset = oldLeftOffset;
	
	MRect imageRect;
	if (image->HasZoom()) {
		ERect32 cropZoom;
		ERect32 expandedOffsetImage;
		image->DeriveCropZoomRect(cropZoom, expandedOffsetImage);
		imageRect.top = cropZoom.top;
		imageRect.left = cropZoom.left;
		imageRect.bottom = cropZoom.bottom;
		imageRect.right = cropZoom.right;
		}//endif crop-zoom in effect
	else
		imageRect = image->GetImageRect();

	if (oldCrop.Height() < newCrop.Height()) {

		// did top or bottom of crop move?
		if (oldCrop.top > newCrop.top) {
			//top moved.  make sure there is no white space at top of frame
			double maxTopOffset = (newCrop.top - imageRect.top) / (double)imageRect.Height();
			if (oldTopOffset > maxTopOffset)
				newTopOffset = maxTopOffset;
			else
				newTopOffset = oldTopOffset;
			}//endif top moved
		else {
			//top moved.  make sure there is no white space at top of frame
			double maxBotOffset = (newCrop.bottom - imageRect.bottom) / (double)imageRect.Height();
			if (oldTopOffset < maxBotOffset)
				newTopOffset = maxBotOffset;
			else
				newTopOffset = oldTopOffset;
			}//else bottom moved
		}//endif top or bottom moved
	else
		newTopOffset = oldTopOffset;


	if (oldCrop.Width() < newCrop.Width()) {
		if (oldCrop.left > newCrop.left) {
			double maxLeftOffset = (newCrop.left - imageRect.left) /  (double) imageRect.Width();
			if (oldLeftOffset > maxLeftOffset)
				newLeftOffset = maxLeftOffset;
			else
				newLeftOffset = oldLeftOffset;
			}//endif left moved
		else {
			double maxRightOffset = (newCrop.right - imageRect.right) / (double)imageRect.Width();
			if (oldLeftOffset < maxRightOffset)
				newLeftOffset = maxRightOffset;
			else
				newLeftOffset = oldLeftOffset;
				
			}//else right moved
		}//endif right or left moved


	}//end CalculateNewOffsets



//---------------------------------------------------------
// ClampPointBetween
//---------------------------------------------------------
void
CropController::ClampPointBetween(Point& ioClamp, const Point& minPoint, const Point& maxPoint) {
	ioClamp.h = max(ioClamp.h, minPoint.h);
	ioClamp.v = max(ioClamp.v, minPoint.v);
	ioClamp.h = min(ioClamp.h, maxPoint.h);
	ioClamp.v = min(ioClamp.v, maxPoint.v);
}//end ClampPointBetween


/*
DoClickHandle
	Dragging a handle means crop in that direction
	Assumes that the view is focused
*/
void 
CropController::DoClickHandle(ClickEventT& inEvent)
{
	Point			last = inEvent.whereLocal;
	PhotoItemRef	image (inEvent.target.item);
	MRect			bounds;
	double			rot (image->GetRotation());
	double 			skew (image->GetSkew());
	MatrixRecord	mat;
	MatrixRecord	inverse;
	HandlesT		handles;
	MRect			cropRect;
	ERect32			crop32;
	ERect32			image32;

	// Get rid of original handles
	CalculateHandlesForItem(inEvent.target.item, handles);
	this->DrawHandles(handles, inEvent.target.item->GetRotation());

	// setup handles and matrix for cropRect
	image->DeriveCropRect(bounds);
	image->DeriveCropZoomRect(crop32, image32);
	cropRect.top = crop32.top;
	cropRect.left = crop32.left;
	cropRect.bottom = crop32.bottom;
	cropRect.right = crop32.right;

	SetupDestMatrix(&mat, rot, skew, image->GetImageRect().MidPoint(), true);	
	Boolean happy (::InverseMatrix (&mat, &inverse));

	// determine the min + max points that a handle can be dragged in normalized coords
	// by getting the image rect.  it's in normalized coordinates, but that's ok in loop below after inverse xform
	MRect imageRect (image->GetImageRect());

	// Draw new ones
	this->RecalcHandlesForDestMatrix(handles, bounds, &mat);		
	this->DrawHandles(handles, inEvent.target.item->GetRotation(), kMarchingAnts);

	while (::StillDown()) {
		Point		dragged;
		::GetMouse(&dragged);
		if (::EqualPt(last, dragged))
			continue;
		last = dragged;
		
		// Undraw the handles
		this->DrawHandles(handles, inEvent.target.item->GetRotation(), kMarchingAnts);

		// xform the point by the inverse of the current matrix
		if (happy) {
			::TransformPoints(&inverse, &dragged, 1);
			}//endif able to transform

		// clamp point to image rect bounds
		ClampPointBetween(dragged, cropRect.TopLeft(), cropRect.BotRight());
				
		// based on which handle, and the xformed point, update the rect
		UpdateDraggedRect(inEvent.target.handle,
						bounds,
						dragged);
	
		//compute and draw the handles
		RecalcHandlesForDestMatrix(handles, bounds, &mat);
		this->DrawHandles(handles, inEvent.target.item->GetRotation(), kMarchingAnts);
	}

	bool forceRefreshToCleanupDrag (true);

	double newTopOffset (0.0);
	double newLeftOffset (0.0);
	MRect oldCrop;
	image->DeriveCropRect(oldCrop);
	CalculateNewOffsets(oldCrop, bounds, inEvent, newTopOffset, newLeftOffset);

	if (bounds) {
		PhotoPrintDoc*	doc = mView->GetDocument();
		doc->PostAction(this->MakeCropAction(bounds, newTopOffset, newLeftOffset));
		forceRefreshToCleanupDrag = false;
		}//endif bounds aren't empty


	if (forceRefreshToCleanupDrag) 
		this->DrawHandles(handles, inEvent.target.item->GetRotation());

}//end DoClickHandle


/*
DoClickItem {OVERRIDE}
*/
void 
CropController::DoClickItem(ClickEventT& inEvent)
{
	if (inEvent.target.item == mView->GetPrimarySelection()) {
		PhotoItemRef	image (inEvent.target.item);
		MRect			bounds = image->GetImageRect();
		MRect			offsetExpanded;
		image->GetExpandedOffsetImageRect(offsetExpanded);
		double oldTopOffset;
		double oldLeftOffset;
		image->GetCropZoomOffset(oldTopOffset, oldLeftOffset);
		Point	start = inEvent.whereLocal;
		Point	vanillaStart (start);
		
		// convert starting point to normalized coordinate system
		MatrixRecord	inverse;
		MatrixRecord	imageMatrix;
		image->GetMatrix(&imageMatrix, kIgnoreScale, kDoRotation);
		Boolean inverseAvail (::InverseMatrix (&imageMatrix, &inverse));
    	if (inverseAvail) {
    		::TransformPoints(&inverse, &start, 1);
    		}//endif inverse matrix possible

		double newTopOffset (oldTopOffset);
		double newLeftOffset (oldLeftOffset);

		//figure out what crop rect
		MRect cropRect;
		image->DeriveCropRect(cropRect);
		
		ERect32 cropZoomRect;
		ERect32 offsetExpandedImageRect;
		if (!image->HasZoom()) {
			cropZoomRect = cropRect;
			offsetExpandedImageRect = offsetExpanded;			
			}//endif
		else {
			image->DeriveCropZoomRect(cropZoomRect, offsetExpandedImageRect);
			//apply crop to cropZoomRect, since it is max bounds avail uncropped
			double topCrop;
			double leftCrop;
			double bottomCrop;
			double rightCrop;
			image->GetCrop(topCrop, leftCrop, bottomCrop, rightCrop);
			// cheat a little.  we already have cropRect.  use it's coordinates where appropriate
			if (!PhotoUtility::DoubleEqual(topCrop, 0.0))
				cropZoomRect.top = cropRect.top;
			if (!PhotoUtility::DoubleEqual(leftCrop, 0.0))
				cropZoomRect.left = cropRect.left;
			if (!PhotoUtility::DoubleEqual(bottomCrop, 0.0))
				cropZoomRect.bottom = cropRect.bottom;
			if (!PhotoUtility::DoubleEqual(rightCrop, 0.0))
				cropZoomRect.right = cropRect.right;
			}
			
		// compute the farthest points that we can drag (in normal space!)
		// these are the mouse drags which would place topleft of image at topleft of crop
		// and botright of image at botright of crop
		Point minPoint;
		Point maxPoint;
		minPoint.h =   start.h - (offsetExpandedImageRect.right - cropZoomRect.right);
		minPoint.v =   start.v - (offsetExpandedImageRect.bottom - cropZoomRect.bottom);
		maxPoint.h =   start.h + (cropZoomRect.left - offsetExpandedImageRect.left);
		maxPoint.v =   start.v + (cropZoomRect.top - offsetExpandedImageRect.top);
				
		StColorPenState		savePen;
		::PenMode(patXor);

		MRect viewRevealed;
		mView->CalcRevealedRect();
		mView->GetRevealedRect(viewRevealed);
		Point viewOrigin;
		mView->GetPortOrigin(viewOrigin);
		viewRevealed.Offset(viewOrigin.h, viewOrigin.v);
		
		MNewRegion			clip;
		clip = viewRevealed;

		PhotoDrawingProperties	props (kNotPrinting, kPreview, kDraft, 
										mView->GetDocument()->GetResolution());

		// inside this loop we will draw to the screen
		// that will require a call to GetBodyToScreenMatrix
		MatrixRecord bodyToScreenCorrection;
		mView->GetBodyToScreenMatrix(bodyToScreenCorrection);

		Point vanillaDragged;
		Point		dragged;
		while (::StillDown()) {
			::GetMouse(&dragged);
			vanillaDragged = dragged;
						
			// convert from xformed space (where dragged) to normalized space
			if (inverseAvail)
				::TransformPoints(&inverse, &dragged, 1);
			ClampPointBetween(dragged, minPoint, maxPoint);
			
			Point offset;
			offset.h = dragged.h - start.h;
			offset.v = dragged.v - start.v;
		
			// figure out offset as percentage
			newTopOffset = (offset.v / ((double)offsetExpandedImageRect.Height() )) + oldTopOffset;
			newLeftOffset = (offset.h / ((double)offsetExpandedImageRect.Width() )) + oldLeftOffset;

			image->SetCropZoomOffset(newTopOffset, newLeftOffset);
			image->Draw(props, &bodyToScreenCorrection, 0, 0, clip);						
			} // while stilldown
		
		//RESTORE the image's offsets
		image->SetCropZoomOffset(oldTopOffset,  oldLeftOffset);
		if (!(PhotoUtility::DoubleEqual(oldTopOffset, newTopOffset) &&
			PhotoUtility::DoubleEqual(oldLeftOffset, newLeftOffset))) {
			PhotoPrintDoc*	doc = mView->GetDocument();
			doc->PostAction(CropController::MakeCropAction(cropRect, newTopOffset, newLeftOffset));
			}//endif ants isn't empty
		}//endif clicked on the primary selection

	else
		PhotoController::DoClickItem(inEvent);		// Call inherited
}//end DoClickItem


/*
HandleClick {OVERRIDE}
*/
void 
CropController::HandleClick(const SMouseDownEvent &inMouseDown, const MRect& inBounds,
								SInt16 inClickCount)
{
	mBounds = inBounds;

	// Build our parameter block -- the first part is just the SMouseDownEvent
	ClickEventT		clickEvent;
	::BlockMoveData(&inMouseDown, &clickEvent, sizeof(SMouseDownEvent));
	// And fill in the rest (analyze what the click represents)
	this->InterpretClick(clickEvent);
	
	switch (clickEvent.type) {
		case kClickInsideItem:
			if (inClickCount == 1)
				this->DoClickItem(clickEvent);
			else {
				PhotoPrintDoc*		doc = mView->GetDocument();
				doc->ProcessCommand(cmd_ImageOptions, nil);
				}//else it's a multi-click, bring up the image options dialog
			break;

		case kClickOnHandle:
			this->DoClickHandle(clickEvent);
			break;

		case kClickEmpty:
			DoClickEmpty(clickEvent);
			break;

		default:
			break;
	}//end switch
}//end HandleClick

/*
MakeCropAction
*/
LAction*
CropController::MakeCropAction(const MRect&	inNewCrop,
								const double inNewTopOffset,
								const double inNewLeftOffset)
{
	PhotoPrintDoc*	doc = mView->GetDocument();
	return new CropAction(doc, si_Crop, inNewCrop, inNewTopOffset, inNewLeftOffset);
} // MakeCropAction


//---------------------------------------
// UpdateDraggedRect
// one of the handles has been moved.  figure out what the bounding rect is
//---------------------------------------

void
CropController::UpdateDraggedRect(const HandleType& handle,
								MRect& bounds,
								const Point& dragged)
{	
	switch (handle) {
		case kTopLeft:
			bounds.top = dragged.v;
			bounds.left = dragged.h;
			break;

		case kTopMid:
			bounds.top = dragged.v;
			break;

		case kTopRight:
			bounds.top = dragged.v;
			bounds.right = dragged.h;
			break;

		case kMidLeft:
			bounds.left = dragged.h;
			break;

		case kMidRight:
			bounds.right = dragged.h;
			break;

		case kBotLeft:
			bounds.bottom = dragged.v;
			bounds.left = dragged.h;
			break;

		case kBotMid:
			bounds.bottom = dragged.v;
			break;

		case kBotRight:
			bounds.bottom = dragged.v;
			bounds.right = dragged.h;
			break;
	}//end switch
}//end UpdateDraggedRect
