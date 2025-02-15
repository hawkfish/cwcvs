/*
	File:		PhotoPrintDoc.h

	Contains:	Definition of the document.

	Written by:	Dav Lion and David Dunham

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

		13 Aug 2001		rmgw	Scroll PhotoPrintView, not the background.  Bug #284.
		03 Aug 2001		rmgw	Add AppleEvent properties.
		31 Jul 2001		drd		256 SetMaximumSize, SetMinimumSize take thoroughness arg
		27 Jul 2001		drd		243 mPaperHeight
		26 Jul 2001		drd		233 Added mScrolledView
		26 Jul 2001		rmgw	Factor out XML parsing.  Bug #228.
		24 Jul 2001		rmgw	Keep files open and use aliases.  Bug #215.
		24 Jul 2001		rmgw	Remove bogus OnModelDirtied.
		23 Jul 2001		rmgw	Listen for new model messages.
		20 jul 2001		dml		204.  make broadcaster.  add SetDirty
		20 Jul 2001		rmgw	Add min/max/orientation undo.
		20 Jul 2001		rmgw	Export layout popup.  Bug #200.
		12 Jul 2001		rmgw	Convert the import event to make new import.
		11 Jul 2001		drd		143 Added mOrientationPopup
		10 Jul 2001		drd		143 GetDuplicatedPopup now returns LBevelButton*
		06 Jul 2001		drd		72 UpdateZoom
		02 Jul 2001		rmgw	AdoptNewItem now takes a PhotoIterator.
		02 Jul 2001		rmgw	Add PhotoItem AEOM handlers.
		29 Jun 2001		drd		MatchPopupsToPrintRec
		28 Jun 2001		rmgw	Zoom on center point.  Bug #102.
		28 jun 2001		dml		70 add WarnAboutAlternate
		14 Jun 2001		drd		73 ObeyCommand
		23 May 2001		drd		74 Removed GetDescriptor
		22 May 2001		drd		GetDuplicatedPopup
		25 Apr 2001		drd		FixPopups
		23 Apr 2001		drd		Documents now have their own mMaximumSize, mMinimumSize;
								UpdatePreferences; now an LListener; mMaxPopup, mMinPopup now
								LPopupButton
		26 Mar 2001		drd		Added mMinMaxGroup, mMinPopup, mMaxPopup
		23 Mar 2001		drd		Fixed GetDuplicated, GetLayout to use GetCurrentMenuItem
		21 Mar 2001		drd		Added mDupPopup, mLayoutPopup
		09 mar 2001		dml		add FinishHandlePrint
		16 jan 2001		dml		added isTemplate to Write(),
		05 Dec 2000		drd		Added gCount
		18 sep 2000		dml		add GetPageHeight(), UpdatePageHeight(), mPageHeight()
		14 sep 2000		dml		added header/footer support, mBodySize
		13 Sep 2000		drd		Added gWindowProxies
		13 sep 2000		dml		moved kFeelGoodMargin here, removed CalcInitialWindowRect (see PhotoWindow.cp)
		12 Sep 2000		drd		Added UpdatePageNumber
		12 sep 2000		dml		add CalcInitialWindowRect
		07 aug 2000		dml		added mPageCount, mZoomDisplay
		06 aug 2000		dml		add mScroller (support for zooming), removed mPhotoPrintView
		23 Aug 2000		drd		Override HandleAppleEvent
		21 Aug 2000		drd		Removed ParseLayout, renamed sParseLayout
		11 aug 2000		dml		add SetController();
		04 Aug 2000		drd		Removed ObeyCommand; added HandleKeyPress
		27 Jul 2000		drd		No longer an LPeriodical; override IsModified; switched to
								overriding HandlePageSetup, HandlePrint
		14 jul 2000		dml		add StPrintSession accessors
		13 jul 2000		dml		add mNumPages, GetPageCount()
		11 jul 2000		dml		add numPages arg to MatchViewToPrintRec
		28 jun 2000		dml		add serialization of layout
		27 Jun 2000		drd		Made MatchViewToPrintRec public
		27 june 2000 	dml		add MatchViewToPrintRec
		27 june 2000	dml		add DoPageSetup
		26 June 2000 	dml		add Initialize
		21 june 2000	dml		exposed PrintProperties (for custom print dialog)
		20 Jun 2000		drd		Added gCurDocument, so others know who we are at constructor time
		15 jun 2000		dml		changed mDPI to be SInt16, now that xmlio supports it
		14 Jun 2000		drd		Added GetModel
*/

#pragma once

#include <LSingleDoc.h>
#include <LListener.h>
#include <LBroadcaster.h>

//	PhotoGrid
#include "PhotoPrintController.h"
#include "PhotoPrintView.h"						// So we can get model
#include "PrintProperties.h"
#include "DocumentProperties.h"

//	Epp
#include "EPrintSpec.h"

//	Toolbox++
#include "MAlias.h"
#include "MFile.h"
#include "HORef.h"

//	PowerPlant
#include <LScrollerView.h>
#include <LBevelButton.h>						// For our inline accessors
#include <LString.h>

class LPopupButton;

namespace XML {
	class Output;
	class Element;
	class Handler;
}

class MAEList;

class PhotoPrintDoc 	: public LSingleDoc
						, public LListener
						, public LBroadcaster
{
	public:
		static SInt16	kFeelGoodMargin;
		
		enum Orientation {
			kLandscapeIndex = 1,
			kPortraitIndex,
			kFlexibleIndex
		};

		// For use in WarnAboutAlternate
		enum {
			kWarnOK = 1,
			kWarnCancel,
			kWarnBlurb,
			kWarnUseAlternate,
			kWarnDontShowAgain,
			kWarnFnord
		};

		// For SetMaximumSize, SetMinimumSize
		enum {
			kComplete = true,
			kMinimal = false
		};

		enum AETypes {
			cImportClass		=	FOUR_CHAR_CODE('phim'),
			cClass = cDocument
			};
		
		enum AEProperties {
			//	pFile			//	File list
			pLayoutType			=	FOUR_CHAR_CODE('Plyt'),
			pLayoutCount		=	FOUR_CHAR_CODE('Plyc')
			};
			
	protected:
		HORef<MAlias>			mFileAlias;
		HORef<MFile>			mFileFork;			//	Keep the file open
		OSType					mFileType;
		PrintProperties			mPrintProperties;
		DocumentProperties		mProperties;
		HORef<EPrintSpec>		mPrintSpec;
		HORef<StPrintSession>	mPrintSession;
		PMSheetDoneUPP			mSheetDoneUPP;

		LBevelButton*			mDupPopup;		// Indicates individual or duplicated
		LBevelButton*			mLayoutPopup;	// Indicates layout type and count
		LPane*					mMinMaxGroup;
		LPopupButton*			mMaxPopup;
		LPopupButton*			mMinPopup;
		LBevelButton*			mOrientationPopup;	// Landscape/portrait/flexible
		LScrollerView*			mScroller;
		LPane*					mPageCount;
		LPane*					mZoomDisplay;

		SDimension32			mBodySize;
		SizeLimitT				mMaximumSize;
		SizeLimitT				mMinimumSize;
		double					mPageHeight;	// body (not counting margins)
		double					mPaperHeight;	// total

		// HOW BIG IS IT?!
		double					mWidth; 		//floating point inches.  hah!
		double					mHeight;
		SInt16					mDPI;
		SInt16					mNumPages;
		
		PhotoPrintView*			mScreenView;
		
		// header/footer
		LStr255					mHeader;
		LStr255					mFooter;
		
		void					CreateWindow		(ResIDT				inWindowID, 
													 Boolean 			inVisible);
		
		void					Initialize			(void);
		void					AddEvents			(void);
		void					AddCommands			(void);
		void					FixPopups			(void);
		
		virtual LModelObject*	HandleCreateImportEvent		(DescType				inElemClass,
															 DescType				inInsertPosition,
															 LModelObject*			inTargetObject,
															 const AppleEvent		&inAppleEvent,
															 AppleEvent				&outAEReply);
		virtual LModelObject*	HandleCreatePhotoItemEvent	(DescType				inElemClass,
															 DescType				inInsertPosition,
															 LModelObject*			inTargetObject,
															 const AppleEvent		&inAppleEvent,
															 AppleEvent				&outAEReply);

	public:

		static	void			MakeNewAEFileItem 			(MAEList&				outProps,
															 const MFileSpec&		inSpec);
		static	void			MakeNewAEFolderItem 		(MAEList&				outProps,
															 const MFileSpec&		inSpec);
															 
								PhotoPrintDoc		(LCommander*		inSuper,
													Boolean inVisible = true);
								PhotoPrintDoc		(LCommander*		inSuper,
													 const	FSSpec&		inSpec,
													 Boolean inVisible = true);
		virtual					~PhotoPrintDoc	(void);

	// LListener
		virtual	void			OnModelItemsAdded	(PhotoPrintModel::MessageRange*	inRange);
		virtual	void			OnModelItemsChanged	(PhotoPrintModel::MessageRange*	inRange);
		virtual	void			OnModelItemsRemoved	(PhotoPrintModel::MessageRange*	inRange);
		virtual void			ListenToCommand(MessageT inMessage, void* ioParam);
		virtual void			ListenToMessage(MessageT inMessage, void* ioParam);
	
		virtual double			GetWidth(void) const		{ return mWidth; }
		virtual void			SetWidth(double inWidth);
		virtual double			GetHeight(void) const		{ return mHeight; }
		virtual void			SetHeight(double inHeight);
		SizeLimitT				GetMaximumSize() const		{ return mMaximumSize; }
		void					SetMaximumSize(const SizeLimitT inMax, const bool inMinimal = kComplete);
		SizeLimitT				GetMinimumSize() const		{ return mMinimumSize; }
		void					SetMinimumSize(const SizeLimitT inMin, const bool inMinimal = kComplete);
		Orientation				GetOrientation() const;
		void					SetOrientation(Orientation	inOrient);
		virtual SInt16			GetPageCount(void) const	{ return mNumPages; }
		SInt32					GetPageHeight(void) const;
		SInt32					GetPaperHeight(void) const;
		PhotoPrintView*			GetView(void) const			{ return mScreenView; }

	// LCommander
		virtual Boolean			ObeyCommand(
								CommandT			inCommand,
								void*				ioParam = nil);	

		virtual	void			GetDisplayCenter			(double&	h,
															 double&	v) const;
		virtual	void			SetDisplayCenter			(double		h,
															 double		v,
															 Boolean	inRefresh);
															 
		virtual void			SetResolution(SInt16 inRes);
		virtual SInt16			GetResolution(void) const	{ return mDPI; }	

		// Pay attention!.  We do NOT use LDocument's stupidly built-in 
		// mPrintSpec (why isn't a pointer, dang-it), instead we always use
		// GetPrintRec().  Eventually this will be a per-document gizmo, but now
		// it actually is a global living in the App
		HORef<EPrintSpec>&		GetPrintRec(void);
		PrintProperties&		GetPrintProperties (void) {return mPrintProperties;};
		const PrintProperties&	GetPrintProperties (void) const {return mPrintProperties;};
		void					SetPrintProperties (const PrintProperties& inProps);
		void					ForceNewPrintSession(void);

		DocumentProperties&		GetProperties(void)	{ return mProperties; }
		const DocumentProperties& GetProperties(void) const {return mProperties;}
		virtual bool			IsFileSpecified(void) const {return IsSpecified ();}
		PhotoPrintModel*		GetModel(void)		{ return mScreenView->GetModel(); }
		const PhotoPrintModel*	GetModel(void) const{ return mScreenView->GetModel(); }

		void					SetController(OSType newController);

		void					UpdatePageNumber(const SInt16 inPageCount);
		void					UpdatePreferences();
		void					UpdateZoom();

		void					SetHeader(ConstStr255Param	inString) {mHeader = inString;}
		void					SetFooter(ConstStr255Param	inString) {mFooter = inString;}
		ConstStr255Param		GetHeader(void) const {return mHeader;}
		ConstStr255Param		GetFooter(void) const {return mFooter;}

		void					MatchPopupsToPrintRec();
		void					MatchViewToPrintRec (SInt16 numPages = 1);

			//	LCommander
		virtual Boolean			HandleKeyPress(const EventRecord&	inKeyEvent);

			//	LDocument			
		virtual StringPtr		GetDescriptor( Str255 outDescriptor ) const;
		virtual Boolean			UsesFileSpec( const FSSpec& inFileSpec ) const;
		
		virtual OSType			GetFileType			(void) const;
		virtual Boolean			AskSaveAs			(FSSpec&			outFSSpec,
													 Boolean			inRecordIt);
		virtual SInt16			AskSaveChanges		(bool				inQuitting);

		// LModelObject
		virtual LModelObject*	HandleCreateElementEvent	(DescType				inElemClass,
															 DescType				inInsertPosition,
															 LModelObject*			inTargetObject,
															 const AppleEvent		&inAppleEvent,
															 AppleEvent				&outAEReply);
		virtual	SInt32			CountSubModels				(DescType				inModelID) const;
		virtual void			GetAEProperty				(DescType				inProperty,
															 const AEDesc			&inRequestedType,
															 AEDesc					&outPropertyDesc) const;
		virtual	void			GetPhotoItemModel 			(const	PhotoItemRef&	inItem,
															 AEDesc					&outToken) const;
		virtual void			GetSubModelByPosition		(DescType				inModelID,
															 SInt32					inPosition,
															 AEDesc					&outToken) const;
		virtual void			GetSubModelByName			(DescType				inModelID,
															 Str255					inName,
															 AEDesc					&outToken) const;
		virtual SInt32			GetPositionOfSubModel		(DescType				inModelID,
															 const LModelObject	*	inSubModel) const;

		virtual void			DoOpen				(const FSSpec& inSpec);
		virtual void			DoSave				(void);
		virtual void			DoSaveToSpec		(const FSSpec& inSpec, bool isTemplate = false);
		virtual void			DoSaveWithProperties (const FSSpec& inSpec, OSType inType, const AEDesc* inProps);
		virtual void			DoRevert			(void);

		virtual void			DoPrint				(void);
		virtual void			HandlePrint			(void);
		virtual void			FinishHandlePrint	(bool printIt);
		virtual bool			WarnAboutAlternate	(OSType inPrinterCreator);
		static	pascal void		PMSheetDoneProc	(PMPrintSession inSession,
												 WindowRef		inDocWindow,
												 Boolean		accepted);
		virtual void			HandlePrintPreview	(void);
		virtual void			HandlePageSetup		(void);
		virtual Boolean			IsModified();

				SInt32			GetDuplicated() const	{ return mDupPopup->GetCurrentMenuItem(); }
				LBevelButton*	GetDuplicatedPopup() const	{ return mDupPopup; }
				SInt32			GetLayout() const		{ return mLayoutPopup->GetCurrentMenuItem(); }
				LBevelButton*	GetLayoutPopup() const		{ return mLayoutPopup; }
				LPane*			GetMinMaxGroup() const	{ return mMinMaxGroup; }
				LBevelButton*	GetOrientationPopup() const	{ return mOrientationPopup; }
				void			JamDuplicated(const SInt16 inValue);
				void			JamLayout(const SInt16 inValue);

		virtual void			SetDirty(bool inState);
		virtual bool			GetDirty(void) {return GetProperties().GetDirty();};

			//	LSingleDoc

		static	SInt32			gCount;				// Keeps track of how many we've made
		static	PhotoPrintDoc*	gCurDocument;		// Used in setting up window
		static	bool			gWindowProxies;		// Do we show proxies in title bar?
	}; // end PhotoPrintDoc
