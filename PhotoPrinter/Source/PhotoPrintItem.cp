/*
	File:		PhotoPrintItem.cp

	Contains:	Definition of an item (i.e. an image).

	Written by:	Dav Lion and David Dunham

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

		06 nov 2001		dml		drd 364 cont add tiling to fix problems with gigantic single images + hp drivers
		04 Nov 2001		drd		dml 364 DrawImage uses offscreen pixmap if there's clipping
		10 sep 2001		dml		189.  fewer pixels drawn
		05 sep 2001		dml		342.  HasCrop checks CZ rects also
		05 Sep 2001		drd		347 Rotate captions for caption_RightVertical again
		04 sep 2001		dml		345.  CopyForTemplate must copy new crop-zoom fields
		31 aug 2001		dml		275, 282.  rewrite CropZoom logic.
		22 Aug 2001		drd		Fixed potential leak in DrawProxy
		21 Aug 2001		drd		340 Be more paranoid about using GetFileSpec() since it can be nil
		21 Aug 2001		rmgw	Switch to inline members of StQTImportComponent.
		19 aug 2001		dml		make crop-zoom relative
		17 Aug 2001		rmgw	Protect proxies while they are being built.  Bug #232.
		17 Aug 2001		rmgw	Improve alias resolution.  Bug #330.
		15 Aug 2001		rmgw	Change rectangle interfaces to return copies.
		02 Aug 2001		drd		Added quality arg to DrawImage
		01 aug 2001		dml		262, 225 CalcImageCaptionRects sets mImageMaxBounds
		31 jul 2001		dml		212 Caption_RightHorizontal needs to shimmy over 1 pixel to right (and shrink, too)
		30 jul 2001		dml		252, 258 add Caption_None to CalcImageCaptionRects
		27 jul 2001		dml		whoops.  interior captions don't distort, and we don't short circuit anymore
		27 Jul 2001		rmgw	Be vewy careful when hunting proxies.  Bug #244.
		27 jul 2001		dml		fix various caption bugs 212, 217, 224, 236
		26 Jul 2001		rmgw	Factor out XML parsing.  Bug #228.
		25 Jul 2001		drd		15 Use ESpinCursor::SpinWatch instead of UCursor::SetWatch, removed
								ESpinCursor arg from Draw
		25 Jul 2001		drd		211 Added inCopyRotateAndSkew arg to CopyForTemplate
		24 jul 2001		dml		SetupDestMatrix, CalcTransformedBounds const
		24 Jul 2001		drd		214 CalcImageCaptionRects(caption_RightHorizontal) centers vertically
		19 jul 2001		dml		replace true/false w/ symbolic constants in Draw catch handler call to SetupDestMatrix
		19 jul 2001		dml		19, 160  CopyForTemplate fakes out AdjustRectangles to fix proxy stupidity
		19 Jul 2001		drd		198 GetFileSpec swallows exceptions and nils the spec
		18 jul 2001		dml		56, 189.  DrawImage failure caught above, calls to DrawMissing from Draw if.
								Operator= now copies proxies and QTI
		18 Jul 2001		drd		56 DrawEmpty(kMissing) fills with red pattern
		17 Jul 2001		rmgw	Add async exception reporting to Draw.
		12 jul 2001		dml		add Operator= 
		10 Jul 2001		drd		146 Use DrawTruncatedWithJust, not DrawWithJustification
		10 jul 2001		dml		DrawEmpty uses ItemRect, ImageMaxBounds
		 7 Jul 2001		rmgw	Add full dest rect access; fix alias/file access and construction. 
		 7 Jul 2001		rmgw	Add full alias/file spec access.
		 7 Jul 2001		rmgw	Fix copy/assignment badness.
		06 Jul 2001		drd		128 MakeProxy calls SetWatch
		06 jul 2001		dml		CalcImageCaptionRects must handle empty NaturalBounds (templates strike again!)
		05 jul 2001		dml		don't copy rotation in Operator= (see comments below)
		03 jul 2001		dml		104, 25.  Rotation of Caption controlled by ItemProperty
		02 jul 2001		dml		remove StValueChanger blocks in Draw concerning debugging of exceptions
		02 jul 2001		dml		17.  changed order of matrix concat in DrawCaptionText
		 2 Jul 2001		rmgw	Remove reference in GetName.
		27 Jun 2001		drd		Changed color of debug gDrawMaxBounds to chartreuse; 56 added DrawEmpty
								arg for DrawMissing, added try/catch to GetCreatedTime, GetModifiedTime
		26 Jun 2001		drd		88 GetFileSpec can return nil, test for this
		25 Jun 2001		drd		85 Default constructor initializes mCanResolveAlias
		14 Jun 2001		rmgw	First pass at handling missing files.  Bug #56.
		25 Apr 2001		drd		removed clipping hack in DrawEmpty
		06 Apr 2001		drd		Handle printing of empty item
		22 Mar 2001		drd		Hacks to try to get image under new PowerPlant
		15 mar 2001		dml		txtSize calcs done always.  copy ct copies maxbounds, ResolveCropStuff handles worldSpace
		14 mar 2001		dml		fix some crop handling
		12 mar 2001		dml		fix captions to respect WorldSpace
		09 mar 2001		dml		Draw must call ::TransformRgn w/ worldspace on resolveCropRgn
		07 mar 2001		dml		IsLandscape uses naturalbounds, not imageRect
		06 mar 2001		dml		bug 54.  GetDimensions more attentive to idealized sizes (gridlayout sets in MaxBounds)
		01 mar 2001		dml		mMaxBounds now transient, not serialized
		28 feb 2001		dml		operator=, changes to AdjustRectangles to support operator= when used w/ templates
		26 feb 2001		dml		ResolveCropStuff no longer accepts matrix
		19 feb 2001		dml		refactor for rmgw, fix bug 3
		01 feb 2001		dml		add MakePict
		17 jan 2001		dml		DrawCaption, DrawCaptionText attentive to on-screen resolution (zooming) bug 29
		16 jan 2001		dml		added isTemplate to Write()
		11 Dec 2000		drd		GetDimensions uses 3.5 inch
		05 Oct 2000		drd		Use std:: for sscanf
		21 sep 2000		dml		fix leak caused by MakeProxy calling DrawImage.  DrawImage now nils mQTI
		21 sep 2000		dml		MakeProxy (and GetProxy) no longer take matrix, since create w/o rotation
		20 sep 2000		dml		MakeProxy uses SilentExceptionEater
		19 Sep 2000		drd		DrawProxyIntoPicHandle is paranoid about there being a current port
		19 Sep 2000		drd		CheckExact* take doubles
		19 sep 2000		dml		DrawIntoNewPictureWithRotation makes minimally sized dest picture
		18 Sep 2000		drd		DrawCaptionText, rather than DrawCaption, erases for caption_Inside with no rotation
		18 sep 2000		dml		fixed bug 8 (placeholders + ImageOptions)
		18 Sep 2000		drd		Added ESpinCursor arg to Draw
		15 Sep 2000		drd		GetDimensions checks for si_NaturalBounds; fixed a test for proxy existence
		14 sep 2000		dml		bug 5:  DrawIntoNewPictureWithRotation must handle lack of proxy.  same with MakeIcon
		14 sep 2000		dml		fix HasCrop (bug 1)
		14 Sep 2000		drd		Add more codes (and a resolution arg) to GetDimensions
		11 sep 2000		dml		revert some of DrawCaptionText, fix some clip problems w/ captions, more double comparisons fixed
		08 sep 2000		dml		changes to DrawCaptionText
		07 sep 2000		dml		IsLandscape handles absence of imageRect
		07 Sep 2000		drd		MakeIcon uses Apple sample code; GetDimension is sloppier
		06 Sep 2000		drd		MakeIcon (not working yet)
		06 sep 2000		dml		implement caption_RightHorizontal style
		31 aug 2000		dml		fix proxies! (esp w/ cropping, rotation).  DrawIntoNewPicture compensates for existing rot
		31 Aug 2000		drd		DrawIntoNewPictureWithRotation uses dithering to try to improve looks
		30 aug 2000		dml		draw thumbnails at correct aspect ratio
		30 aug 2000		dml		thumbnails drawn with proxy again. fast + stable (thanks to proxy + StLockPixels)
		30 Aug 2000		drd		Fixed spelling of SetPurgeable
		30 aug 2000		dml		add a StLockPixels whenever we draw the proxy (doh!)
		29 Aug 2000		drd		SetFile copies mFileSpec too
		29 Aug 2000		drd		GetProxy; use GetProxy in Draw
		24 aug 2000		dml		added DrawProxyIntoNewPictureWithRotation
		23 aug 2000		dml		proxy should respect expand + offset.  
		23 aug 2000		dml		change storage of Crop percentages to double
		22 aug 2000		dml		SetupDestRect uses expanded only for rect mapping, placement rect for rotation midpoint
		22 aug 2000		dml		bottleneced QTI instantiation into ReanimateQTI.  removed QTI from SetupDestMatrix (!!).
		22 aug 2000		dml		Draw should pass workingCrop to DrawImage, also, ResolveCropStuff must handle xformed coordinates + crop rect
		22 aug 2000		dml		perhaps it is better to throw in the CT if we can't make the qti (fix iMac death)
		21 Aug 2000		drd		The class function is now called ParseProperties; removed ParseBounds
								and made ParseRect the class function
		21 aug 2000		dml		move qti instantiation in ct inside try-catch block, to allow unable-to-draw objects
		21 aug 2000		dml		make serialization respect cropping
		18 aug 2000		dml		make crop (and cropzoom) relative
		16 Aug 2000		drd		DrawEmpty doesn't normalize entire StColorPortState, and adjusts points
								for the way QuickDraw rectangles enclose stuff
		16 aug 2000		dml		remove eggregious casting in HORef tests
		16 aug 2000		dml		GetFileSpec returns an HORef&
		16 aug 2000		dml		GetFileSpec doesn't always install a new spec, only if needed or changed
		15 Aug 2000		drd		GetFileSpec makes sure we have an alias
		15 aug 2000		dml		fixed bug in SetupDestMatrix having to do w/ qti ownership
		15 aug 2000		dml		changes to copy ct (don't copy qti); clarifications of emptiness test casts
		14 Aug 2000		drd		DrawCaption doesn't draw file-related things if we're empty
		07 aug 2000		dml		if GetDestRect(recompute == true) make sure qti is valid
		07 Aug 2000		drd		Only use StQuicktimeRenderer if we rotate
		04 Aug 2000		drd		Fixed DrawCaptionText to handle multiple lines of rotated text
		04 aug 2000		dml		GetName handles Empty case correctly
		04 aug 2000		dml		change from mSpec to mAlias
		03 Aug 2000		drd		DrawCaption observes date/time format prefs
		03 Aug 2000		drd		Better caption_RightVertical (1-line); DrawCaption handles dates
		01 Aug 2000		drd		Started dealing with caption_RightVertical
		01 Aug 2000		drd		Back to 1-bit StQuicktimeRenderer
		31 jul 2000		dml		sure, it only needs a 1-bit gworld, but use 32 to reduce issues in debugging
		31 Jul 2000		dml		DrawCaptionText passes clip region to StQuicktimeRenderer
		25 Jul 2000		drd		DrawCaptionText only needs a 1-bit StQuicktimeRenderer
		20 Jul 2000		drd		AdjustRectangles handles caption_Inside
		17 Jul 2000		drd		MakeProxy makes sure we have a QTI
		14 jul 2000		dml		fix bug in Draw having to do w/ component opening (don't make if empty)
		13 jul 2000		dml		open/close component before each draw (free up memory!)
		12 Jul 2000		drd		AdjustRectangles, DrawCaption use GetCaptionLineHeight
		11 jul 2000		dml		adjustRect correctly sets up ItemRect via AlignmentGizmo
		10 jul 2000		dml		moved StQTImportComponent to separate file, Read now sets NaturalBounds,
								DrawCaption uses StQuicktimeRenderer
		10 Jul 2000		dml		SetDest must call AdjustRectangles, MapDestRect maps FontSize also
		10 Jul 2000		drd		DrawCaptionText, filename caption
		07 Jul 2000		drd		AdjustRectangles, DrawCaption (first stab)
		07 Jul 2000		drd		GetDimensions
		06 Jul 2000		drd		MapDestRect maps mImageRect
		05 jul 2000		dml		MakeProxy operates on a local matrix, not mMat
		05 jul 2000		dml		MakeProxy and DrawProxy use TransformedBounds, not mDest
		05 jul 2000		dml		SetupDestMatrix now optionally scales (sometimes want other xforms only)
		05 Jul 2000		drd		MakeProxy(nil) uses a default matrix
		03 jul 2000		dml		ResolveCropStuff should clip against transformed bounds
		03 Jul 2000		drd		MakeProxy doesn't clip as much
		03 Jul 2000		drd		SetFile sends DeleteProxy; added gUseProxies, DrawImage; redo MakeProxy
		30 Jun 2000		drd		SetFile copies QTI (very handy for SchoolLayout)
		29 jun 2000		dml		add proxy support
		29 jun 2000		dml		clean up ownership issues in ResolveCropStuff
		27 jun 2000		dml		added SetScreenBounds, removed CROP_BY_REGION ifdefs, fixed DrawEmpty
		27 Jun 2000		drd		IsLandscape, IsPortrait
		27 jun 2000		dml		doh!  pass reference to cropRgn to ResolveRegionStuff (cropping works again)
		26 Jun 2000		drd		SetFile; improved default constructor
		21 june 2000 	dml		initial crop should be EmptyRect, and we should special case it in Draw()
		20 june 2000	dml		work on cropping.  
		19 june 2000	dml		copy ct copies crop
		19 june 2000	dml		added cropping, alphabetized
*/

#include "PhotoPrintItem.h"

#include <algorithm.h>
#include "AlignmentGizmo.h"
#include "EChrono.h"
#include "ESpinCursor.h"
#include "IconUtil.h"
#include "PhotoExceptionHandler.h"
#include "PhotoPrintPrefs.h"
#include "PhotoUtility.h"
#include "StPixelState.h"
#include "StQuicktimeRenderer.h"
#include "MMatrixRecord.h"
#include "EMatrix.h"
#include "PhotoPrinter.h"

//	Toolbox++
#include "MNewAlias.h"
#include "MNewRegion.h"
#include "MOpenPicture.h"
#include "UState.h"

// Globals
SInt16	PhotoPrintItem::gProxyBitDepth = 16;
bool	PhotoPrintItem::gUseProxies = true;				// For debug purposes
double	kRightHorizontalCoefficient = 0.33; 			// how much of avail space allocated to image?
bool	PhotoPrintItem::gDrawMaxBounds = false;			// handy for debugging

// ---------------------------------------------------------------------------
// PhotoPrintItem empty constructor
// ---------------------------------------------------------------------------

PhotoPrintItem::PhotoPrintItem (void)

	: mXScale (1.0)
	, mYScale (1.0)
	, mTopCrop (0)
	, mLeftCrop (0)
	, mBottomCrop (0)
	, mRightCrop (0)
	, mTopOffset (0.0)
	, mLeftOffset (0.0)
	
	, mTopCZ (0.0)
	, mLeftCZ (0.0)
	, mBottomCZ (0.0)
	, mRightCZ (0.0)

	, mUserTopCrop (0.0)
	, mUserLeftCrop (0.0)
	, mUserBottomCrop (0.0)
	, mUserRightCrop (0.0)

	, mRot (0.0)
	, mSkew (0.0)

	, mCanResolveAlias (true)
{

	::SetIdentityMatrix (&mMat);

}//end empty ct


// ---------------------------------------------------------------------------
// PhotoPrintItem properties constructor
// ---------------------------------------------------------------------------

PhotoPrintItem::PhotoPrintItem (

	const	Rect&					inCaptionRect,
	const	Rect&					inImageRect,
	const	Rect&					inFrameRect,
	const	Rect&					inDest,

	double							inXScale,
	double							inYScale,
	double							inTopCrop,
	double							inLeftCrop,
	double							inBottomCrop,
	double							inRightCrop,
	double							inTopOffset,
	double							inLeftOffset,

	double							inTopCZ,
	double							inLeftCZ,
	double							inBottomCZ,
	double							inRightCZ,
	
	double							inUserTopCrop,
	double							inUserLeftCrop,
	double							inUserBottomCrop,
	double							inUserRightCrop,

	const	PhotoItemProperties&	inProperties,

	double							inRot,
	double							inSkew,

	HORef<MFileSpec>				inFileSpec)

	: mCaptionRect (inCaptionRect)
	, mImageRect (inImageRect)
	, mFrameRect (inFrameRect)
	, mDest (inDest)

	, mXScale (inXScale)
	, mYScale (inYScale)
	, mTopCrop (inTopCrop)
	, mLeftCrop (inLeftCrop)
	, mBottomCrop (inBottomCrop)
	, mRightCrop (inRightCrop)
	, mTopOffset (inTopOffset)
	, mLeftOffset (inLeftOffset)
	
	, mTopCZ (inTopCZ)
	, mLeftCZ (inLeftCZ)
	, mBottomCZ (inBottomCZ)
	, mRightCZ (inRightCZ)
	
	, mUserTopCrop (inUserTopCrop)
	, mUserLeftCrop (inUserLeftCrop)
	, mUserBottomCrop (inUserBottomCrop)
	, mUserRightCrop (inUserRightCrop)
	
	//, mMat (other.mMat)	//	No T++ object so done below
	, mProperties (inProperties)

	, mRot (inRot)
	, mSkew (inSkew)

	, mCanResolveAlias (true)	// ??? why not copied? Because no one will clear it - rmgw
	{

		::SetIdentityMatrix (&mMat);
		
		if (inFileSpec) SetFileSpec (*inFileSpec);
		
	} // end properties ct

// ---------------------------------------------------------------------------
// PhotoPrintItem copy constructor
// ---------------------------------------------------------------------------

PhotoPrintItem::PhotoPrintItem(

	const PhotoPrintItem& other) 

	: mCaptionRect (other.GetCaptionRect())
	, mImageRect (other.GetImageRect())
	, mFrameRect (other.GetFrameRect())
	, mDest (other.GetDestRect())

	, mMaxBounds (other.GetMaxBounds())
	, mImageMaxBounds (other.GetImageMaxBounds())
	, mNaturalBounds (other.GetNaturalBounds())
	
	, mXScale (other.mXScale)
	, mYScale (other.mYScale)
	, mTopCrop (other.mTopCrop)
	, mLeftCrop (other.mLeftCrop)
	, mBottomCrop (other.mBottomCrop)
	, mRightCrop (other.mRightCrop)
	, mTopOffset (other.mTopOffset)
	, mLeftOffset (other.mLeftOffset)
	
	, mTopCZ (other.mTopCZ)
	, mLeftCZ (other.mLeftCZ)
	, mBottomCZ (other.mBottomCZ)
	, mRightCZ (other.mRightCZ)
	
	, mUserTopCrop (other.mUserTopCrop)
	, mUserLeftCrop (other.mUserLeftCrop)
	, mUserBottomCrop (other.mUserBottomCrop)
	, mUserRightCrop (other.mUserRightCrop)
	
	//, mMat (other.mMat)	//	No T++ object so done below
	, mProperties (other.GetProperties())

	, mRot (other.GetRotation())
	, mSkew (other.GetSkew())

	, mAlias (other.mAlias)
	, mCanResolveAlias (true)	// ??? why not copied? Because no one will clear it - rmgw
	, mFileSpec (other.mFileSpec)

	, mQTI (other.mQTI)

	, mProxy (other.mProxy)
{
	// could recompute, but hey, it's a copy constructor
	::CopyMatrix(&(other.mMat), &mMat);	

}//end copy ct


// ---------------------------------------------------------------------------
// PhotoPrintItem constructor
// ---------------------------------------------------------------------------
PhotoPrintItem::PhotoPrintItem(

	const FSSpec& inSpec)
	
	: mXScale (1.0)
	, mYScale (1.0)
	, mTopCrop (0)
	, mLeftCrop (0)
	, mBottomCrop (0)
	, mRightCrop (0)
	, mTopOffset (0.0)
	, mLeftOffset (0.0)
	
	, mTopCZ (0.0)
	, mLeftCZ (0.0)
	, mBottomCZ (0.0)
	, mRightCZ (0.0)

	, mUserTopCrop (0.0)
	, mUserLeftCrop (0.0)
	, mUserBottomCrop (0.0)
	, mUserRightCrop (0.0)
	
	, mRot (0.0)
	, mSkew (0.0)
	
	, mAlias (new MNewAlias (inSpec))
	, mCanResolveAlias (true)
	, mFileSpec (new MFileSpec (inSpec, false))
{
	
	::SetIdentityMatrix (&mMat);

	ReanimateQTI(); // make it just for side effects
	mQTI = nil;		// throw it away
	//	What bloody "side effects"???
	//	A: Presumably because it sets mNaturalBounds
}//end ct
	

// ---------------------------------------------------------------------------
// ~PhotoPrintItem destructor
// ---------------------------------------------------------------------------
PhotoPrintItem::~PhotoPrintItem() {
}//end dt


PhotoPrintItem&
PhotoPrintItem::operator=	(const PhotoPrintItem&	other) {
	mCaptionRect = other.mCaptionRect;
	mImageRect = other.mImageRect;
	mDest = other.mDest;
	mFrameRect = other.mFrameRect;
	
	mMaxBounds = other.mMaxBounds;
	mImageMaxBounds = other.mImageMaxBounds;
	mNaturalBounds = other.mNaturalBounds;	

	mXScale = other.mXScale;
	mYScale = other.mYScale;
	mTopCrop = other.mTopCrop;
	mLeftCrop = other.mLeftCrop;
	mBottomCrop = other.mBottomCrop;
	mRightCrop = other.mRightCrop;
	mTopOffset = other.mTopOffset;
	mLeftOffset = other.mLeftOffset;
	
	mTopCZ = other.mTopCZ;
	mLeftCZ = other.mLeftCZ;
	mBottomCZ = other.mBottomCZ;
	mRightCZ = other.mRightCZ;
	
	mUserTopCrop = other.mUserTopCrop;
	mUserLeftCrop = other.mUserLeftCrop;
	mUserBottomCrop = other.mUserBottomCrop;
	mUserRightCrop = other.mUserRightCrop;
	
	mQTI = other.mQTI;
	mProperties = other.mProperties;

	mMat = other.mMat;
	mProperties = other.mProperties;
	
	mRot = other.mRot;
	mSkew = other.mSkew;
	
	mAlias = other.mAlias;
	mCanResolveAlias = other.mCanResolveAlias;
	mFileSpec = other.mFileSpec;
	
	// inefficient, but safe.  we'll just reanimate and recreate later
	// we could do the = thang, but i have not thought this through clearly
	mQTI = other.mQTI;
	mProxy = other.mProxy;

	return *this;
	}//end operator=	


#pragma mark -

// ---------------------------------------------------------------------------
// AdjustRectangles
//	Adjust rectangles depending on caption.  
// ---------------------------------------------------------------------------
void
PhotoPrintItem::AdjustRectangles(const PhotoDrawingProperties& drawProps)
{
	MRect		oldImageRect;
	if (!mImageRect.IsEmpty())
		oldImageRect = mImageRect;

	CalcImageCaptionRects(mImageRect, mCaptionRect, mMaxBounds, drawProps);
	
	// IFF there was an old imageRect (not an empty template space)
	// AND Rectangles changed and are bigger, invalidate proxy
	if (oldImageRect &&
		(oldImageRect.Width() < mImageRect.Width() ||
		oldImageRect.Height() < mImageRect.Height()))
		this->DeleteProxy();


	if (HasZoom()) {

		// recalc max crop-zoom bounds.  
		ERect32 cropZoomRect;
		ERect32 expandedOffsetImageRect;
		DeriveCropZoomRect(cropZoomRect, expandedOffsetImageRect);
		// reset crop values for pure (maximum) crop-zoom
		PhotoUtility::CalcCropValuesAsPercentages(cropZoomRect, GetImageRect(), 
													mTopCrop, mLeftCrop, mBottomCrop, mRightCrop, kDontClampToBounds);
	
		// the mUserXXXCrop values hold percentages of the full cropZoomRect which are to be applied as user crop
		// (the user may have regular-cropped a crop-zoomed image)
		// reinterpret those as %ages of the ImageRect so that they can be combined into mXXXCrop
		double heightScalar (cropZoomRect.Height() / (double)GetImageRect().Height());
		double widthScalar (cropZoomRect.Width() / (double)GetImageRect().Width());
		
		mTopCrop += mUserTopCrop * heightScalar;
		mLeftCrop += mUserLeftCrop * widthScalar;
		mBottomCrop += mUserBottomCrop * heightScalar;
		mRightCrop += mUserRightCrop * widthScalar;

		}//endif


		
		
} // AdjustRectangles


// -----�----------------------------------------------------------------------
// AdoptAlias
// ---------------------------------------------------------------------------
void
PhotoPrintItem::AdoptAlias(
	AliasHandle	inAlias)

{ // begin AdoptAlias
	
	mAlias = new MNewAlias (inAlias);
	mFileSpec = 0;
	
	ReanimateQTI(); // make it just for side effects
	mQTI = nil;		// throw it away
	//	What bloody "side effects"???
	//	A: Presumably because it sets mNaturalBounds
} // end AdoptAlias
	

void
PhotoPrintItem::CalcImageCaptionRects(MRect& oImageRect, MRect& oCaptionRect,
										const MRect& inMax,
										const PhotoDrawingProperties& drawProps) 
{
	const PhotoItemProperties&	props(this->GetProperties());

		SInt16	height;
		SInt16	width;
		SInt16	lines = 0;
		if (props.GetCaption().Length() > 0) {
			lines++;
		}

		if (props.GetShowName()) {
			lines++;
		}
		if (props.GetShowDate()) {
			lines++;
		}
		
		SInt16 txtLineHeight (props.GetCaptionLineHeight());
		txtLineHeight *= ((double)drawProps.GetScreenRes()) / 72.0;
		height = lines * txtLineHeight; // must scale to screen res!!

		mImageMaxBounds = inMax;

		MatrixRecord rotation;
		::SetIdentityMatrix(&rotation);
		
		// a template has no natural bounds, only max bounds
		MRect extentsBasis;
		if (this->GetNaturalBounds())
			extentsBasis = this->GetNaturalBounds();
		else
			extentsBasis = this->GetMaxBounds();
			
		switch (props.GetCaptionStyle()) {
			case caption_None: {
				// make a copy of this rectangle so that we can move to its midpoint later
				MRect copyImageForMidpoint (mImageMaxBounds);
		
				// make the rotation around the center of the newly determined image rect
				Point midPoint (mImageMaxBounds.MidPoint());
				::RotateMatrix (&rotation, Long2Fix((long)mRot), Long2Fix(midPoint.h), Long2Fix(midPoint.v));		

				// fit extentsBasis inside image rect using given transform
				AlignmentGizmo::FitTransformedRectInside(extentsBasis, &rotation, mImageMaxBounds, oImageRect);
				
				// move midpoint of transformed rect
				AlignmentGizmo::MoveMidpointTo(oImageRect, copyImageForMidpoint, oImageRect);

				//caption rect is empty
				oCaptionRect = MRect ();
				break;
				}//end case

			case caption_Bottom: {
			
				// remove the caption rect by adjusting the height
				mImageMaxBounds.SetHeight(mImageMaxBounds.Height() - height);
									
				// make a copy of this rectangle so that we can move to its midpoint later
				MRect copyImageForMidpoint (mImageMaxBounds);
		
				// make the rotation around the center of the newly determined image rect
				Point midPoint (mImageMaxBounds.MidPoint());
				::RotateMatrix (&rotation, Long2Fix((long)mRot), Long2Fix(midPoint.h), Long2Fix(midPoint.v));		

				// fit extentsBasis inside image rect using given transform
				AlignmentGizmo::FitTransformedRectInside(extentsBasis, &rotation, mImageMaxBounds, oImageRect);

				// move midpoint of transformed rect
				AlignmentGizmo::MoveMidpointTo(oImageRect, copyImageForMidpoint, oImageRect);
				//move down and to the right one pixel to handle qd rects
				oImageRect.Offset(1,1);

				oCaptionRect = inMax;
				oCaptionRect.top = oCaptionRect.bottom - height;
				break;
				}//end case
			case caption_Inside: {
				// make a copy of this rectangle so that we can move to its midpoint later
				MRect copyImageForMidpoint (mImageMaxBounds);
		
				// make the rotation around the center of the newly determined image rect
				Point midPoint (mImageMaxBounds.MidPoint());
				::RotateMatrix (&rotation, Long2Fix((long)mRot), Long2Fix(midPoint.h), Long2Fix(midPoint.v));		

				// fit extentsBasis inside image rect using given transform
				AlignmentGizmo::FitTransformedRectInside(extentsBasis, &rotation, mImageMaxBounds, oImageRect);
				
				// move midpoint of transformed rect
				AlignmentGizmo::MoveMidpointTo(oImageRect, copyImageForMidpoint, oImageRect);

				//nothing fancy needed, since caption is inside max bounds, and image can be max
				oCaptionRect = inMax;
				oCaptionRect.top = oCaptionRect.bottom - height;
				width = min(max((long)kNarrowestCaption, inMax.Width() / 3), inMax.Width());
				oCaptionRect.SetWidth(width);
				break;
				}//end case 
			case caption_RightHorizontal: {
				SInt16 captionWidth (0);
				if (height > 0)
					captionWidth = inMax.Width() * kRightHorizontalCoefficient;;
				
				// remove the caption area by adjusting the width			
				mImageMaxBounds.SetWidth(inMax.Width() - captionWidth);
				
				// make a copy of this rectangle so that we can move to its midpoint later
				MRect copyImageForMidpoint (mImageMaxBounds);

				// make the rotation around the center of the newly determined image rect
				Point midPoint (mImageMaxBounds.MidPoint());
				::RotateMatrix (&rotation, Long2Fix((long)mRot), Long2Fix(midPoint.h), Long2Fix(midPoint.v));		

				// fit extentsBasis inside image rect using given transform
				AlignmentGizmo::FitTransformedRectInside(extentsBasis, &rotation, mImageMaxBounds, oImageRect);
				// move midpoint of transformed rect
				AlignmentGizmo::MoveMidpointTo(oImageRect, copyImageForMidpoint, oImageRect);
				//move down and to the right one pixel to handle qd rects
				oImageRect.Offset(1,1);
				
				
				oCaptionRect = inMax;
				oCaptionRect.Offset(copyImageForMidpoint.Width(), 0); // place it at start of caption area
				
				oCaptionRect.SetWidth(captionWidth - 1); // pixel smaller due to 212 fix 31 july 2001 with next line
				// fix issues w/ qd rotation which can spook this up
				oCaptionRect.Offset(1,0); 

				// Now center it vertically (214 and make it small)
				oCaptionRect.Offset(0, (oCaptionRect.Height() - height) / 2);
				oCaptionRect.bottom = oCaptionRect.top + height;
				break;
				}//end case
			case caption_RightVertical: {
				mImageMaxBounds.SetWidth(mImageMaxBounds.Width() - height);

				// make a copy of this rectangle so that we can move to its midpoint later
				MRect copyImageForMidpoint (mImageMaxBounds);

				// make the rotation around the center of the newly determined image rect
				Point midPoint (mImageMaxBounds.MidPoint());
				::RotateMatrix (&rotation, Long2Fix((long)mRot), Long2Fix(midPoint.h), Long2Fix(midPoint.v));		

				// fit extentsBasis inside image rect using given transform
				AlignmentGizmo::FitTransformedRectInside(extentsBasis, &rotation, mImageMaxBounds, oImageRect);

				// move midpoint of transformed rect
				AlignmentGizmo::MoveMidpointTo(oImageRect, copyImageForMidpoint, oImageRect);
				//move down and to the right one pixel to handle qd rects
				oImageRect.Offset(1,1);

				oCaptionRect = inMax;
				oCaptionRect.left = oCaptionRect.right - height;
				break;
				}//end case
		}//switch

}//end CalcImageCaptionRects


// ---------------------------------------------------------------------------
// CanUseProxy
// ---------------------------------------------------------------------------
bool
PhotoPrintItem::CanUseProxy(const PhotoDrawingProperties& props) const
{
	bool happy (false);

	if (!props.GetPrinting() && gUseProxies)
//		if ((PhotoUtility::DoubleEqual(mRot, 0.0)) && (PhotoUtility::DoubleEqual(mSkew,0.0)))
			happy = true;

	return happy;
} // CanUseProxy


/*
CopyForTemplate.  Note:  does NOT copy the Dest Rect
		because its intended use it for replacement into an existing location (templates!)
		AdjustRectangles is called, which derives all other rects from old destRect
			with the new properties (caption, etc)

	5 jul 2001.  also don't copy rotation/skew, so that template's values can override
	25 Jul 2001 [211] Actually, we sometimes do want to, if we're called from TryToFillFirstEmpty
*/
void
PhotoPrintItem::CopyForTemplate	(
	const PhotoPrintItem&	other,
	const bool				inCopyRotateAndSkew)
{
	mAlias = other.mAlias;
	mFileSpec = other.mFileSpec;
	
	mNaturalBounds = other.GetNaturalBounds();
	mTopCrop = other.mTopCrop;
	mLeftCrop = other.mLeftCrop;
	mBottomCrop = other.mBottomCrop;
	mRightCrop = other.mRightCrop;
	mXScale = other.mXScale;
	mYScale = other.mYScale;
	mTopOffset = other.mTopOffset;
	mLeftOffset = other.mLeftOffset;
	// relative crop-zooming adds these new support fields
	mTopCZ = other.mTopCZ;
	mLeftCZ = other.mLeftCZ;
	mBottomCZ = other.mBottomCZ;
	mRightCZ = other.mRightCZ;
	mUserTopCrop = other.mUserTopCrop;
	mUserLeftCrop = other.mUserLeftCrop;
	mUserBottomCrop = other.mUserBottomCrop;
	mUserRightCrop = other.mUserRightCrop;
	
	mQTI = other.mQTI;
	mProperties = other.GetProperties();

	// hopefully this is a big speed-up
	mProxy = other.mProxy;

	// 211 Do copy skew & rotation if this is part of a drag
	if (inCopyRotateAndSkew) {
		mRot = other.mRot;
		mSkew = other.mSkew;
	}

	//we don't copy the rectangles from the other object
	PhotoDrawingProperties defaultProps;
	{
		MRestoreValue<MRect> restoreImageMax (mImageMaxBounds);
		mImageMaxBounds = MRect ();
		AdjustRectangles(defaultProps);
	}
}//end CopyForTemplate

// ---------------------------------------------------------------------------
// DeriveCropRect
//
// crop is kept as 4 doubles representing percentage offsets from each of 4 sides
// we take those percentages, and convert the image rect
// ---------------------------------------------------------------------------
void
PhotoPrintItem::DeriveCropRect(MRect& outRect) const {
	outRect = GetImageRect();

	SInt16 width (outRect.Width());
	SInt16 height (outRect.Height());

	outRect.top += height * (mTopCrop / 100.0);
	outRect.left += width * (mLeftCrop / 100.0);
	outRect.bottom -= height * (mBottomCrop / 100.0);
	outRect.right -= width * (mRightCrop / 100.0);
	}//end DeriveCropRect



// ---------------------------------------------------------------------------
// DeriveCropZoomRect
//
// cropZoom is kept as 4 doubles representing percentage offsets from each of 4 sides
//	using those percentages, we get a rect which is fit inside (and aligned) to ImageMaxBounds
// this gives us the
//
// requires:  mXScale, mYScale, mTopOffset, mLeftOffset
// 
// ---------------------------------------------------------------------------
void
PhotoPrintItem::DeriveCropZoomRect(ERect32& outCropZoomRect, ERect32& outOffsetExpandedImageRect) const {
	
	ERect32 cropRect;
	ERect32 expandedOffset;
	ERect32 imageRect (GetImageRect());

	// start with the imageRect
	expandedOffset = GetImageRect();
	// bring it to full size using scales
	expandedOffset.SetWidth(imageRect.Width() * mXScale);
	expandedOffset.SetHeight(imageRect.Height() * mYScale);
	SInt32 width (expandedOffset.Width());
	SInt32 height (expandedOffset.Height());

	cropRect = expandedOffset;
	// apply the cropzoom offsets to get the rectangle of interest
	cropRect.top += height * (mTopCZ / 100.0);
	cropRect.left += width * (mLeftCZ / 100.0);
	cropRect.bottom -= height * (mBottomCZ / 100.0);
	cropRect.right -= width * (mRightCZ / 100.0);
	
	// expand this rectangle to fit available space, taking into account any rotation
	ERect32 expandedCropRect (cropRect);
	ERect32 imageMaxBounds (GetImageMaxBounds());
	MMatrixRecord rotation;
	Point midPoint (mImageMaxBounds.MidPoint());
	::RotateMatrix (&rotation, Long2Fix((long)mRot), Long2Fix(midPoint.h), Long2Fix(midPoint.v));		
	// first call just scales rect, doesn't actually move it
	AlignmentGizmo::FitTransformedRectInside(expandedCropRect, &rotation, imageMaxBounds, 
											expandedCropRect);
	// so explicitly move it to middle of image area (imageMaxBounds)
	AlignmentGizmo::MoveMidpointTo(expandedCropRect, imageMaxBounds, expandedCropRect);
	// this is rectangle inside image area which will hold the image.  it does not handle nudging/cropping
	outCropZoomRect = expandedCropRect;
	
	// derive scalars, in case size has changed at this fitting.
	double xScale (expandedCropRect.Width() / (double) cropRect.Width());
	double yScale (expandedCropRect.Height() / (double) cropRect.Height());
	// use one or the other (must be kept in sync.  any difference are rounding error or problem elsewhere
	yScale = xScale;

	outOffsetExpandedImageRect = expandedOffset;
	// apply scalars to outgoing rect to take into account actual (new) scaling
	outOffsetExpandedImageRect.SetHeight(outOffsetExpandedImageRect.Height() * yScale);
	outOffsetExpandedImageRect.SetWidth(outOffsetExpandedImageRect.Width() * xScale);

	// move outImageRect so area of interest lines up w/ (visible) outCropZoomRect

	// first, nudge expanded image rect (hand-tool offset)
	outOffsetExpandedImageRect.Offset(mLeftOffset * outOffsetExpandedImageRect.Width(),
										mTopOffset * outOffsetExpandedImageRect.Height());

	// determine top left of region of interest.
	SPoint32 roi (outOffsetExpandedImageRect.TopLeft());
	roi.h += (mLeftCZ / 100.) * outOffsetExpandedImageRect.Width();
	roi.v += (mTopCZ / 100.) * outOffsetExpandedImageRect.Height();
	
	// we want to focus on roi, so move imageRect by topLeft - roi
	outOffsetExpandedImageRect.Offset(outOffsetExpandedImageRect.left - roi.h, 
										outOffsetExpandedImageRect.top - roi.v);

	// compensate for the difference between imageRect and expandedCropRect
	outOffsetExpandedImageRect.Offset(outCropZoomRect.left - mImageRect.left,
										outCropZoomRect.top - mImageRect.top);

	}//end DeriveCropZoomRect




//--------------------------------------------------
// DeriveSourceRect
//
// when drawing a qti, there may be benefit to only drawing the visible portion of the
// source-rect, since crop-zoom often expands an image mightily.  (save time + space by not
// expanding, caching, and drawing clipped pixels
//--------------------------------------------------
void
PhotoPrintItem::DeriveSourceRect(MRect& outSource, const RgnHandle inClip) {
	// if there is incoming clipping, it's worth doing this.
	if (inClip != nil) {
		ERect32 cropZoom;
		ERect32 expandedOffsetImage;
		DeriveCropZoomRect(cropZoom, expandedOffsetImage);
		
		MRect derivedCrop;
		DeriveCropRect(derivedCrop);
		ERect32 derivedCrop32 (derivedCrop);
		
		double topCrop, leftCrop, bottomCrop, rightCrop;
		PhotoUtility::CalcCropValuesAsPercentages(derivedCrop32, expandedOffsetImage, 
										 topCrop,  leftCrop, 
										 bottomCrop,  rightCrop, kClampToBounds);
		
		outSource = GetNaturalBounds();
		UInt32 width (outSource.Width());
		UInt32 height (outSource.Height());
		
		outSource.top += height * (topCrop / 100.);
		outSource.left += width * (leftCrop / 100.);
		outSource.bottom -= height * (bottomCrop / 100.);
		outSource.right -= width * (rightCrop / 100.);
					
		}//endif incoming clipping
	else
		outSource = MRect ();
	}//end



// ---------------------------------------------------------------------------
// PhotoPrintItem::Draw
//			set the qt matrix + have the component render
// ---------------------------------------------------------------------------
void
PhotoPrintItem::Draw(
	const PhotoDrawingProperties&	props,
	MatrixRecord*					worldSpace,
	CGrafPtr						inDestPort,
	GDHandle						inDestDevice,
	RgnHandle						inClip)
{
	StValueChanger<Boolean>	saveCanResolveAlias (mCanResolveAlias, false);

	if (gDrawMaxBounds && mMaxBounds) {
		StColorPenState saveState;
		StColorPenState::Normalize();
		RGBColor		chartreuse = { 38912, 57856, 1000 };
		::RGBForeColor(&chartreuse);
		::PenSize(2, 2);
		MRect			copyMaxBounds (mMaxBounds);
		if (worldSpace)
			::TransformRect(worldSpace, &copyMaxBounds, NULL);
		::FrameRect(&copyMaxBounds);
		}//endif debugging the cell
	
	ProxyRef			localProxy;
	HORef<StLockPixels> possibleProxyLocker;
	
	if (this->CanUseProxy (props)) localProxy = this->GetProxy ();
	
	if (localProxy) 
		possibleProxyLocker = new StLockPixels (localProxy->GetMacGWorld());
	else
		this->DeleteProxy (); // ensure that if we aren't using proxy, it's deleted (mainly debugging issue)
	
	//	At this point, we should only use our local locked proxy which is not going anywhere.

	try {
		MatrixRecord	imageSpace;
		SetupDestMatrix(&imageSpace, kDoScale, kDoRotation);

		MatrixRecord	compositeSpace;
		// if we're drawing proxy, matrix is slightly different (since proxy size differs)
		if (localProxy) {
			SetupProxyMatrix(localProxy, &compositeSpace, kDoScale, kDoRotation);
			}//endif
		
		else {
			// else start with the imageSpace matrix
			::CopyMatrix(&imageSpace, &compositeSpace);
			}//else			
		
		if (worldSpace) { // composite in any worldspace xforms
			::ConcatMatrix(worldSpace, &compositeSpace);
			}//endif					
				
		HORef<MRegion>	cropRgn;
		RgnHandle		workingCrop(this->ResolveCropStuff(cropRgn, inClip, worldSpace));

		do {
			if (this->IsEmpty()) {
				if (!props.GetPrinting()) {
					this->DrawEmpty(&compositeSpace, inDestPort, inDestDevice, workingCrop);
				}
				continue;
			} //endif empty

			if (localProxy) {
				this->DrawProxy(localProxy, props, &compositeSpace, inDestPort, inDestDevice, workingCrop);
				continue;
				}

			//and try to draw the image
			try {
				CodecQ		quality;
				if (props.GetPrinting())
					quality = kPrintQuality;
				else
					quality = kProxyQuality;
				this->DrawImage(&compositeSpace, inDestPort, inDestDevice, workingCrop, quality);
				}//end try
			catch (...) {
				MatrixRecord rotatedWorldSpace;
				SetupDestMatrix(&rotatedWorldSpace, kIgnoreScale, kDoRotation); // no scale, just rotation);
				::ConcatMatrix(worldSpace, &rotatedWorldSpace);
				DrawMissing (&rotatedWorldSpace, inDestPort, inDestDevice, inClip);
				mQTI = nil;
				}//end catch
		} while (false);

		if (this->GetProperties().HasCaption()) {
			this->DrawCaption(worldSpace, inClip, props); //caption should just deal with inner clip (cause image cropping could screw it up)
		}
	}//end try
	
	catch (LException& e) {
		// An exception during drawing is pretty bad news.
		mQTI = nil;
		ExceptionHandler::HandleKnownExceptions (e, true);
	} // catch
	
	catch (...) {
		mQTI = nil;
		//	???
	}//end catch
	
	mQTI = nil; // if we've made it during this draw operation, make sure to free it here
} // Draw

void
PhotoPrintItem::DrawCaption(MatrixRecord* inWorldSpace, RgnHandle inPassthroughClip, const PhotoDrawingProperties& drawProps)
{
	PhotoItemProperties&	props(this->GetProperties());
	MPString			theCaption(props.GetCaption());

	StColorPenState		restorePen;
	StTextState			restoreText;
	::RGBForeColor(&Color_Black);

	SInt16				offset = 0;
	SInt16				captionLineHeight (props.GetCaptionLineHeight());
	captionLineHeight *= ((double)drawProps.GetScreenRes()) / 72.0;

	if (theCaption.Length() > 0) {
		this->DrawCaptionText(inWorldSpace, theCaption, offset, inPassthroughClip, drawProps);
		offset += captionLineHeight;
	}

	if (! this->IsEmpty() && props.GetShowName()) {
		Str255	fileName;
		this->GetName(fileName);
		this->DrawCaptionText(inWorldSpace, fileName, offset, inPassthroughClip, drawProps);
		offset += captionLineHeight;
	}

	if (! this->IsEmpty() && props.GetShowDate()) {
		LStr255			date;
		UInt32			modTime = this->GetModifiedTime();
		if (modTime != 0) {
			EChrono::GetDateTime(date, modTime,
				PhotoPrintPrefs::Singleton()->GetDateFormat(), PhotoPrintPrefs::Singleton()->GetTimeFormat());
			this->DrawCaptionText(inWorldSpace, date, offset, inPassthroughClip, drawProps);
		}
		offset += captionLineHeight;
	}
} // DrawCaption

/*
DrawCaptionText
*/
void
PhotoPrintItem::DrawCaptionText(MatrixRecord* inWorldSpace, ConstStr255Param inText, const SInt16 inVerticalOffset, 
								RgnHandle inClip, const PhotoDrawingProperties& drawProps)
{
#ifndef TEXT_ROTATES_WITH_IMAGE
#pragma unused (inClip)
#endif
	MRect				bounds(mCaptionRect);

	// and respect any worldspace xform that has come in
	if (inWorldSpace) // composite in any worldspace xforms
		::TransformRect(inWorldSpace, &bounds, NULL);

	// setup the matrix
	MatrixRecord		mat;
	::SetIdentityMatrix(&mat);

	MatrixRecord		rotator;
	bool				additionalRotation = false;
	if (this->GetProperties().GetCaptionStyle() == caption_RightVertical) {
		additionalRotation = true;
		Point			midPoint = bounds.MidPoint(); //rotate around center of full caption rect
		::SetIdentityMatrix(&rotator);
		::RotateMatrix(&rotator, ::Long2Fix(270), ::Long2Fix(midPoint.h), ::Long2Fix(midPoint.v));
		// and we have to change the rectangle
		::TransformRect(&rotator, &bounds, nil);
	}

	// Take the offset into account (this moves down to individual lines in the caption)
	bounds.top += inVerticalOffset;

	// start with a translate to topleft of caption rect
	::TranslateMatrix(&mat, ::FixRatio(bounds.left, 1), ::FixRatio(bounds.top, 1));

	// If the caption is in a rotated style, include that transformation
	if (additionalRotation) {
		::ConcatMatrix(&rotator, &mat);
	}

	// then any rotation happens around center of image rect
	if (mProperties.GetCaptionLinkedRotation() &&
		(!PhotoUtility::DoubleEqual(mRot, 0.0))) {
		MRect		dest (this->GetImageRect());
		if (inWorldSpace)
			::TransformRect(inWorldSpace, &dest, NULL);
		Point		midPoint = dest.MidPoint();
		::RotateMatrix(&mat, ::Long2Fix(static_cast<long>(mRot)),
			::Long2Fix(midPoint.h), ::Long2Fix(midPoint.v));
	}

	{
	// Use a StQuicktimeRenderer to draw rotated text (we only make one if we have to, both as an
	// optimization, and to work around a Mac OS X DP4 bug)
	HORef<StQuicktimeRenderer>		qtr;
	if (additionalRotation
#ifdef TEXT_ROTATES_WITH_IMAGE //taken out 20 aug 2001 dml
		|| !PhotoUtility::DoubleEqual(mRot, 0.0)
#endif
		) {
		qtr = new StQuicktimeRenderer(bounds, 1, useTempMem, &mat, inClip);
	} else {
		// We're not rotating, so we won't be blitting white pixels from the GWorld. We may
		// want to draw our own white pixels.
		if (this->GetProperties().GetCaptionStyle() == caption_Inside) {
			::RGBBackColor(&Color_White);
			bounds.Erase();
		}//endif caption_inside requires erase
	}//else not rotating

	::TextFont(this->GetProperties().GetFontNumber());
	SInt16 txtSize (this->GetProperties().GetFontSize());
	txtSize *= ((double)drawProps.GetScreenRes()) / 72.0;
	::TextSize(txtSize);
	UTextDrawing::DrawTruncatedWithJust(inText, bounds, teJustCenter, true, true, truncEnd);
	}//end QTRendering block
} // end DrawCaptionText

/*
DrawEmpty
*/
void
PhotoPrintItem::DrawEmpty(MatrixRecord* localSpace, // already composited and ready to use
						 CGrafPtr inDestPort,
						 GDHandle inDestDevice,
						 RgnHandle inClip,
						 const OSType inKind)
{
	MRect	bounds (GetImageRect());
	if (!bounds) // if no bounds, use the maxBounds
		bounds = GetImageMaxBounds();

	enum cornerType {
		kTopLeft = 0,
		kTopRight,
		kBotRight,
		kBotLeft,
		kFnordCorner};
		
	Point	corners[kFnordCorner];
	corners[kTopLeft] = bounds.TopLeft();
	corners[kBotRight] = bounds.BotRight();
	corners[kBotRight].h--;						// Take into account the way QuickDraw rects work
	corners[kBotRight].v--;						// Take into account the way QuickDraw rects work
	corners[kTopRight].h = corners[kBotRight].h;
	corners[kTopRight].v = corners[kTopLeft].v;
	corners[kBotLeft].h = corners[kTopLeft].h;
	corners[kBotLeft].v = corners[kBotRight].v;

	// transform those corners by composite matrix
	::TransformPoints(localSpace, corners, kFnordCorner);

	HORef<StGDeviceSaver> saveDevice;
	CGrafPtr	destPort;
	if (inDestPort != nil)
		destPort = inDestPort;
	else
		::GetPort((GrafPtr*)&destPort);
		
	StColorPortState		saveState ((GrafPtr)destPort);
	if (inDestDevice != nil) {
		saveDevice = new StGDeviceSaver;
		::SetGWorld(destPort, inDestDevice);
		}//endif device specified 

	// Just initialize the stuff we care about (don't normalize entire StColorPortState)
	StColorPenState::Normalize();
// !?!?	StClipRgnState::Normalize();

	HORef<StClipRgnState>	saveClip;
	if (inClip != nil) {
		saveClip = new StClipRgnState (inClip);
		}//endif clipping to do

	if (inKind == kMissing) {
		RGBColor	fireEngine = { 65000, 0, 0 };
		::RGBForeColor(&fireEngine);
		StColorPenState		restorePattern;
		StColorPenState::SetGrayPattern();
		PhotoUtility::DrawXformedRect(bounds, localSpace, kPaint);
	} else {
		::RGBForeColor(&PhotoUtility::sNonReproBlue);
	}
	
	::MoveTo(corners[kTopLeft].h, corners[kTopLeft].v);
	::LineTo(corners[kTopRight].h, corners[kTopRight].v);
	::LineTo(corners[kBotRight].h, corners[kBotRight].v);
	::LineTo(corners[kBotLeft].h, corners[kBotLeft].v);
	::LineTo(corners[kTopLeft].h, corners[kTopLeft].v);
	if (inKind == kMissing) {
		::PenSize(2, 2);
	}
	::LineTo(corners[kBotRight].h, corners[kBotRight].v);
	::MoveTo(corners[kBotLeft].h, corners[kBotLeft].v);
	::LineTo(corners[kTopRight].h, corners[kTopRight].v);
}//end DrawEmpty

// ---------------------------------------------------------------------------
// DrawImage
//	Does the basic drawing. Called by Draw and MakeProxy.  Errors caught upstream
// ---------------------------------------------------------------------------

static const long kHeightThreshold (4000);
static const long kWidthThreshold (4000);

void
PhotoPrintItem::DrawImage(
	MatrixRecord*	inLocalSpace, 
	 CGrafPtr		inDestPort,
	 GDHandle		inDestDevice,
	 RgnHandle		inClip,
	 const CodecQ	inQuality) 
{
	if ((mQTI == nil) && (mAlias != nil)) {
		ReanimateQTI();		
		}//endif
	
	mQTI->SetMatrix (*inLocalSpace);

	MRect				derivedSource;
	this->DeriveSourceRect(derivedSource, inClip);

	// 364 Set up a GWorld to draw in if we're printing.
	// We're not directly checking for printing, but MakeProxy passes nil for inClip, and
	// DeriveSourceRect will return an empty rect if there's no clipping.
	GrafPtr				drawingPort = inDestPort;
	HORef<EGWorld>		possibleOffscreen;
	MRect				drawingRect;
	MRect				tileRect;
	MRect derivedTile;

	if (!derivedSource.IsEmpty()) {
		drawingRect = derivedSource;
		::TransformRect(inLocalSpace, &drawingRect, nil);
		tileRect = drawingRect;
		tileRect.SetHeight(min(drawingRect.Height(), kHeightThreshold));
		tileRect.SetWidth(min(drawingRect.Width(), kWidthThreshold));
		try {
			possibleOffscreen = new EGWorld(tileRect, 32, nil, nil, nil, EGWorld::kTryLocalMemFirst);
		} catch (LException e) {
			// Swallow out of memory
			if (e.GetErrorCode() != memFullErr && e.GetErrorCode() != cTempMemErr) {
				mQTI = nil;
				throw;
				}//endif
		}//catch
	}//endif we're not making a proxy and thus oestensibly printing


	for (long row = 0; row <= drawingRect.Height() / kHeightThreshold; ++row) {
		for (long col = 0; col <= drawingRect.Width() / kWidthThreshold; ++col) {
			derivedTile = tileRect;
			derivedTile.Offset(kWidthThreshold * col, kHeightThreshold * row); 
			if (possibleOffscreen) {
				possibleOffscreen->SetBounds(derivedTile);
				}//endif

			try {
				// 364 Draw offscreen
				if (possibleOffscreen) {
					possibleOffscreen->BeginDrawing();
					StColorPenState::Normalize();
					::EraseRect(&tileRect);
					inDestPort = possibleOffscreen->GetMacGWorld();
					inDestDevice = ::GetGWorldDevice(possibleOffscreen->GetMacGWorld());
				}//endif

				if (inDestPort && inDestDevice) 
					mQTI->SetGWorld (inDestPort, inDestDevice);

				if (derivedSource.IsEmpty())
					mQTI->SetSourceRect(nil);
				else
					mQTI->SetSourceRect(&derivedSource);	
				
				mQTI->SetClip (inClip);
				mQTI->SetQuality (inQuality);
				mQTI->Draw();

			} catch (...) {
				if (possibleOffscreen) {
					// Be sure we're not left with offscreen as the grafPort
					possibleOffscreen->EndDrawing();
				}//endif offscreen is around
				mQTI = nil;
				throw;
			}//catch

			// 364 Blit to actual destination
			if (possibleOffscreen) {
				possibleOffscreen->EndDrawing();
				MRect clampedDerivedTile (derivedTile);
				clampedDerivedTile *= drawingRect;
				possibleOffscreen->CopyPartialImage(clampedDerivedTile, drawingPort, clampedDerivedTile, srcCopy, inClip);
			} //endif end drawing offscreen + copy back

			}//for cols
		}//for rows


	// free 'dat QT memory!!
	mQTI = nil; // if we've made it during this draw operation, make sure to free it here
} // DrawImage

// ---------------------------------------------------------------------------
// DrawMissing
//	Draws a missing image.  for now it just looks like an empty image.
// ---------------------------------------------------------------------------
void
PhotoPrintItem::DrawMissing(
	MatrixRecord*	inLocalSpace, // already composited and ready to use
	 CGrafPtr		inDestPort,
	 GDHandle		inDestDevice,
	 RgnHandle		inClip) 
{
	this->DrawEmpty(inLocalSpace, inDestPort, inDestDevice, inClip, kMissing);
} // DrawMissing

// ---------------------------------------------------------------------------
// DrawProxy
// ---------------------------------------------------------------------------
//	Precondition: inProxy exists and is locked

void
PhotoPrintItem::DrawProxy(ProxyRef localProxy,
						 const PhotoDrawingProperties& props,
						 MatrixRecord* inLocalSpace, // already composited and ready to use
						 CGrafPtr inDestPort,
						 GDHandle inDestDevice,
						 RgnHandle inClip)
{
		//	Make sure it doesn't go away
	Assert_(localProxy);
	
	PixMapHandle			sourcePixels = ::GetGWorldPixMap(localProxy->GetMacGWorld());
	StLockPixels 			locker (sourcePixels);

	HORef<StClipRgnState> saveClip;
	if (inClip != nil)
		saveClip = new StClipRgnState(inClip);
	HORef<StGDeviceSaver> saveDevice;
	CGrafPtr	destPort;
	if (inDestPort != nil)
		destPort = inDestPort;
	else
		::GetPort((GrafPtr*)&destPort);
		
	StColorPortState	saveState ((GrafPtr)destPort);
	if (inDestDevice != nil) {
		saveDevice = new StGDeviceSaver;
		::SetGWorld(destPort, inDestDevice);
	}//endif device specified 

	ImageDescriptionHandle	sourceDesc;
	OSErr					err = ::MakeImageDescriptionForPixMap(sourcePixels, &sourceDesc);
	ThrowIfOSErr_(err);


	UInt32					quality;
	ComponentRecord*		codec;
	if (props.GetDraft()) {
		quality = codecLowQuality;
		codec = bestSpeedCodec;
		}//endif draft mode
	else {
		quality = codecMaxQuality;
		codec = bestFidelityCodec;
		}//else

	err = ::FDecompressImage(::GetPixBaseAddr(sourcePixels),
						sourceDesc,
						::GetPortPixMap(destPort),
						nil,					// Decompress entire source
						inLocalSpace,
						srcCopy,
						inClip,					// mask
						nil,					// matte
						nil,					// matteRect
						quality,				// accuracy
						codec,					// codec
						0,						// dataSize not needed with no dataProc
						nil,					// dataProc
						nil);					// progressProc

	::DisposeHandle((Handle)sourceDesc);
	ThrowIfOSErr_(err);
}//end DrawProxy				


//-------------------------------------------------------------------------------------
//	DrawIntoNewPictureWithRotation
//-------------------------------------------------------------------------------------
//	Precondition: localProxy has been locked if it exists

void
PhotoPrintItem::DrawIntoNewPictureWithRotation(ProxyRef localProxy, double inRot, const MRect& destBounds, MNewPicture& destPict)
{
	MRect			imageBounds;
	
	// MakeRotatedThumbnails has already tried to make the proxy (if needed) and lock it down (if avail)
	if (localProxy) {
		localProxy->GetBounds(imageBounds);
		imageBounds.Offset(-imageBounds.left, -imageBounds.top);
		}//endif proxy is avail, lock it down for this duration!!
	else {
		if (IsEmpty()) {
			imageBounds = GetDestRect();
			if (!imageBounds)
				imageBounds = GetMaxBounds();
			}// endif empty placeholder
		else
			imageBounds = GetNaturalBounds();
		}//else no proxy
	
	MRect			aspectDest;
	AlignmentGizmo::FitAndAlignRectInside(imageBounds, destBounds, kAlignAbsoluteCenter, aspectDest, EUtil::kDontExpand);

	MatrixRecord mat;
	::SetIdentityMatrix(&mat);
	::RectMatrix(&mat, &imageBounds, &aspectDest);
	::RotateMatrix(&mat, ::Long2Fix(inRot ), 
						::Long2Fix(destBounds.MidPoint().h), ::Long2Fix(destBounds.MidPoint().v));
	// be a good citizen and setup clipping
	MNewRegion clip;
	clip = destBounds; 

	// rotate the aspectDest rect so we can make a minimal sized picture
	MatrixRecord rotOnly;
	::SetIdentityMatrix(&rotOnly);
	::RotateMatrix(&rotOnly, ::Long2Fix(inRot ), 
					::Long2Fix(destBounds.MidPoint().h), ::Long2Fix(destBounds.MidPoint().v));		
	MRect rotAspectDest (aspectDest);
	::TransformRect(&rotOnly, &rotAspectDest, nil);

	EGWorld			offscreen(rotAspectDest, gProxyBitDepth);
	GWorldPtr		theGWorld = offscreen.GetMacGWorld();
	GDHandle		offscreenDevice(::GetGWorldDevice(theGWorld));
	StGrafPortSaver	savePort(theGWorld);		// Out of paranoia, make sure we've got a port

	// render the rotated proxy
	PhotoDrawingProperties	props (kNotPrinting, kPreview, kDraft);
	if (localProxy != nil)
		this->DrawProxy(localProxy, props, &mat, theGWorld, offscreenDevice, clip);
	else {
		if (this->IsEmpty())
			this->DrawEmpty(&mat, theGWorld, offscreenDevice, clip);
		else {
			try {
				this->DrawImage(&mat, theGWorld, offscreenDevice, clip, kProxyQuality);
				}//end try
			catch (...) {
				this->DrawEmpty(&mat, theGWorld, offscreenDevice, clip);
				}//end catch
			}//else we're not empty
		}//else no proxy
	// now we need to blit that offscreen into another, while a Picture is open to capture it into a pict
	LGWorld			offscreen2(rotAspectDest, gProxyBitDepth);
	if (offscreen2.BeginDrawing()) {
		MNewPicture			pict;			// Creates a PICT and destroys it
		{
			MOpenPicture	openPicture(pict, rotAspectDest);
			offscreen.CopyImage(UQDGlobals::GetCurrentPort(), rotAspectDest, ditherCopy);
		}
		offscreen2.EndDrawing();

		destPict.Attach(pict.Detach());		// Transfer ownership of the PICT
	} // if
}//end DrawIntoNewPictureWithRotation

#pragma mark -

/*
CheckExactHeight
*/
void
PhotoPrintItem::CheckExactHeight(
	long&			ioWidth,
	long&			ioHeight,
	OSType&			outCode, 
	SInt16&			outUnits,
	const double	inTestWidth,
	const double	inTestHeight,
	const OSType	inCode) const
{
	if (std::abs((long)(ioWidth - inTestWidth * kDPI)) < kDimDelta && std::abs((long)(ioHeight - inTestHeight * kDPI)) < kTinyDelta) {
		outUnits = si_Inches;
		ioWidth = inTestWidth;
		ioHeight = inTestHeight;
		outCode = inCode;
	}
} // CheckExactHeight

/*
CheckExactWidth
*/
void
PhotoPrintItem::CheckExactWidth(
	long&			ioWidth,
	long&			ioHeight,
	OSType&			outCode, 
	SInt16&			outUnits,
	const double	inTestWidth,
	const double	inTestHeight,
	const OSType	inCode) const
{
	if (std::abs((long)(ioWidth - (inTestWidth * kDPI))) < kTinyDelta && 
		std::abs((long)(ioHeight - (inTestHeight * kDPI))) < kDimDelta) {
		outUnits = si_Inches;
		ioWidth = inTestWidth;
		ioHeight = inTestHeight;
		outCode = inCode;
	}
} // CheckExactWidth

// ---------------------------------------------------------------------------
// GetCaptionRect
// ---------------------------------------------------------------------------
PhotoPrintItem::ItemRect
PhotoPrintItem::GetCaptionRect (void) const {
	return mCaptionRect;
}//end GetCaptionRect 

// ---------------------------------------------------------------------------
// GetCreatedTime.  useful for 
// ---------------------------------------------------------------------------
UInt32
PhotoPrintItem::GetCreatedTime() 
{
	UInt32		theTime = 0;

	if (!this->IsEmpty()) {
		// Don't put up error dialogs in the middle of this
		StDisableDebugThrow_();
		StDisableDebugSignal_();
		try {
			CInfoPBRec			info;
			HORef<MFileSpec>	theSpec = this->GetFileSpec();
			if (theSpec != nil) {
				theSpec->GetCatInfo(info);
				theTime = info.hFileInfo.ioFlCrDat;
			}
		} catch (...) {
			// Eat exceptions; the most likely one here is a file no longer being present,
			// and there are other places we should notice that
		}
	}

	return theTime;
} // GetCreatedTime

// ---------------------------------------------------------------------------
// GetCrop
// 
void
PhotoPrintItem::GetCrop(double& outTopCrop, double& outLeftCrop, double& outBottomCrop, double& outRightCrop) const {
	outTopCrop = mTopCrop;
	outLeftCrop = mLeftCrop;
	outBottomCrop = mBottomCrop;
	outRightCrop = mRightCrop;
}//end GetCrop


//---------------------------------------------------------------------------
// GetCropZoom
//---------------------------------------------------------------------------
void
PhotoPrintItem::GetCropZoom(double& outTop, double& outLeft, double& outBottom, double& outRight) {
	outTop = mTopCZ;
	outLeft = mLeftCZ;
	outBottom = mBottomCZ;
	outRight = mRightCZ;
	}//end GetCropZoom



// ---------------------------------------------------------------------------
// GetCropZoomOffset
// 
void
PhotoPrintItem::GetCropZoomOffset(double& outTopOffset, double& outLeftOffset) {

//	ERect32 cropZoomRect;
//	ERect32 expandedOffsetImageRect;
//	DeriveCropZoomRect(cropZoomRect, expandedOffsetImageRect);
	
	outTopOffset = mTopOffset;
	outLeftOffset = mLeftOffset;
	}//end GetCropZoomOffset

// ---------------------------------------------------------------------------
// GetCropZoomScales
// ---------------------------------------------------------------------------
void
PhotoPrintItem::GetCropZoomScales(double& outZoomScaleX, double& outZoomScaleY) {	
//	ERect32 cropZoomRect;
//	ERect32	expandedOffsetImageRect;
//	DeriveCropZoomRect(cropZoomRect, expandedOffsetImageRect);

	outZoomScaleX = mXScale;
	outZoomScaleY = mYScale;
}//end GetCropZoomScales


// ---------------------------------------------------------------------------
// GetDestRect
// ---------------------------------------------------------------------------
PhotoPrintItem::ItemRect
PhotoPrintItem::GetDestRect (void) const {
	return mDest;
}//end GetDestRect 

// ---------------------------------------------------------------------------
// GetDimensions 
// 	Returns both a 4-letter code and a human-readable version H x W
// ---------------------------------------------------------------------------
OSType
PhotoPrintItem::GetDimensions(Str255 outDescriptor, const SInt16 inResolution, const SInt16 inWhich) const
{
	OSType		code = 'cust';
	long		width;
	long		height;
	SInt16		unitIndex = si_Pixels;

	if (inWhich == si_NaturalBounds) {
		// Get the natural bounds of the image
		width = this->GetNaturalBounds().Width();
		height = this->GetNaturalBounds().Height();
	} else {
		// Get the size in printed points

		// if maxBounds is set, then use that, otherwise use actual bounds
		MRect referenceBounds;
		if (this->GetMaxBounds()) // boolean operator returns empty or not
			referenceBounds = GetMaxBounds();
		else
			referenceBounds = GetDestRect();

		width = referenceBounds.Width() * ((double)kDPI / (double)inResolution);
		height = referenceBounds.Height() * ((double)kDPI / (double)inResolution);
		this->CheckExactHeight(width, height, code, unitIndex, 2, 3, '3*2 ');
		this->CheckExactWidth(width, height, code, unitIndex, 3, 2, '2*3 ');
		this->CheckExactHeight(width, height, code, unitIndex, 3.5, 5, '5*3 ');
		this->CheckExactWidth(width, height, code, unitIndex, 5, 3.5, '3*5 ');
		this->CheckExactHeight(width, height, code, unitIndex, 4, 6, '6*4 ');
		this->CheckExactWidth(width, height, code, unitIndex, 6, 4, '4*6 ');
		this->CheckExactHeight(width, height, code, unitIndex, 5, 7, '7*5 ');
		this->CheckExactWidth(width, height, code, unitIndex, 7, 5, '5*7 ');
		this->CheckExactHeight(width, height, code, unitIndex, 7.5, 10, '10*7');
		this->CheckExactWidth(width, height, code, unitIndex, 10, 7.5, '7*10');
	}

	LStr255		w(width);
	LStr255		h(height);
	MPString	text(str_ImageSize, inWhich);
	MPString	units(str_ImageSize, unitIndex);
	text.Replace(h, "\p^0");
	text.Replace(w, "\p^1");
	text.Replace(units, "\p^2");
	::BlockMoveData(text, outDescriptor, text.Length() + 1);

	return code;
} // GetDimensions

// ---------------------------------------------------------------------------
// GetFrameRect
// ---------------------------------------------------------------------------
PhotoPrintItem::ItemRect
PhotoPrintItem::GetFrameRect (void) const {
	return mFrameRect;
}//end GetFrameRect 

// ---------------------------------------------------------------------------
//	GetExpandedOffsetImageRect  
//		returns the size of the entire image at current resolution offset to appear correcty in cropwindow
//		take the image rect.  scale it by our cropzoom factors, 
//		and offset in case we dragged the image within the crop'd window
// ---------------------------------------------------------------------------
void
PhotoPrintItem::GetExpandedOffsetImageRect(MRect& outRect) const
{
	outRect = GetImageRect();

	outRect.SetWidth(outRect.Width() * mXScale);
	outRect.SetHeight(outRect.Height() * mYScale);

	outRect.Offset(mLeftOffset * outRect.Width(), mTopOffset * outRect.Height());	
}//end GetExpandedOffsetImageaRect


// ---------------------------------------------------------------------------
// GetFileSpec:  forces resolution of the alias.
//		Note that we may return nil (especially in the case of a placeholder)
// ---------------------------------------------------------------------------
HORef<MFileSpec>
PhotoPrintItem::GetFileSpec() const
{
	if (mAlias != nil) {
		try {
			AEInteractAllowed	level;
			AEGetInteractionAllowed (&level);
				
			bool	canResolveUI = mCanResolveAlias && (level != kAEInteractWithSelf);
			
			Boolean outChanged;
			FSSpec	newSpec;
			
			if (canResolveUI)
				mAlias->Resolve (newSpec, outChanged);
			else mAlias->ResolveWithMountFlags (newSpec, outChanged, kResolveAliasFileNoUI);
				
			if (outChanged || (mFileSpec == nil))
				mFileSpec = new MFileSpec (newSpec);
			} // try
			
		catch (...) {
			mFileSpec = nil;
			// No need to propagate the exception
			} // catch
		} // if
		
	return mFileSpec;
} // GetFileSpec


// ---------------------------------------------------------------------------
// GetMatrix 
// (might be stale but faster if don't force recompute)
// ---------------------------------------------------------------------------
void
PhotoPrintItem::GetMatrix(MatrixRecord* pDestMatrix,
							bool inDoScale,
							bool inDoRotate)
{
	this->SetupDestMatrix(pDestMatrix, inDoScale, inDoRotate);
}//end GetMatrix	

// ---------------------------------------------------------------------------
// GetImageMaxBounds
// ---------------------------------------------------------------------------
PhotoPrintItem::ItemRect
PhotoPrintItem::GetImageMaxBounds (void) const {
	return mImageMaxBounds;
}//end GetImageMaxBounds 

// ---------------------------------------------------------------------------
// GetImageRect
// ---------------------------------------------------------------------------
PhotoPrintItem::ItemRect
PhotoPrintItem::GetImageRect (void) const {
	return mImageRect;
}//end GetImageRect 

// ---------------------------------------------------------------------------
// GetMaxBounds
// ---------------------------------------------------------------------------
PhotoPrintItem::ItemRect
PhotoPrintItem::GetMaxBounds (void) const {
	return mMaxBounds;
}//end GetMaxBounds 

// ---------------------------------------------------------------------------
// GetModifiedTime
// ---------------------------------------------------------------------------
UInt32
PhotoPrintItem::GetModifiedTime() 
{
	UInt32		theTime = 0;

	if (!this->IsEmpty()) {
		// Don't put up error dialogs in the middle of this
		StDisableDebugThrow_();
		StDisableDebugSignal_();
		try {
			CInfoPBRec			info;
			HORef<MFileSpec>	theSpec = this->GetFileSpec();
			if (theSpec != nil) {
				theSpec->GetCatInfo(info);
				theTime =  info.hFileInfo.ioFlMdDat;
			}
		} catch (...) {
			// Eat exceptions; the most likely one here is a file no longer being present,
			// and there are other places we should notice that
		}
	}

	return theTime;
} // GetModifiedTime


// ---------------------------------------------------------------------------
// GetName.  not unicode ready
// ---------------------------------------------------------------------------
void
PhotoPrintItem::GetName(Str255 outName)
{
	outName[0] = 0;

	if (!this->IsEmpty()) {
		HORef<MFileSpec>	theSpec = this->GetFileSpec();
		if (theSpec != nil) {
			::memcpy(outName, theSpec->Name(), sizeof(Str255));
		}
	}
}//end GetName


// ---------------------------------------------------------------------------
// GetNaturalBounds
// ---------------------------------------------------------------------------
PhotoPrintItem::ItemRect
PhotoPrintItem::GetNaturalBounds (void) const {
	return mNaturalBounds;
}//end GetNaturalBounds 

// ---------------------------------------------------------------------------
//	GetProxy
//		tries hard to return a valid proxy.  will make new if purged or non-existant
// ---------------------------------------------------------------------------
PhotoPrintItem::ProxyRef
PhotoPrintItem::GetProxy() 
{
	// If the proxy pixels were purged, delete the proxy (and start afresh)
	if ((mProxy != nil) && (mProxy->IsPurged()))
		mProxy = nil;

	if (mProxy == nil)
		this->MakeProxy();

	return mProxy;
} // GetProxy


// ---------------------------------------------------------------------------
// GetTransformedBounds 
// 		once we are rotated, our bounding box changes
//
// 26 jul 2001 slithy:  using maxBounds now, as Dest transitioning to meaningless.  
// ---------------------------------------------------------------------------
MRect
PhotoPrintItem::GetTransformedBounds() {
	Point	corners[4];
	MRect	dest (GetMaxBounds());
	corners[0] = dest.TopLeft();
	corners[3] = dest.BotRight();
	corners[1].v = corners[0].v; // topRight
	corners[1].h = corners[3].h;
	corners[2].v = corners[3].v; // bottomLeft
	corners[2].h = corners[0].h;
	
	MatrixRecord m;
	SetupDestMatrix(&m, kIgnoreScale, kDoRotation);
	::TransformPoints(&m, corners, 4);
	
	MRect bounds;
	bounds.left = min(min(corners[0].h, corners[1].h), min(corners[2].h, corners[3].h));
	bounds.right = max(max(corners[0].h, corners[1].h), max(corners[2].h, corners[3].h));
	bounds.top = min(min(corners[0].v, corners[1].v), min(corners[2].v, corners[3].v));
	bounds.bottom = max(max(corners[0].v, corners[1].v), max(corners[2].v, corners[3].v));

	return bounds;
	}//end	 




// ---------------------------------------------------------------------------
// GetUserCrop
// 
void
PhotoPrintItem::GetUserCrop(double& outTopCrop, double& outLeftCrop, double& outBottomCrop, double& outRightCrop) const {
	outTopCrop = mUserTopCrop;
	outLeftCrop = mUserLeftCrop;
	outBottomCrop = mUserBottomCrop;
	outRightCrop = mUserRightCrop;
}//end GetUserCrop




// ---------------------------------------------------------------------------
// HasCrop
// ---------------------------------------------------------------------------
bool
PhotoPrintItem::HasCrop() const {
	return (!(PhotoUtility::DoubleEqual(mTopCrop,0.0) &&
			PhotoUtility::DoubleEqual(mLeftCrop,0.0) &&
			PhotoUtility::DoubleEqual(mRightCrop,0.0) &&
			PhotoUtility::DoubleEqual(mBottomCrop,0.0) &&
			PhotoUtility::DoubleEqual(mTopCZ,0.0) &&
			PhotoUtility::DoubleEqual(mLeftCZ,0.0) &&
			PhotoUtility::DoubleEqual(mRightCZ,0.0) &&
			PhotoUtility::DoubleEqual(mBottomCZ,0.0)));
	}//end HasCrop


// ---------------------------------------------------------------------------
// HasZoom
// ---------------------------------------------------------------------------
bool
PhotoPrintItem::HasZoom() const {
	return (! (PhotoUtility::DoubleEqual(mXScale, 1.0) &&
				PhotoUtility::DoubleEqual(mYScale, 1.0)));
		
	}//end HasZoom


// ---------------------------------------------------------------------------
// IsLandscape
// ---------------------------------------------------------------------------
bool
PhotoPrintItem::IsLandscape(bool useNaturalBounds) 
{
	MRect bounds (GetNaturalBounds()); 

	//handle case where we're empty placeholder and don't have imageRect yet
	if (!bounds)
		bounds = GetMaxBounds();
	
	if (!useNaturalBounds) {
		MatrixRecord m;
		SetupDestMatrix(&m, kDoScale, kDoRotation);
		::TransformRect(&m, &bounds, nil);
		}//endif

	return bounds.Width() >= bounds.Height();

} // IsLandscape


// ---------------------------------------------------------------------------
// IconTypeToPixelSpec
// ---------------------------------------------------------------------------
void
PhotoPrintItem::IconTypeToPixelSpec(ResType inType, SInt16& iconSize, SInt16& pixelDepth) {

	switch (inType) {
		case 'ICN#':
			iconSize = 32;
			pixelDepth = 1;
			break;

		case 'ics#':
			iconSize = 16;
			pixelDepth = 1;
			break;

		case 'icl4':
			iconSize = 32;
			pixelDepth = 4;
			break;

		case 'icl8':
			iconSize = 32;
			pixelDepth = 8;
			break;

		case 'ics4':
			iconSize = 16;
			pixelDepth = 4;
			break;

		case 'ics8':
			iconSize = 16;
			pixelDepth = 8;
			break;
	}//switch

}//end IconTypeToPixelSpec


// ---------------------------------------------------------------------------
//	MakeIcon
//	Creates and returns a handle. Caller owns the handle. 
// ---------------------------------------------------------------------------
Handle
PhotoPrintItem::MakeIcon(const ResType inType)
{
												// Be certain our proxy is present
	ProxyRef	localProxy (this->GetProxy());
	HORef<StLockPixels> possibleProxyLocker;
	if (localProxy) possibleProxyLocker = new StLockPixels (localProxy->GetMacGWorld());
	
	SInt16		iconSize;
	SInt16		pixelDepth;

	IconTypeToPixelSpec(inType, iconSize, pixelDepth);
	MRect			iconBounds(0, 0, iconSize, iconSize);

	MRect			imageBounds;
	if (localProxy != nil)
		localProxy->GetBounds(imageBounds);
	else
		imageBounds = GetNaturalBounds();
		
	MRect			aspectDest;
	AlignmentGizmo::FitAndAlignRectInside(imageBounds, iconBounds, kAlignAbsoluteCenter, aspectDest, EUtil::kDontExpand);

	// Make a small copy of the proxy (or image)
	EGWorld			offscreen(iconBounds, pixelDepth);
	if (localProxy != nil)
		localProxy->CopyImage(offscreen.GetMacGWorld(), aspectDest, ditherCopy);
	else {
		MatrixRecord localSpace;
		GDHandle	offscreenDevice (::GetGWorldDevice(offscreen.GetMacGWorld()));
		ItemRect	naturalBounds (GetNaturalBounds());
		::RectMatrix(&localSpace,&naturalBounds, &aspectDest);
		this->DrawImage(&localSpace, offscreen.GetMacGWorld(), offscreenDevice, nil, kProxyQuality);
	}//else have to draw the image

	// And draw a box around it
	offscreen.BeginDrawing();
	aspectDest.Frame();
	offscreen.EndDrawing();

	Handle			theHandle;
	if (inType == 'ICN#' || inType == 'ics#')
		theHandle = ::MakeICN_pound(offscreen.GetMacGWorld(), &iconBounds, iconSize);
	else
		theHandle = ::MakeIcon(offscreen.GetMacGWorld(), &iconBounds, pixelDepth, iconSize);

	return theHandle;
} // MakeIcon


// ---------------------------------------------------------------------------
// MakeProxy
// 	the proxy is an EGWorld which is cheaper to draw than whatever QTI we have.
// we need to create the proxy w/o rotation, since we will draw it later with rotation
// ---------------------------------------------------------------------------
void
PhotoPrintItem::MakeProxy()
{
	if (this->IsEmpty())
		return;

	ESpinCursor::SpinWatch();

	MatrixRecord rectOnlyMatrix;
	
	ProxyRef	newProxy;
	
	mProxy = 0;
	
	try {
		SilentExceptionEater silence;
		StDisableDebugThrow_();
		
		if (mQTI == nil && mAlias) 
			ReanimateQTI();

		StGrafPortSaver				savePort;		// Be sure we're in the right port even if there's a throw

		SetupDestMatrix(&rectOnlyMatrix, kDoScale, kIgnoreRotation); // proxy is made w/o rotation
	
		//	Create the offscreen GWorld
		MRect					bounds;
#ifdef OLD_COMPATABILITY
		GetExpandedOffsetImageRect(bounds);	
#else
		ERect32 cropZoomRect;
		ERect32 expandedOffsetImageRect;
		DeriveCropZoomRect(cropZoomRect, expandedOffsetImageRect);
		bounds.top = expandedOffsetImageRect.top;
		bounds.left = expandedOffsetImageRect.left;
		bounds.bottom = expandedOffsetImageRect.bottom;
		bounds.right = expandedOffsetImageRect.right;
#endif		
		// make the proxy as unpurgable at first, since we need to draw into it
		newProxy = new EGWorld (bounds, gProxyBitDepth, 0, 0, 0, EGWorld::kNeverTryTempMem, kNoPurge);
	} catch (...) {
		// Swallow the exception, and set mProxy to nil (also mQTI in case DrawImage failed)
		newProxy = 0;
		mQTI = nil;
		return; //failure
	}
		
	// we've gotten this far.  
	// only reason for throwing beneath here should be a DrawImage failure.
	// make sure to cleanup from BeginDrawing;
	try {
		//	Draw into it
		if (newProxy->BeginDrawing ()) {
			this->DrawImage(&rectOnlyMatrix, newProxy->GetMacGWorld(), ::GetGDevice(), nil, kProxyQuality);
			newProxy->EndDrawing();
			} //endif able to lock + draw
		newProxy->SetPurgeable(true);

	} catch (...) {
		//make sure to end drawing, cleaning up the port and unlocking the pixels!
		newProxy->EndDrawing();

		// Swallow the exception, and set mProxy to nil (also mQTI in case DrawImage failed)
		newProxy = 0;
		mQTI = nil;	
	}
	
	//	Do this last so the grow zone doesn't kill it
	mProxy = newProxy;
	
} // MakeProxy


// ---------------------------------------------------------------------------
//	MakeRotatedThumbnails
// ---------------------------------------------------------------------------
void
PhotoPrintItem::MakeRotatedThumbnails(MNewPicture& io0Rotation, MNewPicture& io90Rotation, 
										MNewPicture& io180Rotation, MNewPicture& io270Rotation, 
										const MRect& bounds)
{
	if (IsEmpty()) return;
	
	//try to ensure there is a proxy to draw
	ProxyRef			localProxy (GetProxy());
	HORef<StLockPixels> possibleProxyLocker;
	if (localProxy) possibleProxyLocker = new StLockPixels (localProxy->GetMacGWorld());
			
	DrawIntoNewPictureWithRotation(localProxy, 0.0, bounds, io0Rotation);
	DrawIntoNewPictureWithRotation(localProxy, 90.0, bounds, io90Rotation);
	DrawIntoNewPictureWithRotation(localProxy, 180.0, bounds, io180Rotation);
	DrawIntoNewPictureWithRotation(localProxy, 270.0, bounds, io270Rotation);

}//end MakeRotatedThumbnails


// ---------------------------------------------------------------------------
// MapDestRect.  Used to map mDest (and associated rects) from one space to another
// so that rotation/skewing won't be affected by the transform 
// otherwise we could accomplish this with a matrix operation
//
// ---------------------------------------------------------------------------
void
PhotoPrintItem::MapDestRect(const MRect& sourceRect, const MRect& destRect)
{
	::MapRect(&mDest, &sourceRect, &destRect);
	::MapRect(&mImageRect, &sourceRect, &destRect);
	::MapRect(&mFrameRect, &sourceRect, &destRect);
	::MapRect(&mCaptionRect, &sourceRect, &destRect);
	
	// map the caption point size!
	GetProperties().SetFontSize(GetProperties().GetFontSize() * (destRect.Width() / sourceRect.Width()));
}//end MapDestRect


// ---------------------------------------------------------------------------
//MakePreview.  Caller owns returned Handle
// ---------------------------------------------------------------------------
PicHandle
PhotoPrintItem::MakePict(const MRect& bounds) 
{
	MNewPicture preview;

	//try to ensure there is a proxy to draw
	ProxyRef			localProxy (GetProxy());
	HORef<StLockPixels> possibleProxyLocker;
	if (localProxy) possibleProxyLocker = new StLockPixels (localProxy->GetMacGWorld());
			
	DrawIntoNewPictureWithRotation(localProxy, 0.0, bounds, preview);
	return preview.Detach();
}//end MakePreview


// ---------------------------------------------------------------------------
// ReanimateQTI
// attempt to make a QTI from the filespec corresponding to our alias
// Throw if nil, or if unable to get bounds
// ---------------------------------------------------------------------------
void
PhotoPrintItem::ReanimateQTI() {
	if ((mAlias != nil) && (mQTI == nil)){
		Assert_(this->GetFileSpec() != nil);
		mQTI  = new StQTImportComponent(*GetFileSpec());
		mQTI->GetNaturalBounds (mNaturalBounds);
	}//endif enough knowledge to attempt reanimation
}//end ReandimateQTI


// ---------------------------------------------------------------------------
// ResolveCropStuff
//
// takes our crop region + transforms it by an incoming matrix
// then, clips that against an incoming clip region
// return a RgnHandle that actually points to the HORef<MRegion> which has been passed in
// caller in effect has always owned any region which gets created here
// ---------------------------------------------------------------------------
RgnHandle
PhotoPrintItem::ResolveCropStuff(HORef<MRegion>& cropRgn, RgnHandle inClip, MatrixRecord* inWorldSpace)
{
	MRect xformDest (GetTransformedBounds());
	
	// do we have intrinsic cropping?
	if (HasCrop()) {
		cropRgn = new MNewRegion;
		MRect derivedRect;
		DeriveCropRect(derivedRect);
	
		// now, xform those points (rotation, scale, skew, etc)
		Point corners[4];
		corners[0] = derivedRect.TopLeft();
		corners[2] = derivedRect.BotRight();
		corners[1].v = corners[0].v; // topRight
		corners[1].h = corners[2].h;
		corners[3].v = corners[2].v; // bottomLeft
		corners[3].h = corners[0].h;		

		MatrixRecord m;
		SetupDestMatrix(&m, kIgnoreScale, kDoRotation); // we want to know about rotation, but not scale or offset
		::TransformPoints(&m/*inXform*/, corners, 4);
	
		cropRgn->Open();
		::MoveTo(corners[0].h, corners[0].v);
		::LineTo(corners[1].h, corners[1].v);
		::LineTo(corners[2].h, corners[2].v);
		::LineTo(corners[3].h, corners[3].v);
		::LineTo(corners[0].h, corners[0].v);
		cropRgn->Close();
		
		// fake out clip bug present in some QTImporters.  (Perhaps this is confusing some print drivers?)
		// by creating a tiny region on the topline, union'ed with the rest
		MNewRegion fakey;
		fakey = MRect (xformDest.top, xformDest.left, xformDest.top + 1, xformDest.left + 1);

		cropRgn->Union(*cropRgn, fakey);
		if (inWorldSpace)
			::TransformRgn(inWorldSpace, *cropRgn);
	}//endif we have some intrinsic cropping

	// combine it with any incoming clipping
	if (inClip != nil) {
		if (cropRgn) 
			cropRgn->Intersect(*cropRgn, inClip);
		else {
			cropRgn = new MNewRegion ();
			cropRgn->SetRegion(inClip);
			}//else make a copy of the incoming clip region 

#ifdef banana
		// QTI for SGI crashes if clip outside of dest
		// crop to destRect (which by definition is within NaturalBounds rect)
		MNewRegion destRgn;
		MRect doubleXformDest (xformDest);
		if (inWorldSpace)
			::TransformRect(inWorldSpace, &doubleXformDest, NULL);
		destRgn = doubleXformDest;
		cropRgn->Intersect(*cropRgn, destRgn);
#endif
		}//endif there is some incoming clipping

	// we hold an HORef<MRegion>
	// bool(HORef<>) may be false if nothing is stored
	// and bool(*HORef) may be false if a nil crop region is present
	// here's the two step process in figuring that out.
	RgnHandle workingCrop ((RgnHandle)nil);
	if (cropRgn) 
		workingCrop =  *cropRgn ? *cropRgn : (RgnHandle)nil;

	return workingCrop;
} // ResolveCropStuff


#pragma mark -
// R E C T A N G L E M A N I A !!


// ---------------------------------------------------------------------------
// SetCaptionRect
//
// ---------------------------------------------------------------------------
void
PhotoPrintItem::SetCaptionRect(const MRect& inCaptionRect)
{
	mCaptionRect = inCaptionRect;
}//end SetCaptionRect


// ---------------------------------------------------------------------------
// SetCrop
//
//		new crop percentages
// 				measurements are IN from appropriate edge (i.e. 0 == no crop from that edge)
//			negative percentages are expansions to handle crop-zoom rect being bigger than imagerect 
// 			since crop-zoom rect may have diff proportions than imageRect inside ImageMax
// 		for crop-zooming, some portion of crop value may be user-inset-cropping of crop-zoom extent
// 		so separate that out.
// ---------------------------------------------------------------------------
void			
PhotoPrintItem::SetCrop(double inTopCrop, double inLeftCrop, double inBottomCrop, double inRightCrop)
{
	
	mTopCrop = inTopCrop;
	mLeftCrop = inLeftCrop;
	mBottomCrop = inBottomCrop;
	mRightCrop = inRightCrop;
	
	if (HasCrop() && HasZoom()) {
		ERect32 cropZoomRect;
		ERect32 expandedOffsetImageRect;
		DeriveCropZoomRect(cropZoomRect, expandedOffsetImageRect);
		
		MRect derivedCropRect;
		DeriveCropRect(derivedCropRect);

		PhotoUtility::CalcCropValuesAsPercentages(derivedCropRect, cropZoomRect, 
													mUserTopCrop, mUserLeftCrop, mUserBottomCrop, mUserRightCrop, kDontClampToBounds);

		}//endif crop-zoom in effect, so determine which part of this crop is user, which part is crop-zoom
	else {
		mUserTopCrop = 0.0;
		mUserLeftCrop = 0.0;
		mUserBottomCrop = 0.0;
		mUserRightCrop = 0.0;
		}//else no crop-zoom, so entire amount is user
	
} // SetCrop


// ---------------------------------------------------------------------------
// SetCropZoom
//
// ---------------------------------------------------------------------------
void
PhotoPrintItem::SetCropZoom(const double& inTop,const  double& inLeft,const  double& inBottom,const  double& inRight) {
	ERect32 oldCropRect;
	ERect32 newCropRect;
	ERect32 oldExpandedOffsetImage;
	ERect32 newExpandedOffsetImage;
	DeriveCropZoomRect(oldCropRect, oldExpandedOffsetImage);

	mTopCZ = inTop;
	mLeftCZ = inLeft;
	mBottomCZ = inBottom;
	mRightCZ = inRight;
	
	ERect32 newExpandedOffset;
	DeriveCropZoomRect(newCropRect, newExpandedOffsetImage);
	
	
	double xScale (newExpandedOffsetImage.Width() / (double)oldExpandedOffsetImage.Width());
	double yScale (newExpandedOffsetImage.Height() / (double) oldExpandedOffsetImage.Height());
	// must have equal scaling.  if not equal, either float artifact or problem elsewhere
	yScale = xScale;
	
	// need to update scales	
	SetCropZoomScales(xScale, yScale);

	// clipping is determined from the crop (not crop-zoom) percentages
	// which are based off imageRect.  it's ok for them to be be negative (expanding, as
	// is often the case when crop-zoom is different aspect ratio than image)
	// so we calculate them here based off newCropRect, compared to imageRect.
	PhotoUtility::CalcCropValuesAsPercentages(newCropRect, GetImageRect(),
												mTopCrop, mLeftCrop, mBottomCrop, mRightCrop, kDontClampToBounds);

	SetCrop(mTopCrop, mLeftCrop, mBottomCrop, mRightCrop);
	}//end SetCropZoom


// ---------------------------------------------------------------------------
// SetCropZoomOffset
//
// ---------------------------------------------------------------------------
void
PhotoPrintItem::SetCropZoomOffset(double inTopOffset, double inLeftOffset) {
	mTopOffset = inTopOffset;
	mLeftOffset = inLeftOffset;
	}//end SetCropZoomOffset


// ---------------------------------------------------------------------------
// SetCropZoomScales
//
// ---------------------------------------------------------------------------
void
PhotoPrintItem::SetCropZoomScales(double inZoomScaleX, double inZoomScaleY) {	
	Assert_(inZoomScaleX > 0);
	Assert_(inZoomScaleY > 0);

	bool needsNewProxy (false);
	if ((inZoomScaleX > mXScale) || (inZoomScaleY > mYScale))
		needsNewProxy = true;
	
	mXScale = inZoomScaleX;
	mYScale = inZoomScaleY;

	if (needsNewProxy)
		DeleteProxy(); // proxy is made while drawing
}//end SetCropZoomScales


// ---------------------------------------------------------------------------
// SetDest
//
// ---------------------------------------------------------------------------
//	For serialization clients
void 			
PhotoPrintItem::SetDest(const MRect& inDest)
{
	mDest = inDest;
}//end SetDest


// ---------------------------------------------------------------------------
// SetDest
//
// ---------------------------------------------------------------------------
void 			
PhotoPrintItem::SetDest(const MRect& inDest, const PhotoDrawingProperties& drawProps)
{
	SetDest (inDest);
	AdjustRectangles(drawProps);
}//end SetDest


// ---------------------------------------------------------------------------
// SetFileSpec
//
// ---------------------------------------------------------------------------
void			
PhotoPrintItem::SetFileSpec(const FSSpec& inSpec)
{
	AdoptAlias (MFileSpec (inSpec).MakeAlias ());
	
	this->GetFileSpec ();		//	Force a resolve
}//end SetFrameRect


// ---------------------------------------------------------------------------
// SetFrameRect
//
// ---------------------------------------------------------------------------
void			
PhotoPrintItem::SetFrameRect(const MRect& inFrameRect)
{
	mFrameRect = inFrameRect;
}//end SetFrameRect


// ---------------------------------------------------------------------------
// SetImageRect
//
// ---------------------------------------------------------------------------
void			
PhotoPrintItem::SetImageRect(const MRect& inImageRect)
{
	mImageRect = inImageRect;
}//end SetImageRect


void
PhotoPrintItem::SetMaxBounds(const MRect& inMax, const PhotoDrawingProperties& drawProps) {
	if (inMax != mMaxBounds) {
		mMaxBounds = inMax;
		
		MRect bogusCaption;
		MRect bogusImage;
		CalcImageCaptionRects(bogusImage, bogusCaption, mMaxBounds, drawProps);
		}//endif need to change
}//end

// ---------------------------------------------------------------------------
// SetScreenDest
//
// ---------------------------------------------------------------------------
bool
PhotoPrintItem::SetScreenDest(const MRect& inDest, const PhotoDrawingProperties& drawProps)
{
	MatrixRecord	inverse;
	
	MatrixRecord m;
	SetupDestMatrix(&m);
	Boolean happy (::InverseMatrix (&m, 
                                    &inverse));
    if (happy) {
		Point	corners[2];
		corners[0] = inDest.TopLeft();
		corners[1] = inDest.BotRight();
		::TransformPoints(&inverse, corners, 2);
		
		MRect xformed (corners[0], corners[1]);

		// now:  move xformed so it's midpoint is same as original midpoint
		
		xformed.Offset(inDest.MidPoint().h - xformed.MidPoint().h,
						inDest.MidPoint().v - xformed.MidPoint().v);
		SetDest(xformed, drawProps);
		
		return true;
    	}
	else
		return false;                    
}//end SetScreenDest

bool gCompatMode (false);
// ---------------------------------------------------------------------------
// PhotoPrintItem::SetupDestMatrix 
// create the matrix based on destbounds + rotation (SKEW NOT YET IMPLEMENTED)
// ---------------------------------------------------------------------------
void 
PhotoPrintItem::SetupDestMatrix(MatrixRecord* pMat, bool doScale, bool doRotation) const {


	MRect dest;

	ERect32 cropZoomRect;
	ERect32 expandedOffsetImageRect;
	DeriveCropZoomRect(cropZoomRect, expandedOffsetImageRect);

	if (!this->IsEmpty() && doScale) {
		if (gCompatMode) {
				GetExpandedOffsetImageRect(dest);
			}//endif
		else {
			if ((expandedOffsetImageRect.top > 32767) ||
				(expandedOffsetImageRect.left > 32767) ||
				(expandedOffsetImageRect.bottom > 32767) ||
				(expandedOffsetImageRect.right > 32767))
					DebugStr("\p overflow!!");
			
			dest.top = expandedOffsetImageRect.top;
			dest.left = expandedOffsetImageRect.left;
			dest.bottom = expandedOffsetImageRect.bottom;
			dest.right = expandedOffsetImageRect.right;
		}//else
		ItemRect	naturalBounds (GetNaturalBounds());
		::RectMatrix(pMat, &naturalBounds, &dest);
		}//endif know about a file and supposed to scale it into a dest rect
	else {
		::SetIdentityMatrix(pMat);
		}//else just start off w/ identity matrix

	if (doRotation) {
		dest =	GetImageRect(); // calculate rotation around midpoint of placement on screen, not expanded offset mess
		Point midPoint (dest.MidPoint());
		midPoint.h = cropZoomRect.MidPoint().h;
		midPoint.v = cropZoomRect.MidPoint().v;
		::RotateMatrix (pMat, Long2Fix((long)mRot), Long2Fix(midPoint.h), Long2Fix(midPoint.v));
		}//endif doing rotation
}//end SetupDestMatrix


// ---------------------------------------------------------------------------
// SetupProxyMatrix uses proxy rects to compute scale/rotate matrix
// ---------------------------------------------------------------------------
//	Precondition: inProxy exists and is locked

void
PhotoPrintItem::SetupProxyMatrix(ProxyRef inProxy, MatrixRecord* pMat, bool doScale, bool doRotation) {
	
	Assert_(inProxy);
	
	ERect32 cropZoomRect;
	ERect32 expandedOffsetImageRect;
	DeriveCropZoomRect(cropZoomRect, expandedOffsetImageRect);

	if (!doScale && !doRotation){
		::SetIdentityMatrix(pMat);
		}//endif nothing can be done
	else {
		StLockPixels	locker (inProxy->GetMacGWorld());
		MRect imageRect = GetImageRect();
		if (doScale) {

			MRect dest;
if (gCompatMode) {
			GetExpandedOffsetImageRect(dest);
	}//endif
else {
			dest.top = expandedOffsetImageRect.top;
			dest.left = expandedOffsetImageRect.left;
			dest.bottom = expandedOffsetImageRect.bottom;
			dest.right = expandedOffsetImageRect.right;
}			MRect proxyBounds;
			inProxy->GetBounds(proxyBounds);
			proxyBounds.Offset(-proxyBounds.left, -proxyBounds.top); //ensure at origin for RectMatrix to Xlate
			
			::RectMatrix(pMat, &proxyBounds, &dest);			
			}//end doScale

		if (doRotation) {
			Point midPoint (imageRect.MidPoint());
			midPoint.h = cropZoomRect.MidPoint().h;
			midPoint.v = cropZoomRect.MidPoint().v;
			::RotateMatrix (pMat, Long2Fix((long)mRot), Long2Fix(midPoint.h), Long2Fix(midPoint.v));
			}//endif doing rotation
		}//else
}//end SetupProxyMatrix
