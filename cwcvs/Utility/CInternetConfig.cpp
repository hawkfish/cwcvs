/***********************************************************************\
	CInternetConfig.h
	
	A C++ class for using Internet Config
	
	by Dan Crevier
	
	version 1.1
	
	Some additions by Jerry Aman and Tom Emerson

\***********************************************************************/

#include "CInternetConfig.h"

#ifndef __COMPONENTS__
#include <Components.h>
#endif

#ifndef __GESTALT__
#include <Gestalt.h>
#endif

static void ClearMem(void *start, long len);

/***********************************************************************\
	CInternetConfig - constructor
		If prefFile is NULL, the default preferences are used
\***********************************************************************/

CInternetConfig::CInternetConfig(OSType appSig)
: started(false), mappings(0), applicationSig(appSig), installed(false)
{
	ComponentDescription ICDescription = {'PREF', 'ICAp', 0L, 0L, 0L};
	long response;

	// see if component version of IC is installed
	if (Gestalt(gestaltComponentMgr,&response)==noErr)
	{
		// first, see if IC is installed
		if (CountComponents(&ICDescription))
		{
			installed = true;
		}
	}
}
		
/***********************************************************************\
	~CInternetConfig - destructor
\***********************************************************************/

CInternetConfig::~CInternetConfig()
{
	Stop();
}

/***********************************************************************\
	Start - starts up IC
\***********************************************************************/

ICError CInternetConfig::Start(FSSpec *prefFile)
{
#if !CALL_NOT_IN_CARBON
#pragma unused(prefFile)
#endif
	ICError err;
	
	err = ICStart(&inst, applicationSig); 
	if (err == noErr)
	{
#if CALL_NOT_IN_CARBON
		if (prefFile == NULL) // use default prefs
		{
			err = ICFindConfigFile(inst, 0, NULL);
		}
		else
		{
			ICDirSpec prefDir;
			
			prefDir.vRefNum = prefFile->vRefNum;
			prefDir.dirID = prefFile->parID;
			err = ICFindUserConfigFile(inst, &prefDir);
		}
#endif
		if (err == noErr) // startup was successful
		{
		 	started = true;
		}
		else
		{
			// if ICStart was called, but pref couldn't be found, we
			// need to call ICStop

			ICStop(inst);
		}
	}
	return err;
}

/***********************************************************************\
	Stop - stop the instance
\***********************************************************************/

ICError CInternetConfig::Stop()
{
	if (started)
	{
		started = false;
		
		return ICStop(inst);
	}
	return noErr;
}

/***********************************************************************\
	DoURL - launch the URL past in
\***********************************************************************/

ICError CInternetConfig::DoURL(StringPtr theURL)
{
	long start = 0, end;
	
	if (started)
	{
		end = theURL[0];
		return ICLaunchURL(inst, "\p", (char *)theURL+1, end, &start, &end);
	}
	else
	{
		return -1;
	}
}

/***********************************************************************\
	MapFilename - return the map entry for the given filename
\***********************************************************************/

ICError CInternetConfig::MapFilename(StringPtr name, ICMapEntry *entry)
{
	if (started)
	{
		return ICMapFilename(inst, name, entry);
	}
	else
	{
		return -1;
	}
}

/***********************************************************************\
	MapFilename - return the map entry for the given filename
		takes name as C string
\***********************************************************************/

ICError CInternetConfig::MapFilename(char *name, ICMapEntry *entry)
{
	short i;
	Str255 newName;
	
	// convert name to P string
	for(i=0; name[i] && i<255; i++)
	{
		newName[i+1] = name[i];
	}
	newName[0] = i;
	
	return MapFilename(newName, entry);
}

/***********************************************************************\
	StartMapIteration -- starts iterating over the mappings
		Note: It calls ICBegin() and FinishMapIteration calls
			ICEnd(), so you have can't handle events in between
\***********************************************************************/

ICError CInternetConfig::StartMapIteration(void)
{
	ICError err;
	ICAttr			attr = 0;
	
	currentPos = 1;

	if (!started) return -1;
	
	err = ICBegin(inst, icReadOnlyPerm);
	if (err != noErr) return err;
	
	err = ICGetPrefHandle(inst, kICMapping, &attr, &mappings);
	
	if (!err)
	{
		err = ICCountMapEntries(inst, mappings, &numMappings);
	}
	
	if (err)
	{
		ICEnd(inst);
	}
	
	return err;
}

/***********************************************************************\
	NextMapEntry - get the next map entry after calling
		StartMapIteration.  Returns false if there are no more entries	
\***********************************************************************/

Boolean CInternetConfig::NextMapEntry(ICMapEntry *entry)
{
	long pos;

	if (!started || mappings == NULL) return false;
	
	// are we done?
	if (currentPos > numMappings) return false;
	
	ICGetIndMapEntry(inst, mappings, currentPos, &pos, entry);

	currentPos++;
	
	return true;
}

/***********************************************************************\
	FinishMapIteration -- call after doing a map iteration	
\***********************************************************************/

ICError CInternetConfig::FinishMapIteration(void)
{
	if (!started || mappings == NULL) return -1;
	
	DisposeHandle(mappings);
	mappings = NULL;
	
	return ICEnd(inst);
}

/***********************************************************************\
	GetFontRecord
	Return ICFontRecord for specified key - key names are in "ICKeys.h"
\***********************************************************************/

ICError CInternetConfig::GetFontRecord(ConstStr255Param keyStr, ICFontRecord
	*fontRecord)
{
	ICAttr junkAttr;
	long size = sizeof(ICFontRecord);
	
	if (started)
	{
		return ICGetPref(inst, keyStr, &junkAttr, (char *)fontRecord, &size);
	}
	else
	{
		return -1;
	}
};



/***********************************************************************\
	GetKeysString
	Return Pascal string for specified key - key names are in "ICKeys.h"
	By Jerry Aman
\***********************************************************************/

ICError CInternetConfig::GetKeysString(ConstStr255Param keyStr, Str255
			thePStr)
{
	ICAttr junkAttr;
	long size = 256;
	
	if (started)
	{
		return ICGetPref(inst, keyStr, &junkAttr, (char *)thePStr, &size);
	}
	else
	{
		return -1;
	}
};


/***********************************************************************\
	EditPreferences
	No need to supply a key string to edit the default settings
	By Jerry Aman
\***********************************************************************/

ICError CInternetConfig::EditPreferences(ConstStr255Param key)
{
	if (started)
	{
		return ICEditPreferences(inst, key);
	}
	else
	{
		return -1;
	}
}

/***********************************************************************\
	GetHelperFileSpec -- return the FSSpec to the latest incarnation
		of the specified helper application
	By Tom Emerson
\***********************************************************************/

ICError CInternetConfig::GetHelperFileSpec(StringPtr key, FSSpec *theFileSpec,
	OSType *creator)
{
	ICAttr		junkAttr;
	ICAppSpec	appSpec;
	long		size = sizeof(ICAppSpec);
	
	if (started)
	{
		if (ICGetPref(inst, key, &junkAttr, (char *)&appSpec, &size) == noErr)
		{
			if (LocateApplication(appSpec.fCreator, theFileSpec))
			{
				*creator = appSpec.fCreator;
				return noErr;
			}
		}
	}

	return -1;
}

/***********************************************************************\
	LocateApplication -- find the latest version of the specified
		application on the user's disks
	By Tom Emerson
\***********************************************************************/

Boolean CInternetConfig::LocateApplication(OSType signature, FSSpec *theFileSpec)
{
	short	index = 0;
	Boolean	found = false;
	
	HVolumeParam	hpb;
	DTPBRec			dpb;
	HFileInfo		cpb;
	
	FSSpec			most_recent;
	unsigned long	most_recent_date = 0;
	
	Str63			app_name;
	
	do
	{
		ClearMem(&hpb, sizeof(HVolumeParam));
		ClearMem(&dpb, sizeof(DTPBRec));
		
		hpb.ioVolIndex = ++index;
		if (PBHGetVInfoSync((HParamBlockRec *)&hpb) != noErr)
			break;
		
		dpb.ioVRefNum = hpb.ioVRefNum;
		if (PBDTGetPath(&dpb) == noErr)
		{
			dpb.ioFileCreator = signature;
			dpb.ioNamePtr = app_name;
			
			if (PBDTGetAPPLSync(&dpb) == noErr)
			{
				ClearMem(&cpb, sizeof(HFileInfo));
				cpb.ioNamePtr = app_name;
				cpb.ioVRefNum = dpb.ioVRefNum;
				cpb.ioDirID = dpb.ioAPPLParID;
				
				if ((PBGetCatInfoSync((CInfoPBRec *)&cpb) == noErr) &&
					(cpb.ioFlMdDat > most_recent_date))
				{
					most_recent.vRefNum = dpb.ioVRefNum;
					most_recent.parID = dpb.ioAPPLParID;
					BlockMoveData(app_name, most_recent.name, 64);
					most_recent_date = cpb.ioFlMdDat;
				}
				
				found = true;
			}
		}
	} while (! hpb.ioResult);
	
	if (found)
		*theFileSpec = most_recent;
		
	return found;
}

/***********************************************************************\
	ClearMem - clear a chunk of memory
\***********************************************************************/

void ClearMem(void *start, long len)
{
	char *p = (char *)start;
	
	for(long i=0; i<len; i++)
		p[i] = 0;
}
