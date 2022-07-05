#include "graph.h"

Graph::Graph(int nbPoints, double* array, QWidget* parent)
    : QWidget(parent)
{
    this->nbPoints = nbPoints;
    this->array = array;

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
    
Graph::~Graph()
{

}

void Graph::paintEvent(QPaintEvent *event)
{
    double stepX = (double)(width()-20) / (nbPoints-1);
    double stepY = (double)(height()-20) / 100.0;

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // -- draw graph
    painter.drawLine(10, height() - 10, width()-10, height()-10);
    painter.drawLine(10, height() - 10, 10, 10);

    QPen pen;
    pen.setColor(QColor(50, 50, 50));
    pen.setWidth(5);
    painter.setPen(pen);

    for (int i = 0; i < nbPoints; i++) {
        int positionX = 10 + i*stepX;
        int positionY = height() - 10 - array[i]*stepY;
        painter.drawPoint(positionX, positionY);
    }

    pen.setColor(QColor(128, 128, 128));
    pen.setWidth(1);
    painter.setPen(pen);

    for (int i = 0; i < nbPoints; i++) {
        int positionX = 10 + i*stepX;
        painter.drawLine(positionX, height() - 10, positionX, 10);
    }

    painter.end();
}

void Graph::mouseReleaseEvent(QMouseEvent * event)
{
    double stepX = (double)(width()-20) / (nbPoints-1);
    double stepY = (double)(height()-20) / 100.0;

    if (event->button() == Qt::LeftButton) {
        int indexToPrint = -1;
        int valueToPrint = -1;

        for (int i = 0; i < nbPoints; i++) {
            int positionX = 10 + i*stepX;
            if (std::abs(positionX - event->x()) < 5) {
                indexToPrint = i*100.0/(nbPoints-1);
                valueToPrint = array[i]+0.5;
                break;
            }
        }

        if (indexToPrint != -1) {
            QMessageBox display;
            display.setText(QString::number(indexToPrint) + " --> " + QString::number(valueToPrint));
            display.exec();
        }
    }
    else if (event->button() == Qt::RightButton) {
        if (event->y() >= 10 && event->y() < height() - 10) {

            for (int i = 0; i < nbPoints; i++) {
                int positionX = 10 + i*stepX;
                if (std::abs(positionX - event->x()) < 5) {
                    array[i] = ((double)height() - 10.0 - (double)event->y())/stepY;
                    emit resetEmission();
                    this->repaint();
                    break;
                }
            }
        }
    }
}