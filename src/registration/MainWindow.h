#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfoList>

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
  void OnButtonRegister();
  void OnButtonClear();
  void LoadImage(int n);

private:
  void UpdateIndex();

  Ui::MainWindow *ui;
  QFileInfoList  m_listInputFiles;
  int m_nNumberOfExpectedPoints;
  int m_nIndex;
  QList< QList<QPoint> > m_listData;
};
#endif // MAINWINDOW_H
