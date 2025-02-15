/*
	File:		PhotoItemProperties.cp

	Contains:	properties that an item might use, but which aren't intrinsic to an Item

	Written by:	Dav Lion and David Dunham

	Copyright:	Copyright �2000-2001 by Electric Fish, Inc.  All Rights reserved.

	Change History (most recent first):

		23 Jul 2001		drd		148 SizeLimitToInches uses �DIM#� resource
		03 jul 2001		dml		(104) add mCaptionLinkedRotation
		28 Jun 2001		drd		24 107 Read, Write handle mFontNumber, mCaptionStyle; SetFontName
		21 Aug 2000		drd		Got rid of the method version of ParseAlignment
		17 Jul 2000		drd		Added limit_Index
		12 Jul 2000		drd		GetCaptionLineHeight
		12 jul 2000		dml 	add SizeLimitToInches
		11 Jul 2000		drd		Use PhotoPrintPrefs to determine initial settings
		10 Jul 2000		drd		Some new properties have persistence
		10 jul 2000		dml		copy ct must fully copy caption!
		06 Jul 2000		drd		Added caption, shape, frame stuff
		16 jun 2000		dml		factored alignment into separate object
		14 Jun 2000		dml		Added AlignmentType (and associated map classes), alphabetized
*/

#include "PhotoItemProperties.h"

#include "AlignmentGizmo.h"
#include "PhotoPrintPrefs.h"
#include "xmlinput.h"
#include "xmloutput.h"

#pragma mark Globals

const char	*const PhotoItemProperties::gCaptionStyles[caption_COUNT] =
{
	"None",
	"Bottom",
	"RightHorizontal",
	"RightVertical",
	"LeftHorizontal",
	"LeftVertical",
	"Top",
	"Inside"
};

#pragma mark -
//----------------------------------------
// empty ct is most permissive
//----------------------------------------
PhotoItemProperties::PhotoItemProperties()
{
	SetRotate(true);
	SetMaximize(false);
	SetAspect(false);	
	SetFullSize(false);
	SetAlignment(kAlignNone);

	PhotoPrintPrefs*	prefs = PhotoPrintPrefs::Singleton();

	this->SetBlurEdges(false);
	this->SetCaptionStyle(prefs->GetCaptionStyle());
	SetCaptionLinkedRotation(false);
	this->SetFontNumber(prefs->GetFontNumber());
	this->SetFontSize(prefs->GetFontSize());
	this->SetFrameColor(Color_Black);
	this->SetFrameStyle(frame_None);
	this->SetImageShape(shape_Rectangle);
	this->SetShadow(false);
	this->SetShadowColor(Color_Black);
	this->SetShowDate(prefs->GetShowFileDates());
	this->SetShowName(prefs->GetShowFileNames());
	
}//end empty ct


//----------------------------------------
// fully specified ct
//----------------------------------------
PhotoItemProperties::PhotoItemProperties(bool inRotate, bool inResize, bool inMove,
											bool inMax, bool inAspect, 
											bool inFullSize, AlignmentType inAlignment)
{
	SetRotate(inRotate);
	SetResize(inResize);
	SetMove(inMove);
	SetMaximize(inMax);
	SetAspect(inAspect);	
	SetFullSize(inFullSize);
	SetAlignment(inAlignment);

	// For now, just set them to defaults (as in default constructor)
	PhotoPrintPrefs*	prefs = PhotoPrintPrefs::Singleton();
	this->SetBlurEdges(false);
	this->SetCaptionStyle(prefs->GetCaptionStyle());
	SetCaptionLinkedRotation(false);
	this->SetFontNumber(prefs->GetFontNumber());
	this->SetFontSize(prefs->GetFontSize());
	this->SetFrameColor(Color_Black);
	this->SetFrameStyle(frame_None);
	this->SetImageShape(shape_Rectangle);
	this->SetShadow(false);
	this->SetShadowColor(Color_Black);
	this->SetShowDate(prefs->GetShowFileDates());
	this->SetShowName(prefs->GetShowFileNames());
}//end fully specified ct
	

//----------------------------------------
// copy ct
//----------------------------------------
PhotoItemProperties::PhotoItemProperties(const PhotoItemProperties& other) {
	SetRotate(other.GetRotate());
	SetResize(other.GetResize());
	SetMove(other.GetMove());
	SetMaximize(other.GetMaximize());
	SetAspect(other.GetAspect());
	SetAlignment(other.GetAlignment());

	this->SetBlurEdges(other.GetBlurEdges());

	this->SetCaptionStyle(other.GetCaptionStyle());
	SetCaption(other.GetCaption());
	SetCaptionLinkedRotation(other.GetCaptionLinkedRotation());

	this->SetFontNumber(other.GetFontNumber());
	this->SetFontSize(other.GetFontSize());
	this->SetFrameColor(other.GetFrameColor());
	this->SetFrameStyle(other.GetFrameStyle());
	this->SetImageShape(other.GetImageShape());
	this->SetShadow(other.GetShadow());
	this->SetShadowColor(other.GetShadowColor());
	this->SetShowDate(other.GetShowDate());
	this->SetShowName(other.GetShowName());
}//end copy ct

//----------------------------------------
// ~PhotoItemProperties
//----------------------------------------
PhotoItemProperties::~PhotoItemProperties()
{
}//end dt


//----------------------------------------
// Getters.  all trivial
//----------------------------------------
AlignmentType 	PhotoItemProperties::GetAlignment() const {return mAlignment;};
bool 			PhotoItemProperties::GetAspect() const {return mMaintainAspect;};
bool 			PhotoItemProperties::GetFullSize() const {return mFullSize;};
bool			PhotoItemProperties::GetMaximize() const {return mMaximize;};
bool			PhotoItemProperties::GetMove() const {return mCanMove;};
bool			PhotoItemProperties::GetResize() const {return mCanResize;};
bool			PhotoItemProperties::GetRotate() const {return mCanRotate;};

//----------------------------------------
// GetCaptionLineHeight  
//----------------------------------------
SInt16
PhotoItemProperties::GetCaptionLineHeight() const
{
	StTextState		save;

	::TextFont(this->GetFontNumber());
	::TextSize(this->GetFontSize());
	::TextFace(normal);
	FontInfo		fi;
	::GetFontInfo(&fi);

	return fi.ascent + fi.descent + fi.leading;
} // GetCaptionLineHeight

//----------------------------------------
// HasCaption  
//----------------------------------------
bool
PhotoItemProperties::HasCaption() const
{
	return this->GetCaptionStyle() != caption_None &&
		(this->GetCaption().Length() > 0 || this->GetShowDate() || this->GetShowName());
} // HasCaption

//----------------------------------------
// Setters  
//----------------------------------------
void	PhotoItemProperties::SetAlignment(AlignmentType inAlign) {mAlignment = inAlign;};
void 	PhotoItemProperties::SetAspect(bool inVal) {mMaintainAspect = inVal;};

/*
SetFontName
*/
void
PhotoItemProperties::SetFontName(ConstStr255Param inName)
{
	::GetFNum(inName, &mFontNumber);		// ??? doesn't check validity
} // SetFontName

//------------------------------------
// SetFullSize.  If true, turn Maximize OFF, MaintainAspect ON
//------------------------------------
void	
PhotoItemProperties::SetFullSize(bool inVal) 
{
	mFullSize = inVal;
	if (inVal == true) {
		SetMaximize(false);
		SetAspect(true);
		}//end
	}//end SetFullSize

void	PhotoItemProperties::SetMaximize(bool inVal) {mMaximize = inVal;};
void 	PhotoItemProperties::SetMove(bool inVal) {mCanMove = inVal;};
void 	PhotoItemProperties::SetResize(bool inVal) {mCanResize = inVal;};
void 	PhotoItemProperties::SetRotate(bool inVal) {mCanRotate = inVal;};	

typedef struct {
	double	hInches;
	double	vInches;
} InchDimensionT;

/*
SizeLimitToInches
	Converts an integer such as limit_Slide to a horizontal and vertical number of inches.
	Localizers will have to convert sizes from metric to inches (since we work with dpi).
*/
void 	
PhotoItemProperties::SizeLimitToInches(SizeLimitT inLimit, double& hInches, double& vInches) {
	StResource			dimListRsrc('DIM#', 128);
	InchDimensionT**	dimList = reinterpret_cast<InchDimensionT**> ((Handle) dimListRsrc);
	InchDimensionT*		theLimit = *dimList + (inLimit - 1);

	hInches = theLimit->hInches;
	vInches = theLimit->vInches;
}//end


#pragma mark -

//------------------------------------
// I/O		based on xmlio library
//------------------------------------
void PhotoItemProperties::Read(XML::Element &elem)
{
	char	caption[256];
	caption[0] = 0;
	char	fontName[256];
	fontName[0] = 0;

	XML::Handler handlers[] = {
		XML::Handler("alignment", ParseAlignment, &mAlignment),
		XML::Handler("aspect", &mMaintainAspect),
		XML::Handler("fullSize", &mFullSize),
		XML::Handler("maximize", &mMaximize),
		XML::Handler("move", &mCanMove),
		XML::Handler("resize", &mCanResize),
		XML::Handler("rotate", &mCanRotate),
		XML::Handler("caption", caption, sizeof(caption)),
		XML::Handler("captionStyle", gCaptionStyles, caption_COUNT,
			XML_OBJECT_MEMBER(PhotoItemProperties, mCaptionStyle)),
		XML::Handler("captionLinkedRotation", &mCaptionLinkedRotation),
		XML::Handler("showDate", &mShowDate),
		XML::Handler("showName", &mShowName),
		XML::Handler("fontName", fontName, sizeof(fontName)),
		XML::Handler("fontSize", &mFontSize),
		// !!! ShapeT			mImageShape;
		XML::Handler("shadow", &mShadow),
		// !!! RGBColor		mShadowColor;
		XML::Handler("blurEdges", &mBlurEdges),
		// !!! RGBColor		mFrameColor;
		// !!! FrameT		mFrameStyle;
		XML::Handler::END
		}; //handlers
	elem.Process(handlers, this);

	if (strlen(caption)) {
		mCaption = caption;
	}
	if (strlen(fontName)) {
		MPString	name(fontName);
		this->SetFontName(name);
	}

	// !!! new properties
} // Read

void PhotoItemProperties::Write(XML::Output &out) const
{
	// <name>(X,Y)</name>
	out.WriteElement("alignment", AlignmentGizmo::Find(mAlignment));
	out.WriteElement("aspect", mMaintainAspect);
	out.WriteElement("fullSize", mFullSize);
	out.WriteElement("maximize", mMaximize);
	out.WriteElement("move", mCanMove);
	out.WriteElement("resize", mCanResize);
	out.WriteElement("rotate", mCanRotate);

	// Text caption
	MPString		terminated(mCaption);
	terminated += (unsigned char)'\0';
	out.WriteElement("caption", terminated.Chars());
	out.WriteElement("captionStyle", gCaptionStyles[mCaptionStyle]);
	out.WriteElement("captionLinkedRotation", mCaptionLinkedRotation);
	out.WriteElement("showDate", mShowDate);
	out.WriteElement("showName", mShowName);
	MPString		fontName;
	::GetFontName(mFontNumber, fontName);
	fontName += (unsigned char)'\0';
	out.WriteElement("fontName", fontName.Chars());
	out.WriteElement("fontSize", mFontSize);

	// Image shape
	// ShapeT			mImageShape;
	out.WriteElement("shadow", mShadow);
	// !!! RGBColor		mShadowColor;
	out.WriteElement("blurEdges", mBlurEdges);

	// Decorative frame
	// RGBColor		mFrameColor;
	// FrameT		mFrameStyle;
} // Write

/*
ParseAlignment [static]
*/
void
PhotoItemProperties::ParseAlignment(XML::Element &elem, void *userData) {
	AlignmentType* pAlignment ((AlignmentType*)userData);

	XML::Char tmp[64];
	size_t len = elem.ReadData(tmp, sizeof(tmp));
	tmp[len] = 0;
	
	*pAlignment = AlignmentGizmo::Lookup(tmp);	
}//end ParseAlignment

void	
PhotoItemProperties::ParseProperties(XML::Element &elem, void *userData)
{
	PhotoItemProperties*	props = (PhotoItemProperties*)userData;
	props->Read(elem);
}//end
