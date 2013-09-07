#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include "ui_childwindow.h"

#include <QMainWindow>
#include <QPixmap>
#include <QLabel>
#include <QTimer>
#include <QPoint>

#include <vector>
using std::vector;

namespace Ui {
class ChildWindow;
}

class ChildWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ChildWindow(QWidget *parent = 0);
    ~ChildWindow();

private slots:
	void updateOverlay();

private:
	void paintEvent(QPaintEvent*);
    Ui::ChildWindow *ui;

	// Debug
	QLabel* m_debugLabel;

	// refresh-rate
	QTimer m_timer;

	// data to display
	vector<QPoint> m_debugPoints;
};

#endif // MAINWINDOW_H
