#include "ganttintervalslider.h"
#include "utcdatetime.h"
#include "gantt-lib_global_values.h"

#include <QPainter>
#include <QLinearGradient>
#include <QBrush>
#include <QFrame>
#include <QPen>

#include <QMouseEvent>

#include <QDebug>

GanttIntervalSlider::GanttIntervalSlider(QWidget *parent )
    : IntervalSlider(parent)
{
    m_drawCurrentDt = false;
    setOffsetV(7);
    setHandleSize(6);
    setSliderV(8);

    setCurrentTimeRectWidth(4);
    setCurrentTimeRectColor(Qt::red);

    setStyleSheet("background-color: rgb(21, 168, 194)");

    m_shiftRange = 0;
    m_minTimeSize = 0;

    setMouseTracking(true);
}

void GanttIntervalSlider::drawHandle(QPainter *painter, const QRect &handleRect, bool is_selected) const
{
    painter->setPen(Qt::black);
    qreal penWidth = 1.0;
    QColor color = (is_selected)?(HEADER_COLOR):(SLIDER_COLOR);
    QPen pen(Qt::black,penWidth,Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
    painter->setPen(pen);

    painter->setBrush(QBrush(color));
    painter->drawRoundedRect(handleRect.adjusted(1,1,-1,-1)
                             ,1,1);


}

void GanttIntervalSlider::drawSliderLine(QPainter *painter, const QRect &sliderRect) const
{
    int top = sliderRect.y() + m_offsetV,
            width = sliderRect.width() - handleSize();

    int beginRectLeft = sliderRect.x() + halfHandleSize(),
            beginRectWidth = valueToPoint(m_beginValue,BeginHandle) - halfHandleSize() - sliderRect.x(),
            innerRectLeft = beginRectLeft + beginRectWidth,
            innerRectWidth = valueToPoint(m_endValue,EndHandle) - beginRectWidth,
            endRectLeft = innerRectLeft + innerRectWidth,
            endRectWidth = width - valueToPoint(m_endValue,EndHandle);
    QLinearGradient foregroundGradient(QPoint(0,top),QPoint(0,top + m_sliderV));
    QColor sliderColor = SLIDER_COLOR;
    foregroundGradient.setColorAt(0,sliderColor);
    foregroundGradient.setColorAt(0.5,sliderColor.lighter(180));
    foregroundGradient.setColorAt(1,sliderColor);

//    QLinearGradient backgroundGradient(QPoint(0,top),QPoint(0,top + m_sliderV));
//    backgroundGradient.setColorAt(0,sliderColor);
//    backgroundGradient.setColorAt(0.5,Qt::white);
//    backgroundGradient.setColorAt(1,sliderColor);

    painter->setPen(Qt::gray);

    painter->fillRect(QRect(innerRectLeft,
                            top,
                            innerRectWidth,
                            m_sliderV
                            ), foregroundGradient);

    painter->drawRect(QRect(beginRectLeft,
                            top,
                            width,
                            m_sliderV
                            ));


    if(beginRectWidth > 0)
    {
        QRect beforeBeginRect(beginRectLeft+1,
                              top+1,
                              beginRectWidth,
                              m_sliderV-1);

        painter->fillRect(beforeBeginRect,  QBrush(sliderColor.lighter(190)));

    }


    if(endRectWidth > 0)
    {
        QRect afterEndRect(endRectLeft,
                        top+1,
                        endRectWidth-1,
                        m_sliderV-1);

        painter->fillRect(afterEndRect,  QBrush(sliderColor.lighter(190)));
    }

    if(!outOfLimits(m_currentTime))
        drawCurrentTime(painter,sliderRect);
}

void GanttIntervalSlider::drawCurrentTime(QPainter *painter, const QRect &sliderRect) const
{
    if(!m_drawCurrentDt)
        return;

    QRect currentTimeRect(valueToPoint(dtToVal(m_currentTime),NoHandle)
                          - (m_currentTimeRectWidth)/2,
                          sliderRect.y(),
                          m_currentTimeRectWidth,
                          intervalSliderHeight()
                          );

    painter->setPen(Qt::black);
    painter->setBrush(QBrush(m_currentTimeRectColor));
    painter->drawRect(currentTimeRect.adjusted(0,0,0,-1));
}




long long GanttIntervalSlider::minTimeSize() const
{
    return m_minTimeSize;
}

void GanttIntervalSlider::setBeginHandle(long long beginHandle)
{
    if(endHandle() - beginHandle < minTimeSize())
        beginHandle = endHandle() - minTimeSize();
    IntervalSlider::setBeginHandle(beginHandle);

}

void GanttIntervalSlider::setEndHandle(long long endHandle)
{
    if(endHandle - beginHandle() < minTimeSize())
        endHandle = beginHandle() + minTimeSize();
    IntervalSlider::setEndHandle(endHandle);
}

void GanttIntervalSlider::reset()
{
    setLimits(m_minValue,m_maxValue);
}

void GanttIntervalSlider::setMinTimeSize(long long minTimeSize)
{
    m_minTimeSize = minTimeSize;
}

UtcDateTime GanttIntervalSlider::valToDt(long long val) const
{
    return UtcDateTime::fromMicrosecondsSinceEpoch(val);
}

long long GanttIntervalSlider::dtToVal(const UtcDateTime &dt) const
{
    return dt.toMicrosecondsSinceEpoch();
}

UtcDateTime GanttIntervalSlider::beginDt() const
{
    return valToDt(beginHandle());
}

UtcDateTime GanttIntervalSlider::endDt() const
{
    return valToDt(endHandle());
}


bool GanttIntervalSlider::outOfLimits(const UtcDateTime &dt) const
{
    return dt<valToDt(m_minValue) || dt > valToDt(m_maxValue);
}

void GanttIntervalSlider::setCurrentTimeRectColor(const QColor &currentTimeRectColor)
{
    m_currentTimeRectColor = currentTimeRectColor;

    update();
}

void GanttIntervalSlider::setCurrentTime(const UtcDateTime &dt)
{
    m_currentTime  = dt;

    update();
}

void GanttIntervalSlider::setDrawCurrentDt(bool draw)
{
    m_drawCurrentDt = draw;
}

void GanttIntervalSlider::setCurrentTimeRectWidth(const qreal &currentTimeRectWidth)
{
    m_currentTimeRectWidth = currentTimeRectWidth;

    update();
}
