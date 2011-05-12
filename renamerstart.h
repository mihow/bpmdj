#ifndef RENAMERSTART_H
#define RENAMERSTART_H

#include <qvariant.h>


#include <Qt3Support/Q3ButtonGroup>
#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_RenamerStart
{
public:
    QGridLayout *gridLayout;
    Q3ButtonGroup *buttonGroup1;
    QGridLayout *gridLayout1;
    QRadioButton *already_indexed;
    QRadioButton *not_yet_indexed;
    QLabel *textLabel2_2;
    QLabel *textLabel2;
    QSpacerItem *spacer16_2;
    QSpacerItem *spacer16;
    QSpacerItem *spacer1;
    QPushButton *pushButton22;
    QPushButton *pushButton23;

    void setupUi(QDialog *RenamerStart)
    {
    if (RenamerStart->objectName().isEmpty())
        RenamerStart->setObjectName(QString::fromUtf8("RenamerStart"));
    RenamerStart->resize(483, 384);
    gridLayout = new QGridLayout(RenamerStart);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    buttonGroup1 = new Q3ButtonGroup(RenamerStart);
    buttonGroup1->setObjectName(QString::fromUtf8("buttonGroup1"));
    buttonGroup1->setColumnLayout(0, Qt::Vertical);
    buttonGroup1->layout()->setSpacing(6);
    buttonGroup1->layout()->setMargin(11);
    gridLayout1 = new QGridLayout();
    QBoxLayout *boxlayout = qobject_cast<QBoxLayout *>(buttonGroup1->layout());
    if (boxlayout)
        boxlayout->addLayout(gridLayout1);
    gridLayout1->setAlignment(Qt::AlignTop);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    already_indexed = new QRadioButton(buttonGroup1);
    already_indexed->setObjectName(QString::fromUtf8("already_indexed"));
    already_indexed->setChecked(true);

    gridLayout1->addWidget(already_indexed, 0, 0, 1, 2);

    not_yet_indexed = new QRadioButton(buttonGroup1);
    not_yet_indexed->setObjectName(QString::fromUtf8("not_yet_indexed"));

    gridLayout1->addWidget(not_yet_indexed, 2, 0, 1, 2);

    textLabel2_2 = new QLabel(buttonGroup1);
    textLabel2_2->setObjectName(QString::fromUtf8("textLabel2_2"));
    textLabel2_2->setWordWrap(false);

    gridLayout1->addWidget(textLabel2_2, 1, 1, 1, 1);

    textLabel2 = new QLabel(buttonGroup1);
    textLabel2->setObjectName(QString::fromUtf8("textLabel2"));
    textLabel2->setWordWrap(false);

    gridLayout1->addWidget(textLabel2, 3, 1, 1, 1);

    spacer16_2 = new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

    gridLayout1->addItem(spacer16_2, 3, 0, 1, 1);

    spacer16 = new QSpacerItem(20, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

    gridLayout1->addItem(spacer16, 1, 0, 1, 1);


    gridLayout->addWidget(buttonGroup1, 0, 0, 1, 3);

    spacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacer1, 1, 0, 1, 1);

    pushButton22 = new QPushButton(RenamerStart);
    pushButton22->setObjectName(QString::fromUtf8("pushButton22"));

    gridLayout->addWidget(pushButton22, 1, 1, 1, 1);

    pushButton23 = new QPushButton(RenamerStart);
    pushButton23->setObjectName(QString::fromUtf8("pushButton23"));

    gridLayout->addWidget(pushButton23, 1, 2, 1, 1);

    QWidget::setTabOrder(pushButton22, already_indexed);
    QWidget::setTabOrder(already_indexed, not_yet_indexed);
    QWidget::setTabOrder(not_yet_indexed, pushButton23);

    retranslateUi(RenamerStart);
    QObject::connect(pushButton22, SIGNAL(clicked()), RenamerStart, SLOT(accept()));
    QObject::connect(pushButton23, SIGNAL(clicked()), RenamerStart, SLOT(reject()));

    QMetaObject::connectSlotsByName(RenamerStart);
    } // setupUi

    void retranslateUi(QDialog *RenamerStart)
    {
    RenamerStart->setWindowTitle(QApplication::translate("RenamerStart", "Start Renamer", 0, QApplication::UnicodeUTF8));
    buttonGroup1->setTitle(QApplication::translate("RenamerStart", "Which incorrect title/author/remix information should I look for ?", 0, QApplication::UnicodeUTF8));
    already_indexed->setText(QApplication::translate("RenamerStart", "already indexed songs", 0, QApplication::UnicodeUTF8));
    not_yet_indexed->setText(QApplication::translate("RenamerStart", "not yet indexed songs", 0, QApplication::UnicodeUTF8));
    textLabel2_2->setText(QApplication::translate("RenamerStart", "In this mode the renamer will look for index files that have \n"
"an inappropriate filename. Only the index filename will \n"
"change (as it should), the mp3 file will still have the same\n"
" name.", 0, QApplication::UnicodeUTF8));
    textLabel2->setText(QApplication::translate("RenamerStart", "In this mode the renamer will look for music files (.ogg,<br>\n"
".mp3, ...) that have an incorrect filename. The filename<br>\n"
"of these songs can then be changed. <b>This should <br>\n"
"<font color=\"#ff0000\">not</font> be done on songs which are already known<br>\n"
" to the program.</b> This is mainly usefull to rename songs <br>\n"
"before they are imported in bpmdj.", 0, QApplication::UnicodeUTF8));
    pushButton22->setText(QApplication::translate("RenamerStart", "Ok", 0, QApplication::UnicodeUTF8));
    pushButton23->setText(QApplication::translate("RenamerStart", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(RenamerStart);
    } // retranslateUi

};

namespace Ui {
    class RenamerStart: public Ui_RenamerStart {};
} // namespace Ui

QT_END_NAMESPACE

class RenamerStart : public QDialog, public Ui::RenamerStart
{
    Q_OBJECT

public:
    RenamerStart(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~RenamerStart();

protected slots:
    virtual void languageChange();

};

#endif // RENAMERSTART_H
