#include "Chess.h"
#include <QDebug>

Chess::Chess(bool r, int h, std::string i){
	red = r;
	hierarchy = h;
	tempImage = i;
}
int Chess::pix_x(){
	if (ontable){
		return 210 + 60 * position_y;
	}
	else {
		return offTable_pix_x;
	}
}
int Chess::pix_y(){
	if (ontable){
		return 210 + 60 * position_x;
	}
	else {
		return offTable_pix_y;
	}
}
int Chess::get_x(){
	return position_x;
}
int Chess::get_y(){
	return position_y;
}
void Chess::set_xy(int x, int y){
	position_x = x;
	position_y = y;
}
void Chess::uncovered(){
	image = tempImage;
	covered = false;
}
int Chess::getHierarchy(){
	return hierarchy;
}
bool Chess::isCovered(){
	return covered;
}
bool Chess::isRed(){
	return red;
}
bool Chess::getOnTable() {
	return ontable;
}
void Chess::offTable(int n) {
	ontable = false;
	offTable_pix_y = (n % 6) * 40 + 210;
	if (isPlayerOne) {
		offTable_pix_x = 150 - ((n / 6) * 40);
	}
	else {
		offTable_pix_x = 705 + ((n / 6) * 40);
	}
}
void Chess::playerOne(bool x) {
	isPlayerOne = x;
}
bool Chess::getPlayerOne() {
	return isPlayerOne;
}