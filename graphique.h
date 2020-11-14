#ifndef GRAPHIQUE_H
#define GRAPHIQUE_H

#include <QDateTime>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>


class EchelleHoraire : public QwtScaleDraw
{
    virtual QwtText label(double v) const
    {
        QDateTime t = QDateTime::fromTime_t(static_cast<uint>(v));
        return t.toString("dd MMM");
    }
};

class Zoom: public QwtPlotZoomer
{
public:
    Zoom( QWidget *canvas ):
        QwtPlotZoomer( canvas )
    {
        setRubberBandPen( QColor( Qt::darkGray ) );
        setTrackerMode( QwtPlotPicker::AlwaysOn );
    }

protected:
    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QDateTime t;
        t.setTime_t(static_cast<uint>(pos.x()));

        QString s;
        s += t.toString("dd MMM hh'h'mm") + " = " + QString::number(pos.y(), 'f', 2) + " m";

        QwtText text( s );
        text.setColor( Qt::white );

        QColor c = rubberBandPen().color();
        text.setBorderPen( QPen( c ) );
        text.setBorderRadius( 6 );
        c.setAlpha( 170 );
        text.setBackgroundBrush( c );

        return text;
    }
};


#endif // GRAPHIQUE_H
