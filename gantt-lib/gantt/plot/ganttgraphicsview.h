#ifndef GANTTGRAPHICSVIEW_H
#define GANTTGRAPHICSVIEW_H

#include <QGraphicsView>

#include <QModelIndex>

class GanttWidget;
class GanttScene;


class GanttGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GanttGraphicsView(QWidget *parent = 0);
    GanttGraphicsView(QGraphicsScene * scene, QWidget * parent = 0);

    void setHSliderHeight(int hSliderHeight);
signals:
    void viewResized(const QSize& newSize);

protected:
//    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);
private slots:

private:
    void initialize();

private:
    int m_hSliderHeight;
    QCursor _lastCursor;
};

#endif // GANTTVIEW_H
