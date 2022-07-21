#ifndef WIDGETIMAGEVIEW_H
#define WIDGETIMAGEVIEW_H

#include <QWidget>
#include <QImage>

class WidgetImageView : public QWidget
{
  Q_OBJECT
public:
  explicit WidgetImageView(QWidget *parent = nullptr);

  bool LoadImage(const QString& filename);

  void paintEvent(QPaintEvent* e);
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void resizeEvent(QResizeEvent* e);

signals:  

private:
  void UpdateScaledImage();
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
};

#endif // WIDGETIMAGEVIEW_H
