#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ChildWindow.h"
#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QPixmap>
#include <QLabel>
#include <QTimer>
#include <QPoint>
#include <QMoveEvent>

#include <vector>
using std::vector;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	// to let the child follow me!
	void moveEvent(QMoveEvent*);
	void resizeEvent(QResizeEvent*);

	// exit properly
	void closeEvent(QCloseEvent*);

    Ui::MainWindow *ui;

	ChildWindow m_child;
};

#endif // MAINWINDOW_H
