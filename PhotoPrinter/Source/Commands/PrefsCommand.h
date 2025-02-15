/*
	File:		PrefsCommand.h

	Contains:	Definition of the Preferences dialog.
				PrefsCommand
				PrefsDialog

	Written by:	David Dunham

	Copyright:	Copyright �2000 by Electric Fish, Inc.  All Rights reserved.

	Change History (most recent first):

		09 Nov 2000		drd		Don't need gShowing (EDialog how does this)
		08 Nov 2000		drd		Added PrefsDialog::gShowing
		03 aug 2000		dml		add NeedsSort
		02 aug 2000		dml		add NeedsRefresh, NeedsLayout
		21 Jul 2000		drd		Override ListenToMessage
		10 Jul 2000		drd		Descend from EDialog; added Commit
		28 Jun 2000		drd		Created
*/

#pragma once

#include "PhotoAppCommandAttachment.h"
#include "EDialog.h"
#include "PhotoPrintPrefs.h"

class PrefsCommand : public PhotoAppCommandAttachment
{
public:
						PrefsCommand				(const CommandT		inCommand,
													 PhotoPrintApp*		inApp);
	virtual 			~PrefsCommand				();

protected:
	// ECommandAttachment
	virtual	void		ExecuteCommand				(void*				inCommandData);
	virtual	void		FindCommandStatus			(SCommandStatus*	inStatus);
};

class PrefsDialog : public EDialog
{
public:
	enum {
		PPob_Prefs = 1200
	};
						PrefsDialog					(LCommander*		inSuper);
						~PrefsDialog();

	// LListener
	virtual void		ListenToMessage				(MessageT		inMessage,
													 void*			ioParam);

			void		Commit();

protected:
			bool		NeedsRefresh(const PhotoPrintPrefs& orig, const PhotoPrintPrefs& recent);
			bool		NeedsLayout(const PhotoPrintPrefs& orig, const PhotoPrintPrefs& recent);
			bool		NeedsSort(const PhotoPrintPrefs& orig, const PhotoPrintPrefs& recent);
};
