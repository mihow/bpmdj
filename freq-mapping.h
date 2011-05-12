#ifndef FREQUENCYDIALOG_H
#define FREQUENCYDIALOG_H

#include <qvariant.h>


#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_FrequencyDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *frequency_map;
    QLabel *textLabel5;

    void setupUi(QDialog *FrequencyDialog)
    {
    FrequencyDialog->setObjectName(QString::fromUtf8("FrequencyDialog"));
    gridLayout = new QGridLayout(FrequencyDialog);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    frequency_map = new QLabel(FrequencyDialog);
    frequency_map->setObjectName(QString::fromUtf8("frequency_map"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(7));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(frequency_map->sizePolicy().hasHeightForWidth());
    frequency_map->setSizePolicy(sizePolicy);
    frequency_map->setScaledContents(true);
    frequency_map->setWordWrap(false);

    gridLayout->addWidget(frequency_map, 0, 0, 1, 1);

    textLabel5 = new QLabel(FrequencyDialog);
    textLabel5->setObjectName(QString::fromUtf8("textLabel5"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(1));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(textLabel5->sizePolicy().hasHeightForWidth());
    textLabel5->setSizePolicy(sizePolicy1);
    QFont font;
    font.setPointSize(10);
    textLabel5->setFont(font);
    textLabel5->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    textLabel5->setWordWrap(true);

    gridLayout->addWidget(textLabel5, 1, 0, 1, 1);


    retranslateUi(FrequencyDialog);

    QSize size(420, 426);
    size = size.expandedTo(FrequencyDialog->minimumSizeHint());
    FrequencyDialog->resize(size);


    QMetaObject::connectSlotsByName(FrequencyDialog);
    } // setupUi

    void retranslateUi(QDialog *FrequencyDialog)
    {
    FrequencyDialog->setWindowTitle(QApplication::translate("FrequencyDialog", "Frequency Shaping", 0, QApplication::UnicodeUTF8));
    textLabel5->setText(QApplication::translate("FrequencyDialog", "This graph presents the frequencyshaping needed to map the monitor to the main song. Horizontally 24 bark frequencies, vertically dB. Every green line is 3 dB. grey bars are missing frequencies in either monitor or main", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(FrequencyDialog);
    } // retranslateUi

};

namespace Ui {
    class FrequencyDialog: public Ui_FrequencyDialog {};
} // namespace Ui

class FrequencyDialog : public QDialog, public Ui::FrequencyDialog
{
    Q_OBJECT

public:
    FrequencyDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~FrequencyDialog();

protected slots:
    virtual void languageChange();

};

#endif // FREQUENCYDIALOG_H
