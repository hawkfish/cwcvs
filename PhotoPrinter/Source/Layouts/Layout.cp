/*
	File:		Layout.cp

	Contains:	Implementation of Layout object, which manages positioning of images.

	Written by:	David Dunham and Dav Lion

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

		14 Aug 2001		drd		321 DragIsAcceptable suppresses throws
		13 Aug 2001		rmgw	Don't clear headers and footers. Bug #283.
		10 Aug 2001		drd		Added GetResolution, GetPrintProperties, GetPrintRec for convenience
		07 Aug 2001		drd		294 Don't use sprintf to format numbers!
		03 Aug 2001		rmgw	Only check for 2 files. Bug #162.
		23 jul 2001		dml		add CalcOrientation
		23 Jul 2001		rmgw	Add doc and type to constructor.
		20 Jul 2001		rmgw	Include PhotoPrintDoc.  Bug #200.
		18 Jul 2001		rmgw	Add RemoveItems method.
		18 Jul 2001		rmgw	Add SetItems method.
		17 Jul 2001		rmgw	Reject huge drags (> 500 items). Bug #162.
		16 Jul 2001		drd		166 CommitOptionsDialog reads new font data BEFORE calculating lineHeight
		12 jul 2001		dml		148 add support for Units in PageOptions (BGOptions) dialog
		09 Jul 2001		rmgw	AdoptNewItem now returns a PhotoIterator. Bug #142.
		06 jul 2001		dml		SetOrientation doubled for stupid lexmark
		03 Jul 2001		drd		Use GetValue() to read font popup; font must be in valid range
		03 Jul 2001		drd		GetMarginsFromDialog uses LString::operator double()
		03 Jul 2001		drd		38 Font size popup is now a text field
		02 Jul 2001		rmgw	AdoptNewItem now takes a PhotoIterator.
		29 jun 2001		dml		CommitOptionsDialog must check margins for change --> needsLayout
		29 jun 2001		dml		custom margins should never show holed icons in UpdateMargins
		29 jun 2001		dml		add icons to 'papr' in OptionsDialog to help explain margins + orientation,
								handle rotated orientation in setting custom margins 
		28 jun 2001		dml		26 don't set print properties binder holes if that element is disabled!
		28 Jun 2001		rmgw	Clear both headers and footers in CommitOptionsDialog.  Bug #100.
		14 May 2001		drd		65 CountOrientation no longer returns true int, but is biased
								so that empties don't count if there are any non-empties
		21 mar 2001		dml		add initialization of custom margins fields to SetupOptionsDialog
		12 mar 2001		dml		add ignoreEmpty option to CountOrientation
		12 mar 2001		dml		add more discrimination logic to CountOrientation to support templates
		15 feb 2001		dml		Doh!  CommitOptionsDialog must sanity check for buttons + panels
		23 jan 2001		dml		fix evil kDragPromiseFindFile bug w/ enhanced ExtractFSSpec call
		19 jan 2001		dml		add more margin support to SetupOptionsDialog
		18 Jan 2001		drd		CommitOptionsDialog does layout if necessary
		11 Dec 2000		drd		13 DragIsAcceptable checks for kDragFlavor; re-alphabetized
		07 Dec 2000		drd		CommitOptionsDialog forces redraw (to see header/footer change)
		06 dec 2000		dml		add header fudge (for whitespace immed below header)
		06 dec 2000		dml		set header/footer based on actual line size, not useless hardwired constant
		05 dec 2000		dml		factored out SetAnnoyingwareNotice
		01 Dec 2000		drd		26 Added mBinderMargin, gBinderMargin
		27 Sep 2000		rmgw	Change ItemIsAcceptable to DragIsAcceptable.
		21 sep 2000		dml		changed annoyingware sizes, strategry in CommitOptions
		14 Sep 2000		drd		CommitOptionsDialog actually makes space for header/footer
		14 sep 2000		dml		header/footer support
		07 sep 2000		dml		AddItem calls back to view to select 
		31 aug 2000		dml		pass kCalcWithXforms to CountOrientation
		21 Aug 2000		drd		SetupOptionsDialog handles page title
		15 aug 2000		dml		have AddItem call Doc->View->RefreshItem 
		14 aug 2000		dml		add GetDistinctImages
		13 Jul 2000		drd		Initialize gutter from prefs
		13 jul 2000		dml		store orientation locally
		11 jul 2000		dml		add numPages to AdjustDocumentOrientation
		07 Jul 2000		drd		CommitOptionsDialog is for Background, not item!
		05 Jul 2000		drd		CommitOptionsDialog
		30 Jun 2000		drd		SetupOptionsDialog
		28 jun 2000		dml		fix AdjustDocumentOrientation (EPrintSPec*!!)
		27 Jun 2000		drd		AdjustDocumentOrientation, CountOrientation
		27 jun 2000		dml		add HFSPromise Drag Receiving
		26 Jun 2000		drd		AddItem
		23 Jun 2000		drd		Use HORef<PhotoPrintModel> in constructor
		21 Jun 2000		drd		ItemIsAcceptable; allow nil model
		19 Jun 2000		drd		Added mRows, mColumns, mGutter
		19 Jun 2000		drd		Created
*/

#include "Layout.h"

#include "EDialog.h"
#include "EUtil.h"
#include <LGAColorSwatchControl.h>
#include <LIconPane.h>
#include "PhotoPrintConstants.h"
#include "PhotoPrintDoc.h"
#include "PhotoPrintPrefs.h"
#include "PhotoUtility.h"
#include "PhotoPrintApp.h"

#include "MDragItemIterator.h"
#include "MFolderIterator.h"
#include "Registration.h"
#include "PhotoPrinter.h"
#include <stdio.h>
#include "MP2CStr.h"
#include "BackgroundOptions.h"

static	UInt16		FileIsAcceptable(const CInfoPBRec&);

SInt16	Layout::gBinderMargin = k3HoleWidth;	// Might later set from a resource or something

static const double kHeaderSpacing = 0.0625;  // and eigth of an inch
static const double kFooterSpacing = 0.0625;  // and eigth of an inch

static const PaneIDT	RadioGroupView_Position = 'posi';
static const PaneIDT	RadioGroupView_Margins = 'marg';

static const PaneIDT	Pane_MinimalMargins = 'minm';
static const PaneIDT	Pane_SymmetricMargins = 'symm';
static const PaneIDT	Pane_CustomMargins = 'cust';

static const PaneIDT	Pane_Top = 'top ';
static const PaneIDT	Pane_Left= 'left';
static const PaneIDT	Pane_Bottom = 'bot ';
static const PaneIDT	Pane_Right = 'righ';
static const PaneIDT	Pane_Paper = 'papr';
static const PaneIDT	Pane_Units = 'unit';

static const ResIDT		Icon_Portrait = 4001;
static const ResIDT		Icon_PortraitHoles = 4002;
static const ResIDT		Icon_Landscape = 4003;
static const ResIDT		Icon_LandscapeHoles = 4004;

/*
Layout
*/
Layout::Layout(

	PhotoPrintDoc*				inDoc,
	HORef<PhotoPrintModel>&		inModel,
	LayoutType					inType)
	
	: mType(inType)
	, mDoc(inDoc)
	, mModel(inModel)
	
	, mRows(1)
	, mColumns(1)
	, mGutter(PhotoPrintPrefs::Singleton()->GetGutter())
	, mBinderMargin(PhotoPrintPrefs::Singleton()->GetBinderMargin())
{				
} // Layout

/*
~Layout
*/
Layout::~Layout()
{
} // ~Layout

/*
AddItem
	Add an item to the model, handling multiples properly. Default is to just add
	to the model.
*/
PhotoIterator
Layout::AddItem(

	PhotoItemRef 	inItem,
	PhotoIterator	inBefore)
{
	PhotoIterator	result (mModel->AdoptNewItem(inItem, inBefore));
	GetView()->AddToSelection(*result);
	
	return result;
	
} // AddItem


/*
AdjustDocumentOrientation
	Set the paper to landscape or portrait orientation to fit the most items
*/
void
Layout::AdjustDocumentOrientation(SInt16 numPages)
{
	UInt32		l = this->CountOrientation(kLandscape);
	UInt32		p = this->CountOrientation(kPortrait);

	EPrintSpec* spec = (EPrintSpec*)this->GetPrintRec();
	// Note that we have a slight bias for landscape (since most pictures are done that way)
	if (p > l) {
		mOrientation = kPortrait;
	} else {
		mOrientation = kLandscape;
	}
	spec->SetOrientation(mOrientation, PhotoUtility::gNeedDoubleOrientationSetting);

	GetDocument()->MatchViewToPrintRec(numPages);
} // AdjustDocumentOrientation


/*
CalcOrientation
*/
OSType
Layout::CalcOrientation() const {
	UInt32		l = this->CountOrientation(kLandscape);
	UInt32		p = this->CountOrientation(kPortrait);

	// Note that we have a slight bias for landscape (since most pictures are done that way)
	if (p > l) {
		return kPortrait;
	} else {
		return kLandscape;
	}
}//CalcOrientation


/*
CommitOptionsDialog
	Handles setting things from the Background Properties dialog
*/
bool
Layout::CommitOptionsDialog(EDialog& inDialog, PrintProperties& cleanPrintProps, const bool inDoLayout)
{
	bool					needsLayout = false;
	DocumentProperties&		props = GetDocument()->GetProperties();

	LRadioGroupView*		titlePos = inDialog.FindRadioGroupView('posi');
	PaneIDT					titleButton = titlePos->GetCurrentRadioID();
	if (titleButton != props.GetTitlePosition())
		needsLayout = true;
	props.SetTitlePosition(static_cast<TitlePositionT>(titleButton));

	Str255					title;
	inDialog.FindEditText('titl')->GetDescriptor(title);
	
	LEditText*		sizeField = inDialog.FindEditText('fSiz');
	SInt16			size = sizeField->GetValue();
	if (size >= kMinFontSize && size <= kMaxFontSize) {
		props.SetFontSize(size);
	}

	LPopupButton*	fontPopup = inDialog.FindPopupButton('font');
	Str255			fontName;
	props.SetFontName(fontPopup->GetMenuItemText(fontPopup->GetCurrentMenuItem(), fontName));

	double	lineHeight = PhotoUtility::GetLineHeight(GetDocument()->GetProperties().GetFontNumber(),
													GetDocument()->GetProperties().GetFontSize()) / 72.0;
	
	switch (titleButton) {
		case 'head' :
			this->GetPrintProperties().SetHeader(lineHeight + kHeaderSpacing);
			props.SetHeader(title);
			SetAnnoyingwareNotice(!PhotoPrintApp::gIsRegistered, annoy_footer);
			break;

		case 'foot' :
			GetDocument()->GetPrintProperties().SetFooter(lineHeight + kFooterSpacing);
			props.SetFooter(title);
			SetAnnoyingwareNotice(!PhotoPrintApp::gIsRegistered, annoy_header);
			break;

		case 'none' :
			GetDocument()->GetPrintProperties().SetHeader(0.0);
			SetAnnoyingwareNotice(!PhotoPrintApp::gIsRegistered, annoy_header);
			break;
	}//end switch


//Margin
	PrintProperties&		printProps = this->GetPrintProperties();
	SetupMarginPropsFromDialog(inDialog, printProps);
	StuffCustomMarginsIfNecessary(inDialog, printProps);

	if (cleanPrintProps != printProps)
		needsLayout = true;
		
	LGAColorSwatchControl*	color = dynamic_cast<LGAColorSwatchControl*>(inDialog.FindPaneByID('bCol'));
	if (color != nil) {
		RGBColor		theColor;
		color->GetSwatchColor(theColor);
		// !!! set something
	}

	// Since header/footer might have changed, force redraw
	GetView()->Refresh();

	// If header/footer has changed position, we need to do layout
	if (needsLayout) {
		// Our subclass may be doing layout for its own reasons, so don't blindly do it here
		if (inDoLayout == kDoLayoutIfNeeded) {
			this->LayoutImages();
			this->GetView()->Refresh();
		}
	}

	return needsLayout;
} // CommitOptionsDialog

/*
CountFiles
*/
static UInt16
CountAcceptableFiles (
	const	FSSpec&	inSpec,
	int				inMaxFiles)
{
	
	MFileSpec	spec (inSpec);
	
	CInfoPBRec	pb;
	spec.GetCatInfo (pb);
	if (!(pb.hFileInfo.ioFlAttrib & ioDirMask)) 
		return FileIsAcceptable (pb) ? 1 : 0;
		
	UInt16		count = 0;
	MFolderIterator	end (spec.Volume (), pb.dirInfo.ioDrDirID);
	for (MFolderIterator fi (end); ++fi != end; ) {
		if (count > inMaxFiles) break;

		if (fi.IsFolder ())
			count += CountAcceptableFiles (fi.FileSpec (), inMaxFiles - count);	//	Recurse
		
		else if (FileIsAcceptable (*fi))
			++count;
		} // if
	
	return count;
} // CountAcceptableFiles


void		
Layout::ConvertMarginsToDisplayUnits(EDialog& inDialog, double& top, double& left, double& bottom, double& right) {

	double toInches;
	double fromInches;
	GetUnitsScalars(inDialog, toInches, fromInches);

	double scalar (toInches / kDPI);

	// current resolution to inches
	top *= scalar;
	left *= scalar;
	bottom *= scalar;
	right *= scalar;
}//end ConvertMarginsToDisplayUnits


void		
Layout::ConvertMarginsFromDisplayUnits(EDialog& inDialog, double& top, double& left, double& bottom, double& right) {

	double toInches;
	double fromInches;
	GetUnitsScalars(inDialog, toInches, fromInches);

	double scalar (fromInches);

	// current resolution to inches
	top *= scalar;
	left *= scalar;
	bottom *= scalar;
	right *= scalar;
}//end ConvertMarginsFromDisplayUnits


/*
CountOrientation
	Doesn't return an actual count (bug 65), but rather a relative number.
*/
UInt32
Layout::CountOrientation(const OSType inType, bool ignoreEmpty) const
{
	UInt32		c = 0;

	PhotoIterator	i;
	for (i = mModel->begin(); i != mModel->end(); i++) {
		PhotoItemRef	item = *i;
	
		if (ignoreEmpty && item->IsEmpty()) continue;

		// if there is no rotation, then the natural bounds suffice for determining orientation.
		// this helps clarify items which have just replaced empty templates
		// since the templates may have had opposite but maleable aspect ratio bounds
		bool useNaturalBounds (PhotoUtility::DoubleEqual(item->GetRotation(), 0.0));
			
		if ((inType == kLandscape && item->IsLandscape(useNaturalBounds)) || 
			(inType == kPortrait &&	item->IsPortrait(useNaturalBounds))) {
			if (item->IsEmpty())
				c++;
			else
				c += 0x400;						// A picture is worth 1K empties
		}
	}

	return c;
} // CountOrientation

/*
DragIsAcceptable
*/
Boolean
Layout::DragIsAcceptable (
	DragReference	inDragRef)
{
	StDisableDebugThrow_();						// 321 Alerts inside drag are bad

	//	Count up the items that we will try to add
	const	int			kMaxDragFiles = 2;
	
	UInt16				count = 0;
	MDragItemIterator	end (inDragRef);
	for (MDragItemIterator i = end; ++i != end; ) {
		if (i.HasFlavor(kDragFlavor)) {
			count++;		// ??? Is there really only one item?
		} else {
			FSSpec		fsSpec;
			Boolean		ioAllowEvilPromise (true);
			PromiseHFSFlavor	promise;
			if (!i.ExtractFSSpec (fsSpec, ioAllowEvilPromise, promise)) return false;	//	Unknown item type
			
			if (!ioAllowEvilPromise)
				count += CountAcceptableFiles (fsSpec, kMaxDragFiles - count);
			
			if (count > kMaxDragFiles) continue;
		}
	} // for
		
	// We may not want multiple items
	if (!this->CanAddToBackground (count))
		return false;

	return true;
} // DragIsAcceptable

/*
FileIsAcceptable
*/
static UInt16
FileIsAcceptable (
	const CInfoPBRec&)
{
	return true;
} // FileIsAcceptable

/*
GetDistinctImages
*/
SInt16
Layout::GetDistinctImages() {
	if (mModel)
		return mModel->GetCount();
	else
		return 0;
}//end GetDistinctImages

/*
GetPrintProperties
*/
PrintProperties&
Layout::GetPrintProperties() const
{
	return this->GetDocument()->GetPrintProperties();
} // GetPrintProperties

/*
GetPrintRec
*/
HORef<EPrintSpec>&
Layout::GetPrintRec (void) const
{
	return this->GetDocument()->GetPrintRec();
} // GetPrintRec

/*
GetResolution
*/
SInt16
Layout::GetResolution() const
{
	return this->GetDocument()->GetResolution();
} // GetResolution

/*
GetView
*/
PhotoPrintView*
Layout::GetView() const {
	return this->GetDocument()->GetView ();
}//end GetView


/*
GetMarginsFromDialog
*/
void		
Layout::GetMarginsFromDialog(EDialog& inDialog, double& outTop, double& outLeft, 
												double& outBottom, double& outRight) {
	LPane* top (inDialog.FindPaneByID(Pane_Top));
	ThrowIfNil_(top);
	LPane*  left (inDialog.FindPaneByID(Pane_Left));
	ThrowIfNil_(left);
	LPane*  bottom (inDialog.FindPaneByID(Pane_Bottom));
	ThrowIfNil_(bottom);
	LPane*  right (inDialog.FindPaneByID(Pane_Right));
	ThrowIfNil_(right);

	LStr255 text;
	top->GetDescriptor(text);
	outTop = text;

	left->GetDescriptor(text);
	outLeft = text;

	bottom->GetDescriptor(text);
	outBottom = text;

	right->GetDescriptor(text);
	outRight = text;

	this->ConvertMarginsFromDisplayUnits(inDialog, outTop, outLeft, outBottom, outRight);
}//end GetMarginsFromDialog


void
Layout::GetUnitsScalars(EDialog& inDialog, double& outToInches, double&outFromInches) {

	LPopupButton*	units = inDialog.FindPopupButton('unit');
	SInt16 curUnit (units->GetCurrentMenuItem());
	switch (curUnit) {
		case unit_Inches:
			outToInches = 1.0;
			break;
		case unit_Centimeters:
			outToInches = 2.54;
			break;
		case unit_Points:
			outToInches = 72.0;
			break;
		default:
			break;
			}//end switch
	
	outFromInches = 1.0 / outToInches;
}//end GetUnitsScalars


//---------------------------------
// 	RemoveItems
//---------------------------------

void	
Layout::RemoveItems (

	ConstPhotoIterator 	inBegin,
	ConstPhotoIterator 	inEnd)

{ // begin RemoveItems

	PhotoItemList	localList (inBegin, inEnd);
	mModel->RemoveItems (localList.begin (), localList.end (), PhotoPrintModel::kDelete);
	
}//end RemoveItems

/*
* SetAnnoyingwareNotice
*/
void
Layout::SetAnnoyingwareNotice(bool inState, AnnoyLocationT inWhere) {

	DocumentProperties&		props = GetDocument()->GetProperties();

	double lineHeight = PhotoUtility::GetLineHeight(GetDocument()->GetProperties().GetFontNumber(),
													GetDocument()->GetProperties().GetFontSize()) / 72.0;
	switch (inWhere) {
		case annoy_header: {
			if (inState) {
				this->GetPrintProperties().SetHeader(lineHeight + kHeaderSpacing);
				props.SetHeader(PhotoPrintApp::gAnnoyanceText);
				}//endif need annoyance
			else
				this->GetPrintProperties().SetHeader(0.0);
			break;
			}//case
		case annoy_footer: {
			if (inState) {
				this->GetPrintProperties().SetFooter(lineHeight + kFooterSpacing);
				props.SetFooter(PhotoPrintApp::gAnnoyanceText);
				}//endif need annoyance
			else
				this->GetPrintProperties().SetFooter(0.0);
			break;
			}//case
		case annoy_diagonal:
			break;
		case annoy_none:
			break;
		}//end switch
	}//end SetAnnoyingwareNotice


//------------------------------------------------------------------
// SetItems
//------------------------------------------------------------------

void
Layout::SetItems (

	ConstPhotoIterator	inBegin,
	ConstPhotoIterator	inEnd) 
	
{
	
	mModel->RemoveAllItems (PhotoPrintModel::kDelete);
	for (ConstPhotoIterator i = inBegin; i != inEnd; ++i)
		mModel->AdoptNewItem (new PhotoPrintItem (**i), mModel->end ());

}//end SetItems

//------------------------------------------------------------------
// SetupMargins
//------------------------------------------------------------------

void
Layout::SetupMargins(EDialog& inDialog) {
	PrintProperties&		printProps = this->GetPrintProperties();

	LRadioGroupView*	marginView = inDialog.FindRadioGroupView(RadioGroupView_Position);
	if (marginView != nil) {
		PrintProperties::MarginType margin = printProps.GetMarginType();
		LPane*	button (nil);

		switch (margin) {
			case PrintProperties::kMinimalMargins :
				button = inDialog.FindPaneByID(Pane_MinimalMargins);
				break;
			case PrintProperties::kFullSymmetric :
				button = inDialog.FindPaneByID(Pane_SymmetricMargins);
				break;
			case PrintProperties::kCustom:
				button = inDialog.FindPaneByID(Pane_CustomMargins);
				break;
			}//end switch
		if (button != nil)
			button->SetValue(1);

		// Binder margin (for 3-hole punching, at least in the USA)
		LPane*		binderMargin = inDialog.FindPaneByID('3hol');
		if (binderMargin != nil) {
			if (mBinderMargin != 0)
				binderMargin->SetValue(Button_On);
			else
				binderMargin->SetValue(Button_Off);
		}//endif binderMargin found
		
	UpdateMargins(inDialog, false);
	}//endif marginView exists
}//end SetupMargins


/*
SetupOptionsDialog
	Subclasses should override (and call inherited)
*/
void
Layout::SetupOptionsDialog(EDialog& inDialog)
{
	// Set up title stuff
	DocumentProperties&		props = GetDocument()->GetProperties();

	LRadioGroupView*	titlePos = inDialog.FindRadioGroupView(RadioGroupView_Position);
	titlePos->SetCurrentRadioID(props.GetTitlePosition());

	LEditText*		sizeField = inDialog.FindEditText('fSiz');
	LStr255			sizeText(props.GetFontSize());
	sizeField->SetDescriptor(sizeText);
	LPopupButton*	fontPopup = inDialog.FindPopupButton('font');
	SInt16			nItems = ::CountMenuItems(fontPopup->GetMacMenuH());
	LStr255			defaultFont, fontName;
	::GetFontName(props.GetFontNumber(), defaultFont);
	for (SInt16 i = 1; i <= nItems; i++) {
		fontPopup->GetMenuItemText(i, fontName);
		if (fontName == defaultFont) {
			fontPopup->SetValue(i);
			break;
		}
	}

	LEditText*		title = inDialog.FindEditText('titl');
	if (title != nil) {
		switch (props.GetTitlePosition()) {
			case kHeader:
				title->SetDescriptor(props.GetHeader());
				break;
			case kFooter:
				title->SetDescriptor(props.GetFooter());
				break;
			case kNoTitle:
				title->SetDescriptor("\p");
				break;
			}//end switch
		LCommander::SwitchTarget(title);
		title->SelectAll();
	}

	//Margin Stuff
	SetupMargins(inDialog);
	
	// margins must be installed first before we call units
	LPopupButton*	unitsPopup = inDialog.FindPopupButton(Pane_Units);
	unitsPopup->SetCurrentMenuItem(PhotoPrintPrefs::Singleton()->GetDisplayUnits());
	
	// We no longer have a background color in the dialogs, but this is harmless
	LGAColorSwatchControl*	color = dynamic_cast<LGAColorSwatchControl*>(inDialog.FindPaneByID('bCol'));
	if (color != nil) {
		color->SetSwatchColor(Color_White);
	}
} // SetupOptionsDialog


void
Layout::SetupMarginPropsFromDialog(EDialog& inDialog, PrintProperties& inProps) 
{
	LRadioGroupView*		marginView = inDialog.FindRadioGroupView(RadioGroupView_Margins);
	if (marginView != nil) {
		PaneIDT marginButton = marginView->GetCurrentRadioID();
		if (marginButton != nil) {
			switch (marginButton) {
				case Pane_MinimalMargins:
					inProps.SetMarginType(PrintProperties::kMinimalMargins);
					break;
				case Pane_SymmetricMargins:
					inProps.SetMarginType(PrintProperties::kFullSymmetric);
					break;
				case Pane_CustomMargins:
					inProps.SetMarginType(PrintProperties::kCustom);
					break;
				}//switch	
			}//endif marginButton exists


		LPane*		binderMargin = inDialog.FindPaneByID('3hol');
		if ((binderMargin != nil)  && (binderMargin->IsEnabled())) {
			inProps.SetBinderHoles(binderMargin->GetValue());
		}//endif binderMargin found
	}//endif there is a margin panel
}//end SetupMarginPropsFromDialog


void
Layout::StuffCustomMarginsIfNecessary(EDialog& inDialog, PrintProperties& inProps) {
	if (inProps.GetMarginType() == PrintProperties::kCustom) {
		double top, left, bottom, right;
		GetMarginsFromDialog(inDialog, top, left, bottom, right);

		// if we are portrait, this is simple and straightforward
		if (this->GetPrintRec()->GetOrientation() == kPortrait) 
			inProps.SetMargins(top, left, bottom, right);
		else // but if we're landscape, we must correct the correspondence w/ the canonical portrait props
			inProps.SetMargins(left, bottom, right, top);
		}//endif
	}//end StuffCustomMarginsIfNecesary


void
Layout::UpdateMargins(EDialog& inDialog, bool inUseDialog) {

	PrintProperties& printProps (this->GetPrintProperties());

	if (inUseDialog) {
		if (inDialog.FindRadioGroupView(RadioGroupView_Margins)->GetCurrentRadioID() != Pane_CustomMargins){
			StuffCustomMarginsIfNecessary(inDialog, printProps);
			}//endif switching from custom, save the custom values in the printprops
		SetupMarginPropsFromDialog(inDialog, printProps);
		}//endif

	// get the values (at print rec resolution), convert to display units
	// and stuff those fields!
	short hRes;
	short vRes;
	this->GetPrintRec()->GetResolutions(hRes, vRes);
	double to72dpi ((double)kDPI / hRes);
	MRect paper;
	MRect page;
	PhotoPrinter::CalculatePaperRect(&*this->GetPrintRec(), &printProps, paper, hRes);
	PhotoPrinter::CalculatePrintableRect(&*this->GetPrintRec(), &printProps, page, hRes);
	page.Offset(paper.left, paper.top);
	
	double fTop ((page.top - paper.top) * to72dpi);
	double fLeft ((page.left - paper.left) * to72dpi);
	double fRight ((paper.right - page.right) * to72dpi);
	double fBot ((paper.bottom - page.bottom) * to72dpi);

	// expects incoming at 72dpi
	this->ConvertMarginsToDisplayUnits(inDialog, fTop, fLeft, fRight, fBot);

	// 294 PowerPlant's LString handles international conversion. We need to specify
	// a format string, and the number parts corresponding to that string. The system
	// number parts will be used to actually format the output.
	// @@@ We could put the formats into a resource
	LStr255		text;
	LPane*		pane;
	text.Assign(fTop, "\p#0.###", NoPt_USNumberParts);
	pane = inDialog.FindPaneByID(Pane_Top);
	if (pane != nil)
		pane->SetDescriptor(text);

	text.Assign(fLeft, "\p#0.###", NoPt_USNumberParts);
	pane = inDialog.FindPaneByID(Pane_Left);
	if (pane != nil)
		pane->SetDescriptor(text);

	text.Assign(fRight, "\p#0.###", NoPt_USNumberParts);
	pane = inDialog.FindPaneByID(Pane_Right);
	if (pane != nil)
		pane->SetDescriptor(text);

	text.Assign(fBot, "\p#0.###", NoPt_USNumberParts);
	pane = inDialog.FindPaneByID(Pane_Bottom);
	if (pane != nil)
		pane->SetDescriptor(text);
	
	LIconPane* paperIcon = dynamic_cast<LIconPane*>(inDialog.FindPaneByID(Pane_Paper));
	if (paperIcon != nil) {
		ResIDT icon;
		if (this->GetPrintRec()->GetOrientation() == kPortrait) {
			if ((printProps.GetMarginType() != PrintProperties::kCustom) &&
				(printProps.GetBinderHoles() != 0))
				icon = Icon_PortraitHoles;
			else
				icon = Icon_Portrait;
			}//endif portrait
		else {
			if ((printProps.GetMarginType() != PrintProperties::kCustom) &&
				(printProps.GetBinderHoles() != 0))
				icon = Icon_LandscapeHoles;
			else
				icon = Icon_Landscape;
			}//else
		paperIcon->SetIconID(icon);
		}//endif
}//end UpdateMargins
