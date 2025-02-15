/*
	File:		FixedLayout.cp

	Contains:	Implementation of Layout object, which manages positioning of images.
				FixedLayout has more than one image, of fixed number and size.

	Written by:	David Dunham and Dav Lion

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

		03 Aug 2001		drd		278 Initialize mSizeCode to limit_None: we're fixed and ignore max size
		02 Aug 2001		drd		274 AddItem checks for no items before adding page
		02 Aug 2001		rmgw	One page minimum in Initialize.  Bug #273.
		02 Aug 2001		rmgw	Make Initialize smarter.  Bug #273.
		01 Aug 2001		rmgw	Deal with ItemsPerPage correctly in AddItem and SetItems.  Bug #265.
		31 Jul 2001		drd		238 RemoveItems special-cases for single
		27 Jul 2001		drd		239 Initialize only makes one placeholder
		25 Jul 2001		drd		211 Use kCopyRotateAndSkew when sending CopyForTemplate
		23 jul 2001		dml		179 add CalcOrientation
		23 Jul 2001		drd		Initialize only makes one dummy image, not two
		23 Jul 2001		rmgw	Add doc and type to constructor.
		23 Jul 2001		drd		205 SetItemsPerPage rounds up if model has too many images already
		20 Jul 2001		rmgw	Include PhotoPrintDoc.  Bug #200.
		18 Jul 2001		rmgw	Add RemoveItems method.
		18 Jul 2001		rmgw	Add SetItems method.
		09 Jul 2001		rmgw	AdoptNewItem now returns a PhotoIterator. Bug #142.
		06 Jul 2001		rmgw	Use CopyForTemplate.
		06 jul 2001		dml		moved gNeedDoubleOrientationSetting to PhotoUtility
		03 jul 2001		dml		SetDest, SetMaxBounds take PhotoDrawingProperties
		02 Jul 2001		rmgw	AdoptNewItem now takes a PhotoIterator.
		16 May 2001		drd		38 We can use generic options dialog
		23 Apr 2001		drd		GetMaximumSize constraint comes from document
		21 Mar 2001		drd		SetItemsPerPage now keeps images in sync
		06 mar 2001		dml		replace SetFile with operator=
		15 Feb 2001		rmgw	10 DeleteLastItem => RemoveLastItem
		18 Jan 2001		drd		CommitOptionsDialog returns value and has new arg
		16 jan 2001		dml		add gNeedDoubleOrientationSetting for lexmark debugging
		18 Sep 2000		drd		Allow dropping multiple items
		14 Sep 2000		drd		Added arg to GetDimensions
		07 sep 2000		dml		Initialize should call MakeNewImage, which should set max bounds!
		07 Sep 2000		drd		Override GetName to return size
		07 sep 2000		dml		cleanup AddItem. add TryToFillFirstEmpty, override AdjustDocumentOrientation 
		07 sep 2000		dml		AddItem selects via doc/view
		18 Aug 2000		drd		Use MakeNewImage in CommitOptionsDialog (so subclasses can override)
		16 Aug 2000		drd		CommitOptionsDialog changes number of items; Intialize
								sends LayoutImages so we respect preferences
		15 Aug 2000		drd		Moved Initialize here (from MultipleLayout)
		09 Aug 2000		drd		Added dialog handling (and moved mImageCount to this class)
		23 Jun 2000		drd		Use HORef<PhotoPrintModel> in constructor
		19 Jun 2000		drd		Created
*/

#include "FixedLayout.h"

#include "EDialog.h"
#include "PhotoPrinter.h"
#include "PhotoPrintPrefs.h"
#include "PhotoPrintDoc.h"

/*
FixedLayout
*/
FixedLayout::FixedLayout(

	PhotoPrintDoc*			inDoc, 
	HORef<PhotoPrintModel>& inModel,
	UInt32					inItemsPerPage,
	LayoutType				inType)

	: GridLayout (inDoc, inModel, inItemsPerPage, inType)
{
	mSizeCode = limit_None;
} // FixedLayout

/*
~FixedLayout
*/
FixedLayout::~FixedLayout()
{
} // ~FixedLayout

/*
AddItem {OVERRIDE}
	Add an item to the model, handling multiples properly. We replace placeholders.
	We are responsible for ownership of the item, either passing it on to the model or freeing it.
*/
PhotoIterator
FixedLayout::AddItem(

	PhotoItemRef 	inItem,
	PhotoIterator	inBefore)
{
	PhotoIterator	result = TryToFillFirstEmpty (inItem, inBefore);
	if (result != mModel->end ()) return result;

// if we are here, that means there was no empty slot.  soo, 
// time to add an extra page!!
	if (mModel->GetCount() > 0)				// 274 Unless we had no items
		++mNumPages;

	while (mModel->GetCount () < (mNumPages * GetItemsPerPage ()))
		mModel->AdoptNewItem (MakeNewImage(), mModel->end ());

	// Create it according to the grid
	this->LayoutImages();

	result = TryToFillFirstEmpty(inItem, mModel->end ());
	Assert_(result != mModel->end ());
	
	return result;

} // AddItem

/*
AdjustDocumentOrientation {OVERRIDE}
*/
void
FixedLayout::AdjustDocumentOrientation(SInt16 /*numPages*/)
{
	// printable area (taking into account margins, etc)
	MRect		printableArea;
	EPrintSpec* spec = GetDocument()->GetPrintRec();
	PhotoPrinter::CalculateBodyRect(spec, &(GetDocument()->GetPrintProperties()), printableArea);

	// Figure
	this->CalculateGrid(printableArea, GetItemsPerPage (), mRows, mColumns, mOrientation);
	
	mItemsPerPage = this->GetRows() * this->GetColumns();
	mNumPages = mModel->GetCount() / mItemsPerPage;
	if (mNumPages * mItemsPerPage < mModel->GetCount()) 
		mNumPages++; 

	spec->SetOrientation(mOrientation, PhotoUtility::gNeedDoubleOrientationSetting);
	GetDocument()->MatchViewToPrintRec(mNumPages); // do this anyway, since changes according to #pages
} // AdjustDocumentOrientation

/*
CalcOrientation {OVERRIDE}
*/
OSType		
FixedLayout::CalcOrientation() const {
	MRect		printableArea;
	EPrintSpec* spec = GetDocument()->GetPrintRec();
	PhotoPrinter::CalculateBodyRect(spec, &(GetDocument()->GetPrintProperties()), printableArea);

	// Figure
	OSType		orientation;
	SInt16		bogusRows (mRows);
	SInt16		bogusCols (mColumns);
	this->CalculateGrid(printableArea, GetItemsPerPage (), bogusRows, bogusCols, orientation);

	return orientation;
}//end


/*
CanAddToBackground {OVERRIDE}
*/
bool
FixedLayout::CanAddToBackground(const UInt16 inCount)
{
#pragma unused(inCount)

	return true;								// Since we make more pages, it's OK to accept any number
} // CanAddToBackground

/*
GetName {OVERRIDE}
	Return the name of this layout. We override to include the size of the images
	(since they're all the same, and don't change).
*/
LStr255
FixedLayout::GetName() const
{
	LStr255			theName(Layout::GetName());		// Get the basic name (i.e. "Fixed")
	PhotoItemRef	firstImage = *mModel->begin();
	LStr255			theSize;
	firstImage->GetDimensions(theSize, GetDocument()->GetResolution(), PhotoPrintItem::si_DimensionsInParens);
	theName += theSize;

	return theName;
} // GetName

/*
Initialize {OVERRIDE}
*/
void
FixedLayout::Initialize()
{
	//	Find the last page with a non-empty item on it
	mNumPages = 1;

	UInt32	itemCount = GetItemsPerPage ();
	UInt32	itemPage = 0;
	for (PhotoIterator i = mModel->begin (); i != mModel->end (); ++i) {
		++itemCount;
		if (itemCount > GetItemsPerPage ()) {
			itemCount = 1;
			++itemPage;
			} // if
			
		if (!(*i)->IsEmpty ()) mNumPages = itemPage;
		} // for
	
	//	Calculate how many we really want
	UInt32	actualCount = mNumPages * GetItemsPerPage ();
	
	//	Fill out the last page with blanks
	while (mModel->GetCount () < actualCount)
		mModel->AdoptNewItem (MakeNewImage(), mModel->end ());

	// Get rid of extra images.
	while (mModel->GetCount() > actualCount) 
		mModel->RemoveLastItem(PhotoPrintModel::kDelete);

	//	Create it according to the grid
	this->LayoutImages();
} // Initialize

/*
MakeNewImage
*/
PhotoPrintItem*
FixedLayout::MakeNewImage()
{
	PhotoPrintItem* newItem = new PhotoPrintItem();

	// figure out the maximum sizes
	double		hMax;
	double		vMax;
	PhotoItemProperties::SizeLimitToInches(GetDocument()->GetMaximumSize(), hMax, vMax);
	// convert inches to screen resolution
	hMax *= GetDocument()->GetResolution();
	vMax *= GetDocument()->GetResolution();
	MRect		maximum(0, 0, vMax, hMax);			

	PhotoDrawingProperties	drawProps (false, false, false, GetDocument()->GetResolution());
	newItem->SetMaxBounds(maximum, drawProps);
	return newItem;
} // MakeNewImage

//---------------------------------
// 	RemoveItems
//---------------------------------

void	
FixedLayout::RemoveItems (

	ConstPhotoIterator 	inBegin,
	ConstPhotoIterator 	inEnd)

{ // begin RemoveItems
	// 238 If we have only one image per page, it doesn't make sense to replace them
	if (this->GetColumns() == 1 && this->GetRows() == 1 && !this->ImagesAreDuplicated()) {
		GridLayout::RemoveItems(inBegin, inEnd);
		// If we deleted all of them, we need to add a placeholder
		if (mModel->GetCount() == 0) {
			this->AddItem(this->MakeNewImage(), mModel->end());
		}
	} else {
		HORef<PhotoPrintItem>	blank = MakeNewImage();
		for (ConstPhotoIterator i = inBegin; i != inEnd; ++i) 
			**i = *blank;
	}

}//end RemoveItems

//------------------------------------------------------------------
// SetItems
//------------------------------------------------------------------

void
FixedLayout::SetItems (

	ConstPhotoIterator	inBegin,
	ConstPhotoIterator	inEnd) 
	
{
	
	//	Get new item count
	UInt32		actualCount = (inEnd - inBegin);

	// Get rid of extra images.
	while (mModel->GetCount() > actualCount) {
		mModel->RemoveLastItem(PhotoPrintModel::kDelete);
	}
	// Make new items if necessary
	while (mModel->GetCount() < actualCount) {
		mModel->AdoptNewItem(this->MakeNewImage(), mModel->end());
	}
	
	//	Copy the new list in
	for (PhotoIterator i = mModel->begin (); i != mModel->end (); ++i, ++inBegin) {
		Assert_(inBegin != inEnd);
		**i = **inBegin;
		} // for
	Assert_(inBegin == inEnd);

}//end SetItems

/*
SetItemsPerPage
*/
void
FixedLayout::SetItemsPerPage(const UInt32 inItemsPerPage)
{
	mItemsPerPage = inItemsPerPage;
	
	Initialize ();
} // SetItemsPerPage

/*
TryToFillFirstEmpty
*/
PhotoIterator
FixedLayout::TryToFillFirstEmpty(
	PhotoItemRef 	inItem,
	PhotoIterator	inBefore) 
{	
	//	See if the given slot is free
	if ((inBefore != mModel->end()) && (*inBefore)->IsEmpty()) {
		(*inBefore)->CopyForTemplate (*inItem, PhotoPrintItem::kCopyRotateAndSkew);
		GetView()->AddToSelection(*inBefore);
		delete inItem;							// Since it's not in the model
		
		return inBefore;
	}
		
	// See if there's anything we can take over
	for (PhotoIterator	i = mModel->begin(); i != mModel->end(); i++) {
		if (!(*i)->IsEmpty()) continue;
			
		(*i)->CopyForTemplate (*inItem, PhotoPrintItem::kCopyRotateAndSkew);
		GetView()->AddToSelection(*i);
		delete inItem;							// Since it's not in the model
		
		return i;
	}//for all items in model

	return mModel->end();
}//end TryToFillFirstEmpty
