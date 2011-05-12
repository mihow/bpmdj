#include "ui-includes.h" // PCH include
#include "similars.h"

#include <qvariant.h>
/*
 *  Constructs a Similars as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
Similars::Similars(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, name, modal, fl)
{
    setupUi(this);

}

/*
 *  Destroys the object and frees any allocated resources
 */
Similars::~Similars()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Similars::languageChange()
{
    retranslateUi(this);
}

void Similars::startStop()
{
    qWarning("Similars::startStop(): Not implemented yet");
}

void Similars::goFetch()
{
    qWarning("Similars::goFetch(): Not implemented yet");
}

void Similars::ignore()
{
    qWarning("Similars::ignore(): Not implemented yet");
}

void Similars::alreadyHave()
{
    qWarning("Similars::alreadyHave(): Not implemented yet");
}

