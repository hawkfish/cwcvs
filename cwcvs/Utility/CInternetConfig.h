#pragma once
/***********************************************************************\
	CInternetConfig.h
	
	A C++ class for using Internet Config
	
	by Dan Crevier
	
	version 1.1

\***********************************************************************/

#ifndef __INTERNETCONFIG__
#include "InternetConfig.h"
#endif

typedef	OSStatus	ICError;

class CInternetConfig
{
	public:
		
		CInternetConfig(OSType appSig);
		virtual ~CInternetConfig();

		ICError Start(FSSpec *prefFile = NULL);
		ICError Stop();

		Boolean ICComponentInstalled() { return installed; };

		ICError DoURL(StringPtr theURL);
		
		// preferences that return strings
		ICError GetRealName(StringPtr result)
			{ return GetKeysString(kICRealName, result); };
		ICError GetEmailAddress(StringPtr result)
			{ return GetKeysString(kICEmail, result); };
		ICError GetMailAccount(StringPtr result)
			{ return GetKeysString(kICMailAccount, result); };
		ICError GetMailPassword(StringPtr result)
			{ return GetKeysString(kICMailPassword, result); };
		ICError GetNewsAuthUsername(StringPtr result)
			{ return GetKeysString(kICNewsAuthUsername, result); };
		ICError GetNewsAuthPassword(StringPtr result)
			{ return GetKeysString(kICNewsAuthPassword, result); };
		ICError GetArchiePrefered(StringPtr result)
			{ return GetKeysString(kICArchiePreferred, result); };
		ICError GetUMichPreferred(StringPtr result)
			{ return GetKeysString(kICUMichPreferred, result); };
		ICError GetInfoMacPreferred(StringPtr result)
			{ return GetKeysString(kICInfoMacPreferred, result); };
		ICError GetPhHost(StringPtr result)
			{ return GetKeysString(kICPhHost, result); };
		ICError GetWhoisHost(StringPtr result)
			{ return GetKeysString(kICWhoisHost, result); };
		ICError GetFingerHost(StringPtr result)
			{ return GetKeysString(kICFingerHost, result); };
		ICError GetFTPHost(StringPtr result)
			{ return GetKeysString(kICFTPHost, result); };
		ICError GetTelnetHost(StringPtr result)
			{ return GetKeysString(kICTelnetHost, result); };
		ICError GetSMTPHost(StringPtr result)
			{ return GetKeysString(kICSMTPHost, result); };
		ICError GetNNTPHost(StringPtr result)
			{ return GetKeysString(kICNNTPHost, result); };
		ICError GetGopherHost(StringPtr result)
			{ return GetKeysString(kICGopherHost, result); };
		ICError GetLDAPServer(StringPtr result)
			{ return GetKeysString(kICLDAPServer, result); };
		ICError GetLDAPSearchbase(StringPtr result)
			{ return GetKeysString(kICLDAPSearchbase, result); };
		ICError GetWWWHomePage(StringPtr result)
			{ return GetKeysString(kICWWWHomePage, result); };
		ICError GetWAISGateway(StringPtr result)
			{ return GetKeysString(kICWAISGateway, result); };
		ICError GetOrganization(StringPtr result)
			{ return GetKeysString(kICOrganization, result); };

		// key based				
		ICError GetKeysString(ConstStr255Param keyStr, Str255 thePStr);

		// font records
		ICError GetListFont(ICFontRecord *fontRecord)
			{ return GetFontRecord(kICListFont, fontRecord); };
		ICError GetScreenFont(ICFontRecord *fontRecord)
			{ return GetFontRecord(kICScreenFont, fontRecord); };
		ICError GetPrinterFont(ICFontRecord *fontRecord)
			{ return GetFontRecord(kICPrinterFont, fontRecord); };
		ICError GetFontRecord(ConstStr255Param keyStr, ICFontRecord *fontRecord);

		// File mappings
		ICError MapFilename(StringPtr name, ICMapEntry *entry);
		ICError MapFilename(char *name, ICMapEntry *entry);
		
		ICError StartMapIteration(void);
		Boolean NextMapEntry(ICMapEntry *entry);
		ICError	FinishMapIteration(void);

		// editing preferences
		ICError EditPreferences(ConstStr255Param key = "\p");
		
		// Routines to get helper application FSSpec's
		ICError GetEditorHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�editor", theFileSpec, creator); };
		ICError GetFTPHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�fto", theFileSpec, creator); };
		ICError GetNewsHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�nntp", theFileSpec, creator); };
		ICError GetMailHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�mailto", theFileSpec, creator); };
		ICError GetGopherHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�gopher", theFileSpec, creator); };
		ICError GetWebHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�http", theFileSpec, creator); };
		ICError GetFingerHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�finger", theFileSpec, creator); };
		ICError GetTelnetHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�editor", theFileSpec, creator); };
		ICError GetTN3270Helper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�telnet", theFileSpec, creator); };
		ICError GetWAISHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�tn3270", theFileSpec, creator); };
		ICError GetWhoIsHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�wais", theFileSpec, creator); };
		ICError GetNetPhHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�netphone", theFileSpec, creator); };
		ICError GetXNetPhHelper(FSSpec *theFileSpec, OSType *creator)
			{ return GetHelperFileSpec("\pHelper�x-netphone", theFileSpec, creator); };
	
	protected:
		ICError GetHelperFileSpec(StringPtr key, FSSpec *theFileSpec, OSType *creator);
		Boolean LocateApplication(OSType signature, FSSpec *theFileSpec);

		ICInstance inst;
		Boolean installed, started;
		Handle mappings;
		long numMappings, currentPos;
		OSType applicationSig;
};


