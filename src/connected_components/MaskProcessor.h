#ifndef MASKPROCESSOR_H
#define MASKPROCESSOR_H

#include <QString>
#include <QPoint>
#include <QList>
#include <QColor>
#include <QImage>

class MaskProcessor
{
public:
  MaskProcessor();  
  ~MaskProcessor();

  bool Load(const QString& mask_fn);
  void LoadSelections(const QList<QPoint>& pts);
  bool ProcessSelection(const QPoint& pt1, const QPoint& pt2, int nFillValue);
  bool SaveToNpy(const QString& fn);

  void ClearBuffer();

  QImage GetMaskImage(const QList<QColor>& colors);

private:
  void ClearData();

  unsigned int* m_data;
  unsigned int* m_dataInBuffer;
  unsigned char* m_dataOutBuffer;
  int m_nWidth;
  int m_nHeight;
};

#endif // MASKPROCESSOR_H
