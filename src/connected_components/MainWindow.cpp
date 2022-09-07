#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DialogWelcome.h"
#include "DialogSelectFolder.h"
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

#define WND_TITLE "Connected Components"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow), m_nIndex(0)
{
  ui->setupUi(this);

  ui->widgetImageView->SetEditMode(WidgetImageView::EM_REGION);

  connect(ui->pushButtonNext, SIGNAL(clicked(bool)), SLOT(OnButtonNext()));
  connect(ui->pushButtonCreateMask, SIGNAL(clicked(bool)), SLOT(OnButtonCreateMask()));
  connect(ui->pushButtonClear, SIGNAL(clicked(bool)), SLOT(OnButtonClear()));

  setWindowTitle(WND_TITLE);

  DialogWelcome dlg;
  dlg.setWindowTitle(WND_TITLE);
  if (dlg.exec() != QDialog::Accepted)
  {
    QTimer::singleShot(0, this, SLOT(close()));
    return;
  }

  DialogSelectFolder dlgSelect;
  dlgSelect.setWindowTitle(WND_TITLE);
  if (dlgSelect.exec() != QDialog::Accepted)
  {
    QTimer::singleShot(0, this, SLOT(close()));
    return;
  }

  show();

  QString path = dlgSelect.GetInputPath();
  m_listInputFiles = QDir(path).entryInfoList(QDir::Files, QDir::Name);

  path = dlgSelect.GetMaskPath();
  m_listMaskFiles = QDir(path).entryInfoList(QDir::Files, QDir::Name);

  if (!m_listInputFiles.isEmpty() && !m_listMaskFiles.isEmpty())
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
    ui->pushButtonNext->setEnabled(false);
  }
  else
  {
    if (QMessageBox::question(this, "", "No more images to process. Quit the program?") == QMessageBox::Yes)
      qApp->quit();
  }
}

void MainWindow::OnButtonCreateMask()
{
  // execute script here
  ui->pushButtonNext->setEnabled(true);
}

void MainWindow::OnButtonClear()
{
  ui->widgetImageView->Clear();
  ui->pushButtonNext->setEnabled(false);
}

void MainWindow::LoadImage(int n)
{
  ui->widgetImageView->LoadImage(m_listInputFiles[n].absoluteFilePath(), m_listMaskFiles[n].absoluteFilePath());
}
