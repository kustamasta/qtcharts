#include "bargroup.h"
#include "bar.h"
#include <QDebug>

QTCOMMERCIALCHART_BEGIN_NAMESPACE

BarGroup::BarGroup(BarChartSeries& series, QGraphicsItem *parent) :
  QGraphicsItem(parent)
  ,mSeries(series)
  ,mLayoutSet(false)
  ,mLayoutDirty(true)
  ,mBarDefaultWidth(10)
{
    dataChanged();
}

void BarGroup::setPos(const QPointF & pos)
{
    QGraphicsItem::setPos(pos);
}

void BarGroup::resize(const QSize& size)
{
    qDebug() << "BarGroup::setSize";
    mWidth = size.width();
    mHeight = size.height();
    layoutChanged();
    mLayoutSet = true;
}

void BarGroup::setPlotDomain(const PlotDomain& data)
{
    qDebug() << "BarGroup::setPlotDomain";
    // TODO:
    mPlotDomain = data;
}

void BarGroup::setTheme(ChartTheme *theme)
{
    // TODO
}

void BarGroup::setBarWidth( int w )
{
    mBarDefaultWidth = w;
}

int BarGroup::addColor( QColor color )
{
    int colorIndex = mColors.count();
    mColors.append(color);
    return colorIndex;
}

void BarGroup::resetColors()
{
    mColors.clear();
}

void BarGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!mLayoutSet) {
        qDebug() << "QBarChart::paint called without layout set. Aborting.";
        return;
    }
    if (mLayoutDirty) {
        // Layout or data has changed. Need to redraw.
        foreach(QGraphicsItem* i, childItems()) {
            i->paint(painter,option,widget);
        }
    }
}

QRectF BarGroup::boundingRect() const
{
    return QRectF(0,0,mWidth,mHeight);
}


void BarGroup::dataChanged()
{
    qDebug() << "QBarChart::dataChanged mSeries";

    // Find out maximum and minimum of all series
    mMax = mSeries.max();
    mMin = mSeries.min();

    // Delete old bars
    // Is this correct way to delete childItems?
    foreach (QGraphicsItem* item, childItems()) {
        delete item;
    }

    // Create new graphic items for bars
    int totalItems = mSeries.countTotalItems();
    for (int i=0; i<totalItems; i++) {
        Bar *bar = new Bar(this);
        childItems().append(bar);
    }

    mLayoutDirty = true;
}

void BarGroup::layoutChanged()
{
    // Scale bars to new layout
    // Layout for bars:
    if (mSeries.countRows() <= 0) {
        // Nothing to do.
        return;
    }

    // TODO: better way to auto-layout?
    // Use reals for accurancy (we might get some compiler warnings... :)
    int columnCount = mSeries.countColumns();
    int rowCount = mSeries.countRows();

    qreal tW = mWidth;
    qreal tH = mHeight;
    qreal tM = mMax;
    qreal scale = (tH/tM);

    qreal tC = columnCount+1;
    qreal xStepPerSeries = (tW/tC);

    qDebug() << "XSTEP:" << xStepPerSeries;

    // Scaling.
    int itemIndex(0);
    for (int column=0; column < columnCount; column++) {
        qreal xPos = xStepPerSeries * column + ((tW + mBarDefaultWidth*rowCount)/(columnCount*2));
        for (int row = 0; row < rowCount; row++) {
            qreal barHeight = mSeries.valueAt(row, column) * scale;
            Bar* bar = reinterpret_cast<Bar*> (childItems().at(itemIndex));

            // TODO: width settable per bar?
            bar->resize(mBarDefaultWidth, barHeight);
            bar->setColor(mColors.at(row));
            bar->setPos(xPos, mHeight); // item*posStep+startPos + series * mBarDefaultWidth, mHeight);
            itemIndex++;
            xPos += mBarDefaultWidth;
        }
    }

    mLayoutDirty = true;
}

QTCOMMERCIALCHART_END_NAMESPACE
