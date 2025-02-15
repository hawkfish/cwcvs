/*
	File:		PhotoPrintApp_Register.cp

	Contains:	PowerPlant registration for the application class.

	Written by:	David Dunham

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights reserved.

	Change History (most recent first):

		30 Aug 2001		drd		Register LAMTextGroupBoxImp (PowerPlant 2.2)
		28 Aug 2001		rmgw	PowerPlant 2.2fc4.
		16 Aug 2001		drd		323 Register ThemeColorEraseAttachment
		25 Jul 2001		drd		Suppress warning about obsolete LGADialog
		16 Jul 2001		rmgw	Add ABalloon.  Bug #201.
		16 Jul 2001		rmgw	Add LPane, LPaintAttachment, LGADialog/LDialogBox (debug only).
		16 Jul 2001		rmgw	Add LEditTextView.
		28 jun 2001		dml		add LIconPane
		23 May 2001		drd		Register LPicture (we now use this instead of LImageWell)
		23 May 2001		drd		38 We don't use PaletteButton any more, so don't register it
		23 May 2001		drd		Removed ALL of LImageWell
		22 May 2001		drd		Removed LGAColorSwatchControl, LImageWell
		21 Mar 2001		drd		PopupBevelButton
		22 feb 2001		dml		add PhotoBadge 
		08 Feb 2001		drd		LGADialog is obsolete (in PP 2.1.1), so don't register it
		15 Sep 2000		drd		Register E2ClickAttachment
		21 aug 2000		dml		add imageWell
		10 Jul 2000		drd		Split from PhotoPrintApp.cp
*/

#include "PhotoPrintApp.h"

#include "FileEditText.h"
#include "FPEditText.h"
#include "PhotoPrintView.h"
#include "PhotoWindow.h"
#include "PhotoBadge.h"
#include "PopupBevelButton.h"
#include "ThemeColorEraseAttachment.h"

#include "CURLPushButton.h"

#include "E2ClickAttachment.h"

#include <URegistrar.h>

#include <LBevelButton.h>
#include <LCheckBox.h>
#include <LCmdBevelButton.h>
#include <LDebugMenuAttachment.h>
#include <LDialogBox.h>
#include <LEditText.h>
#include <LIconControl.h>
#include <LIconPane.h>
#include <LMultiPanelView.h>
#include <LPictureControl.h>
#include <LPlacard.h>
#include <LPopupButton.h>
#include <LProgressBar.h>
#include <LPushButton.h>
#include <LRadioButton.h>
#include <LScrollBar.h>
#include <LScrollerView.h>
#include <LSeparatorLine.h>
#include <LSlider.h>
#include <LStaticText.h>
#include <LTabsControl.h>
#include <LTextEditView.h>
#include <LTextGroupBox.h>
#include <LWindow.h>
#include <UAttachments.h>

#if PP_DEBUG
#undef PP_Warn_Obsolete_Classes
#define PP_Warn_Obsolete_Classes 0			// Yes, I know it�s obsolete
#include <LGADialog.h>
#endif

// Appearance Manager Implementation (for registration)
#include <LAMBevelButtonImp.h>
#include <LAMControlImp.h>
#include <LAMEditTextImp.h>
#include <LAMImageWellImp.h>
#include <LAMPlacardImp.h>
#include <LAMPopupButtonImp.h>
#include <LAMPushButtonImp.h>
#include <LAMStaticTextImp.h>
#include <LAMTabsControlImp.h>
#include <LAMTrackActionImp.h>
#include <LAMTextGroupBoxImp.h>

#include "ABalloon.h"

// ---------------------------------------------------------------------------
//	� RegisterClasses								[protected]
// ---------------------------------------------------------------------------
//	To reduce clutter within the Application object's constructor, class
//	registrations appear here in this seperate function for ease of use.

void
PhotoPrintApp::RegisterClasses()
{
	// Register core PowerPlant classes.
	RegisterClass_(LColorEraseAttachment);
	RegisterClass_(LDialogBox);
	RegisterClass_(LMultiPanelView);
	RegisterClass_(LPane);
	RegisterClass_(LPlaceHolder);
	RegisterClass_(LPrintout);
	RegisterClass_(LRadioGroupView);
	RegisterClass_(LScrollerView);
	RegisterClass_(LTabGroup);
	RegisterClass_(LTextEditView);
	RegisterClass_(LView);
	RegisterClass_(LWindow);
	RegisterClass_(LWindowThemeAttachment);
	RegisterClass_(LBorderAttachment);
	RegisterClass_(LPaintAttachment);

#if PP_DEBUG
	RegisterClassID_(LDialogBox,		LGADialog::class_ID);		//	PP is being bad�
#endif

	// Register the Appearance Manager/GA classes we actually use, rather than just
	// registering all of them via UControlRegistryRegisterClasses().
	RegisterClass_(LBevelButton);
	RegisterClass_(LCheckBox);
	RegisterClass_(LCmdBevelButton);
	RegisterClass_(LEditText);
	RegisterClass_(LIconControl);
	RegisterClass_(LIconPane);
	RegisterClass_(LPictureControl);
	RegisterClass_(LPlacard);
	RegisterClass_(LPopupButton);
	RegisterClass_(LProgressBar);
	RegisterClass_(LPushButton);
	RegisterClass_(LRadioButton);
	RegisterClass_(LScrollBar);
	RegisterClass_(LSeparatorLine);
	
	RegisterClass_(LSlider);
	RegisterClass_(LPicture);
	RegisterClass_(LStaticText);
	RegisterClass_(LTabsControl);
	RegisterClass_(LTextGroupBox);

	RegisterClassID_(LAMBevelButtonImp,		LBevelButton::imp_class_ID);
	RegisterClassID_(LAMControlImp,			LCheckBox::imp_class_ID);
	RegisterClassID_(LAMEditTextImp,		LEditText::imp_class_ID);
	RegisterClassID_(LAMControlImp, 		LIconControl::imp_class_ID);
	RegisterClassID_(LAMControlImp,			LPictureControl::imp_class_ID);
	RegisterClassID_(LAMPlacardImp,		 	LPlacard::imp_class_ID);
	RegisterClassID_(LAMPopupButtonImp,	 	LPopupButton::imp_class_ID);
	RegisterClassID_(LAMTrackActionImp,		LProgressBar::imp_class_ID);
	RegisterClassID_(LAMPushButtonImp,		LPushButton::imp_class_ID);
	RegisterClassID_(LAMControlImp,			LRadioButton::imp_class_ID);
	RegisterClassID_(LAMControlImp,			LSeparatorLine::imp_class_ID);
	RegisterClassID_(LAMTrackActionImp,		LScrollBar::imp_class_ID);
	RegisterClassID_(LAMTrackActionImp,		LSlider::imp_class_ID);
	RegisterClassID_(LAMStaticTextImp,		LStaticText::imp_class_ID);
	RegisterClassID_(LAMTabsControlImp,		LTabsControl::imp_class_ID);
	RegisterClassID_(LAMTextGroupBoxImp,	LTextGroupBox::imp_class_ID);
	
	// E++
	RegisterClass_(E2ClickAttachment);
	
	// third party
	RegisterClass_(CURLPushButton);
	RegisterClass_(ABalloon);
	RegisterClass_(AAutoBalloon);

	// Register app-specific classes
	RegisterClass_(FileEditText);
	RegisterClass_(FPEditText);
	RegisterClass_(PhotoPrintView);
	RegisterClass_(PhotoWindow);
	RegisterClass_(PhotoBadge);
	RegisterClass_(PopupBevelButton);
	RegisterClass_(ThemeColorEraseAttachment);
} // RegisterClasses
