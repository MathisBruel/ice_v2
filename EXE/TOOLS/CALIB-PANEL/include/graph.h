#include <stdlib.h>
#include <iostream>
#include <queue>
#include <QWidget>
#include <QObject>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPen>

#pragma once

class Graph : public QWidget
{
    Q_OBJECT

public:

    explicit Graph(int nbPoints, double* array, double* curve = nullptr, QWidget* parent = nullptr);
    ~Graph();

    void setColorPoint(QColor *colorPoint);

signals:

    void resetEmission(int index, double oldValue, double newValue);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private:

    void displayInfo();
    void changeValue();
    int nbPoints;
    double* array;
    double* curve;
    QColor *colorPoint;
};

