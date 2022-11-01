#include "WidgetImageView.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include "math.h"

WidgetImageView::WidgetImageView(QWidget *parent)
  : QWidget(parent), m_dScale(1.0), m_ptOffset(QPoint(0,0)), m_bPanning(false), m_bZooming(false), m_bDrawing(false),
    m_nNumberOfExpectedPoints(2), m_nEditMode(0)
{
  setMouseTracking(true);
  m_colorPen = QColor(50,255,50);
}

bool WidgetImageView::LoadImage(const QString& filename, const QString& mask, const QList<QPoint>& points, const QList<RECT_REGION>& regions)
{
  m_sMessage.clear();
  m_sFilename = filename;
  m_sMaskFilename = mask;
  PrepareImage();
  if (!m_image.isNull())
  {
    m_dScale = 1.0;
    m_ptOffset = QPoint(0,0);
    m_listPoints = points;
    m_listRegions = regions;
    UpdateScaledImage();
    return true;
  }
  else
    return false;
}

void WidgetImageView::PrepareImage()
{
  QImage image(m_sFilename);
  if (!image.isNull())
  {
    m_image = image;
    if (!m_sMaskFilename.isEmpty())
    {
      QImage mask_image(m_sMaskFilename);
      QPainter p(&m_image);
      p.setCompositionMode(QPainter::CompositionMode_Multiply);
      p.drawImage(0, 0, mask_image);
      p.end();
    }
  }
}

void WidgetImageView::AddOverlay(const QImage& overlay_image)
{
  QPainter p(&m_image);
  p.drawImage(0, 0, overlay_image);
  p.end();
  UpdateScaledImage();
}

void WidgetImageView::paintEvent(QPaintEvent *e)
{
  QRect rc = rect();
  QPainter p(this);
  p.fillRect(rc, Qt::black);
  QRect target = m_imageScaled.rect();
  target.moveCenter(rect().center()+m_ptOffset);
  p.drawImage(target, m_imageScaled);
  if (m_nEditMode == 0)
  {
    p.setPen(m_colorPen);
    p.setBrush(m_colorPen);
    QList<QPoint> pts;
    foreach (QPoint pt, m_listPoints)
    {
      pt = pt*m_imageScaled.width()/m_image.width() + target.topLeft();
      p.drawEllipse(pt, 2, 2);
      pts << pt;
    }
    if (pts.size() > 1)
    {
      p.setPen(QPen(m_colorPen, 2));
      p.drawLine(pts[0], pts[1]);
    }
  }
  else if (m_nEditMode == 1)
  {
    QColor pen_color(255,255,255);
    QColor active_color(255,30,30);
    p.setPen(QPen(pen_color, 2));
    p.setBrush(Qt::NoBrush);
    for (int i = 0; i < m_listRegions.size(); i++)
    {
      if (i == m_listRegions.size()-1 && m_bDrawing)
        p.setPen(QPen(active_color, 2));

      QRect rc(m_listRegions[i].first*m_imageScaled.width()/m_image.width() + target.topLeft(),
               m_listRegions[i].second*m_imageScaled.width()/m_image.width() + target.topLeft());
      p.drawRect(rc);
    }
  }
  if (!m_sMessage.isEmpty())
  {
    QFont f = p.font();
    f.setPixelSize(16);
    p.setFont(f);
    QRect rc = p.fontMetrics().boundingRect(m_sMessage);
    rc.adjust(-20,-9, 20, 9);
    rc.moveCenter(rect().center());
    rc.moveBottom(rect().bottom()-18);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0,0,0,200));
    p.drawRoundedRect(rc, rc.height()/2, rc.height()/2);
    p.setPen(Qt::white);
    p.drawText(rc, m_sMessage, Qt::AlignHCenter|Qt::AlignVCenter);
  }
}

void WidgetImageView::mousePressEvent(QMouseEvent *e)
{
  m_ptPress = e->pos();
  m_ptOldOffset = m_ptOffset;
  m_dOldScale = m_dScale;
  if (e->button() == Qt::LeftButton)
  {
    if (e->modifiers() & Qt::ControlModifier)
    {
      m_bDrawing = true;
      QPoint pt = ScreenToImage(m_ptPress);
      RECT_REGION region;
      region.first = pt;
      region.second = pt;
      m_listRegions << region;
    }
    else
      m_bPanning = true;
  }
  else if (e->button() == Qt::RightButton)
    m_bZooming = true;
}

QPoint WidgetImageView::ScreenToImage(const QPoint &pt_in)
{
  QRect target = m_imageScaled.rect();
  target.moveCenter(rect().center()+m_ptOffset);
  QPoint pt = (pt_in - target.topLeft());
  pt = pt*m_image.width()/m_imageScaled.width();
  return pt;
}

void WidgetImageView::mouseReleaseEvent(QMouseEvent *e)
{
  QPoint dpt = e->pos() - m_ptPress;
  if (m_bDrawing && !m_imageScaled.isNull())
  {
    if (m_nEditMode == EM_POINT && qAbs(dpt.x()) < 2 && qAbs(dpt.y()) < 2)
    {
      QPoint pt = ScreenToImage(m_ptPress);
      if (m_listPoints.size() < m_nNumberOfExpectedPoints)
        m_listPoints << pt;
      else
        m_listPoints[m_nNumberOfExpectedPoints-1] = pt;
      update();
    }
    else if (m_nEditMode == EM_REGION)
    {
      update();
      QPoint pt = m_listRegions.last().first - m_listRegions.last().second;
      if (qAbs(pt.x()) < 3 || qAbs(pt.y()) < 3)
        m_listRegions.removeLast();
      else
        emit LastRegionEdited(m_listRegions.size()-1);
    }
  }
  else if (m_bZooming)
  {
    UpdateScaledImage(true);
  }
  m_bPanning = false;
  m_bZooming = false;
  m_bDrawing = false;
}

void WidgetImageView::mouseMoveEvent(QMouseEvent *e)
{
  QPoint dpt = e->pos() - m_ptPress;
  if (m_bPanning)
  {
    m_ptOffset = m_ptOldOffset + dpt;
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
      m_dScale = 8;
    UpdateScaledImage();
  }
  else if (m_bDrawing && m_nEditMode == EM_REGION && !m_listRegions.isEmpty())
  {
    QPoint pt_0 = ScreenToImage(m_ptPress), pt_1 = ScreenToImage(e->pos());
    if (pt_0.x() > pt_1.x())
    {
      int temp = pt_0.x();
      pt_0.setX(pt_1.x());
      pt_1.setX(temp);
    }
    if (pt_0.y() > pt_1.y())
    {
      int temp = pt_0.y();
      pt_0.setY(pt_1.y());
      pt_1.setY(temp);
    }
    RECT_REGION region = m_listRegions.last();
    region.first = pt_0;
    region.second = pt_1;
    m_listRegions[m_listRegions.size()-1] = region;
    update();
  }
}

void WidgetImageView::wheelEvent(QWheelEvent* e)
{
  //  qDebug() << e;
}

void WidgetImageView::resizeEvent(QResizeEvent *e)
{
  UpdateScaledImage();
}

void WidgetImageView::UpdateScaledImage(bool bSmooth)
{
  if (!m_image.isNull() && height() > 0)
  {
    if (1.0*m_image.width()/m_image.height() > 1.0*width()/height())
      m_imageScaled = m_image.scaledToWidth(width()*m_dScale, bSmooth?Qt::SmoothTransformation:Qt::FastTransformation);
    else
      m_imageScaled = m_image.scaledToHeight(height()*m_dScale, bSmooth?Qt::SmoothTransformation:Qt::FastTransformation);
  }
  update();
}

void WidgetImageView::Clear()
{
  m_listPoints.clear();
  m_listRegions.clear();
  PrepareImage();
  UpdateScaledImage();
  update();
}
