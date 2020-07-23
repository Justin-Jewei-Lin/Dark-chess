#include "MainWindow.h"
#include "Chess.h"
#include <QApplication>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QImage>
#include <QTimer>
#include <QTime>
#include <QCheckBox>
#include <QDebug>
#include <QEventLoop>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <cmath>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setFixedSize(890, 650);
	setMouseTracking(true);
	invisCheck = new QCheckBox(this);
	contiCheck = new QCheckBox(this);
	aiCheckBox = new QCheckBox(this);
	invisCheck->move(233, 479);
	contiCheck->move(462, 479);
	aiCheckBox->move(233, 540);
	invisCheck->resize(15, 15);
	contiCheck->resize(15, 15);
	aiCheckBox->resize(15, 15);
	invisCheck->setChecked(false);
	contiCheck->setChecked(false);
	aiCheckBox->setChecked(true);
	connect(invisCheck, SIGNAL(clicked(bool)), this, SLOT(setInvisable(bool)));
	connect(contiCheck, SIGNAL(clicked(bool)), this, SLOT(setContinuously(bool)));
	connect(aiCheckBox, SIGNAL(clicked(bool)), this, SLOT(setAICheck(bool)));
	update();
	setChess();
}
void MainWindow::mousePressEvent(QMouseEvent *event){
	if (event->button() == Qt::LeftButton) {
		if (startwin){
			if (event->x() > 220 && event->x() < 650 && event->y() > 400 && event->y() < 450){
				startwin = false;
				disconnect(invisCheck, SIGNAL(clicked(bool)), this, SLOT(setInvisable(bool)));
				disconnect(contiCheck, SIGNAL(clicked(bool)), this, SLOT(setContinuously(bool)));
				disconnect(aiCheckBox, SIGNAL(clicked(bool)), this, SLOT(setAICheck(bool)));
				delete invisCheck;
				delete contiCheck;
				delete aiCheckBox;
				qDebug() << "Invisable eat: " << invisEat;
				qDebug() << "Continuiously eat: " << contiEat;
				qDebug() << "AI: " << ai;
			}
			update();
		}
		else if (!startwin){
			pressPosition(event->x(), event->y());
		}
	}
	else if (event->button() == Qt::RightButton) {
		if (startContiEat) {
			startContiEat = false;
			countDown = 15;
			redPlaying = !redPlaying;
			selected = NULL;
		}
		else {
			selected = NULL;
		}
		update();
	}
}
void MainWindow::paintEvent(QPaintEvent *){
	QPainter painter(this);
	QPen pen;
	pen.setColor(Qt::black);
	QFont font1("Tahoma", 15, 30, false);
	QFont font2("Tahoma", 25, 80, false);
	painter.setPen(pen);
	if (startwin && !endGame){ // paint start window
		QImage startwin;
		startwin.load("./dataset/startwin.png");
		painter.drawImage(0, 0, startwin);
	}
	if (!startwin && (playerOneRed != -1) && !endGame) {
		QImage red_text, black_text;
		red_text.load("./dataset/red_text.png");
		black_text.load("./dataset/black_text.png");
		if (playerOneRed) {
			painter.drawImage(75, 140, red_text);
			painter.drawImage(690, 140, black_text);
		}
		else {
			painter.drawImage(690, 140, red_text);
			painter.drawImage(75, 140, black_text);
		}
	}
	if (!startwin && !endGame){	// paint time
		painter.setFont(font1);
		painter.drawText(640, 500, QString("%1:%2").arg(totalMin, 2, 10, QChar('0')).arg(totalSec, 2, 10, QChar('0')));
		painter.setFont(font2);
		if ((playerOneRed && redPlaying) || (!playerOneRed && !redPlaying)) {
			painter.drawText(113, 120, QString("%1").arg(countDown, 2, 10, QChar('0')));
			painter.drawText(733, 120, QString("- -"));
		}
		else {
			painter.drawText(733, 120, QString("%1").arg(countDown, 2, 10, QChar('0')));
			painter.drawText(113, 120, QString("- -"));
		}
	}
	if (!startwin && !endGame){
		for (int i = 0; i < 16; i++){	// paint red chess
			QImage chessPic;
			QString qstr = QString::fromStdString(redChess.at(i).image);
			chessPic.load(qstr);
			if (!redChess.at(i).getOnTable()) {
				QImage chessPic_small = chessPic.scaled(35, 35);
				painter.drawImage(redChess.at(i).pix_x(), redChess.at(i).pix_y(), chessPic_small);
				continue;
			}
			painter.drawImage(redChess.at(i).pix_x(), redChess.at(i).pix_y(), chessPic);
		}
		for (int i = 0; i < 16; i++){	// paint black chess
			QImage chessPic;
			QString qstr = QString::fromStdString(blackChess.at(i).image);
			chessPic.load(qstr);
			if (!blackChess.at(i).getOnTable()) {
				QImage chessPic_small = chessPic.scaled(35, 35);
				painter.drawImage((blackChess.at(i).pix_x()), (blackChess.at(i).pix_y()), chessPic_small);
				continue;
			}
			painter.drawImage(blackChess.at(i).pix_x(), blackChess.at(i).pix_y(), chessPic);
		}
		if (selected != NULL) {	//paint selected
			QImage sel;
			sel.load("./dataset/selected.png");
			painter.drawImage(selected->pix_x(), selected->pix_y(), sel);
		}
	}
	if (!startwin && endGame) {
		QImage endPicture;
		endPicture.load(endPic);
		painter.drawImage(0, 0, endPicture);
		if (endPic != "./dataset/tie.png") {
			QImage red_text, black_text;
			red_text.load("./dataset/red_text.png");
			black_text.load("./dataset/black_text.png");
			if (redPlaying)
				painter.drawImage(382, 453, red_text);
			else
				painter.drawImage(382, 453, black_text);
		}
	}
}
void MainWindow::setTimer(){
    this->timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(countTotalTime()));
	this->timer->start(1000);
}
void MainWindow::countTotalTime(){
	totalTime++;
	totalSec = totalTime % 60;
	totalMin = totalTime / 60;
	countDown--;
	if (countDown <= 0 && !endGame) {
		end(2);
	}
	update();
}
void MainWindow::setChess(){
	redChess.push_back(Chess(true, 1, "./dataset/red1.png"));
	redChess.push_back(Chess(true, 2, "./dataset/red2.png"));
	redChess.push_back(Chess(true, 2, "./dataset/red2.png"));
	redChess.push_back(Chess(true, 3, "./dataset/red3.png"));
	redChess.push_back(Chess(true, 3, "./dataset/red3.png"));
	redChess.push_back(Chess(true, 4, "./dataset/red4.png"));
	redChess.push_back(Chess(true, 4, "./dataset/red4.png"));
	redChess.push_back(Chess(true, 5, "./dataset/red5.png"));
	redChess.push_back(Chess(true, 5, "./dataset/red5.png"));
	redChess.push_back(Chess(true, 6, "./dataset/red6.png"));
	redChess.push_back(Chess(true, 6, "./dataset/red6.png"));
	redChess.push_back(Chess(true, 7, "./dataset/red7.png"));
	redChess.push_back(Chess(true, 7, "./dataset/red7.png"));
	redChess.push_back(Chess(true, 7, "./dataset/red7.png"));
	redChess.push_back(Chess(true, 7, "./dataset/red7.png"));
	redChess.push_back(Chess(true, 7, "./dataset/red7.png"));
	blackChess.push_back(Chess(false, 1, "./dataset/black1.png"));
	blackChess.push_back(Chess(false, 2, "./dataset/black2.png"));
	blackChess.push_back(Chess(false, 2, "./dataset/black2.png"));
	blackChess.push_back(Chess(false, 3, "./dataset/black3.png"));
	blackChess.push_back(Chess(false, 3, "./dataset/black3.png"));
	blackChess.push_back(Chess(false, 4, "./dataset/black4.png"));
	blackChess.push_back(Chess(false, 4, "./dataset/black4.png"));
	blackChess.push_back(Chess(false, 5, "./dataset/black5.png"));
	blackChess.push_back(Chess(false, 5, "./dataset/black5.png"));
	blackChess.push_back(Chess(false, 6, "./dataset/black6.png"));
	blackChess.push_back(Chess(false, 6, "./dataset/black6.png"));
	blackChess.push_back(Chess(false, 7, "./dataset/black7.png"));
	blackChess.push_back(Chess(false, 7, "./dataset/black7.png"));
	blackChess.push_back(Chess(false, 7, "./dataset/black7.png"));
	blackChess.push_back(Chess(false, 7, "./dataset/black7.png"));
	blackChess.push_back(Chess(false, 7, "./dataset/black7.png"));
	srand(time(NULL));
	ptrMap.resize(4);
	for (std::vector<Chess*> &i : ptrMap)
		i.resize(8, NULL);
	for(int i = 0; i < 16;){
		int n = rand() % 4;
		int m = rand() % 8;
		if (ptrMap.at(n).at(m) == NULL){
			ptrMap.at(n).at(m) = &redChess.at(i);
			redChess.at(i).set_xy(n, m);
			i++;
		}
	}
	for(int i = 0; i < 16;){
		int n = rand() % 4;
		int m = rand() % 8;
		if (ptrMap.at(n).at(m) == 0){
			ptrMap.at(n).at(m) = &blackChess.at(i);
			blackChess.at(i).set_xy(n, m);
			i++;
		}
	}
}
void MainWindow::pressPosition(int x, int y) {
	int row = -1, column = -1;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 8; j++){
			if ((pow((x - (235 + 60 * j)), 2) + pow((y - (235 + 60 * i)), 2)) < 625){
				row = i;
				column = j;
				break;
			}
		}
	}
	if (!invisEat && !contiEat) {
		pressAction(row, column);
		if (ai && ((playerOneRed && !redPlaying) || (!playerOneRed && redPlaying))) {
			QTime dieTime = QTime::currentTime().addMSecs(400);
			while (QTime::currentTime() < dieTime)
				QApplication::processEvents(QEventLoop::AllEvents, 100);
			machinePlaying();
			pressAction(-1, -1);
			update();
		}
	}
	else if (invisEat && !contiEat) {
		pressAction_invis(row, column);
		if (ai && ((playerOneRed && !redPlaying) || (!playerOneRed && redPlaying))) {
			while ((playerOneRed && !redPlaying) || (!playerOneRed && redPlaying)) {
				int n = rand() % 4;
				int m = rand() % 8;
				pressAction(n, m);
			}
			update();
		}
	}
	else if (contiEat) {
		pressAction_conti(row, column);
		if (ai && ((playerOneRed && !redPlaying) || (!playerOneRed && redPlaying))) {
			while ((playerOneRed && !redPlaying) || (!playerOneRed && redPlaying)) {
				int n = rand() % 4;
				int m = rand() % 8;
				pressAction(n, m);
			}
			update();
		}
	}
}
void MainWindow::pressAction(int row, int column) {
	if (row == -1 && column == -1) {
		selected = NULL;
		update();
	}
	else {
		if (ptrMap.at(row).at(column) == NULL) {
			if (selected == NULL)
				update();
			else {
				chessMove(row, column);
			}
		}
		else {
			if (ptrMap.at(row).at(column)->isCovered()) {
				if (playerOneRed == -1) {	//first move(start)
					if (ptrMap.at(row).at(column)->isRed()) {
						playerOneRed = 1;
						redPlaying = true;
						for (int i = 0; i < 16; i++) {
							redChess.at(i).playerOne(true);
							blackChess.at(i).playerOne(false);
						}
					}
					else {
						playerOneRed = 0;
						redPlaying = false;
						for (int i = 0; i < 16; i++) {
							redChess.at(i).playerOne(false);
							blackChess.at(i).playerOne(true);
						}
					}
					setTimer();
				}
				ptrMap.at(row).at(column)->uncovered();
				selected = NULL;
				redPlaying = !redPlaying;
				countDown = 15;
				tieCount++;
				update();
			}
			else {
				if (selected == NULL) {
					if (ptrMap.at(row).at(column)->isRed() == redPlaying) {
						selected = ptrMap.at(row).at(column);
						update();
					}
				}
				else {
					if (ptrMap.at(row).at(column)->isRed() != redPlaying) {
						chessEat(row, column);
					}
					else {
						selected = ptrMap.at(row).at(column);
						update();
					}
				}
			}
		}
	}
	if (tieCount == 15)
		end(1);
	if (redOffTableChess >= 15) {
		redPlaying = !redPlaying;
		end(4);
	}
	else if (blackOffTableChess >= 15) {
		redPlaying = !redPlaying;
		end(3);
	}
}
void MainWindow::pressAction_invis(int row, int column) {
	if (row == -1 && column == -1) {
		selected = NULL;
		update();
	}
	else {
		if (ptrMap.at(row).at(column) == NULL) {
			if (selected == NULL)
				update();
			else {
				chessMove(row, column);
			}
		}
		else {
			if (ptrMap.at(row).at(column)->isCovered()) {
				if (selected == NULL) {
					if (playerOneRed == -1) {	// first move(start)
						if (ptrMap.at(row).at(column)->isRed()) {
							playerOneRed = 1;
							redPlaying = true;
							for (int i = 0; i < 16; i++) {
								redChess.at(i).playerOne(true);
								blackChess.at(i).playerOne(false);
							}
						}
						else {
							playerOneRed = 0;
							redPlaying = false;
							for (int i = 0; i < 16; i++) {
								redChess.at(i).playerOne(false);
								blackChess.at(i).playerOne(true);
							}
						}
						setTimer();
					}
					ptrMap.at(row).at(column)->uncovered();
					selected = NULL;
					redPlaying = !redPlaying;
					countDown = 15;
					tieCount++;
					update();
				}
				else {
					invisibleEat(row, column);
				}
			}
			else {
				if (selected == NULL) {
					if (ptrMap.at(row).at(column)->isRed() == redPlaying) {
						selected = ptrMap.at(row).at(column);
						update();
					}
				}
				else {
					if (ptrMap.at(row).at(column)->isRed() != redPlaying) {
						chessEat(row, column);
					}
					else {
						selected = ptrMap.at(row).at(column);
						update();
					}
				}
			}
		}
	}
	if (tieCount == 15) {
		end(1);
	}
	if (redOffTableChess >= 15) {
		redPlaying = !redPlaying;
		end(4);
	}
	else if (blackOffTableChess >= 15) {
		redPlaying = !redPlaying;
		end(3);
	}
}
void MainWindow::pressAction_conti(int row, int column) {
	if (row == -1 && column == -1 && !startContiEat) {
		selected = NULL;
		update();
	}
	else {
		if (ptrMap.at(row).at(column) == NULL && !startContiEat) {
			if (selected == NULL)
				update();
			else
				chessMove(row, column);
		}
		else if (ptrMap.at(row).at(column) != NULL) {
			if (ptrMap.at(row).at(column)->isCovered()) {
				if (selected == NULL && !startContiEat) {
					if (playerOneRed == -1) {	// first move(start)
						if (ptrMap.at(row).at(column)->isRed()) {
							playerOneRed = 1;
							redPlaying = true;
							for (int i = 0; i < 16; i++) {
								redChess.at(i).playerOne(true);
								blackChess.at(i).playerOne(false);
							}
						}
						else {
							playerOneRed = 0;
							redPlaying = false;
							for (int i = 0; i < 16; i++) {
								redChess.at(i).playerOne(false);
								blackChess.at(i).playerOne(true);
							}
						}
						setTimer();
					}
					ptrMap.at(row).at(column)->uncovered();
					selected = NULL;
					redPlaying = !redPlaying;
					countDown = 15;
					tieCount++;
					update();
				}
				else {
					invisibleEat_conti(row, column);
				}
			}
			else {
				if (selected == NULL && !startContiEat) {
					if (ptrMap.at(row).at(column)->isRed() == redPlaying) {
						selected = ptrMap.at(row).at(column);
						update();
					}
				}
				else {
					if (ptrMap.at(row).at(column)->isRed() != redPlaying) {
						chessEat_conti(row, column);
					}
					else if (!startContiEat) {
						selected = ptrMap.at(row).at(column);
						update();
					}
				}
			}
		}
	}
	// End dictection
	if (tieCount == 15)
		end(1);
	if (redOffTableChess >= 15) { // black win
		end(4);
	}
	else if (blackOffTableChess >= 15) { // red win
		end(3);
	}
}
void MainWindow::chessEat(int x, int y){
	if (eatCheck(x, y)){
		if (ptrMap.at(x).at(y)->isRed()) {
			redOffTableChess++;
			ptrMap.at(x).at(y)->offTable(redOffTableChess);
		}
		else {
			blackOffTableChess++;
			ptrMap.at(x).at(y)->offTable(blackOffTableChess);
		}
		ptrMap.at(selected->get_x()).at(selected->get_y()) = NULL;
		ptrMap.at(x).at(y) = selected;
		selected = NULL;
		ptrMap.at(x).at(y)->set_xy(x, y);
		redPlaying = !redPlaying;
		tieCount = 0;
		countDown = 15;
		update();
	}
	else {
		selected = NULL;
		update();
	}
}
bool MainWindow::eatCheck(int to_x, int to_y){
	if (selected->getHierarchy() != 6) {
		bool adjacent = (abs(selected->get_x() - to_x) == 1 && selected->get_y() == to_y) || (abs(selected->get_y() - to_y) == 1 && selected->get_x() == to_x);
		if (adjacent) {
			if (selected->getHierarchy() == 1) {
				if (1 <= ptrMap.at(to_x).at(to_y)->getHierarchy() && ptrMap.at(to_x).at(to_y)->getHierarchy() != 7)
					return true;
				else
					return false;
			}
			else if (selected->getHierarchy() == 7) {
				if (ptrMap.at(to_x).at(to_y)->getHierarchy() == 1 || ptrMap.at(to_x).at(to_y)->getHierarchy() == 7)
					return true;
				else
					return false;
			}
			else {
				if (selected->getHierarchy() <= ptrMap.at(to_x).at(to_y)->getHierarchy())
					return true;
				else
					return false;
			}
		}
		else
			return false;
	}
	else if (selected->getHierarchy() == 6) {
		int count = 0;
		if (selected->get_x() == to_x && selected->get_y() != to_y) {
			if (selected->get_y() < to_y) {
				for (int i = selected->get_y() + 1; i < to_y; i++) {
					if (ptrMap.at(to_x).at(i) != NULL)
						count++;
				}
			}
			else {
				for (int i = selected->get_y() - 1; i > to_y; i--) {
					if (ptrMap.at(to_x).at(i) != NULL)
						count++;
				}
			}
		}
		else if (selected->get_y() == to_y && selected->get_x() != to_x) {
			if (selected->get_x() < to_x) {
				for (int i = selected->get_x() + 1; i < to_x; i++) {
					if (ptrMap.at(i).at(to_y) != NULL)
						count++;
				}
			}
			else {
				for (int i = selected->get_x() - 1; i > to_x; i--) {
					if (ptrMap.at(i).at(to_y) != NULL)
						count++;
				}
			}
		}
		else
			return false;
		if (count == 1)
			return true;
		else
			return false;
	}
	else
		return false;
}
void MainWindow::invisibleEat(int x, int y) {
	if (eatCheck(x, y) && ptrMap.at(x).at(y)->isRed() != redPlaying) {
		ptrMap.at(x).at(y)->uncovered();
		if (ptrMap.at(x).at(y)->isRed()) {
			redOffTableChess++;
			ptrMap.at(x).at(y)->offTable(redOffTableChess);
		}
		else {
			blackOffTableChess++;
			ptrMap.at(x).at(y)->offTable(blackOffTableChess);
		}
		ptrMap.at(selected->get_x()).at(selected->get_y()) = NULL;
		ptrMap.at(x).at(y) = selected;
		selected = NULL;
		ptrMap.at(x).at(y)->set_xy(x, y);
		redPlaying = !redPlaying;
		tieCount = 0;
		countDown = 15;
		update();
	}
	else {
		ptrMap.at(x).at(y)->uncovered();
		selected = NULL;
		redPlaying = !redPlaying;
		countDown = 15;
		tieCount++;
		update();
	}
}
void MainWindow::invisibleEat_conti(int x, int y) {
	if (!startContiEat) {
		if (eatCheck(x, y) && ptrMap.at(x).at(y)->isRed() != redPlaying) {
			ptrMap.at(x).at(y)->uncovered();
			if (ptrMap.at(x).at(y)->isRed()) {
				redOffTableChess++;
				ptrMap.at(x).at(y)->offTable(redOffTableChess);
			}
			else {
				blackOffTableChess++;
				ptrMap.at(x).at(y)->offTable(blackOffTableChess);
			}
			ptrMap.at(selected->get_x()).at(selected->get_y()) = NULL;
			ptrMap.at(x).at(y) = selected;
			ptrMap.at(x).at(y)->set_xy(x, y);
			tieCount = 0;
			startContiEat = true;
			update();
		}
		else {
			ptrMap.at(x).at(y)->uncovered();
			selected = NULL;
			redPlaying = !redPlaying;
			countDown = 15;
			tieCount++;
			update();
		}
	}
	else {
		bool canEatPos;
		if (selected->getHierarchy() == 6) {
			int count = 0;
			if (selected->get_x() == x && selected->get_y() != y) {
				if (selected->get_y() < y) {
					for (int i = selected->get_y() + 1; i < y; i++) {
						if (ptrMap.at(x).at(i) != NULL)
							count++;
					}
				}
				else {
					for (int i = selected->get_y() - 1; i > y; i--) {
						if (ptrMap.at(x).at(i) != NULL)
							count++;
					}
				}
			}
			else if (selected->get_y() == y && selected->get_x() != x) {
				if (selected->get_x() < x) {
					for (int i = selected->get_x() + 1; i < x; i++) {
						if (ptrMap.at(i).at(y) != NULL)
							count++;
					}
				}
				else {
					for (int i = selected->get_x() - 1; i > x; i--) {
						if (ptrMap.at(i).at(y) != NULL)
							count++;
					}
				}
			}
			else
				canEatPos = false;
			if (count == 1)
				canEatPos = true;
			else
				canEatPos = false;
		}
		else
			canEatPos = (abs(selected->get_x() - x) == 1 && selected->get_y() == y) || (abs(selected->get_y() - y) == 1 && selected->get_x() == x);
		if (canEatPos) {
			if (eatCheck(x, y) && ptrMap.at(x).at(y)->isRed() != redPlaying) {
				ptrMap.at(x).at(y)->uncovered();
				if (ptrMap.at(x).at(y)->isRed()) {
					redOffTableChess++;
					ptrMap.at(x).at(y)->offTable(redOffTableChess);
				}
				else {
					blackOffTableChess++;
					ptrMap.at(x).at(y)->offTable(blackOffTableChess);
				}
				ptrMap.at(selected->get_x()).at(selected->get_y()) = NULL;
				ptrMap.at(x).at(y) = selected;
				ptrMap.at(x).at(y)->set_xy(x, y);
				tieCount = 0;
				update();
			}
			else {
				ptrMap.at(x).at(y)->uncovered();
				selected = NULL;
				redPlaying = !redPlaying;
				startContiEat = false;
				countDown = 15;
				tieCount++;
				update();
			}
		}
	}
}
void MainWindow::chessEat_conti(int x, int y) {
	if (!startContiEat) {
		if (eatCheck(x, y)) {
			if (ptrMap.at(x).at(y)->isRed()) {
				redOffTableChess++;
				ptrMap.at(x).at(y)->offTable(redOffTableChess);
			}
			else {
				blackOffTableChess++;
				ptrMap.at(x).at(y)->offTable(blackOffTableChess);
			}
			ptrMap.at(selected->get_x()).at(selected->get_y()) = NULL;
			ptrMap.at(x).at(y) = selected;
			ptrMap.at(x).at(y)->set_xy(x, y);
			tieCount = 0;
			startContiEat = true;
			update();
		}
		else {
			selected = NULL;
			update();
		}
	}
	else {
		if (eatCheck(x, y)) {
			if (ptrMap.at(x).at(y)->isRed()) {
				redOffTableChess++;
				ptrMap.at(x).at(y)->offTable(redOffTableChess);
			}
			else {
				blackOffTableChess++;
				ptrMap.at(x).at(y)->offTable(blackOffTableChess);
			}
			ptrMap.at(selected->get_x()).at(selected->get_y()) = NULL;
			ptrMap.at(x).at(y) = selected;
			ptrMap.at(x).at(y)->set_xy(x, y);
			tieCount = 0;
			update();
		}
	}
}
void MainWindow::chessMove(int x, int y) {
	if (moveCheck(x, y)) {
		ptrMap.at(selected->get_x()).at(selected->get_y()) = NULL;
		ptrMap.at(x).at(y) = selected;
		ptrMap.at(x).at(y)->set_xy(x, y);
		selected = NULL;
		redPlaying = !redPlaying;
		countDown = 15;
		tieCount++;
		update();
	}
	else {
		selected = NULL;
		update();
	}
}
bool MainWindow::moveCheck(int to_x, int to_y) {
	bool adjacent = (abs(selected->get_x() - to_x) == 1 && selected->get_y() == to_y) || (abs(selected->get_y() - to_y) == 1 && selected->get_x() == to_x);
	if (adjacent && ptrMap.at(to_x).at(to_y) == NULL)
		return true;
	else
		return false;
}
void MainWindow::end(int n) {
	update();
	endGame = true;
	disconnect(timer, SIGNAL(timeout()), this, SLOT(countTotalTime()));
	setEnabled(false);
	if (n == 1) {	// tie
		endPic = "./dataset/tie.png";
	}
	else if (n == 2) {	//out of time
		if ((redPlaying && playerOneRed) || (!redPlaying && !playerOneRed))
			endPic = "./dataset/outOfTime1.png";
		else
			endPic = "./dataset/outOfTime2.png";
	}
	else if (n == 3) { // red win
		if (playerOneRed)
			endPic = "./dataset/player1win.png";
		else
			endPic = "./dataset/player2win.png";
	}
	else if (n == 4) {	// black win
		if (playerOneRed)
			endPic = "./dataset/player2win.png";
		else
			endPic = "./dataset/player1win.png";
	}
	update();
}
void MainWindow::setInvisable(bool checked) {
	if (checked)
		invisEat = true;
	else if (!checked && !contiEat)
		invisEat = false;
	else if (!checked && contiEat)
		invisCheck->setChecked(true);
}
void MainWindow::setContinuously(bool checked) {
	if (checked) {
		contiEat = true;
		invisCheck->setChecked(true);
		invisEat = true;
	}
	else
		contiEat = false;
}
void MainWindow::setAICheck(bool checked) {
	if (checked) {
		ai = true;
	}
	else {
		ai = false;
	}
}
void MainWindow::machinePlaying() {
	bool allCovered = true;
	bool canEat = false;
	int highestHierarchy = 7;
	int from_x;
	int from_y;
	int to_x;
	int to_y;
	selected = NULL;

	if (playerOneRed) {
		for (int i = 0; i < 16; i++) {
			if (!blackChess.at(i).isCovered() && blackChess.at(i).getOnTable()) {
				allCovered = false;
				selected = &blackChess.at(i);
				if (blackChess.at(i).getHierarchy() != 6) {
					if (blackChess.at(i).get_x() != 0) {
						if (ptrMap.at(blackChess.at(i).get_x() - 1).at(blackChess.at(i).get_y()) != NULL) {
							if (!ptrMap.at(blackChess.at(i).get_x() - 1).at(blackChess.at(i).get_y())->isCovered() && ptrMap.at(blackChess.at(i).get_x() - 1).at(blackChess.at(i).get_y())->isRed()) {
								if (eatCheck(blackChess.at(i).get_x() - 1, blackChess.at(i).get_y())) {
									canEat = true;
									if (ptrMap.at(blackChess.at(i).get_x() - 1).at(blackChess.at(i).get_y())->getHierarchy() <= highestHierarchy) {
										highestHierarchy = ptrMap.at(blackChess.at(i).get_x() - 1).at(blackChess.at(i).get_y())->getHierarchy();
										from_x = blackChess.at(i).get_x();
										from_y = blackChess.at(i).get_y();
										to_x = blackChess.at(i).get_x() - 1;
										to_y = blackChess.at(i).get_y();
									}
								}
							}
						}
					}
					if (blackChess.at(i).get_x() != 3) {
						if (ptrMap.at(blackChess.at(i).get_x() + 1).at(blackChess.at(i).get_y()) != NULL) {
							if (!ptrMap.at(blackChess.at(i).get_x() + 1).at(blackChess.at(i).get_y())->isCovered() && ptrMap.at(blackChess.at(i).get_x() + 1).at(blackChess.at(i).get_y())->isRed()) {
								if (eatCheck(blackChess.at(i).get_x() + 1, blackChess.at(i).get_y())) {
									canEat = true;
									if (ptrMap.at(blackChess.at(i).get_x() + 1).at(blackChess.at(i).get_y())->getHierarchy() <= highestHierarchy) {
										highestHierarchy = ptrMap.at(blackChess.at(i).get_x() + 1).at(blackChess.at(i).get_y())->getHierarchy();
										from_x = blackChess.at(i).get_x();
										from_y = blackChess.at(i).get_y();
										to_x = blackChess.at(i).get_x() + 1;
										to_y = blackChess.at(i).get_y();
									}
								}
							}
						}
					}
					if (blackChess.at(i).get_y() != 0) {
						if (ptrMap.at(blackChess.at(i).get_x()).at(blackChess.at(i).get_y() - 1) != NULL) {
							if (!ptrMap.at(blackChess.at(i).get_x()).at(blackChess.at(i).get_y() - 1)->isCovered() && ptrMap.at(blackChess.at(i).get_x()).at(blackChess.at(i).get_y() - 1)->isRed()) {
								if (eatCheck(blackChess.at(i).get_x(), blackChess.at(i).get_y() - 1)) {
									canEat = true;
									if (ptrMap.at(blackChess.at(i).get_x()).at(blackChess.at(i).get_y() - 1)->getHierarchy() <= highestHierarchy) {
										highestHierarchy = ptrMap.at(blackChess.at(i).get_x()).at(blackChess.at(i).get_y() - 1)->getHierarchy();
										from_x = blackChess.at(i).get_x();
										from_y = blackChess.at(i).get_y();
										to_x = blackChess.at(i).get_x();
										to_y = blackChess.at(i).get_y() - 1;
									}
								}
							}
						}
					}
					if (blackChess.at(i).get_y() != 7) {
						if (ptrMap.at(blackChess.at(i).get_x()).at(blackChess.at(i).get_y() + 1) != NULL) {
							if (!ptrMap.at(blackChess.at(i).get_x()).at(blackChess.at(i).get_y() + 1)->isCovered() && ptrMap.at(blackChess.at(i).get_x()).at(blackChess.at(i).get_y() + 1)->isRed()) {
								if (eatCheck(blackChess.at(i).get_x(), blackChess.at(i).get_y() + 1)) {
									canEat = true;
									if (ptrMap.at(blackChess.at(i).get_x()).at(blackChess.at(i).get_y() + 1)->getHierarchy() <= highestHierarchy) {
										highestHierarchy = ptrMap.at(blackChess.at(i).get_x()).at(blackChess.at(i).get_y() + 1)->getHierarchy();
										from_x = blackChess.at(i).get_x();
										from_y = blackChess.at(i).get_y();
										to_x = blackChess.at(i).get_x();
										to_y = blackChess.at(i).get_y() + 1;
									}
								}
							}
						}
					}
				}
				else {
					for (int n = 0; n < 4 && n != blackChess.at(i).get_x(); n++) {
						if (ptrMap.at(n).at(blackChess.at(i).get_y()) != NULL) {
							if (ptrMap.at(n).at(blackChess.at(i).get_y())->isRed() && !ptrMap.at(n).at(blackChess.at(i).get_y())->isCovered()) {
								if (eatCheck(n, blackChess.at(i).get_y())) {
									canEat = true;
									highestHierarchy = 1;
									from_x = blackChess.at(i).get_x();
									from_y = blackChess.at(i).get_y();
									to_x = n;
									to_y = blackChess.at(i).get_y();
								}
							}
						}
					}
					for (int n = 0; n < 8 && n != blackChess.at(i).get_y(); n++) {
						if (ptrMap.at(blackChess.at(i).get_x()).at(n) != NULL) {
							if (ptrMap.at(blackChess.at(i).get_x()).at(n)->isRed() && !ptrMap.at(blackChess.at(i).get_x()).at(n)->isCovered()) {
								if (eatCheck(blackChess.at(i).get_x(), n)) {
									canEat = true;
									highestHierarchy = 1;
									from_x = blackChess.at(i).get_x();
									from_y = blackChess.at(i).get_y();
									to_x = blackChess.at(i).get_x();
									to_y = n;
								}
							}
						}

					}
				}
				selected = NULL;
			}
		}
	}
	else {
		for (int i = 0; i < 16; i++) {
			if (!redChess.at(i).isCovered() && redChess.at(i).getOnTable()) {
				allCovered = false;
				selected = &redChess.at(i);
				if (redChess.at(i).getHierarchy() != 6) {
					if (redChess.at(i).get_x() != 0) {
						if (ptrMap.at(redChess.at(i).get_x() - 1).at(redChess.at(i).get_y()) != NULL) {
							if (!ptrMap.at(redChess.at(i).get_x() - 1).at(redChess.at(i).get_y())->isCovered() && !ptrMap.at(redChess.at(i).get_x() - 1).at(redChess.at(i).get_y())->isRed()) {
								if (eatCheck(redChess.at(i).get_x() - 1, redChess.at(i).get_y())) {
									canEat = true;
									if (ptrMap.at(redChess.at(i).get_x() - 1).at(redChess.at(i).get_y())->getHierarchy() <= highestHierarchy) {
										highestHierarchy = ptrMap.at(redChess.at(i).get_x() - 1).at(redChess.at(i).get_y())->getHierarchy();
										from_x = redChess.at(i).get_x();
										from_y = redChess.at(i).get_y();
										to_x = redChess.at(i).get_x() - 1;
										to_y = redChess.at(i).get_y();
									}
								}
							}
						}
					}
					if (redChess.at(i).get_x() != 3) {
						if (ptrMap.at(redChess.at(i).get_x() + 1).at(redChess.at(i).get_y()) != NULL) {
							if (!ptrMap.at(redChess.at(i).get_x() + 1).at(redChess.at(i).get_y())->isCovered() && !ptrMap.at(redChess.at(i).get_x() + 1).at(redChess.at(i).get_y())->isRed()) {
								if (eatCheck(redChess.at(i).get_x() + 1, redChess.at(i).get_y())) {
									canEat = true;
									if (ptrMap.at(redChess.at(i).get_x() + 1).at(redChess.at(i).get_y())->getHierarchy() <= highestHierarchy) {
										highestHierarchy = ptrMap.at(redChess.at(i).get_x() + 1).at(redChess.at(i).get_y())->getHierarchy();
										from_x = redChess.at(i).get_x();
										from_y = redChess.at(i).get_y();
										to_x = redChess.at(i).get_x() + 1;
										to_y = redChess.at(i).get_y();
									}
								}
							}
						}
					}
					if (redChess.at(i).get_y() != 0) {
						if (ptrMap.at(redChess.at(i).get_x()).at(redChess.at(i).get_y() - 1) != NULL) {
							if (!ptrMap.at(redChess.at(i).get_x()).at(redChess.at(i).get_y() - 1)->isCovered() && !ptrMap.at(redChess.at(i).get_x()).at(redChess.at(i).get_y() - 1)->isRed()) {
								if (eatCheck(redChess.at(i).get_x(), redChess.at(i).get_y() - 1)) {
									canEat = true;
									if (ptrMap.at(redChess.at(i).get_x()).at(redChess.at(i).get_y() - 1)->getHierarchy() <= highestHierarchy) {
										highestHierarchy = ptrMap.at(redChess.at(i).get_x()).at(redChess.at(i).get_y() - 1)->getHierarchy();
										from_x = redChess.at(i).get_x();
										from_y = redChess.at(i).get_y();
										to_x = redChess.at(i).get_x();
										to_y = redChess.at(i).get_y() - 1;
									}
								}
							}
						}
					}
					if (redChess.at(i).get_y() != 7) {
						if (ptrMap.at(redChess.at(i).get_x()).at(redChess.at(i).get_y() + 1) != NULL) {
							if (!ptrMap.at(redChess.at(i).get_x()).at(redChess.at(i).get_y() + 1)->isCovered() && !ptrMap.at(redChess.at(i).get_x()).at(redChess.at(i).get_y() + 1)->isRed()) {
								if (eatCheck(redChess.at(i).get_x(), redChess.at(i).get_y() + 1)) {
									canEat = true;
									if (ptrMap.at(redChess.at(i).get_x()).at(redChess.at(i).get_y() + 1)->getHierarchy() <= highestHierarchy) {
										highestHierarchy = ptrMap.at(redChess.at(i).get_x()).at(redChess.at(i).get_y() + 1)->getHierarchy();
										from_x = redChess.at(i).get_x();
										from_y = redChess.at(i).get_y();
										to_x = redChess.at(i).get_x();
										to_y = redChess.at(i).get_y() + 1;
									}
								}
							}
						}
					}
				}
				else {
					for (int n = 0; n < 4 && n != redChess.at(i).get_x(); n++) {
						if (ptrMap.at(n).at(redChess.at(i).get_y()) != NULL) {
							if (!ptrMap.at(n).at(redChess.at(i).get_y())->isRed() && !ptrMap.at(n).at(redChess.at(i).get_y())->isCovered()) {
								if (eatCheck(n, redChess.at(i).get_y())) {
									canEat = true;
									highestHierarchy = 1;
									from_x = redChess.at(i).get_x();
									from_y = redChess.at(i).get_y();
									to_x = n;
									to_y = redChess.at(i).get_y();
								}
							}
						}
					}
					for (int n = 0; n < 8 && n != redChess.at(i).get_y(); n++) {
						if (ptrMap.at(redChess.at(i).get_x()).at(n) != NULL) {
							if (!ptrMap.at(redChess.at(i).get_x()).at(n)->isRed() && !ptrMap.at(redChess.at(i).get_x()).at(n)->isCovered()) {
								if (eatCheck(redChess.at(i).get_x(), n)) {
									canEat = true;
									highestHierarchy = 1;
									from_x = redChess.at(i).get_x();
									from_y = redChess.at(i).get_y();
									to_x = redChess.at(i).get_x();
									to_y = n;
								}
							}
						}

					}
				}
				selected = NULL;
			}
		}
	}

	if (allCovered) {
		while (true) {
			int n = rand() % 4;
			int m = rand() % 8;
			if (ptrMap.at(n).at(m) != NULL) {
				if (ptrMap.at(n).at(m)->isCovered()) {
					ptrMap.at(n).at(m)->uncovered();
					selected = NULL;
					redPlaying = !redPlaying;
					countDown = 15;
					tieCount++;
					break;
				}
			}
		}
	}
	else {
		if (canEat) {
			selected = ptrMap.at(from_x).at(from_y);
			chessEat(to_x, to_y);
		}
		else {
			while ((playerOneRed && !redPlaying) || (!playerOneRed && redPlaying)) {
				int n = rand() % 4;
				int m = rand() % 8;
				pressAction(n, m);
			}
		}
	}
	selected = NULL;
}