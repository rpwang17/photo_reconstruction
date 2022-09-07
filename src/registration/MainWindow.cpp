#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DialogWelcome.h"
#include "DialogSelectFolder.h"
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

#define WND_TITLE "Retrospective Registration"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow), m_nIndex(0)
{
  ui->setupUi(this);

  connect(ui->pushButtonNext, SIGNAL(clicked(bool)), SLOT(OnButtonNext()));
  connect(ui->pushButtonRegister, SIGNAL(clicked(bool)), SLOT(OnButtonRegister()));
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

  m_nNumberOfExpectedPoints = dlgSelect.IsFourPoint()?4:2;
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
    ui->pushButtonNext->setEnabled(false);
  }
  else
  {
    if (QMessageBox::question(this, "", "No more images to register. Quit the program?") == QMessageBox::Yes)
      qApp->quit();
  }
}

void MainWindow::OnButtonRegister()
{
  bool bOK;
  double val = ui->lineEditRulerLength->text().trimmed().toDouble(&bOK);
  if (ui->widgetImageView->GetNumberOfEditedPoints().size() < m_nNumberOfExpectedPoints)
    QMessageBox::warning(this, "", "Please click on the image to add another point");
  else if (!bOK || val <= 0)
    QMessageBox::warning(this, "", "Please enter a valid value for ruler length");
  else
  {
    // execute script
    ui->pushButtonNext->setEnabled(true);
  }
}

void MainWindow::OnButtonClear()
{
  ui->widgetImageView->Clear();
  ui->pushButtonNext->setEnabled(false);
}

void MainWindow::LoadImage(int n)
{
  ui->widgetImageView->LoadImage(m_listInputFiles[m_nIndex].absoluteFilePath());
}
