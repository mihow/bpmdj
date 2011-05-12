#include "ui-includes.h" // PCH include
#include "renamer.h"

#include <qvariant.h>
/*
 *  Constructs a Renamer as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
Renamer::Renamer(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, name, modal, fl)
{
    setupUi(this);

}

/*
 *  Destroys the object and frees any allocated resources
 */
Renamer::~Renamer()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Renamer::languageChange()
{
    retranslateUi(this);
}

void Renamer::capitalizeAfterSpace()
{
    qWarning("Renamer::capitalizeAfterSpace(): Not implemented yet");
}

void Renamer::changeSelection()
{
    qWarning("Renamer::changeSelection(): Not implemented yet");
}

void Renamer::deleteSubString()
{
    qWarning("Renamer::deleteSubString(): Not implemented yet");
}

void Renamer::ignoreSelection()
{
    qWarning("Renamer::ignoreSelection(): Not implemented yet");
}

void Renamer::realizeSelection()
{
    qWarning("Renamer::realizeSelection(): Not implemented yet");
}

void Renamer::removeFirstChar()
{
    qWarning("Renamer::removeFirstChar(): Not implemented yet");
}

void Renamer::removeSpaces()
{
    qWarning("Renamer::removeSpaces(): Not implemented yet");
}

void Renamer::removeSpecials()
{
    qWarning("Renamer::removeSpecials(): Not implemented yet");
}

void Renamer::replaceSubString()
{
    qWarning("Renamer::replaceSubString(): Not implemented yet");
}

void Renamer::smallCapsInWords()
{
    qWarning("Renamer::smallCapsInWords(): Not implemented yet");
}

void Renamer::subStringIsAuthor()
{
    qWarning("Renamer::subStringIsAuthor(): Not implemented yet");
}

void Renamer::beforeMinusIsAuthor()
{
    qWarning("Renamer::beforeMinusIsAuthor(): Not implemented yet");
}

void Renamer::substringIsMix()
{
    qWarning("Renamer::substringIsMix(): Not implemented yet");
}

void Renamer::keySelectionIsAuthor()
{
    qWarning("Renamer::keySelectionIsAuthor(): Not implemented yet");
}

void Renamer::betweenBracesIsTrash()
{
    qWarning("Renamer::betweenBracesIsTrash(): Not implemented yet");
}

void Renamer::removeFirstSpecials()
{
    qWarning("Renamer::removeFirstSpecials(): Not implemented yet");
}

void Renamer::replaceUnderscores()
{
    qWarning("Renamer::replaceUnderscores(): Not implemented yet");
}

void Renamer::fixExtention()
{
    qWarning("Renamer::fixExtention(): Not implemented yet");
}

void Renamer::betweenBracesIsMix()
{
    qWarning("Renamer::betweenBracesIsMix(): Not implemented yet");
}

