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
  connect(ui->pushButtonPrev, SIGNAL(clicked(bool)), SLOT(OnButtonPrev()));
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

  UpdateIndex();

  if (!m_listInputFiles.isEmpty())
    LoadImage(m_nIndex);

  m_proc = new QProcess(this);
  connect(m_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(OnProcessOutputMessage()));
  connect(m_proc, SIGNAL(readyReadStandardError()), this, SLOT(OnProcessOutputMessage()));
  connect(m_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(OnProcessFinish()));
  connect(m_proc, SIGNAL(error(QProcess::ProcessError)),
          this, SLOT(OnProcessError(QProcess::ProcessError)));
}

MainWindow::~MainWindow()
{
  if (m_proc && m_proc->state() == QProcess::Running)
  {
    m_proc->kill();
    m_proc->waitForFinished();
  }
  delete ui;
}

void MainWindow::OnButtonNext()
{
  if (m_nIndex < m_listInputFiles.size()-1)
  {
    LoadImage(++m_nIndex);
    UpdateIndex();
  }
  else
  {
    if (QMessageBox::question(this, "", "No more images to register. Quit the program?") == QMessageBox::Yes)
      qApp->quit();
  }
}

void MainWindow::OnButtonPrev()
{
  LoadImage(--m_nIndex);
  UpdateIndex();
}

void MainWindow::OnButtonRegister()
{
  bool bOK;
  double val = ui->lineEditRulerLength->text().trimmed().toDouble(&bOK);
  if (ui->widgetImageView->GetEditedPoints().size() < m_nNumberOfExpectedPoints)
    QMessageBox::warning(this, "", "Please click on the image to add another point");
  else if (!bOK || val <= 0)
    QMessageBox::warning(this, "", "Please enter a valid value for ruler length");
  else
  {
    // execute script
    if (m_nIndex < m_listData.size())
      m_listData[m_nIndex] = ui->widgetImageView->GetEditedPoints();
    else
      m_listData << ui->widgetImageView->GetEditedPoints();
 // ui->pushButtonNext->setEnabled(true);
  }
}

void MainWindow::OnProcessError(QProcess::ProcessError)
{

}

void MainWindow::OnProcessFinished()
{

}

void MainWindow::OnProcessOutputMessage()
{

}

void MainWindow::OnButtonClear()
{
  ui->widgetImageView->Clear();
  ui->pushButtonNext->setEnabled(false);
}

void MainWindow::LoadImage(int n)
{
  QList<QPoint> pts;
  if (m_listData.size() > n)
    pts = m_listData[n];
  ui->widgetImageView->LoadImage(m_listInputFiles[n].absoluteFilePath(), "", pts);
}

void MainWindow::UpdateIndex()
{
  ui->labelIndex->setText(tr("%1 / %2").arg(m_nIndex+1).arg(m_listInputFiles.size()));
  ui->pushButtonPrev->setEnabled(m_nIndex > 0);
  ui->pushButtonNext->setEnabled(m_nIndex < m_listData.size());
}
