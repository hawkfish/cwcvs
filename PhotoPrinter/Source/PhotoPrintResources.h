/*
	File:		PhotoPrintResources.h

	Contains:	Resource constants.

	Written by:	David Dunham

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

		06 Aug 2001		rmgw	Added str_CommandName.  Bug #295.
		06 Aug 2001		rmgw	Added si_ImportImage.  Bug #286.
		20 Jul 2001		rmgw	Implement document undo.
		18 Jul 2001		rmgw	Implement layout undo.  Bug #200.
		18 Jul 2001		rmgw	Implement image options undo.  Bug #165.
		02 Jul 2001		drd		cicn_FlexibleLandscape, cicn_FlexiblePortrait
		21 May 2001		drd		Moved MENU_FontCopy here
		13 mar 2001		dml		added si_RenameFile, si_ChangeFont
		19 Sep 2000		drd		Added str_Navigation
		12 Sep 2000		drd		Added str_Page; moved str_Zoom here
		16 aug 2000		dml		add curs_Rot
		15 Aug 2000		drd		Added si_RemoveCrop, si_Rotate, si_RemoveRotation
		14 Aug 2000		drd		Added si_Crop, si_CropZoom
		11 Aug 2000		drd		Moved curs_ here
		04 Aug 2000		drd		Created
*/

#pragma once

enum {
	cicn_FlexibleLandscape = 403,
	cicn_FlexiblePortrait = 404,

	curs_Hand = 1000,
	curs_Crop = 1001,
	curs_Rot = 1002,

	MENU_FontCopy = 253,

	str_Navigation = 600,
		si_Import = 1,
	str_Page = 302,
		si_SinglePage = 1,
		si_MultiplePages,
	str_Redo = 259,							// 'STR#'
	str_Undo = 260,							// 'STR#'
	str_CommandName = 1261,					// 'STR#'
		si_PasteImage = 1,
		si_PastePicture,
		si_PasteText,
		si_DeleteImage,
		si_CutImage,
		si_DropImage,
		si_ImportImage,
		si_DuplicateImage,
		si_DropPicture,
		si_DropText,
		si_Nudge,
		si_Move,
		si_Copy,
		si_Resize,
		si_MoveToBack,
		si_MoveToFront,
		si_MoveBackward,
		si_MoveForward,
		si_Crop,
		si_CropZoom,
		si_RemoveCrop,
		si_Rotate,
		si_RemoveRotation,
		si_RenameFile,
		si_ChangeFont,
		si_ImageOptionsAction,
		si_DocPreferences,
		si_ChangeLayoutType,
		si_ChangeMinSize,
		si_ChangeMaxSize,
		si_ChangeOrientation,
	str_Zoom = 301,
		si_Normal = 1,
		si_Precise,

	rsrc_Fnord		// Allows us to put commas everywhere
};
