#ifndef WIDGETIMAGEVIEW_H
#define WIDGETIMAGEVIEW_H

#include <QWidget>
#include <QImage>

#ifndef RECT_REGION
#define RECT_REGION QPair<QPoint,QPoint>
#endif

class WidgetImageView : public QWidget
{
  Q_OBJECT
public:
  explicit WidgetImageView(QWidget *parent = nullptr);

  bool LoadImage(const QString& filename, const QString& mask = "",
                 const QList<QPoint>& points = QList<QPoint>(),
                 const QList<RECT_REGION>& regions = QList<RECT_REGION>());

  void paintEvent(QPaintEvent* e);
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void wheelEvent(QWheelEvent* e);
  void resizeEvent(QResizeEvent* e);

  QList<QPoint> GetEditedPoints()
  {
    return m_listPoints;
  }

  QList<RECT_REGION> GetEditedRegions()
  {
    return m_listRegions;
  }

  QString GetFilename()
  {
    return m_sFilename;
  }

  QString GetMaskFilename()
  {
    return m_sMaskFilename;
  }

  enum EditMode { EM_POINT = 0, EM_REGION };

signals:
  void LastRegionEdited(int n);

public slots:
  void SetNumberOfExpectedPoints(int n)
  {
    m_nNumberOfExpectedPoints = n;
  }

  void SetEditMode(int n)
  {
    m_nEditMode = n;
  }

  void ShowMessage(const QString& msg)
  {
    m_sMessage = msg;
    update();
  }

  void HideMessage()
  {
    m_sMessage.clear();
    update();
  }

  void AddOverlay(const QImage& overlay_image);

  void Clear();

private:
  void PrepareImage();
  void UpdateScaledImage(bool bSmooth = false);
  QPoint ScreenToImage(const QPoint& pt);

  QString   m_sFilename;
  QString   m_sMaskFilename;
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
  QList<RECT_REGION> m_listRegions;
  QColor    m_colorPen;

  QString   m_sMessage;
};

#endif // WIDGETIMAGEVIEW_H
