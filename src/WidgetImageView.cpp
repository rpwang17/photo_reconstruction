#include "WidgetImageView.h"
#include <QPainter>
#include <QMouseEvent>

WidgetImageView::WidgetImageView(QWidget *parent)
  : QWidget(parent), m_dScale(1.0), m_ptOffset(QPoint(0,0)), m_bPanning(false), m_bZooming(false)
{
  setMouseTracking(true);
}

bool WidgetImageView::LoadImage(const QString& filename)
{
  QImage image(filename);
  if (!image.isNull())
  {
    m_image = image;
    m_sFilename = filename;
    m_dScale = 1.0;
    m_ptOffset = QPoint(0,0);
    UpdateScaledImage();
    return true;
  }
  else
    return false;
}

void WidgetImageView::paintEvent(QPaintEvent *e)
{
  QRect rc = rect();
  QPainter p(this);
  p.fillRect(rc, Qt::black);
  QRect target = rect(), src = m_imageScaled.rect();
  src.moveCenter(rect().center()+m_ptOffset);
  p.drawImage(src, m_imageScaled);
}

void WidgetImageView::mousePressEvent(QMouseEvent *e)
{
  m_ptPress = e->pos();
  m_ptOldOffset = m_ptOffset;
  m_dOldScale = m_dScale;
  if (e->button() == Qt::LeftButton)
    m_bPanning = true;
  else if (e->button() == Qt::RightButton)
    m_bZooming = true;
}

void WidgetImageView::mouseReleaseEvent(QMouseEvent *e)
{
  m_bPanning = false;
  m_bZooming = false;
}

void WidgetImageView::mouseMoveEvent(QMouseEvent *e)
{
  if (m_bPanning)
  {
    m_ptOffset = m_ptOldOffset + (e->pos() - m_ptPress);
    update();
  }
  else if (m_bZooming)
  {
    int dn = e->y()-m_ptPress.y();
    if (dn > 0)
      m_dScale = m_dOldScale/pow(1.002, dn);
    else
      m_dScale = m_dOldScale*pow(1.002, -dn);
    if (m_dScale < 0.25)
      m_dScale = 0.25;
    else if (m_dScale > 10)
      m_dScale = 10;
    UpdateScaledImage();
  }
}

void WidgetImageView::resizeEvent(QResizeEvent *e)
{
  UpdateScaledImage();
}

void WidgetImageView::UpdateScaledImage()
{
  if (!m_image.isNull() && height() > 0)
  {
    if (1.0*m_image.width()/m_image.height() > 1.0*width()/height())
       m_imageScaled = m_image.scaledToWidth(width()*m_dScale, Qt::FastTransformation);
    else
      m_imageScaled = m_image.scaledToHeight(height()*m_dScale, Qt::FastTransformation);
  }
  update();
}
