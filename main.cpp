#include "mainwindow.h"
#include <QApplication>
#include <QImage>
#include <QPainter>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	a.setStyleSheet("QWidget {background-image: url(./dataset/bg.png) }");
	w.show();
	return a.exec();
}