#ifndef WIDGETIMAGEVIEW_H
#define WIDGETIMAGEVIEW_H

#include <QWidget>
#include <QImage>

class WidgetImageView : public QWidget
{
  Q_OBJECT
public:
  explicit WidgetImageView(QWidget *parent = nullptr);

  bool LoadImage(const QString& filename, const QString& mask = "", const QList<QPoint>& points = QList<QPoint>());

  void paintEvent(QPaintEvent* e);
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void resizeEvent(QResizeEvent* e);

  QList<QPoint> GetEditedPoints()
  {
    return m_listPoints;
  }

  QList< QPair<QPoint,QPoint> > GetEditedRegions()
  {
    return m_listRegions;
  }

  enum EditMode { EM_POINT = 0, EM_REGION };

signals:

public slots:
  void SetNumberOfExpectedPoints(int n)
  {
    m_nNumberOfExpectedPoints = n;
  }

  void SetEditMode(int n)
  {
    m_nEditMode = n;
  }

  void Clear()
  {
    m_listPoints.clear();
    m_listRegions.clear();
    update();
  }

private:
  void UpdateScaledImage(bool bSmooth = false);
  QPoint ScreenToImage(const QPoint& pt);

  QString   m_sFilename;
  QImage    m_image;
  QImage    m_imageScaled;
  double    m_dScale;
  double    m_dOldScale;
  QPoint    m_ptOffset;
  QPoint    m_ptOldOffset;
  QPoint    m_ptPress;
  bool      m_bPanning;
  bool      m_bZooming;
  bool      m_bDrawing;

  int       m_nNumberOfExpectedPoints;
  int       m_nEditMode;
  QList<QPoint> m_listPoints;
  QList< QPair<QPoint, QPoint> > m_listRegions;
  QColor    m_colorPen;
};

#endif // WIDGETIMAGEVIEW_H
