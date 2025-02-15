/*
	File:		GridLayout.h

	Contains:	Definition of Layout object, which manages positioning of images.
				GridLayout keeps multiple images in a best-fit grid

	Written by:	David Dunham

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

		02 Aug 2001		rmgw	Add ItemsPerPage to constructor.  Bug #273.
		23 jul 2001		dml		179 add CalcOrientation
		23 Jul 2001		rmgw	Add doc and type to constructor.
		19 Jul 2001		drd		173 176 IsFlexible
		12 mar 2001		dml		add CalcRowsColsOrientation
		19 Sep 2000		drd		Added ResizeImage, mSizeCode
		15 Aug 2000		drd		Removed AddItem (no need to override), unused DrawEmptyRect
		14 Jul 2000		drd		CalculateGrid; changed CalculateCellSize args; removed CalculateRowsCols,
								MaxItemsPerPage
		12 jul 2000		dml		more aux functions for multipage layouts
		11 jul 2000		dml		multipage support
		06 Jul 2000		drd		Override AdjustDocumentOrientation for 2-landscape special case
		26 Jun 2000		drd		GetNameIndex
		23 Jun 2000		drd		Use HORef<PhotoPrintModel> in constructor
		19 Jun 2000		drd		Created
*/

#pragma once

#include "Layout.h"
#include "ERect32.h"

class GridLayout : public Layout
{
protected:
	UInt32	mItemsPerPage;
	SInt16	mNumPages;
	SInt16  mMaxRows;
	SInt16	mMaxCols;
	OSType	mSizeCode;			// '3*2 ' etc.
	

	virtual void		CalcRowsColsOrientation(const SInt32& inCount, SInt16& outRows, SInt16& outCols, 
												OSType& outOrientation, OSType& outConstraintOrientation) const;
	virtual void		CalculateCellSize(
							const ERect32&	inPageSize,
							const SInt16	inRows,
							const SInt16	inCols,
							ERect32&		outCellSize,
							ERect32&		outUnusedBottomPad) const;
	virtual	void		CalculateGrid(
							const ERect32&	inPageSize,
							const SInt32	inCount,
							SInt16&			outRows,
							SInt16&			outCols,
							OSType&			outOrientation) const ;
	virtual void		CalcMaxBounds(const ERect32& inCellRect, MRect& outMaxBounds);
	virtual	OSType		CalcOrientation() const;
	virtual void		LayoutItem(PhotoItemRef item, const MRect& inMaxBounds);
	virtual void		LayoutPage(const ERect32&	pageBounds, const ERect32& cellRect, PhotoIterator& iterator);

public:
						GridLayout(PhotoPrintDoc*			inDoc, 
								   HORef<PhotoPrintModel>&	inModel,
								   UInt32					inItemsPerPage = 0,
								   LayoutType 				inType = kGrid);
	virtual 			~GridLayout();

	virtual	void		AdjustDocumentOrientation(SInt16  numPages = 1);
	virtual	bool		CanAddToBackground(const UInt16 /*inCount*/)	{ return true; }
	virtual	SInt16		GetNameIndex() const				{ return 1; }
	virtual	bool		IsFlexible() const							{ return true; }
	virtual	void		Initialize();
	virtual	void		LayoutImages();
	virtual	bool		ResizeImage(const OSType inCode, const FitT inFit, PhotoItemRef ioItemRef);
};
