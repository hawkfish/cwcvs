/*
	File:		ERegistrationFile.cp

	Contains:	Implementation of a Serial Number RegistrationFile file.

	Written by:	Richard Wesley

	Copyright:	Copyright �1999-2001 by Electric Fish, Inc.  All Rights Reserved.

	Change History (most recent first):

				20 Dec 01	drd		Condition permissions on OS X
				16 Dec 01	drd		Added "where" args to constructor
				14 Dec 01	drd		Set permissions
				10 Dec 01	drd		Added StDisableDebugThrow_() to GetRegString
         <6>    11/15/01	rmgw    Add default time for UseTime attribute.
         <5>    11/15/01	rmgw    Use DATE resource for UseTime attribute.
         <4>    11/15/01	rmgw    Add UseTime attribute.
         <3>    11/9/01		rmgw    Guard against clock diddling.
         <2>    11/1/01		rmgw    Use local domain.
         <1>    11/1/01		rmgw    Created from old RegistrationSerial.cp.
*/

#include "ERegistrationFile.h"

#include "MResFile.h"
#include "MFileSpec.h"
#include "MSpecialFolder.h"

// #include <sys/stat.h>	// ??? the Metrowerks valus seem wrong, and this isn't their path

#include <UResourceMgr.h>

// ---------------------------------------------------------------------------
//		� operator!=
// ---------------------------------------------------------------------------

static int
operator!= (

	const	NumVersion&	lhs,
	const	NumVersion&	rhs)
	
	{ // begin operator!=
		
		//	majorRev
		if (lhs.majorRev > rhs.majorRev)
			return 1;
			
		if (lhs.majorRev < rhs.majorRev)
			return -1;
			
		//	minorAndBugRev
		if (lhs.minorAndBugRev > rhs.minorAndBugRev)
			return 1;
			
		if (lhs.minorAndBugRev < rhs.minorAndBugRev)
			return -1;
			
		//	stage
		if (lhs.stage > rhs.stage)
			return 1;
			
		if (lhs.stage < rhs.stage)
			return -1;
		
		if (lhs.stage == finalStage) 
			return 0;
		
		//	nonRelRev
		if (lhs.nonRelRev > rhs.nonRelRev)
			return 1;
			
		if (lhs.nonRelRev < rhs.nonRelRev)
			return -1;
		
		//	Matches
		return 0;
		
	} // end operator!=
	
// ---------------------------------------------------------------------------
//		� ERegistrationFile
// ---------------------------------------------------------------------------

ERegistrationFile::ERegistrationFile (

	const	FSSpec&	inSpec)
	
	{ // begin UpdateLicenseFile
		
		SetRegSpec (inSpec);
			
	} // end ERegistrationFile
	
// ---------------------------------------------------------------------------
//		� ERegistrationFile
// ---------------------------------------------------------------------------

ERegistrationFile::ERegistrationFile (

	ConstStr255Param	inRegName,
	const OSType		inFolderType,	// e.g. kApplicationSupportFolderType or kPreferencesFolderType
	const SInt16		inDiskOrDomain)	// e.g. kLocalDomain
	
	{ // begin ERegistrationFile
		
		MFileSpec		regSpec(MSpecialFolder(inFolderType, inDiskOrDomain), inRegName, false); 
		this->SetRegSpec(regSpec);
			
	} // end ERegistrationFile
	
// ---------------------------------------------------------------------------
//		� SetRegSpec
// ---------------------------------------------------------------------------

void
ERegistrationFile::SetRegSpec (

	const	FSSpec&	inSpec)
	
	{ // begin SetRegSpec
		
		//	Find the registration inSpec
		MFileSpec		regSpec (inSpec, false); 
		if (!regSpec.Exists ()) regSpec.CreateResFile ('pref', 'MACS');
		
		// Be sure the file is writable by all users under OS X
		if (UEnvironment::IsRunningOSX()) {
			OSErr			err;
			FSRef			theRef;
			err = ::FSpMakeFSRef(&inSpec, &theRef);
			FSCatalogInfo	ci;
			err = ::FSGetCatalogInfo(&theRef, kFSCatInfoPermissions, &ci, nil, nil, nil);
			FSPermissionInfo*	perms = (FSPermissionInfo*) &ci.permissions[0];
			// It doesn't look like MSL defines the bits the same way as chmod(2), so we'll
			// set the bits numerically.
			// perms->mode |= S_IWGRP | S_IWOTH;
			perms->mode |= 0x12;	// W for group, W for other
			err = ::FSSetCatalogInfo(&theRef, kFSCatInfoPermissions, &ci);
		}

		//	Hide it
		FInfo			fInfo;
		regSpec.GetFinderInfo (fInfo);
		fInfo.fdFlags |= kIsInvisible;
		regSpec.SetFinderInfo (fInfo);
		
		//	Get the dates
		CInfoPBRec		pb;
		regSpec.GetCatInfo (pb);
		
		//	Get the app vers resource
		StResource		appVersRsrc ('vers', 1);
		StHandleLocker	appVersLock (appVersRsrc);
		VersRecHndl		appVers (reinterpret_cast<VersRecHndl> (appVersRsrc.Get ()));
		
		//	Get the reg vers resource
		StCurResFile	saveResFile;
		MResFile		regFile (regSpec);
		StNewResource	regVersRsrc ('vers', 1);
		
		//	Compare them
		if (regVersRsrc.ResourceExisted ()) {
			//	Reg vers existed, so compare
			VersRecHndl	regVers (reinterpret_cast<VersRecHndl> (regVersRsrc.Get ()));
			int			regComp = ((**regVers).numericVersion != (**appVers).numericVersion);
			
			if (regComp < 0) {
				//	reg earlier than app, so reset dates
				::GetDateTime (&pb.hFileInfo.ioFlCrDat);
				pb.hFileInfo.ioFlMdDat = pb.hFileInfo.ioFlCrDat;
				regSpec.SetCatInfo (pb);
				} // if
			} // if
			
		//	Make the reg vers match us
		::PtrToXHand (*appVersRsrc, regVersRsrc.Get (), ::GetHandleSize (appVersRsrc));
		
		//	Change the spec
		mRegSpec = inSpec;

	} // end SetRegSpec
	
// ---------------------------------------------------------------------------
//		� GetRegTime
// ---------------------------------------------------------------------------

UInt32
ERegistrationFile::GetRegTime (void) const
	
	{ // begin GetRegTime
	
		//	Find the registration file
		MFileSpec		regSpec (GetRegSpec ());

		//	Get the dates
		CInfoPBRec		pb;
		regSpec.GetCatInfo (pb);
		
		//	Make sure they haven't diddled the clock.
		UInt32			now;
		::GetDateTime (&now);
		if (pb.hFileInfo.ioFlCrDat > now) {
			//	Created in the future!  Move it back to now...
			pb.hFileInfo.ioFlCrDat = now;
			regSpec.SetCatInfo (pb);
			} // if
		
		//	Return corrected value
		return pb.hFileInfo.ioFlCrDat;
		
	} // end GetRegTime
	
// ---------------------------------------------------------------------------
//		� SetRegTime
// ---------------------------------------------------------------------------

void
ERegistrationFile::SetRegTime (

	UInt32	inRegTime)
	
	{ // begin SetRegTime
		
		//	Find the registration file
		MFileSpec		regSpec (GetRegSpec ());

		//	Get the dates
		CInfoPBRec		pb;
		regSpec.GetCatInfo (pb);
		
		//	Set the create date == reg time
		pb.hFileInfo.ioFlCrDat = inRegTime;
		
		//	Maintain order
		if (pb.hFileInfo.ioFlMdDat < pb.hFileInfo.ioFlCrDat)
			pb.hFileInfo.ioFlMdDat = pb.hFileInfo.ioFlCrDat;
		
		//	Make the changes
		regSpec.SetCatInfo (pb);
			
	} // end SetRegTime
	
// ---------------------------------------------------------------------------
//		� GetUseTime
// ---------------------------------------------------------------------------

UInt32
ERegistrationFile::GetUseTime (void) const
	
	{ // begin GetUseTime
	
		//	Find the registration file
		MFileSpec		regSpec (GetRegSpec ());
			
		//	Use its resources
		StCurResFile	saveResFile;
		MResFile		regFile (regSpec);
		
		//	Get the current time.
		UInt32			now;
		::GetDateTime (&now);
		
		//	Get the resource
		UInt32			outUseTime;
		StNewResource	h ('DATE', 128, sizeof (outUseTime), true);
		if (!h.ResourceExisted ()) ::BlockMoveData (&now, *h, sizeof (now));
		
		//	Get the use time
		::BlockMoveData (*h, &outUseTime, sizeof (outUseTime));
			
		//	Make sure they haven't diddled the clock.
		if (outUseTime < now) {
			//	Modified in the past!  Move it forward to now...
			outUseTime = now;
			::BlockMoveData (&outUseTime, *h, sizeof (outUseTime));
			} // if
		
		//	Return corrected value
		return outUseTime;
		
	} // end GetUseTime
	
// ---------------------------------------------------------------------------
//		� SetUseTime
// ---------------------------------------------------------------------------

void
ERegistrationFile::SetUseTime (

	UInt32	inUseTime)
	
	{ // begin SetUseTime
		
		//	Find the registration file
		MFileSpec		regSpec (GetRegSpec ());
			
		//	Use its resources
		StCurResFile	saveResFile;
		MResFile		regFile (regSpec);
		
		//	Make the resource
		StNewResource	h ('DATE', 128, sizeof (inUseTime), true);
		::BlockMoveData (&inUseTime, *h, sizeof (inUseTime));
			
	} // end SetUseTime
	
// ---------------------------------------------------------------------------
//		� GetRegString
// ---------------------------------------------------------------------------

StringPtr
ERegistrationFile::GetRegString (

	StringPtr	outRegString) const
	
	{ // begin GetRegString
		
		//	Find the registration file
		MFileSpec		regSpec (GetRegSpec ());
		
		//	Use its resources
		StCurResFile	saveResFile;
		MResFile		regFile (regSpec);
		
		//	Get the resource
		StDisableDebugThrow_();						// It's OK for this to fail
		Handle			h = ::Get1Resource ('STR ', 128);
		ThrowIfResFail_(h);
			
		//	Copy it out
		StringHandle	s = reinterpret_cast<StringHandle> (h);
		::BlockMoveData (*s + 1, outRegString + 1, outRegString[0] = **s);
		
		//	Return it
		return outRegString;
		
	} // end GetRegString
	
// ---------------------------------------------------------------------------
//		� SetRegString
// ---------------------------------------------------------------------------

void
ERegistrationFile::SetRegString (

	ConstStr255Param	inRegString)
	
	{ // begin SetRegString
		
		//	Find the registration file
		MFileSpec		regSpec (GetRegSpec ());
			
		//	Use its resources
		StCurResFile	saveResFile;
		MResFile		regFile (regSpec);
		
		//	Make the resource
		StNewResource	h ('STR ', 128, StrLength (inRegString) + 1, true);
		::BlockMoveData (inRegString, *h, StrLength (inRegString) + 1);
		
	} // end SetRegString
	
