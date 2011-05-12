/********************************************************************************
** Form generated from reading UI file 'ui-metrics.ui'
**
** Created: Thu May 12 15:19:16 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_2D_METRICS_H
#define UI_2D_METRICS_H

#include <Qt3Support/Q3MimeSourceFactory>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MetricWidget
{
public:
    QGridLayout *gridLayout;
    QLabel *textLabel2_2_2;
    QLabel *textLabel5;
    QSpinBox *spectrumSpin;
    QSpinBox *tempoDistanceSpin;
    QSpinBox *echoSpin;
    QLabel *textLabel1;
    QLabel *textLabel2_2_2_2;
    QSpinBox *compositionSpin;
    QLabel *textLabel2_2;
    QSpinBox *rhythmSpin;
    QLabel *textLabel2_2_3;

    void setupUi(QWidget *MetricWidget)
    {
        if (MetricWidget->objectName().isEmpty())
            MetricWidget->setObjectName(QString::fromUtf8("MetricWidget"));
        MetricWidget->resize(266, 281);
        gridLayout = new QGridLayout(MetricWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        textLabel2_2_2 = new QLabel(MetricWidget);
        textLabel2_2_2->setObjectName(QString::fromUtf8("textLabel2_2_2"));
        textLabel2_2_2->setWordWrap(false);

        gridLayout->addWidget(textLabel2_2_2, 5, 0, 1, 1);

        textLabel5 = new QLabel(MetricWidget);
        textLabel5->setObjectName(QString::fromUtf8("textLabel5"));
        textLabel5->setWordWrap(false);

        gridLayout->addWidget(textLabel5, 9, 0, 1, 1);

        spectrumSpin = new QSpinBox(MetricWidget);
        spectrumSpin->setObjectName(QString::fromUtf8("spectrumSpin"));
        spectrumSpin->setMinimum(0);
        spectrumSpin->setMaximum(1000);
        spectrumSpin->setSingleStep(10);
        spectrumSpin->setValue(100);

        gridLayout->addWidget(spectrumSpin, 4, 0, 1, 1);

        tempoDistanceSpin = new QSpinBox(MetricWidget);
        tempoDistanceSpin->setObjectName(QString::fromUtf8("tempoDistanceSpin"));
        tempoDistanceSpin->setMinimum(0);
        tempoDistanceSpin->setMaximum(1000);
        tempoDistanceSpin->setSingleStep(10);
        tempoDistanceSpin->setValue(100);

        gridLayout->addWidget(tempoDistanceSpin, 2, 0, 1, 1);

        echoSpin = new QSpinBox(MetricWidget);
        echoSpin->setObjectName(QString::fromUtf8("echoSpin"));
        echoSpin->setMinimum(0);
        echoSpin->setMaximum(1000);
        echoSpin->setSingleStep(10);
        echoSpin->setValue(100);

        gridLayout->addWidget(echoSpin, 6, 0, 1, 1);

        textLabel1 = new QLabel(MetricWidget);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setWordWrap(false);

        gridLayout->addWidget(textLabel1, 0, 0, 1, 1);

        textLabel2_2_2_2 = new QLabel(MetricWidget);
        textLabel2_2_2_2->setObjectName(QString::fromUtf8("textLabel2_2_2_2"));
        textLabel2_2_2_2->setWordWrap(false);

        gridLayout->addWidget(textLabel2_2_2_2, 7, 0, 1, 1);

        compositionSpin = new QSpinBox(MetricWidget);
        compositionSpin->setObjectName(QString::fromUtf8("compositionSpin"));
        compositionSpin->setMinimum(0);
        compositionSpin->setMaximum(1000);
        compositionSpin->setSingleStep(10);
        compositionSpin->setValue(0);

        gridLayout->addWidget(compositionSpin, 10, 0, 1, 1);

        textLabel2_2 = new QLabel(MetricWidget);
        textLabel2_2->setObjectName(QString::fromUtf8("textLabel2_2"));
        textLabel2_2->setWordWrap(false);

        gridLayout->addWidget(textLabel2_2, 3, 0, 1, 1);

        rhythmSpin = new QSpinBox(MetricWidget);
        rhythmSpin->setObjectName(QString::fromUtf8("rhythmSpin"));
        rhythmSpin->setMinimum(0);
        rhythmSpin->setMaximum(1000);
        rhythmSpin->setSingleStep(10);
        rhythmSpin->setValue(100);

        gridLayout->addWidget(rhythmSpin, 8, 0, 1, 1);

        textLabel2_2_3 = new QLabel(MetricWidget);
        textLabel2_2_3->setObjectName(QString::fromUtf8("textLabel2_2_3"));
        textLabel2_2_3->setWordWrap(false);

        gridLayout->addWidget(textLabel2_2_3, 1, 0, 1, 1);

        QWidget::setTabOrder(tempoDistanceSpin, spectrumSpin);
        QWidget::setTabOrder(spectrumSpin, echoSpin);
        QWidget::setTabOrder(echoSpin, rhythmSpin);
        QWidget::setTabOrder(rhythmSpin, compositionSpin);

        retranslateUi(MetricWidget);

        QMetaObject::connectSlotsByName(MetricWidget);
    } // setupUi

    void retranslateUi(QWidget *MetricWidget)
    {
        MetricWidget->setWindowTitle(QApplication::translate("MetricWidget", "Form2", 0, QApplication::UnicodeUTF8));
        textLabel2_2_2->setText(QApplication::translate("MetricWidget", "Echo weight (default = 62)", 0, QApplication::UnicodeUTF8));
        textLabel5->setText(QApplication::translate("MetricWidget", "Composition weight (default = 33)", 0, QApplication::UnicodeUTF8));
        textLabel1->setText(QApplication::translate("MetricWidget", "Specify the weights of the different metrics.", 0, QApplication::UnicodeUTF8));
        textLabel2_2_2_2->setText(QApplication::translate("MetricWidget", "Rhythm weight (default = 26)", 0, QApplication::UnicodeUTF8));
        textLabel2_2->setText(QApplication::translate("MetricWidget", "Spectrum weight (default = 11)", 0, QApplication::UnicodeUTF8));
        textLabel2_2_3->setText(QApplication::translate("MetricWidget", "Tempo weight (default = 2)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MetricWidget: public Ui_MetricWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_2D_METRICS_H
