#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include "ui_childwindow.h"
#include "ui_settings.h"

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
	void estimateCurrentState();

private:
	void paintEvent(QPaintEvent*);

	typedef struct tinfo
	{
		QPoint pos;
		int velocity;
		int miss;
	} TargetInfo;

    Ui::ChildWindow *ui;

	// quick and dirty
	Ui::SettingsWidget* m_settingsUi;
	QWidget m_settingsWidget;

	// Debug
	QLabel* m_debugLabel;

	// refresh-rate
	QTimer m_timer;

	// data to display
	QPoint m_playerPosition;
	vector<TargetInfo> m_enemyTargetInfos;
	vector<QPoint> m_debugPoints;
	int m_angle;
};

#endif // MAINWINDOW_H
