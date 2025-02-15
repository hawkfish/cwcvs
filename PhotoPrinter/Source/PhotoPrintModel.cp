/*
	File:		PhotoPrintModel.cp

	Contains:	Implementation of the model, which has all the data (in the MVC pattern).

	Written by:	Dav Lion and David Dunham

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights reserved.

	Change History (most recent first):

		17 Aug 2001		rmgw	Add msg_ModelItemsChanged utility.  Bug #332.
		16 Aug 2001		drd		327 Use GetMaxBounds, not GetDestRect, in Draw for logical clipping (see also 323)
		25 Jul 2001		drd		15 Removed ESpinCursor arg from PhotoPrintItem::Draw
		25 Jul 2001		drd		Cleanup unused stuff in Sort, shorter-circuit sort_None
		24 Jul 2001		rmgw	Remove SetDirty (ick!)
		23 Jul 2001		rmgw	Broadcast change messages.
		20 Jul 2001		drd		199 RemoveEmptyItems
		20 jul 2001		dml		Don't change Doc's Properties to dirty, change Doc!!
		19 Jul 2001		drd		195 GetFirstNonEmptyItem
		16 Jul 2001		rmgw	Listen for low memory.  Bug #163.
		09 jul 200		dml		123.  clarify sorting behavior (requires new ESortedFileList.cp,h).  add defensive code
		09 jul 2001		dml		move more sorting control here
		09 Jul 2001		rmgw	AdoptNewItem now returns a PhotoIterator. Bug #142.
		02 Jul 2001		drd		Turned assert in RemoveItems into if, and fixed iteration
		02 Jul 2001		rmgw	AdoptNewItem now takes a PhotoIterator.
		02 Jul 2001		rmgw	Convert item list to vector representation.
		16 mar 2001		dml		do Draw RectInRgn test in destination coords!
		15 Feb 2001		rmgw	10 Bottleneck ALL item deletion in iterator routine
		4  jan 2000		dml		make sure that DeleteLastItem and DeleteItems remove from pane's selection also
		18 Sep 2000		drd		Draw passes spin cursor down
		31 aug 2000		dml		added 	CheckEventQueueForUserCancel() to Draw
		16 Aug 2000		drd		Added DeleteLastItem
		15 aug 2000		dml		stop --howMany in Sort (even though not used)
		11 Aug 2000		drd		DocumentProperties no longer has Empty
		04 Aug 2000		drd		Added disposal arg to DeleteItems
		04 aug 2000		dml		change DeleteItem to DeleteItems
		03 Aug 2000		drd		Tweaked sort constant names
		03 aug 2000		dml		move selection to view
		02 aug 2000		dml		add Sort
		19 jul 2000		dml		Draw culls based on clipRgn
		18 jul 2000		dml		added spinning cursor during Draw.  cursors defined in PhotoUtility.h
		29 Jun 2000		drd		Destructor gets rid of items
		20 Jun 2000		drd		Use PhotoPrintDoc::gCurDocument, so we're complete at constructor time
*/

#include "PhotoPrintModel.h"

#include "ESpinCursor.h"
#include "PhotoUtility.h"
#include "ESortedFileList.h"
#include "PhotoPrintPrefs.h"
#include <vector>

//---------------------------------
// PhotoPrintModel ct
//---------------------------------
PhotoPrintModel::PhotoPrintModel(void) 
{
	LGrowZone::GetGrowZone()->AddListener(this);
}//end ct

//---------------------------------
// PhotoPrintModel ct
//---------------------------------
PhotoPrintModel::PhotoPrintModel(const PhotoPrintModel& other)
{
	LGrowZone::GetGrowZone()->AddListener(this);

	for (ConstPhotoIterator item (other.begin ()); item != other.end (); ++item) {	// for each item
		PhotoItemRef	copyRef (new PhotoPrintItem (**item));
		AdoptNewItem (copyRef, end ());
      	} // for all items in other

}//end copy ct


//---------------------------------
// PhotoPrintModel dt
//---------------------------------
PhotoPrintModel::~PhotoPrintModel() {
	for (PhotoIterator i = begin(); i != end(); ++i)
		delete (*i);
} // ~PhotoPrintModel

//---------------------------------
// AdoptNewItem
//---------------------------------
PhotoIterator	
PhotoPrintModel::AdoptNewItem(

	PhotoItemRef item, 
	PhotoIterator inBefore) {
	
	PhotoIterator	result = mItemList.insert(inBefore, item);
	
	MessageRange	range = {result, result + 1};
	BroadcastMessage (msg_ModelItemsAdded, &range);
	
	return result;
	
}//end AdoptNewItem
	
//---------------------------------
// ChangedItems
//---------------------------------
void	
PhotoPrintModel::ChangedItems(

	ConstPhotoIterator inBegin, 
	ConstPhotoIterator inEnd) 

{
	
	MessageRange	range = {inBegin, inEnd};
	BroadcastMessage (msg_ModelItemsChanged, &range);
	
}//end ChangedItems
	
//---------------------------------
// ChangedItems
//---------------------------------
void	
PhotoPrintModel::ChangedItems(

	const	PhotoItemList& 	inList) 

{
	ChangedItems (inList.begin (), inList.end ());
	
}//end ChangedItems
	
//---------------------------------
// ChangedAllItems
//---------------------------------
void	
PhotoPrintModel::ChangedAllItems(void) 

{
	PhotoItemList	listCopy (mItemList);
	ChangedItems (listCopy);
	
}//end ChangedItems
	
//---------------------------------
// Draw
//
// inClip is in destination space coordinates.
//---------------------------------
void	
PhotoPrintModel::Draw(MatrixRecord* destinationSpace,
						CGrafPtr destPort,
						GDHandle destDevice,
						RgnHandle inClip)
{	
	for(PhotoIterator i = begin(); i != end(); ++i) {
		if (inClip) {
			MRect dest ((*i)->GetMaxBounds());
			if (destinationSpace)
				::TransformRect(destinationSpace, &dest, NULL);
			if (::RectInRgn(&dest, inClip) == false)
				continue;
			}//endif we have a clip region
		(*i)->Draw(GetDrawingProperties(), destinationSpace, destPort, destDevice, inClip);		
		if (::CheckEventQueueForUserCancel())
			break;
	}//for
}//end Draw

/*
GetFirstNonEmptyItem
	Returns the first PhotoItem which is not a placeholder, or else nil
*/
PhotoItemRef
PhotoPrintModel::GetFirstNonEmptyItem() const
{
	for (ConstPhotoIterator i = this->begin(); i != this->end(); ++i) {
		if (!(*i)->IsEmpty())
			return *i;
	}
	return nil;
} // GetFirstNonEmptyItem

/*
GetNonEmptyCount
	Returns the number of PhotoItems which are not a placeholder
*/
UInt32
PhotoPrintModel::GetNonEmptyCount() const
{
	UInt32		count = 0;
	for (ConstPhotoIterator i = this->begin(); i != this->end(); ++i) {
		if (!(*i)->IsEmpty())
			count++;
	}
	return count;
} // GetNonEmptyCount

// ---------------------------------------------------------------------------
//	� ListenToMessage													  [public]
// ---------------------------------------------------------------------------

void
PhotoPrintModel::ListenToMessage (

	MessageT	inMessage, 
	void *		ioParam) 
	
{ // begin ListenToMessage
	
	switch (inMessage) {
		case msg_GrowZone:
			{
			SInt32*	bytesNeeded = (SInt32*) ioParam;
			if (*bytesNeeded < 0) break;
			
			SInt32	bytesFreed = 0;
			long	startFree = FreeMem ();
			for (PhotoIterator i = begin(); i != end(); ++i) {
				(*i)->DeleteProxy ();
				bytesFreed = FreeMem () - startFree;
				if (bytesFreed >= *bytesNeeded) break;
				} // for
				
			*bytesNeeded = bytesFreed;
			break;
			} // case
		} // switch
		
} // ListenToMessage

//---------------------------------
// MapItems
//---------------------------------
void
PhotoPrintModel::MapItems(const MRect& sourceRect, const MRect& destRect) {
	for (PhotoIterator i = begin(); i != end(); ++i)
		(*i)->MapDestRect(sourceRect, destRect);
	
	MessageRange	range = {begin (), end ()};
	BroadcastMessage (msg_ModelItemsChanged, &range);
	
} // MapItems

/*
RemoveEmptyItems
	Remove placeholders from the model, optionally deleting them
*/
void
PhotoPrintModel::RemoveEmptyItems(const bool inDelete)
{
	PhotoItemList	doomed;
	for (PhotoIterator i = mItemList.begin(); i != mItemList.end(); ++i) {
		PhotoItemRef		theItem = *i;
		if (theItem->IsEmpty()) {
			doomed.push_back(theItem);
		}
	}
	if (!doomed.empty()) {
		this->RemoveItems(doomed, inDelete);
	}
} // RemoveEmptyItems

//---------------------------------
// 	RemoveItems
//---------------------------------
//	Basic item removal method.
void	
PhotoPrintModel::RemoveItems (

	PhotoIterator 	inBegin,
	PhotoIterator 	inEnd,
	const bool 		inDelete)

{ // begin RemoveItems

	//	Clear the selection
	PhotoItemList	localList (inBegin, inEnd);
	
	MessageRange	range = {localList.begin (), localList.end ()};
	BroadcastMessage (msg_ModelItemsRemoved, &range);

	int				dbgCount = localList.size();
	if (dbgCount == 0)
		dbgCount++;

	//	Remove the items
	for (PhotoIterator i = localList.begin (); i != localList.end (); ++i) {
		PhotoIterator	dead = std::find (mItemList.begin (), mItemList.end (), *i);
		if (dead == mItemList.end ()) continue;
		
		mItemList.erase (dead);
		dbgCount = mItemList.size();
		if (dbgCount == 0)
			dbgCount++;

		if (inDelete) delete (*i);
		} // if
		
}//end RemoveItems

//---------------------------------
// RemoveItems
//---------------------------------
void	
PhotoPrintModel::RemoveItems(

	PhotoItemList& 	doomed, 
	const bool 		inDelete)
{
	RemoveItems (doomed.begin(), doomed.end(), inDelete);

}//end DeleteItem

//---------------------------------
// RemoveLastItem
//	Removes the last item in our list, optionally deleting the object
//---------------------------------
void
PhotoPrintModel::RemoveLastItem(const bool inDelete)
{
	if (mItemList.rbegin () == mItemList.rend ()) return;
	
	PhotoItemList listCopy (mItemList.rbegin (), ++mItemList.rbegin ());
	RemoveItems (listCopy, inDelete);
}

//---------------------------------
// RemoveAllItems
//	Removes the last item in our list, optionally deleting the object
//---------------------------------
void
PhotoPrintModel::RemoveAllItems(const bool inDelete)
{
	RemoveItems (mItemList, inDelete);
}

//---------------------------------
// Sort
//---------------------------------
void
PhotoPrintModel::Sort()
{
	// No need to do anything if we're not sorting
	if (PhotoPrintPrefs::Singleton()->GetSorting() == sort_None)
		return;

	// make the vector of FullFileInfo objects
	std::vector<FullFileInfo> sortedList;
	
	int added (0);
	for (ConstPhotoIterator i = begin(); i != end(); ++i) {
		FullFileInfo ffi (*i);
		sortedList.push_back(ffi);
		++added;
		}//end

	// make the basic predicate (type of sort, not direction)
	switch (PhotoPrintPrefs::Singleton()->GetSorting()) {
		case sort_Name: 
			std::sort(sortedList.begin(), sortedList.end(), SortedFilePredicate::NameComparator ());
			break;
		case sort_Creation:
			std::sort(sortedList.begin(), sortedList.end(), SortedFilePredicate::CreatedComparator ());
			break;
		case sort_Modification:
			std::sort(sortedList.begin(), sortedList.end(), SortedFilePredicate::ModifiedComparator ());
			break;
		case sort_None: 
			return;
		default:
			SignalStringLiteral_("Illegal Sorting code ");
			break;
		}//switch

	// empty out our list, and fill it with the sorted list
	mItemList.clear();
	if (!PhotoPrintPrefs::Singleton()->GetSortAscending()) {
		for (std::vector<FullFileInfo>::iterator i (sortedList.begin()); i != sortedList.end(); ++i) {
			FullFileInfo&	thing = *i;
			PhotoPrintItem*		item = dynamic_cast<PhotoPrintItem*>(thing.GetProvider());
			if (!item->IsEmpty())
				mItemList.push_back(item);
		}//for all items in list
		for (std::vector<FullFileInfo>::iterator i (sortedList.begin()); i != sortedList.end(); ++i) {
			FullFileInfo&	thing = *i;
			PhotoPrintItem*		item = dynamic_cast<PhotoPrintItem*>(thing.GetProvider());
			if (item->IsEmpty())
				mItemList.push_back(item);
		}//for all items in list
		}//endif sort upward
	else {
		for (std::vector<FullFileInfo>::reverse_iterator i (sortedList.rbegin()); i != sortedList.rend(); ++i) {
			FullFileInfo&	thing = *i;
			PhotoPrintItem*		item = dynamic_cast<PhotoPrintItem*>(thing.GetProvider());
			if (!item->IsEmpty())
				mItemList.push_back(item);
		}//for all non-items in list
		for (std::vector<FullFileInfo>::reverse_iterator i (sortedList.rbegin()); i != sortedList.rend(); ++i) {
			FullFileInfo&	thing = *i;
			PhotoPrintItem*		item = dynamic_cast<PhotoPrintItem*>(thing.GetProvider());
			if (item->IsEmpty())
				mItemList.push_back(item);
		}//for all non-items in list
		
		}//else	
	
	BroadcastMessage (msg_ModelSorted, this);
}//end Sort

	