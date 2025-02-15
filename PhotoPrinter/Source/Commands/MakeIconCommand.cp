/*
	File:		MakeIconCommand.cp

	Contains:	Implementation of the Make Icon command.

	Written by:	David Dunham

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights reserved.

	Change History (most recent first):

		26 Jun 2001		drd		88 Test GetFileSpec, it might be nil
		01 feb 2001		dml		add pict preview to construction of icons
		08 sep 2000		dml		add spinning cursor
		07 Sep 2000		drd		FindCommandStatus makes sure we have data
		07 Sep 2000		drd		Make more icon depths and sizes
		06 Sep 2000		drd		Actually create icons
		18 Aug 2000		drd		Created
*/

#include "MakeIconCommand.h"

#include <Icons.h>
#include <map>
#include "MIconSuite.h"
#include "MResFile.h"
#include "PhotoPrintDoc.h"
#include "ESpinCursor.h"
#include "PhotoUtility.h"
#include "MRes1Iterator.h"
#include <QuicktimeComponents.h>
#include <UResourceMgr.h>

short FirstAvailableResource(OSType inType);


/*
MakeIconCommand
*/
MakeIconCommand::MakeIconCommand(const CommandT inCommand, PhotoPrintDoc* inDoc)
	: PhotoDocCommandAttachment(inCommand, inDoc)
{
} // MakeIconCommand

/*
~MakeIconCommand
*/
MakeIconCommand::~MakeIconCommand()
{
} // ~MakeIconCommand

typedef std::map<MFileSpec, PhotoItemRef>		FileItemMap;


short
FirstAvailableResource(OSType inType) {
	short resID (128);
	{
	MResLoad dontLoad;
	do {	
		StResource lookingFor(inType, resID, false, true); // don't throw, curfile only
		if ((Handle)lookingFor == nil)
			return (resID);
		++resID;
		} while (resID < 32767);
	}//end MResLoad block
	
	return (0);
	}//FirstAvailableResource


void
MakeIconCommand::CheckAddPreview(PhotoItemRef image, MResFile& theFile) {
	bool bAlreadyPresent (false);
	
	// see if there are any pnot's there.
	for (MRes1Iterator	i (ShowFilePreviewComponentType); i; ++i) {
		PreviewResource preview = (PreviewResource)i.Resource();
		if ((*preview)->resType == 'PICT') {
			bAlreadyPresent = true;
			break;
			}//endif found one
		}//end for all preview gizmos
	
	if (!bAlreadyPresent) {
		short pictID = FirstAvailableResource('PICT');
		short pnotID = FirstAvailableResource(ShowFilePreviewComponentType);

		if (pictID && pnotID) {	

			MRect bounds (0,0,128,128);
			PicHandle newPreview = image->MakePict(bounds);
			theFile.AddResource((Handle)newPreview, 'PICT', pictID);

			StNewResource newPnot (ShowFilePreviewComponentType, pnotID, sizeof(PreviewResourceRecord), true);
			PreviewResource preview = (PreviewResource)(Handle)newPnot;
			unsigned long now;
			::GetDateTime(&now);
			(*preview)->modDate = now;
			(*preview)->version = 0;
			(*preview)->resType = 'PICT';
			(*preview)->resID = pictID;
			
			}//endif there's some room in the PICT space
			
		}//endif get to make one

	}//end CheckAddPreviewPict


/*
ExecuteCommand
*/
void		
MakeIconCommand::ExecuteCommand(void* inCommandData)
{
#pragma unused(inCommandData)
	PhotoItemList		selection = mDoc->GetView()->Selection();
	PhotoIterator		i;
	PhotoItemRef		image;
	FileItemMap			files;			// The map makes sure we only get each file once
	for (i = selection.begin(); i != selection.end(); i++) {
		image = *i;
		const MFileSpec*	fs = image->GetFileSpec();
		if (fs != nil) {
			FileItemMap::value_type		thePair(*fs, image);
			files.insert(thePair);
		}
	}

	FileItemMap::iterator	f;
	ESpinCursor	spinCursor(kFirstSpinCursor, kNumCursors);
	for (f = files.begin(); f != files.end(); f++) {
		CInfoPBRec	pb;
		FInfo		fi;
		MFileSpec	theSpec((*f).first);	// Make a copy since we will use non-const methods
		theSpec.GetCatInfo(pb);
		unsigned long	origModDate = pb.hFileInfo.ioFlMdDat;
		theSpec.GetFinderInfo(fi);
		if (!theSpec.HasResourceFork()) {
			// Be sure we have a resource fork (so MResFile doesn't throw)
			theSpec.CreateResFile(fi.fdType, fi.fdCreator);
		}

		// Do resource stuff
		{
			MResFile	theFile(theSpec, fsRdWrPerm);
			Handle		h;
			image = (*f).second;
			h = image->MakeIcon('ICN#');
			theFile.AddOrReplaceResource(h, 'ICN#', kCustomIconResource);

			h = image->MakeIcon('ics#');
			theFile.AddOrReplaceResource(h, 'ics#', kCustomIconResource);

			h = image->MakeIcon('icl4');
			theFile.AddOrReplaceResource(h, 'icl4', kCustomIconResource);

			h = image->MakeIcon('ics4');
			theFile.AddOrReplaceResource(h, 'ics4', kCustomIconResource);

			h = image->MakeIcon('icl8');
			theFile.AddOrReplaceResource(h, 'icl8', kCustomIconResource);

			h = image->MakeIcon('ics8');
			theFile.AddOrReplaceResource(h, 'ics8', kCustomIconResource);

			CheckAddPreview(image, theFile);

		} // Resource fork is now closed, so we can mess with meta-data

		// Make sure the Finder knows it has a custom icon, and clear the inited flag
		pb.hFileInfo.ioFlFndrInfo.fdFlags |= kHasCustomIcon;
		pb.hFileInfo.ioFlFndrInfo.fdFlags &= ~kHasBeenInited;

		// Reset modification date
		pb.hFileInfo.ioFlMdDat = origModDate;
		theSpec.SetCatInfo(pb);

		spinCursor.Spin();
		if (::CheckEventQueueForUserCancel())
			break;
	}//for
} // ExecuteCommand
									 
/*
FindCommandStatus {OVERRIDE}
*/
void		
MakeIconCommand::FindCommandStatus(SCommandStatus*	ioStatus)
{
	*ioStatus->enabled = false;
	ConstPhotoIterator	i;
	for (i = mDoc->GetView()->Selection().begin(); i != mDoc->GetView()->Selection().end(); i++) {
		if (!(*i)->IsEmpty()) {
			*ioStatus->enabled = true;
			return;
		}
	}
} // FindCommandStatus
