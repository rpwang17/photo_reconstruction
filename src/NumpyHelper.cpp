#include "NumpyHelper.h"
#include "cnpy.h"
#include <QDebug>
#include <QFile>

NumpyHelper::NumpyHelper()
{
}

QImage NumpyHelper::NumpyToImage(const QString& npy_in, const QColor& replace_color)
{
  return NumpyToImage(npy_in, (QList<QColor>() << replace_color));
}

QImage NumpyHelper::NumpyToImage(const QString& npy_in, const QList<QColor>& colors)
{
  if (!QFile::exists(npy_in))
  {
    qDebug() << "File does not exist:" << npy_in;
    return QImage();
  }
  cnpy::NpyArray ar = cnpy::npy_load(qPrintable(npy_in));
  if (ar.shape.size() != 2)
  {
    qDebug() << "Could not load numpy file " << npy_in;
    return QImage();
  }

  unsigned char* ptr = ar.data<unsigned char>();
  QImage image(ar.shape[1], ar.shape[0], QImage::Format_ARGB32);
  image.fill(QColor(0,0,0,0));
  for (int j = 0; j < image.height(); j++)
  {
    QRgb* p = (QRgb*)image.scanLine(j);
    for (int i = 0; i < image.width(); i++)
    {
      int n = j*image.width()+i;
      if (ptr[n] > 0)
      {
        p[i] = colors[(ptr[n]-1)%colors.size()].rgb();
      }
    }
  }
  return image;
}
