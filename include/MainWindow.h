#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QLabel>
#include <QTimer>
#include <QPoint>

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

private slots:
	void updateOverlay();

private:
	void paintEvent(QPaintEvent*);
    Ui::MainWindow *ui;

	// Debug
	QLabel* m_debugLabel;

	// refresh-rate
	QTimer m_timer;

	// data to display
	vector<QPoint> m_debugPoints;
};

#endif // MAINWINDOW_H
