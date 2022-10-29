#ifndef NUMPYHELPER_H
#define NUMPYHELPER_H

#include <QString>
#include <QColor>
#include <QImage>

class NumpyHelper
{
public:
  NumpyHelper();

  static QImage NumpyToImage(const QString& npy_in, const QColor& replace_color);
  static QImage NumpyToImage(const QString& npy_in, const QList<QColor>& color_list);
};

#endif // NUMPYHELPER_H
