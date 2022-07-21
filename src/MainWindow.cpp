#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DialogWelcome.h"
#include "DialogSelectFolder.h"
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow), m_nIndex(0)
{
  ui->setupUi(this);

  connect(ui->pushButtonNext, SIGNAL(clicked(bool)), SLOT(OnButtonNext()));

  DialogWelcome dlg;
  if (dlg.exec() != QDialog::Accepted)
  {
    QTimer::singleShot(0, this, SLOT(close()));
    return;
  }

  DialogSelectFolder dlgSelect;
  if (dlgSelect.exec() != QDialog::Accepted)
  {
    QTimer::singleShot(0, this, SLOT(close()));
    return;
  }

  show();

  QString input_path = dlgSelect.GetInputPath();
  m_listInputFiles = QDir(input_path).entryInfoList(QDir::Files, QDir::Name);

  if (!m_listInputFiles.isEmpty())
    LoadImage(m_nIndex);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::OnButtonNext()
{
  if (m_nIndex < m_listInputFiles.size()-1)
  {
    LoadImage(++m_nIndex);
  }
}

void MainWindow::LoadImage(int n)
{
  ui->widgetImageView->LoadImage(m_listInputFiles[m_nIndex].absoluteFilePath());
}
