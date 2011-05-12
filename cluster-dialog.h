#ifndef CLUSTERDIALOG_H
#define CLUSTERDIALOG_H

#include <qvariant.h>


#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include "metric-widget.h"

class Ui_ClusterDialog
{
public:
    QGridLayout *gridLayout;
    QFrame *line4;
    QSpacerItem *spacerItem;
    QPushButton *pushButton27;
    QPushButton *pushButton28;
    MetricWidget *metrics;
    QLabel *textLabel1;

    void setupUi(QDialog *ClusterDialog)
    {
    ClusterDialog->setObjectName(QString::fromUtf8("ClusterDialog"));
    gridLayout = new QGridLayout(ClusterDialog);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(11);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    line4 = new QFrame(ClusterDialog);
    line4->setObjectName(QString::fromUtf8("line4"));
    line4->setFrameShape(QFrame::HLine);
    line4->setFrameShadow(QFrame::Sunken);

    gridLayout->addWidget(line4, 2, 0, 2, 3);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacerItem, 4, 0, 1, 1);

    pushButton27 = new QPushButton(ClusterDialog);
    pushButton27->setObjectName(QString::fromUtf8("pushButton27"));

    gridLayout->addWidget(pushButton27, 3, 1, 2, 1);

    pushButton28 = new QPushButton(ClusterDialog);
    pushButton28->setObjectName(QString::fromUtf8("pushButton28"));

    gridLayout->addWidget(pushButton28, 3, 2, 2, 1);

    metrics = new MetricWidget(ClusterDialog);
    metrics->setObjectName(QString::fromUtf8("metrics"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(3), static_cast<QSizePolicy::Policy>(3));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(metrics->sizePolicy().hasHeightForWidth());
    metrics->setSizePolicy(sizePolicy);

    gridLayout->addWidget(metrics, 1, 0, 1, 3);

    textLabel1 = new QLabel(ClusterDialog);
    textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
    textLabel1->setAlignment(Qt::AlignVCenter);
    textLabel1->setWordWrap(true);

    gridLayout->addWidget(textLabel1, 0, 0, 1, 3);


    retranslateUi(ClusterDialog);

    QSize size(382, 400);
    size = size.expandedTo(ClusterDialog->minimumSizeHint());
    ClusterDialog->resize(size);

    QObject::connect(pushButton27, SIGNAL(clicked()), ClusterDialog, SLOT(accept()));
    QObject::connect(pushButton28, SIGNAL(clicked()), ClusterDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(ClusterDialog);
    } // setupUi

    void retranslateUi(QDialog *ClusterDialog)
    {
    ClusterDialog->setWindowTitle(QApplication::translate("ClusterDialog", "Cluster Metrics", 0, QApplication::UnicodeUTF8));
    pushButton27->setText(QApplication::translate("ClusterDialog", "Start Clustering", 0, QApplication::UnicodeUTF8));
    pushButton28->setText(QApplication::translate("ClusterDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    textLabel1->setText(QApplication::translate("ClusterDialog", "The clustering only works elements with all information (rythm, composition, echo, ...)", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(ClusterDialog);
    } // retranslateUi


protected:
    enum IconID
    {
        image0_ID,
        unknown_ID
    };
    static QPixmap icon(IconID id)
    {
    static const unsigned char image0_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x03,
    0xb1, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xad, 0x94, 0x5f, 0x4c, 0x5b,
    0x55, 0x1c, 0xc7, 0x3f, 0xe7, 0xdc, 0x4b, 0x7b, 0x4b, 0xcb, 0xa0, 0x76,
    0x2d, 0x45, 0xc4, 0x31, 0x14, 0x32, 0x35, 0x99, 0xee, 0x61, 0x92, 0x60,
    0x9c, 0x51, 0xd8, 0x83, 0x89, 0x2c, 0xe0, 0x83, 0xf1, 0x71, 0x8b, 0x3e,
    0xbb, 0x18, 0x5f, 0x8d, 0xc9, 0x1e, 0x97, 0x2c, 0xf3, 0x9d, 0x2d, 0x2a,
    0x2f, 0x1a, 0xf6, 0x64, 0xb6, 0xf1, 0xe0, 0xfe, 0x38, 0x63, 0xa0, 0x71,
    0x89, 0x69, 0x70, 0x0e, 0xb0, 0xc5, 0x21, 0x42, 0xda, 0x02, 0x42, 0xa1,
    0xbd, 0x6d, 0x69, 0x6f, 0x7b, 0xcf, 0xf1, 0x01, 0x58, 0x52, 0x03, 0xce,
    0xb8, 0xfd, 0x9e, 0xce, 0x39, 0xf9, 0x9d, 0xcf, 0xf9, 0xfe, 0x7e, 0xdf,
    0x93, 0x9f, 0x88, 0xc5, 0x62, 0xec, 0x46, 0x5f, 0x5f, 0x9f, 0xe6, 0x09,
    0x44, 0x2c, 0x16, 0x13, 0x62, 0x17, 0x3c, 0x3e, 0x3e, 0xae, 0x7b, 0x7a,
    0x7a, 0xc8, 0xe7, 0xf3, 0x64, 0x32, 0x19, 0x6c, 0xdb, 0xfe, 0x4f, 0x90,
    0x7c, 0x3e, 0x4f, 0x22, 0x91, 0x20, 0x1e, 0x8f, 0xe3, 0x38, 0xce, 0xc3,
    0x73, 0x73, 0x57, 0xe9, 0xe8, 0xe8, 0x28, 0xad, 0xed, 0x1e, 0x22, 0x9d,
    0x65, 0x0e, 0x1f, 0x2d, 0x51, 0x75, 0x4b, 0x08, 0x21, 0x10, 0x12, 0x4c,
    0x13, 0xa4, 0xdc, 0x5e, 0xa3, 0x41, 0xeb, 0x9d, 0xc2, 0x84, 0xc0, 0x55,
    0x8a, 0x85, 0x3f, 0x3c, 0xe8, 0xcb, 0x06, 0x77, 0xef, 0x50, 0x0f, 0xde,
    0x7d, 0x39, 0xd2, 0x59, 0x66, 0x79, 0xe3, 0x26, 0x59, 0x7b, 0x8e, 0x9a,
    0xbb, 0x85, 0xd7, 0xa7, 0x70, 0xab, 0x16, 0xab, 0x69, 0x83, 0xec, 0x5a,
    0x05, 0xb4, 0x87, 0xa7, 0x0e, 0x36, 0xf0, 0xf4, 0xe1, 0x0a, 0xfe, 0x40,
    0x8d, 0x6a, 0x55, 0x83, 0x10, 0x98, 0x01, 0x08, 0x47, 0x8d, 0xba, 0x4a,
    0x1e, 0x82, 0x33, 0x99, 0x0c, 0x5d, 0x47, 0x4b, 0x64, 0xed, 0x39, 0xaa,
    0x3a, 0x8f, 0xe5, 0xf3, 0x31, 0x7f, 0xbf, 0x81, 0xdb, 0xd7, 0x0b, 0xcc,
    0xfe, 0xb2, 0x05, 0x94, 0x76, 0x32, 0xfd, 0x74, 0xf6, 0x58, 0x9c, 0x1c,
    0x6e, 0xa2, 0xf7, 0x0d, 0x03, 0xa5, 0x8b, 0xa0, 0xc1, 0xf2, 0xc9, 0xbd,
    0xc1, 0xb6, 0x6d, 0xe3, 0xb8, 0x25, 0x6a, 0x6e, 0x11, 0xcb, 0xe7, 0xe3,
    0xee, 0x1d, 0x18, 0x1b, 0x59, 0x01, 0x24, 0xfe, 0x26, 0x93, 0xae, 0xee,
    0x08, 0xd2, 0x23, 0xc8, 0x2c, 0x3a, 0x2c, 0x24, 0xb7, 0xb8, 0x74, 0xbe,
    0xc8, 0xf2, 0x62, 0x88, 0x77, 0x4f, 0x7b, 0xd0, 0x54, 0x50, 0x4a, 0xef,
    0x0d, 0xde, 0x6e, 0x99, 0xc0, 0xeb, 0x83, 0xf9, 0xfb, 0x26, 0x63, 0x23,
    0xcb, 0x80, 0xa2, 0x7f, 0xb0, 0x95, 0x81, 0x41, 0x83, 0x60, 0x44, 0x60,
    0x5a, 0x2e, 0xe5, 0x52, 0x33, 0x93, 0x37, 0x1c, 0xc6, 0x46, 0xfe, 0xe2,
    0xda, 0x37, 0x19, 0x5a, 0xa3, 0x5d, 0x0c, 0x0c, 0x5b, 0x48, 0x59, 0xac,
    0x03, 0xd7, 0xe9, 0x17, 0x12, 0xdc, 0xaa, 0xc5, 0xad, 0xab, 0x36, 0x50,
    0xa3, 0xff, 0x9d, 0x08, 0xef, 0x7d, 0xd8, 0x40, 0x4b, 0xb4, 0x88, 0x69,
    0xe5, 0xd9, 0x58, 0xb5, 0xb8, 0x7d, 0xad, 0xc4, 0xc9, 0xa1, 0x46, 0x4e,
    0x9f, 0x0d, 0x03, 0x26, 0xdf, 0x7e, 0xbd, 0x86, 0xbd, 0x2e, 0x31, 0x0c,
    0xb1, 0xbf, 0x62, 0xd3, 0x84, 0xd5, 0x94, 0x41, 0xf2, 0xd7, 0x0a, 0x07,
    0x0e, 0x1c, 0x60, 0xe0, 0x94, 0x89, 0x92, 0x9b, 0x98, 0x86, 0x81, 0xbd,
    0xd9, 0xcc, 0x97, 0x17, 0x0b, 0xcc, 0x4c, 0x65, 0x59, 0x5f, 0x71, 0xf8,
    0xe0, 0xe3, 0x30, 0x13, 0x37, 0xfc, 0x24, 0xa7, 0x73, 0x24, 0x67, 0x83,
    0x18, 0x75, 0xa4, 0x7f, 0x28, 0x96, 0x52, 0xb0, 0xbe, 0x5e, 0x41, 0x51,
    0xe6, 0xd0, 0x73, 0x16, 0xc1, 0xb0, 0xc0, 0x34, 0x0d, 0x8a, 0xb9, 0x20,
    0x23, 0xe7, 0x6d, 0x66, 0xa6, 0xb2, 0x84, 0x0e, 0x36, 0xd2, 0xfb, 0x7a,
    0x13, 0xfe, 0xe6, 0x32, 0x47, 0x5e, 0x6e, 0xdc, 0x36, 0x7e, 0xa9, 0x8a,
    0x90, 0xfb, 0x98, 0xb7, 0xdd, 0x63, 0x10, 0xca, 0x03, 0x28, 0xa4, 0x47,
    0x61, 0x58, 0x2e, 0x1b, 0xab, 0x41, 0xbe, 0xfa, 0xbc, 0xc0, 0xec, 0x94,
    0x0d, 0x28, 0xbc, 0x5e, 0x93, 0xb6, 0x8e, 0x06, 0x4c, 0xab, 0x84, 0xe1,
    0xd9, 0xbe, 0xae, 0xb4, 0x89, 0x34, 0xea, 0xc1, 0xf5, 0x3b, 0x01, 0xc1,
    0xb0, 0x00, 0xfc, 0xa4, 0x96, 0xaa, 0x54, 0xb6, 0x1a, 0x99, 0xfc, 0xde,
    0x61, 0x66, 0x2a, 0x4b, 0x4b, 0x4b, 0x23, 0xd1, 0x68, 0x0b, 0xe9, 0xd4,
    0x26, 0x17, 0x3e, 0x4d, 0xf3, 0x60, 0x2a, 0x48, 0xea, 0x41, 0x19, 0x89,
    0xa4, 0xfd, 0x59, 0x0f, 0x52, 0xa8, 0xfd, 0x15, 0x6b, 0x05, 0xed, 0x9d,
    0x35, 0x0e, 0x3d, 0xef, 0xe3, 0xcf, 0xdf, 0x4b, 0x4c, 0x7c, 0xe7, 0x70,
    0xea, 0x7d, 0x3f, 0xb9, 0xf5, 0x20, 0xaf, 0xbe, 0x16, 0x20, 0xda, 0xee,
    0xe5, 0xc2, 0x67, 0x35, 0xd2, 0x0b, 0x9b, 0x9c, 0xfb, 0x68, 0x11, 0xa7,
    0x54, 0xa4, 0x39, 0xe4, 0xe5, 0xc5, 0x63, 0x9a, 0x4c, 0xaa, 0x1e, 0x5c,
    0xaf, 0x58, 0x6b, 0xfc, 0x01, 0x97, 0xb7, 0x87, 0x02, 0x00, 0x5c, 0xb9,
    0xb4, 0xca, 0xe4, 0xcd, 0x32, 0x67, 0xce, 0x86, 0x38, 0xfe, 0x96, 0x4b,
    0xd7, 0x2b, 0x39, 0x3e, 0x39, 0xd7, 0x49, 0x28, 0xd2, 0x42, 0x61, 0x73,
    0x03, 0xa7, 0x56, 0xa3, 0x7f, 0x28, 0x44, 0x47, 0x77, 0x0d, 0xcd, 0xbf,
    0xfc, 0x63, 0x84, 0xa0, 0x5a, 0x85, 0xde, 0x13, 0x06, 0xe9, 0xa5, 0x20,
    0x57, 0xc7, 0x52, 0x7c, 0x71, 0x31, 0xc3, 0xc4, 0x2d, 0x3f, 0x47, 0x5e,
    0xb2, 0x30, 0x3c, 0x82, 0xd4, 0xfc, 0x32, 0x76, 0xd6, 0x81, 0x1d, 0xb3,
    0x7e, 0xfe, 0xb1, 0x48, 0x72, 0x30, 0x82, 0xd9, 0xb0, 0x8f, 0x79, 0xf9,
    0x7c, 0x1e, 0x57, 0x29, 0x10, 0x9a, 0x9a, 0x28, 0x30, 0x7c, 0xc6, 0x22,
    0xd2, 0xd6, 0xcd, 0xf5, 0x2b, 0x2b, 0x24, 0xef, 0xe5, 0x48, 0xde, 0xdb,
    0x00, 0x14, 0x20, 0x09, 0x86, 0x2c, 0xfa, 0x87, 0x9e, 0xe1, 0xa7, 0x1f,
    0x6c, 0xec, 0x92, 0x83, 0x53, 0x51, 0x14, 0x72, 0xfb, 0xf4, 0x38, 0x91,
    0x48, 0xb0, 0xb0, 0xe0, 0xc1, 0x08, 0x08, 0x14, 0x12, 0x45, 0x8d, 0xfe,
    0x21, 0xc9, 0xf1, 0x13, 0x51, 0xe6, 0x7e, 0x73, 0x58, 0x59, 0x54, 0x68,
    0x24, 0x6d, 0x1d, 0x1e, 0x5e, 0x38, 0xa6, 0xe9, 0xe8, 0x51, 0xbc, 0x39,
    0xd8, 0x4a, 0xb5, 0x02, 0xa6, 0x69, 0x33, 0x1d, 0xaf, 0xec, 0x0d, 0x8e,
    0xc7, 0xe3, 0xe8, 0xcb, 0x06, 0xe1, 0xa8, 0x81, 0xd7, 0x27, 0xd1, 0xae,
    0x40, 0x18, 0x0a, 0x43, 0x4a, 0x8c, 0x9d, 0xb1, 0x29, 0xa5, 0x41, 0x26,
    0xad, 0x48, 0xa7, 0x35, 0x8c, 0x2b, 0x4c, 0x53, 0x52, 0xc8, 0xc3, 0x74,
    0xbc, 0xcc, 0xda, 0xb2, 0xbb, 0x37, 0xd8, 0x71, 0x9c, 0xba, 0x79, 0xfa,
    0xb8, 0x21, 0x1f, 0x9d, 0xf2, 0x18, 0xe0, 0x58, 0x2c, 0x26, 0x1e, 0x95,
    0xf8, 0xbf, 0xc0, 0x4f, 0x1a, 0x1e, 0x8b, 0xc5, 0xc4, 0xdf, 0xe0, 0xa1,
    0x90, 0x17, 0x2a, 0xf6, 0xa9, 0x69, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
    0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

    switch (id) {
        case image0_ID:  { QImage img; img.loadFromData(image0_data, sizeof(image0_data), "PNG"); return QPixmap::fromImage(img); }
        default: return QPixmap();
    } // switch
    } // icon

};

namespace Ui {
    class ClusterDialog: public Ui_ClusterDialog {};
} // namespace Ui

class ClusterDialog : public QDialog, public Ui::ClusterDialog
{
    Q_OBJECT

public:
    ClusterDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~ClusterDialog();

protected slots:
    virtual void languageChange();

};

#endif // CLUSTERDIALOG_H
