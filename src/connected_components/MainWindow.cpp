#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DialogWelcome.h"
#include "DialogSelectFolder.h"
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <QDesktopWidget>
#include "NumpyHelper.h"
#include "CommonDef.h"
#include <QTemporaryDir>

#define WND_TITLE "Connected Components"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow), m_nIndex(0), m_bProfiling(false)
{
  ui->setupUi(this);

  m_strPythonCmd = PY_COMMAND;

  // setup script
  static QTemporaryDir dir;
  m_strTempFolder = dir.path();
  QFile::copy(":/func_masking.py", dir.filePath("func_masking.py"));
  m_strPyScriptPath = dir.filePath("func_masking.py");

  if (!QFile::exists(m_strPyScriptPath))
  {
    QMessageBox::critical(this, "Error", "Could not locate func_masking.py script");
    qApp->quit();
  }

  m_listStockColors << QColor(255,100,100) << QColor(255,255,100) << QColor(100,255,100)
                    << QColor(110,245,255) << QColor(75,100,255) << QColor(255,128,0)
                    << QColor(100,150,170) << QColor(120,60,220);

  ui->widgetImageView->SetEditMode(WidgetImageView::EM_REGION);
  connect(ui->widgetImageView, SIGNAL(LastRegionEdited(int)),
                                      SLOT(OnLastRegionEdited(int)), Qt::QueuedConnection);

  connect(ui->pushButtonNext, SIGNAL(clicked(bool)), SLOT(OnButtonNext()));
  connect(ui->pushButtonPrev, SIGNAL(clicked(bool)), SLOT(OnButtonPrev()));
  connect(ui->pushButtonCreateMask, SIGNAL(clicked(bool)), SLOT(OnButtonCreateMask()));
  connect(ui->pushButtonClear, SIGNAL(clicked(bool)), SLOT(OnButtonClear()));

  setWindowTitle(WND_TITLE);
  QSettings s;
  QRect rc = s.value("MainWindow/Geometry").toRect();
  if (rc.isValid() && QApplication::desktop()->screenGeometry(this).contains(rc))
    setGeometry(rc);

  m_proc = new QProcess(this);
  connect(m_proc, SIGNAL(started()), SLOT(OnProcessStarted()));
  connect(m_proc, SIGNAL(readyReadStandardOutput()), SLOT(OnProcessOutputMessage()));
  connect(m_proc, SIGNAL(readyReadStandardError()), SLOT(OnProcessOutputMessage()));
  connect(m_proc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(OnProcessFinished()));
  connect(m_proc, SIGNAL(error(QProcess::ProcessError)), SLOT(OnProcessError(QProcess::ProcessError)));
}

MainWindow::~MainWindow()
{
  QSettings s;
  s.setValue("MainWindow/Geometry", geometry());

  if (m_proc && m_proc->state() == QProcess::Running)
  {
//    m_proc->kill();
    m_proc->waitForFinished();
  }
  delete ui;
}

void MainWindow::ShowDialog()
{
  hide();
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

  m_strOutputFolder = dlgSelect.GetOutputPath();

  if (!m_listInputFiles.isEmpty() && !m_listMaskFiles.isEmpty())
    LoadImage(m_nIndex);

  UpdateIndex();
}

void MainWindow::OnButtonNext()
{
  if (m_nIndex < m_listInputFiles.size()-1)
  {
    LoadImage(++m_nIndex);
    UpdateIndex();
    ui->pushButtonNext->setEnabled(false);
  }
  else
  {
    if (QMessageBox::question(this, "", "No more images to process. Quit the program?") == QMessageBox::Yes)
      qApp->quit();
  }
}

void MainWindow::OnButtonPrev()
{
  LoadImage(--m_nIndex);
  UpdateIndex();
}

void MainWindow::OnButtonCreateMask()
{
  // execute script here
  QList<RECT_REGION> list = ui->widgetImageView->GetEditedRegions();
  if (m_nIndex < m_listData.size())
    m_listData[m_nIndex] = list;
  else
    m_listData << list;

  m_proc->setProperty("region_index", -1);
  CreateComponents(list);
}

void MainWindow::CreateComponents(const QList<RECT_REGION>& rects, bool bTemp)
{
  QStringList strList;
  foreach (RECT_REGION rc, rects)
    strList << QString::number(rc.first.x()) << QString::number(rc.first.y())
            << QString::number(rc.second.x()) << QString::number(rc.second.y());
  QStringList cmd;
  cmd << m_strPythonCmd << m_strPyScriptPath
       << "--in_img" << ui->widgetImageView->GetFilename()
       << "--rectangle_coordinates" << strList.join(" ")
       << "--in_mask" << ui->widgetImageView->GetMaskFilename()
       << "--out_dir" << (bTemp?m_strTempFolder:m_strOutputFolder);
  m_proc->start(cmd.join(" "));
}

void MainWindow::OnLastRegionEdited(int n)
{
  m_proc->setProperty("region_index", n);
  QList<RECT_REGION> list;
  list << ui->widgetImageView->GetEditedRegions().last();
  ui->pushButtonNext->setEnabled(false);
  ui->widgetImageView->setEnabled(false);
  CreateComponents(list, true);
}

void MainWindow::OnButtonClear()
{
  ui->widgetImageView->Clear();
  ui->pushButtonNext->setEnabled(false);
}

void MainWindow::LoadImage(int n)
{
  QList<RECT_REGION> rects;
  if (m_listData.size() > n)
    rects = m_listData[n];
  ui->widgetImageView->LoadImage(m_listInputFiles[n].absoluteFilePath(), m_listMaskFiles[n].absoluteFilePath(),
                                 QList<QPoint>(), rects);
  if (!rects.isEmpty())
    OnButtonCreateMask();
}
void MainWindow::OnProcessError(QProcess::ProcessError)
{
  QMessageBox::warning(this, "Error", "Error occurred during process.");
}

void MainWindow::OnProcessStarted()
{
  ui->widgetImageView->ShowMessage("Processing...");
  ui->pushButtonCreateMask->setEnabled(false);
  if (m_bProfiling)
    m_timer.start();
}

void MainWindow::OnProcessFinished()
{
  ui->widgetImageView->setEnabled(true);
  if (m_bProfiling)
  {
    qDebug() << "Elapsed time by py script: " << m_timer.elapsed() << "ms";
    m_timer.restart();
  }

  int region_n = m_proc->property("region_index").toInt();
  QString fn = QFileInfo(ui->widgetImageView->GetFilename()).fileName();
  fn.replace(".JPG", "_mask.npy", Qt::CaseInsensitive);
  QImage image;
  if (region_n < 0)
  {
    image = NumpyHelper::NumpyToImage(QFileInfo(m_strOutputFolder, fn).absoluteFilePath(),
                                             m_listStockColors);
    ui->pushButtonNext->setEnabled(true);
  }
  else
  {
    image = NumpyHelper::NumpyToImage(QFileInfo(m_strTempFolder, fn).absoluteFilePath(),
                                             m_listStockColors[region_n%m_listStockColors.size()]);
  }
  if (!image.isNull())
    ui->widgetImageView->AddOverlay(image);

  ui->widgetImageView->HideMessage();
  ui->pushButtonCreateMask->setEnabled(true);
  if (m_bProfiling)
  {
    qDebug() << "Elapsed time by image processing: " << m_timer.elapsed() << "ms";
    m_timer.invalidate();
  }
}

void MainWindow::OnProcessOutputMessage()
{
  qDebug() << m_proc->readAllStandardError();
  qDebug() << m_proc->readAllStandardOutput();
}

void MainWindow::UpdateIndex()
{
  ui->labelIndex->setText(tr("%1 / %2").arg(m_nIndex+1).arg(m_listInputFiles.size()));
  ui->pushButtonPrev->setEnabled(m_nIndex > 0 && m_proc->state() != QProcess::Running);
  ui->pushButtonNext->setEnabled(m_nIndex < m_listData.size());
}
