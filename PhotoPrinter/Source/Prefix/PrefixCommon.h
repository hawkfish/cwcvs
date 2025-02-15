// ===========================================================================
//	PrefixCommon.h		�1999 Metrowerks Inc. All rights reserved.
// ===========================================================================
//	This file contains settings/material common to all targets for the
//	prefix file/precompiled header.

//	16 jun 2000		dml		moved AlignmentType typedef here
//	15 Jun 2000		drd		Use PP_StdDialogs_NavServicesOnly


	// Option for using PowerPlant namespace
#define PP_Uses_PowerPlant_Namespace		0	// OFF, don't use PowerPlant namespace

	// Standard Dialogs
#define PP_StdDialogs_Option				PP_StdDialogs_NavServicesOnly	// we require Nav, no need for conditional
																			// Actually, we use MNavigation

	// Force the use of new types
#define PP_Uses_Old_Integer_Types			0	// OFF, use new types

	// Don't allow use of the obsolete AllowTargetSwitch
#define PP_Obsolete_AllowTargetSwitch		0

	// Don't show release notes warnings for old projects
#define PP_Suppress_Notes_20				1
#define PP_Suppress_Notes_21				1
#define PP_Suppress_Notes_211				1
#define PP_Suppress_Notes_22				1


typedef unsigned short AlignmentType;
