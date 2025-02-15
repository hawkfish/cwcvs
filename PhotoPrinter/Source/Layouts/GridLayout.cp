/*
	File:		GridLayout.cp

	Contains:	Implementation of Layout object, which manages positioning of images.
				GridLayout keeps multiple images in a best-fit grid

	Written by:	David Dunham and Dav Lion

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

		16 Aug 2001		rmgw	Broadcast layout changes.  Bug #332.
		10 Aug 2001		drd		305 Handle empty page in AdjustDocumentOrientation,
								CalcRowsColsOrientation, LayoutImages
		02 Aug 2001		rmgw	Implement Initialize.  Bug #273.
		25 Jul 2001		rmgw	Remove tail recursion from CalculateGrid.  Bug #207.
		25 Jul 2001		drd		15 Use ESpinCursor::SpinWatch instead of UCursor::SetWatch
		23 jul 2001		dml		add CalcOrientation, make many funcs const
		23 Jul 2001		rmgw	Add doc and type to constructor.
		20 Jul 2001		rmgw	Include PhotoPrintDoc.  Bug #200.
		19 Jul 2001		drd		173 176 Test IsFlexible in CalculateGrid
		06 Jul 2001		drd		128 LayoutImages calls SetWatch
		03 jul 2001		dml		SetDest, SetMaxBounds take PhotoDrawingProperties
		02 Jul 2001		drd		Fixed incorrectly initialized var in CalcRowsColsOrientation
		23 Apr 2001		drd		Min, Max constraints come from document
		12 mar 2001		dml		CalculateGrid must also use aspect of body to force orientation of constraints
		09 mar 2001		dml		bug 34, bug 58.  LayoutImages offsets by Header.Height(), not body.top
		06 mar 2001		dml		CalcMaxBounds should use aspect of cell to determine aspect of constraint
		04 jan 2001		dml		CalculateGrid must swap minHeight, minWidth iff already landscape pre-cell-size check
		14 dec 2000		dml		better functioning w/ headers+footers
		07 dec 2000		dml		Offset Printable or Page, but not body
		05 dec 2000		dml		changes to LayoutImages.  more attentive to body rect vs page rect
		05 Oct 2000		drd		Added using for max, swap
		28 Sep 2000		drd		Rolled out the last change � it seemed to offset off the page
		21 sep 2000		dml		LayoutImages must use BodyRect (offset for proper page), not PageBounds
		19 Sep 2000		drd		ResizeImage
		14 Sep 2000		drd		Even more respectful of maximum size
		13 sep 2000		dml		add header/footer support and annoyingware.  Fix Zoom caused bug in LayoutPage
		11 sep 2000		dml		a bit longer of a short-circuit exit at top of LayoutImages ensures view rects are updated
		07 sep 2000		dml		better respect of MaxBounds
		31 aug 2000		dml		layout must take into account xformed naturalbounds (rotation changes bounding)
		16 Aug 2000		drd		Be sure LayoutPage doesn't wipe out placeholder
		15 Aug 2000		drd		Removed AddItem (no need to override)
		18 jul 2000		dml		CalculateGrid uses desired-orientation internally (even if not yet set)
								also, adds 2 to sqrt call (make 7 force 3 cols)
		17 Jul 2000		drd		Make better use of max size (in LayoutPage, not CalculateCellSize)
		14 Jul 2000		drd		AdjustDocumentOrientation calls SetOrientation twice; fixed 5/6
		14 Jul 2000		drd		Changed layout (CalculateGrid instead of CalculateRowsCols,
								MaxItemsPerPage)
		13 jul 2000		dml		removed side-effect from MaxItemsPerPage
		12 jul 2000		dml		more aux functions for multipage support
		11 Jul 2000		drd		Eliminate warnings
		11 jul 2000		dml		multipage support
		06 Jul 2000		drd		Override AdjustDocumentOrientation for 2-landscape special case
		28 jun 2000		dml		use EUtil symbolic constants, use FitRectInside instead of BestFit
		27 Jun 2000		drd		LayoutImages sends AdjustDocumentOrientation
		26 Jun 2000		drd		Get rid of a few conversion warnings
		23 Jun 2000		drd		Use HORef<PhotoPrintModel> in constructor
		21 jun 2000 	dml		BestFit not takes (optional) parm -- don't expand past natural bounds
		20 jun 2000		dml		using EUtil
		20 Jun 2000		drd		Rudimentary gridding works
		19 Jun 2000		drd		Created
*/

#include "GridLayout.h"
#include <cmath>
#include "ESpinCursor.h"
#include "EUtil.h"
#include "PhotoUtility.h"
#include "PhotoPrinter.h"
#include "PhotoPrintDoc.h"
#include "PhotoPrintPrefs.h"
#include "AlignmentGizmo.h"
#include "PrintProperties.h"

using std::max;
using std::swap;

/*
GridLayout
*/
GridLayout::GridLayout(

	PhotoPrintDoc*			inDoc, 
	HORef<PhotoPrintModel>& inModel,
	UInt32					inItemsPerPage,
	LayoutType				inType)

	: Layout (inDoc, inModel, inType)
	
	, mItemsPerPage (inItemsPerPage)
	, mSizeCode ('****')
{

} // GridLayout

/*
~GridLayout
*/
GridLayout::~GridLayout()
{
} // ~GridLayout

/*
AdjustDocumentOrientation {OVERRIDE}
	Special case to put two landscapes on portrait paper
	Side effect: sets mRows, mColumns
*/
void
GridLayout::AdjustDocumentOrientation(SInt16 /*numPages*/)
{
	// printable area (taking into account margins, etc)
	MRect		printableArea;
	EPrintSpec* spec = this->GetPrintRec();
	PhotoPrinter::CalculateBodyRect(spec, &(this->GetPrintProperties()), printableArea); // at 72dpi

	// Figure
	this->CalculateGrid(printableArea, mModel->GetCount(), mRows, mColumns, mOrientation);

	mItemsPerPage = this->GetRows() * this->GetColumns();
	mNumPages = mModel->GetCount() / mItemsPerPage;
	if (mNumPages * mItemsPerPage < mModel->GetCount()) 
		mNumPages++; 
	if (mNumPages < 1)
		mNumPages = 1;

	spec->SetOrientation(mOrientation, PhotoUtility::gNeedDoubleOrientationSetting);
	GetDocument ()->MatchViewToPrintRec(mNumPages); // do this anyway, since changes according to #pages
} // AdjustDocumentOrientation

//--------------------------------------------------------------
// CalculateCellSize
//--------------------------------------------------------------
void		
GridLayout::CalculateCellSize(
	const ERect32&	inPageSize,
	const SInt16	inRows,
	const SInt16	inCols,
	ERect32&		outCellSize,
	ERect32&		outUnusedBottomPad) const
{
	double hMin;
	double vMin;
	if (mSizeCode == '****') {
		PhotoItemProperties::SizeLimitToInches(GetDocument ()->GetMinimumSize(), hMin, vMin);
	} else {
		PhotoUtility::GetSize(mSizeCode, hMin, vMin);
	}

	// reinterpret cell constraints to match aspect ratio of page
	if ((inPageSize.Height() > inPageSize.Width()) &&
		(hMin < vMin)) {
		swap(hMin, vMin);
		}//endif swap to match aspect ratio
	

	// convert inches to screen resolution
	SInt16		resolution = this->GetResolution();
	hMin *= resolution;
	vMin *= resolution;
	
	// starting with page size, remove unavailable gutter
	SInt32		width = inPageSize.Width() - ((inCols - 1) * (this->GetGutter() * (kDPI / (double)resolution)));
	SInt32		height = inPageSize.Height() - ((inRows - 1) * (this->GetGutter() * (kDPI / (double)resolution)));

	//divide by rows, cols
	width /= inCols;
	height /= inRows;

	outCellSize.MakeEmpty();
	outCellSize.SetWidth(width);
	outCellSize.SetHeight(height);

	// Note that we don't care about maximum image size here, we are just allocating the grid

	//ok.  now that we know the cell size, determine how much space is unused at the bottom of the page
	outUnusedBottomPad.MakeEmpty();
	outUnusedBottomPad.SetWidth(inPageSize.Width());
	outUnusedBottomPad.SetHeight(inPageSize.Height() - (outCellSize.Height() * GetRows()) - ((GetRows() - 1) * GetGutter()));
	outUnusedBottomPad.Offset(0, inPageSize.bottom - outUnusedBottomPad.Height());
}//end CalculateCellSize

/*
CalcOrientation
*/
OSType		
GridLayout::CalcOrientation() const {
	SInt16 bogusRows (mRows);
	SInt16 bogusCols (mColumns);
	OSType orientation ;
	
	MRect		printableArea;
	EPrintSpec* spec = this->GetPrintRec();
	PhotoPrinter::CalculateBodyRect(spec, &(this->GetPrintProperties()), printableArea); // at 72dpi
	
	this->CalculateGrid(printableArea, mModel->GetCount(), bogusRows, bogusCols, orientation);

	return orientation;
}//end


/*
CalcRowsColsOrientation
*/
void
GridLayout::CalcRowsColsOrientation(const SInt32& inCount, SInt16& outRows, SInt16& outCols, 
									OSType& outOrientation, OSType& outConstraintOrientation) const
{
	bool 		forced = true;
	UInt32		l 		(CountOrientation(kLandscape));
	UInt32		p 		(CountOrientation(kPortrait));
	OSType		forcedOrientation;
	OSType		reversedOrientation;
	
	// first decision based on ratio of portrait to landscape items
	if (p > l) {
		outOrientation = kPortrait;
		reversedOrientation = kLandscape;
		outConstraintOrientation = kPortrait;
	} else {
		outOrientation = kLandscape;
		reversedOrientation = kPortrait;
		outConstraintOrientation = kLandscape;
	}

	// next we consider any document print settings (user might restrict paper's rotation)
	switch (this->GetPrintProperties().GetRotationBehavior()) {
		case PrintProperties::kForceLandscape:
			forcedOrientation = kLandscape;
			break;

		case PrintProperties::kForcePortrait:
			forcedOrientation = kPortrait;
			break;

		default:
			forcedOrientation = outOrientation;
			forced = false;
	}

	// finally, some heuristics as to spacing
	switch (inCount) {
		case 1:
			outRows = 1;
			outCols = 1;
			outOrientation = forcedOrientation;
			return;								// Nothing else to do! (Limits recursion)
			break;

		case 2:
			if (l == p || forced)				// Both have different orientation
				outOrientation = forcedOrientation;
			else
				outOrientation = reversedOrientation;
			if (outOrientation == kPortrait) {
				outRows = 2;
				outCols = 1;
			} else {
				outRows = 1;
				outCols = 2;
			}
			break;

		case 5:
		case 6:
			if (forced)
				outOrientation = forcedOrientation;
			else
				outOrientation = reversedOrientation;
			if (outOrientation == kPortrait) {
				outRows = 3;
				outCols = 2;
			} else {
				outRows = 2;
				outCols = 3;
			}
			break;

		default:
			// Just divide up the page, using the same orientation as the majority of the images
			outCols = std::sqrt(inCount + 2);		// was 1, but 7 needs to go to three cols
			outRows = (inCount + outCols - 1) / outCols;
			outOrientation = forcedOrientation;
			break;
	} // end switch

	// Quick sanity check since we may be called with an empty model
	if (inCount == 0) {
		outCols = outRows = 1;
	}
} // CalcRowsColsOrientation


/*
CalculateGrid
	Given a page and how many items we have to lay out, determines how many rows and columns
	are on each page.
*/
void
GridLayout::CalculateGrid(
	const ERect32&	inPageSize,
	const SInt32	inCount,
	SInt16&			outRows,
	SInt16&			outCols,
	OSType&			outOrientation) const
{
	ERect32	loopPageSize (inPageSize);
	SInt32	loopCount (inCount);
	
	ERect32	cellSize;
	double	minWidth;
	double	minHeight;
	
	do {
		OSType	constraintOrientation;
		this->CalcRowsColsOrientation(loopCount, outRows, outCols, outOrientation, constraintOrientation);

		// 173 176 If we are not a flexible grid, there's nothing else to do
		if (!this->IsFlexible())
			return;

		// if there is no minimum size, we are done
		SizeLimitT		minimumSize (GetDocument ()->GetMinimumSize());
		if (minimumSize == limit_None && mSizeCode == '****') return;

		//Get minimum dimensions (return is inches).
		if (mSizeCode == '****') {
			PhotoItemProperties::SizeLimitToInches(minimumSize, minWidth, minHeight);
		} else {
			PhotoUtility::GetSize(mSizeCode, minWidth, minHeight);
		}
		minWidth *= kDPI; // convert to 72dpi
		minHeight *= kDPI;

		// printable area (taking into account margins, etc)
		MRect		printableArea;
		EPrintSpec*	spec = this->GetPrintRec();
		PhotoPrinter::CalculateBodyRect(spec, &(this->GetPrintProperties()), printableArea); // at kDPI == 72!!

		// just before checking for goodness of cellsize, make sure pageSize reflects
		// our desired orientation
		ERect32 desiredPageSize (loopPageSize);
		if (((outOrientation == kLandscape) && (desiredPageSize.Height() > desiredPageSize.Width())) ||
			((outOrientation == kPortrait) && (desiredPageSize.Width() > desiredPageSize.Height()))) {
				SInt32 temp (desiredPageSize.Width());
				desiredPageSize.SetWidth(desiredPageSize.Height());
				desiredPageSize.SetHeight(temp);
			}//endif need to swap width + height of desired rect to match desired orientation

		// adjust the min rectangle to the aspect ratio of the majority of items 
		if ((constraintOrientation == kLandscape) &&	(minWidth < minHeight))
			swap(minWidth, minHeight);
		
		ERect32		unusedBottomPad;
		this->CalculateCellSize(desiredPageSize, outRows, outCols, cellSize, unusedBottomPad);

		// See if it fits. If not, try again (non-recursively!) to fit fewer
		loopPageSize = desiredPageSize;
		--loopCount;
	} while (cellSize.Width() < minWidth || cellSize.Height() < minHeight);
} // CalculateGrid


//--------------------------------------------------------------
// CalcMaxBounds
//--------------------------------------------------------------
void
GridLayout::CalcMaxBounds(const ERect32& inCellRect, MRect& outMaxBounds) {

	double		hMax;
	double		vMax;
	// get any user specified max size
	if (mSizeCode == '****') {
		PhotoItemProperties::SizeLimitToInches(GetDocument ()->GetMaximumSize(), hMax, vMax);
	} else {
		PhotoUtility::GetSize(mSizeCode, hMax, vMax);
	}//else

	// convert inches to screen resolution
	hMax *= this->GetResolution();
	vMax *= this->GetResolution();

	MRect maximum;
	// cell is portrait, so interpret constraint as portrait
	if (inCellRect.Height() > inCellRect.Width()) {
		maximum.SetWidth(std::min(hMax, vMax));
		maximum.SetHeight(std::max(hMax, vMax));
		}//endif
	else { // cell is landscape, so interpret constraint as landscape
		maximum.SetWidth(std::max(hMax, vMax));
		maximum.SetHeight(std::min(hMax, vMax));
		}//else

	MRect			cellBounds (0, 0, inCellRect.Height(), inCellRect.Width());

	outMaxBounds = cellBounds;
	if (!maximum.IsEmpty())
		outMaxBounds *= maximum;

	// Fit imageBounds into cellBounds.
	AlignmentGizmo::FitAndAlignRectInside(outMaxBounds, cellBounds, kAlignAbsoluteCenter, 
											outMaxBounds, EUtil::kDontExpand);
	// !!! note: If the max is 3*5, we don't change both dimensions. This is because
	// we'd probably have to crop.
}//end CalcMaxBounds



#ifdef DEBUG_LAYOUT
//--------------------------------------------------------------
// DrawEmptyRect
//--------------------------------------------------------------
void
GridLayout::DrawEmptyRect(const ERect32& where, RGBColor inColor) {
	HORef<StGDeviceSaver> saveDevice;
	CGrafPtr	destPort;
	::GetPort((GrafPtr*)&destPort);
	StColorPortState	saveState ((GrafPtr)destPort);
	saveState.Normalize();
	::RGBForeColor(&inColor);

	::MoveTo(where.left, where.top);
	::LineTo(where.right, where.top);
	::LineTo(where.right, where.bottom);
	::LineTo(where.left, where.bottom);
	::LineTo(where.left, where.top);
}//end DrawEmptyRect
#endif

/*
Initialize {OVERRIDE}
*/
void
GridLayout::Initialize()
{
	// 199 If we're switching back to a Grid, we don't want any placeholders
	mModel->RemoveEmptyItems(PhotoPrintModel::kDelete);

	//	Create it according to the grid
	this->LayoutImages();
} // Initialize

/*
LayoutImages {OVERRIDE}
*/
void
GridLayout::LayoutImages()
{	
	// sanity check:  make sure there are some images to layout
	if (mModel->GetCount() == 0) {
		this->AdjustDocumentOrientation();
		return;
	}//endif

	ESpinCursor::SpinWatch();

	// First be sure the paper is switched optimally (also sets view size depending on mNumPages)
	this->AdjustDocumentOrientation();

	// get size of a single body rect (printable minus header/footer)
	SInt32		docW = (SInt16)(GetDocument ()->GetWidth() * this->GetResolution());
	SInt32		docH = GetDocument ()->GetPageHeight(); // body rect, comes at doc resolution
	ERect32		bodySize (0, 0, docH, docW);

	ERect32		cellRect;
	ERect32		padRect;
	this->CalculateCellSize(bodySize, mRows, mColumns, cellRect, padRect);

	// layout the pages
	ERect32	pageBounds (bodySize);
	// offset down to the start of the body rect (top margin + header)
	MRect		body;
	MRect		header;
	EPrintSpec* spec = this->GetPrintRec();
	PhotoPrinter::CalculateBodyRect(spec, &(this->GetPrintProperties()), 
										body, this->GetResolution()); 
	PhotoPrinter::CalculateHeaderRect(spec, &(this->GetPrintProperties()), 
										header, this->GetResolution()); 
	
	pageBounds.Offset(0, header.Height());//dX,dY

	MRect	paper;
	PhotoPrinter::CalculatePaperRect(spec, &(this->GetPrintProperties()), 
										paper, this->GetResolution()); 
	

	MRect printable;
	PhotoPrinter::CalculatePrintableRect(this->GetPrintRec(), &this->GetPrintProperties(), printable, this->GetResolution());		
			
	PhotoIterator	iter (mModel->begin());
	for (SInt16 pageCount = 0; pageCount < mNumPages; ++pageCount) {
		this->LayoutPage(pageBounds, cellRect, iter);		
		if (iter == mModel->end())
			break;

	// offset full paper size, not just printable
		pageBounds.Offset(0, paper.Height()); // subsequent pages appear below, no horiz offset
	}//end for

	mModel->ChangedAllItems ();	//	Could be smarter

	Assert_(iter == mModel->end()); // if we haven't processed all items, something is WRONG
}//end LayoutImages


//LayoutItem
void
GridLayout::LayoutItem(PhotoItemRef item, const MRect& inMaxBounds) {

	MRect			itemBounds = item->GetNaturalBounds();
	if (!itemBounds) // if empty (proxy, placeholder, template) use max bounds 
		itemBounds = inMaxBounds;
		
	// scale it by the resolution to handle zooming
	RectScale(itemBounds, (double)this->GetResolution() / (double)kDPI);

	MatrixRecord	rotator;
	::SetIdentityMatrix(&rotator);
	if (!PhotoUtility::DoubleEqual(item->GetRotation(), 0.0)) {				
		::RotateMatrix(&rotator, ::Long2Fix(item->GetRotation()), ::Long2Fix(itemBounds.MidPoint().h), 
																	::Long2Fix(itemBounds.MidPoint().v));
		}//endif setup rotation matrix	

	AlignmentGizmo::FitTransformedRectInside(itemBounds, &rotator, inMaxBounds, itemBounds);
	AlignmentGizmo::MoveMidpointTo(itemBounds, inMaxBounds, itemBounds);
		
	PhotoDrawingProperties	drawProps (false, false, false, this->GetResolution());
	item->SetMaxBounds(inMaxBounds, drawProps);
	item->SetDest(itemBounds, drawProps);
}//end LayoutItem


//--------------------------------------------------------------
// LayoutPage
//--------------------------------------------------------------
void
GridLayout::LayoutPage(const ERect32& inPageBounds, const ERect32& inCellRect, PhotoIterator& iter)
{
	MRect maxBounds;
	CalcMaxBounds(inCellRect, maxBounds);

	do {
		for (SInt16 row = 0; row < GetRows(); ++row) {
			for (SInt16 col = 0; col < GetColumns(); ++col) {
				PhotoItemRef	item = *iter;
				MRect maxCellBounds (maxBounds);
				maxCellBounds.Offset(inPageBounds.left + (GetGutter() * col) + (inCellRect.Width() * col),	
									inPageBounds.top + (GetGutter() * row) + (inCellRect.Height() * row));

				LayoutItem(item, maxCellBounds);				

				++iter;
				if (iter == mModel->end())
					break;
			}//for all columns
			if (iter == mModel->end())
				break;
		}//end for all rows
		
	} while (false);
}//end LayoutPage

/*
ResizeImage {OVERRIDE}
	Resizes an image within us -- we need to resize *all* our images
	Returns TRUE if there was an actual size change
*/
bool
GridLayout::ResizeImage(const OSType inCode, const FitT inFit, PhotoItemRef ioItemRef)
{
#pragma unused(inFit)	// !!!

	if (mSizeCode != inCode) {
		mSizeCode = inCode;		// We'll rely on layout being called

		// it is no longer necessary to delete proxies, the items themselves do it when needed
	}

	Str255		sizeText;
	OSType		oldCode = ioItemRef->GetDimensions(sizeText, this->GetResolution(),
		PhotoPrintItem::si_OtherDimensions);
	return oldCode != inCode;
} // ResizeImage
