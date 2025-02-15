/*
File:		FloatEditor.java

Contains:	Interface for a string value editor in an AltoStratus strip.

Written by:	David Dunham
		Electric Fish, Inc. <http://www.electricfish.com>

Copyright:	Copyright �2001 by Applied Microsystems Corporation.  All Rights reserved.

Change History (most recent first):

	05 Apr 2001		drd	Changes for packages
	07 Mar 2001		drd	Set fEditingParameter
	07 Mar 2001		drd	Created
*/

package amc.altostratus;

import amc.altostratus.constants.ParamConstants;
import java.awt.*;				// AWT classes
import java.awt.event.*;			// AWT events
import javax.swing.*;				// Swing components and classes
import javax.swing.text.*;

class FloatEditor extends StringEditor implements ParamConstants
{
	public FloatEditor(Strip aStrip, Parameter aParameter) {
		super(aParameter.getValueString(aStrip.getFormat(), radix_Decimal));
		fEditingParameter = true;
	} // FloatEditor

	/**
	Creates the default implementation of the model to be used at construction if one isn't
	explicitly given.
		@return the default model implementation
	*/
	protected Document createDefaultModel() {
		return new FloatDocument();
	}

	/**
	Determine whether or not the current text is a valid integer
		@return true if it's valid, otherwise false
	*/
	public boolean	validateCurrentValue() {
		boolean		returnVal = false;
		try {
			Double		value = Double.valueOf(this.getText());
			returnVal = true;
		} catch (Exception e) {
		}
		return returnVal;
	} // validateCurrentValue

	/**
	This inner class represents the data in the field; it's where we validate typing (and
	Paste)
	*/
	protected class FloatDocument extends StringEditor.StringDocument
	{
		/**
		Inserts some content into the document.
			@param anOffset the starting offset >= 0
			@param aString the string to insert; does nothing with null/empty strings
			@param anAttr the attributes for the inserted content
			@exception BadLocationException  anOffset is not a valid position within the document
			@see Document#insertString
		*/
		public void insertString(int anOffset, String aString, AttributeSet anAttr)
				throws BadLocationException {
			for (int i = 0; i < aString.length(); i++) {
				char	c = aString.charAt(i);
				if (!Character.isDigit(c) && "-.".indexOf(c) < 0) {		// 1.3E4 ???
					Toolkit.getDefaultToolkit().beep();
					return;
				}
			} // end for
			super.insertString(anOffset, aString, anAttr);
		}
	}
}
