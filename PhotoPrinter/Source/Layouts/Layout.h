/*
	File:		Layout.h

	Contains:	Definition of Layout object, which manages positioning of images.

	Written by:	David Dunham

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

		10 Aug 2001		drd		Added GetResolution, GetPrintProperties, GetPrintRec for convenience
		06 Aug 2001		drd		SetGutter
		03 Aug 2001		rmgw	Remove bogus AppleEvent keys.
		02 Aug 2001		rmgw	Add AppleEvent enums.  Bug #273.
		01 Aug 2001		rmgw	Rename ImageCount property to ItemsPerPage.  Bug #265.
		01 Aug 2001		drd		161 266 Added arg to GetCellBounds
		29 Jul 2001		drd		248 ImagesAreDuplicated
		23 jul 2001		dml		206 add GetCount, CalcOrientation
		23 Jul 2001		rmgw	Add doc and type to constructor.
		20 Jul 2001		rmgw	Remove PhotoPrintDoc include.  Bug #200.
		19 Jul 2001		drd		173 176 IsFlexible
		19 Jul 2001		rmgw	Add HasPlaceholders method.  Bug #183.
		18 Jul 2001		rmgw	Add RemoveItems method.
		18 Jul 2001		rmgw	Add SetItems method.
		12 jul 2001		dml		148 add support for multiple units
		09 Jul 2001		rmgw	AdoptNewItem now returns a PhotoIterator. Bug #142.
		02 Jul 2001		rmgw	AdoptNewItem now takes a PhotoIterator.
		29 jun 2001		dml		added parm to CommitOptionsDialog
		22 mar 2001		dml		add all sorts of custom margin helper functions
		21 Mar 2001		drd		Added (empty) SetItemsPerPage
		21 mar 2001		dml		add SetupMargins
		12 mar 2001		dml		add ignoreEmpty option to CountOrientation
		18 Jan 2001		drd		CommitOptionsDialog returns value and has new arg
		05 dec 2000		dml		factored out SetAnnoyingwareNotice
		01 Dec 2000		drd		26 Added mBinderMargin, gBinderMargin
		27 Sep 2000		rmgw	Change ItemIsAcceptable to DragIsAcceptable.
		19 Sep 2000		drd		ResizeImage
		18 Sep 2000		drd		CanEditImages
		07 Sep 2000		drd		Added GetName
		21 Aug 2000		drd		Removed HasOptions (all layouts now have them again)
		14 aug 2000		dml		add GetDistinctImages
		13 Jul 2000		drd		Removed kDefaultGutter (now in prefs)
		13 jul 2000		dml		add orientation field
		11 jul 2000		dml		add numPages to AdjustDocumentOrientation
		05 Jul 2000		drd		CommitOptionsDialog no longer declared empty
		30 Jun 2000		drd		Added GetCellBounds, dialog methods
		28 jun 2000		dml		add mType, GetType()
		27 Jun 2000		drd		AdjustDocumentOrientation, CountOrientation
		26 Jun 2000		drd		GetNameIndex; AddItem, Initialize
		23 Jun 2000		drd		Don't use HORef for mDocument (so it's not deleted inadvertently)
		23 Jun 2000		drd		Use HORef<PhotoPrintModel> in constructor
		21 Jun 2000		drd		ItemIsAcceptable
		19 Jun 2000		drd		Added mRows, mColumns, mGutter
		19 Jun 2000		drd		Created
*/

#pragma once

#include "PhotoPrintModel.h"
#include "PhotoUtility.h"

class EDialog;
class PhotoPrintDoc;
class PhotoPrintView;
class PrintProperties;
class	EPrintSpec;

class Layout {
public:

	enum LayoutType {
		kUnspecified = 'none',
		kGrid = 'grid',
		kSingle = 'sing',
		kFixed = '2fix',
		kMultiple = '2dup',
		kSchool = 'mult',
		kCollage = 'coll',
		kFnordLayout = 'bad '
	};
	
	enum {
		PPob_BackgroundOptions = 1100,
		str_LayoutNames = 500,

		// For CommitOptionsDialog
		kDoLayoutIfNeeded = true,
		kDontLayout = false,

		// For GetCellBounds
		kRecalcIfNeeded = false,
		kRawBounds = true,

		k3HoleWidth = 54		// 3/4 inch
	};

	virtual 			~Layout();

	// Accessors
	PhotoPrintView*		GetView () const;
	PhotoPrintDoc*		GetDocument () const						{ return mDoc; }
	
			SInt16		GetBinderMargin() const						{ return mBinderMargin; }
			SInt16		GetColumns() const							{ return mColumns; }
			SInt16		GetGutter() const							{ return mGutter; }
	virtual	LStr255		GetName() const			{ return LStr255(str_LayoutNames, this->GetNameIndex()); }
	virtual	SInt16		GetNameIndex() const = 0;
			SInt16		GetRows() const								{ return mRows; }
			LayoutType	GetType() const								{ return mType; }
			OSType		GetOrientation() const						{ return mOrientation;}
	virtual	OSType		CalcOrientation() const;
			void		SetGutter(const SInt16 inGutter)			{ mGutter = inGutter; }
			
	virtual	PhotoIterator	AddItem(PhotoItemRef inItem, PhotoIterator inBefore);
	virtual	void		SetItems (ConstPhotoIterator inBegin, ConstPhotoIterator inEnd);
	virtual	void		RemoveItems (ConstPhotoIterator inBegin, ConstPhotoIterator inEnd);
	virtual	void		AdjustDocumentOrientation(SInt16 numPages = 1);
	virtual	bool		CanAddToBackground(const UInt16 /*inCnt*/)	{ return false; }
	virtual	bool		CanEditImages() const						{ return true; }
	virtual	bool		HasPlaceholders() const						{ return false; }
	virtual	bool		IsFlexible() const							{ return false; }
			UInt32		CountOrientation(const OSType inType, bool ignoreEmpty = false) const;
	virtual SInt16		GetDistinctImages(void);
	virtual	bool		ImagesAreDuplicated() const					{ return false; }
	virtual	Boolean		DragIsAcceptable(DragReference inDragRef);

	virtual	void		GetCellBounds(const UInt32 inI, MRect& outB, const bool inRaw = kRecalcIfNeeded);
	virtual	void		Initialize()	{} // = 0 !!!
	virtual	void		LayoutImages()								{ this->AdjustDocumentOrientation(); }
	virtual	bool		ResizeImage(const OSType /*inCode*/,
									const FitT /*inFit*/,
									PhotoItemRef /*ioItemRef*/)		{ return false; }	// = 0; !!!
	virtual UInt32		GetItemsPerPage(void) const {return 0;};
	virtual	void		SetItemsPerPage(const UInt32 /*inCount*/)			{}

	virtual	bool		CommitOptionsDialog(EDialog& inDialog, PrintProperties& cleanPrintProps, const bool inDoLayout);
	virtual	ResIDT		GetDialogID() const							{ return PPob_BackgroundOptions; }
	virtual	void		SetupOptionsDialog(EDialog& inDialog);

	virtual void		ConvertMarginsToDisplayUnits(EDialog& inDialog, double& top, double& left, double& bottom, double& right);
	virtual void		ConvertMarginsFromDisplayUnits(EDialog& inDialog, double& top, double& left, double& bottom, double& right);
	virtual void		SetupMargins(EDialog& inDialog);
	virtual void		SetupMarginPropsFromDialog(EDialog& inDialog, PrintProperties& inProps);
	virtual void		StuffCustomMarginsIfNecessary(EDialog& inDialog, PrintProperties& inProps);
	virtual void		UpdateMargins(EDialog& inDialog, bool inUseDialog = true);
	virtual void		GetMarginsFromDialog(EDialog& inDialog, double& top, double& left, double& bottom, double& right);

	virtual void		GetUnitsScalars(EDialog& inDialog, double& outToInches, double& outFromInches);
	virtual void		SetAnnoyingwareNotice(bool inState, AnnoyLocationT inWhere = annoy_none);

		PrintProperties&	GetPrintProperties() const;
		HORef<EPrintSpec>&	GetPrintRec(void) const;
			SInt16		GetResolution() const;

protected:
						Layout	(PhotoPrintDoc*				inDoc, 
								 HORef<PhotoPrintModel>&	inModel,
								 LayoutType					inType = kGrid);

	LayoutType					mType;
	PhotoPrintDoc*				mDoc;
	HORef<PhotoPrintModel>		mModel;

	SInt16						mRows;
	SInt16						mColumns;
	SInt16						mGutter;		// Minimum separation, in pixels
	SInt16						mBinderMargin;	// Width, in pixels

	OSType						mOrientation;

static SInt16					gBinderMargin;
};

inline void Layout::GetCellBounds(const UInt32 inI, MRect& outB, const bool inRaw)
{
#pragma unused(inI, outB, inRaw)
};

//	AppleEvent stream operators

#include "MAEDescInserters.h"
#include "MAEDescExtractors.h"

template<>
struct MAEDescExtractorTraits<Layout::LayoutType> {typedef Layout::LayoutType value_type ; enum {ae_type = typeEnumerated};};

template<>
struct MAEDescInserterTraits<Layout::LayoutType> {typedef Layout::LayoutType value_type ; enum {ae_type = typeEnumerated};};

