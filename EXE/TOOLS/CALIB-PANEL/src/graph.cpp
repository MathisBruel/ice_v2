#include "graph.h"

Graph::Graph(int nbPoints, double* array, double* curve, QWidget* parent)
    : QWidget(parent)
{
    this->nbPoints = nbPoints;
    this->array = array;
    this->curve = curve;

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
    
Graph::~Graph()
{

}

void Graph::setColorPoint(QColor *colorPoint)
{
    this->colorPoint = colorPoint;
}

void Graph::displayInfo()
{

}
void Graph::changeValue()
{

}

void Graph::paintEvent(QPaintEvent *event)
{
    double ratioX = (double)(width()-20) / 256.0;
    if (nbPoints == 11) {
        ratioX = (double)(width()-20) / 101.0;
    }
    double ratioY = (double)(height()-20);

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // -- draw graph
    painter.drawLine(10, height() - 10, width()-10, height()-10);
    painter.drawLine(10, height() - 10, 10, 10);

    QPen pen;
    pen.setColor(QColor(50, 50, 50));

    if (curve != nullptr) {
        pen.setWidth(1);
        painter.setPen(pen);
        QPainterPath path;
        path.moveTo(10, height() - 10 - array[0]*ratioY);

        if (nbPoints == 16) {
            for (int i = 0; i < 256; i++) {
                int positionX = 10 + i*ratioX;
                int positionY = height() - 10 - curve[i]*ratioY;
                path.lineTo(positionX, positionY);
            }
        }
        else if (nbPoints == 11) {
            for (int i = 0; i < 101; i++) {
                int positionX = 10 + i*ratioX;
                int positionY = height() - 10 - curve[i]*ratioY;
                path.lineTo(positionX, positionY);
            }
        }
        painter.drawPath(path);
    }

    pen.setColor(*colorPoint);
    pen.setWidth(5);
    painter.setPen(pen);

    if (nbPoints == 16) {
        for (int i = 0; i < nbPoints; i++) {
            int index = i*17;
            int positionX = 10 + index*ratioX;
            int positionY = height() - 10 - array[i]*ratioY;
            painter.drawPoint(positionX, positionY);
        }
    }
    else if (nbPoints == 11) {
        for (int i = 0; i < nbPoints; i++) {
            int index = i*10;
            int positionX = 10 + index*ratioX;
            int positionY = height() - 10 - array[i]*ratioY;
            painter.drawPoint(positionX, positionY);
        }
    }

    pen.setColor(QColor(128, 128, 128));
    pen.setWidth(1);
    painter.setPen(pen);

    if (nbPoints == 16) {
        for (int i = 0; i < nbPoints; i++) {
            int index = i*17;
            int positionX = 10 + index*ratioX;
            painter.drawLine(positionX, height() - 10, positionX, 10);
        }
    }
    else if (nbPoints == 11) {
        for (int i = 0; i < nbPoints; i++) {
            int index = i*10;
            int positionX = 10 + index*ratioX;
            painter.drawLine(positionX, height() - 10, positionX, 10);
        }
    }

    painter.end();
}

void Graph::mouseReleaseEvent(QMouseEvent * event)
{
    double ratioX = (double)(width()-20) / 256.0;
    if (nbPoints == 11) {
        ratioX = (double)(width()-20) / 101.0;
    }
    double ratioY = (double)(height()-20);

    if (event->button() == Qt::LeftButton) {
        int indexToPrint = -1;
        int valueToPrint = -1;

        if (nbPoints == 16) {
            for (int i = 0; i < nbPoints; i++) {
                int index = i*17;
                int positionX = 10 + index*ratioX;
                if (std::abs(positionX - event->x()) < 5) {
                    indexToPrint = index;
                    valueToPrint = array[i]*255.0+0.5;
                    break;
                }
            }
        }
        else if (nbPoints == 11) {
            for (int i = 0; i < nbPoints; i++) {
                int index = i*10;
                int positionX = 10 + index*ratioX;
                if (std::abs(positionX - event->x()) < 5) {
                    indexToPrint = index;
                    valueToPrint = array[i]*100.0+0.5;
                    break;
                }
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

            if (nbPoints == 16) {
                for (int i = 0; i < nbPoints; i++) {
                    int index = i*17;
                    int positionX = 10 + index*ratioX;
                    if (std::abs(positionX - event->x()) < 5) {
                        double oldValue = array[i];
                        double newValue = ((double)height() - 10.0 - (double)event->y())/ratioY;
                        array[i] = newValue;
                        emit resetEmission(i, oldValue, newValue);
                        this->repaint();
                        break;
                    }
                }
            }
            else if (nbPoints == 11) {
                for (int i = 0; i < nbPoints; i++) {
                    int index = i*10;
                    int positionX = 10 + index*ratioX;
                    if (std::abs(positionX - event->x()) < 5) {
                        double oldValue = array[i];
                        double newValue = ((double)height() - 10.0 - (double)event->y())/ratioY;
                        array[i] = newValue;
                        emit resetEmission(i, oldValue, newValue);
                        this->repaint();
                        break;
                    }
                }
            }
        }
    }
}