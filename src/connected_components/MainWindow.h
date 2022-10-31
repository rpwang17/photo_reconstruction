#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfoList>
#include <QProcess>
#include "WidgetImageView.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public slots:
  void OnButtonNext();
  void OnButtonPrev();
  void OnButtonCreateMask();
  void OnButtonClear();
  void LoadImage(int n);
  void ShowDialog();

private slots:
  void OnProcessOutputMessage();
  void OnProcessFinished();
  void OnProcessError(QProcess::ProcessError);
  void OnLastRegionEdited(int n);

private:
  void UpdateIndex();
  void CreateComponents(const QList<RECT_REGION>& rects, bool bTemp = false);

  Ui::MainWindow *ui;
  QFileInfoList  m_listInputFiles;
  QFileInfoList  m_listMaskFiles;
  QString m_strOutputFolder;
  int m_nIndex;
  QList< QList<RECT_REGION> > m_listData;

  QList<QColor> m_listStockColors;
  QString   m_strPyScriptPath;

  QProcess* m_proc;
  QString   m_strTempFolder;
};
#endif // MAINWINDOW_H
