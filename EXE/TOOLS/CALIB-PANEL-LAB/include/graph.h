#include <stdlib.h>
#include <iostream>
#include <queue>
#include <QWidget>
#include <QObject>
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPen>

#pragma once

class Graph : public QWidget
{
    Q_OBJECT

public:

    explicit Graph(int nbPoints, double* array, QWidget* parent = nullptr);
    ~Graph();

signals:

    void resetEmission();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private:

    int nbPoints;
    double* array;
};

