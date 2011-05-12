#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlcdnumber.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QTabWidget;
class QWidget;
class QListView;
class QListViewItem;
class QLCDNumber;
class QLineEdit;
class QLabel;
class QSpinBox;
class QPushButton;
class QCheckBox;
class QFrame;
class QVectorView;

class MainWindow : public QDialog
{
    Q_OBJECT
public:
    MainWindow( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MainWindow();
    QTabWidget* tabs;
    QWidget* tab;
    QVectorView* songList;
    QLineEdit* searchLine;
    QWidget* TabPage;
    QListView* albumList;
    QWidget* Tags;
    QListView* tagList;
    QWidget* tab_2;
    QSpinBox* extraSongs;
    QPushButton* pushButton2;
    QSpinBox* tempoWeight;
    QSpinBox* spectrumWeight;
    QCheckBox* weighDuplicateAuthors;
    QWidget* TabPage_2;
    QListView* historyList;
    QWidget* TabPage_3;
    QLabel* frequency_map;
    QFrame* frame3;
    QLCDNumber* mainLCD;
    QLabel* tempo1;
    QLabel* tempo2;
    QLabel* tempo3;
    QLabel* tempo4;
    QLabel* title1;
    QLabel* title2;
    QLabel* title3;
    QLabel* title4;

protected:
  QGridLayout* MainWindowLayout;
  QGridLayout* tabLayout;
  QGridLayout* TabPageLayout;
  QGridLayout* TagsLayout;
  QGridLayout* tabLayout_2;
  QGridLayout* TabPageLayout_2;
  QGridLayout* TabPageLayout_3;
  QGridLayout* frame3Layout;
  QVBoxLayout* layout10;
  QVBoxLayout* layout9;
  QVBoxLayout* layout11;
  
protected slots:
  virtual void languageChange();
  
};

