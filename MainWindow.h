#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QMainWindow>
#include <vector>
#include <QCheckBox>
#include "Chess.h"

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	int totalSec = 0;
	int totalMin = 0;
	MainWindow(QWidget *parent = 0);
protected:
	bool startwin = true;
	void setTimer();
	QTimer* timer;
	int totalTime = 0;
	void mousePressEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
	void setChess();
	void pressPosition(int, int);
	void pressAction(int, int);
	void pressAction_invis(int, int);
	void pressAction_conti(int, int);
	QCheckBox* invisCheck;
	QCheckBox* contiCheck;
	QCheckBox* aiCheckBox;
public slots:
	void countTotalTime();
	void setInvisable(bool);
	void setContinuously(bool);
	void setAICheck(bool);
private:
	std::vector<std::vector<Chess*> > ptrMap;
	std::vector<Chess> redChess;
	std::vector<Chess> blackChess;
	Chess* selected = NULL;
	bool redPlaying;
	int playerOneRed = -1; // 1 for true, 0 for false
	int countDown = 15;
	void chessMove(int, int);
	void chessEat(int, int);
	void chessEat_conti(int, int);
	void invisibleEat(int, int);
	void invisibleEat_conti(int, int);
	bool eatCheck(int, int);
	bool moveCheck(int, int);
	void machinePlaying();
	void end(int);
	int redOffTableChess = -1;
	int blackOffTableChess = -1;
	int tieCount = 0;
	bool endGame = false;
	QString endPic;
	// 1 for tie, 2 for out of time, 3 for red win, 4 for black win
	bool invisEat = false;
	bool contiEat = false;
	bool ai = true;
	bool startContiEat = false;
};
#endif
