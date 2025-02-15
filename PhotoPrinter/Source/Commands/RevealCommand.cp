/*
	File:		RevealCommand.cp

	Contains:	Implementation of the Reveal in Finder command.

	Written by:	David Dunham

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights reserved.

	Change History (most recent first):

		21 Aug 2001		drd		340 Be more paranoid about using GetFileSpec() since it can be nil
		14 Aug 2000		drd		Created
*/

#include "RevealCommand.h"

#include "EUtil.h"
#include "MAEAddressDesc.h"
#include "MAppleEvent.h"
#include "PhotoPrintDoc.h"

/*
RevealCommand
*/
RevealCommand::RevealCommand(const CommandT inCommand, PhotoPrintDoc* inDoc)
	: PhotoDocCommandAttachment(inCommand, inDoc)
{
} // RevealCommand

/*
~RevealCommand
*/
RevealCommand::~RevealCommand()
{
} // ~RevealCommand

/*
ExecuteCommand
*/
void		
RevealCommand::ExecuteCommand(void* inCommandData)
{
#pragma unused(inCommandData)

	PhotoPrintItem*		image = mDoc->GetView()->GetPrimarySelection();
	MFileSpec*			fs;
	fs = image->GetFileSpec();

	if (fs != nil) {
		// send an Apple Event
		MAEAddressDesc	finder('MACS');
		MAppleEvent		theAE(kAEMiscStandards, kAEMakeObjectsVisible, finder);
		theAE.PutParamPtr(typeFSS, fs, sizeof(FSSpec), keyDirectObject);
		OSErr	err = ::AESend(theAE, nil, kAENoReply, kAENormalPriority, kAEDefaultTimeout, nil, nil);

		EUtil::BringFinderToFront();
	} else {
		::SysBeep(1);
	}
} // ExecuteCommand
									 
/*
FindCommandStatus {OVERRIDE}
*/
void		
RevealCommand::FindCommandStatus		(SCommandStatus*	ioStatus)
{
	*ioStatus->enabled = false;
	if (mDoc->GetView()->IsAnythingSelected()) {
		PhotoPrintItem*		image = mDoc->GetView()->GetPrimarySelection();
		if (!image->IsEmpty())
			*ioStatus->enabled = true;
	}
} // FindCommandStatus
