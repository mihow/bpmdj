/********************************************************************************
** Form generated from reading UI file 'ui-spectrum.ui'
**
** Created: Wed May 18 22:34:36 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_SPECTRUM_H
#define UI_2D_SPECTRUM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SpectrumAnalyzerForm
{
public:
    QGridLayout *gridLayout;
    QProgressBar *Progress;
    QPushButton *fetchSpectrum;
    QLabel *distri;
    QLabel *histo;

    void setupUi(QWidget *SpectrumAnalyzerForm)
    {
        if (SpectrumAnalyzerForm->objectName().isEmpty())
            SpectrumAnalyzerForm->setObjectName(QString::fromUtf8("SpectrumAnalyzerForm"));
        SpectrumAnalyzerForm->resize(594, 98);
        gridLayout = new QGridLayout(SpectrumAnalyzerForm);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        Progress = new QProgressBar(SpectrumAnalyzerForm);
        Progress->setObjectName(QString::fromUtf8("Progress"));
        Progress->setValue(0);
        Progress->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(Progress, 2, 0, 1, 1);

        fetchSpectrum = new QPushButton(SpectrumAnalyzerForm);
        fetchSpectrum->setObjectName(QString::fromUtf8("fetchSpectrum"));

        gridLayout->addWidget(fetchSpectrum, 2, 1, 1, 1);

        distri = new QLabel(SpectrumAnalyzerForm);
        distri->setObjectName(QString::fromUtf8("distri"));
        distri->setFrameShape(QFrame::Box);
        distri->setScaledContents(true);
        distri->setWordWrap(false);

        gridLayout->addWidget(distri, 1, 0, 1, 2);

        histo = new QLabel(SpectrumAnalyzerForm);
        histo->setObjectName(QString::fromUtf8("histo"));
        histo->setFrameShape(QFrame::Box);
        histo->setScaledContents(true);
        histo->setWordWrap(false);

        gridLayout->addWidget(histo, 0, 0, 1, 2);


        retranslateUi(SpectrumAnalyzerForm);

        QMetaObject::connectSlotsByName(SpectrumAnalyzerForm);
    } // setupUi

    void retranslateUi(QWidget *SpectrumAnalyzerForm)
    {
        SpectrumAnalyzerForm->setWindowTitle(QApplication::translate("SpectrumAnalyzerForm", "Form", 0, QApplication::UnicodeUTF8));
        fetchSpectrum->setText(QApplication::translate("SpectrumAnalyzerForm", "Fetch Spectrum", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SpectrumAnalyzerForm: public Ui_SpectrumAnalyzerForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_SPECTRUM_H
