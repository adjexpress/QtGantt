#include "ganttgraphicsview.h"
#include "ganttscene.h"
#include "ganttinfonode.h"

#include "gantt-lib_global_values.h"

#include <QResizeEvent>
#include <QScrollBar>

#include <QDebug>

GanttGraphicsView::GanttGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    initialize();
}


GanttGraphicsView::GanttGraphicsView(QGraphicsScene * scene, QWidget * parent) :
    QGraphicsView(scene,parent)
{
    initialize();
}


void GanttGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    emit viewResized(event->size());
}

//void GanttGraphicsView::scrollContentsBy(int dx, int dy)
//{
//    QGraphicsView::scrollContentsBy(dx,dy);

//    if(!m_scene)
//        return;

//    if(dx)
//        m_scene->invalidate(QRectF(),QGraphicsScene::BackgroundLayer);

//    int vs = verticalScrollBar()->value();

//    m_scene->updateHeaderPos(vs);

//    if(!m_treeView)
//        return;

//    m_treeView->verticalScrollBar()->setValue(vs);

//    m_treeView->update();


//}

void GanttGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();

    if(rect().contains(pos) && pos.y() > rect().bottom() - m_hSliderHeight)
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
    else
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GanttGraphicsView::leaveEvent(QEvent *e)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsView::leaveEvent(e);
}




void GanttGraphicsView::initialize()
{
    m_hSliderHeight = 0;

    setMinimumWidth(GANTTGRAPHICSVIEW_MIN_WIDTH);
    setFrameStyle(0);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setHSliderHeight(15);

    setContextMenuPolicy(Qt::CustomContextMenu);
}

void GanttGraphicsView::setHSliderHeight(int hSliderHeight)
{
    if(m_hSliderHeight == hSliderHeight)
        return;

    m_hSliderHeight = hSliderHeight;
    horizontalScrollBar()->setStyleSheet(
                QString("QScrollBar {height:%1px;}").arg(m_hSliderHeight));
}


