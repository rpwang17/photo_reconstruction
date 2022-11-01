#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  bool bProfiling = (argc > 1 && QString(argv[1]) == "-prof");
  MainWindow w(NULL, bProfiling);
  w.ShowDialog();
  return a.exec();
}
