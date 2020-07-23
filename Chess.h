#ifndef _CHESS_H_
#define _CHESS_H_

#include <string>
class Chess{
public:
	Chess(bool r, int h, std::string);	// Initialize red, hierarchy, tempImage
	int pix_x();	// pix_x() and pix_y() returns where the chess should be shown
	int pix_y();
	int get_x();	// get_x() and get_y() returns row and column on the map
	int get_y();
	void set_xy(int, int);
	void uncovered();	// uncovers the chess
	void offTable(int);	// when this chess is eaten
	void playerOne(bool);	// To set whether this chess belong to which player
	bool getPlayerOne();
	bool getOnTable();
	int getHierarchy();
	bool isCovered();
	bool isRed();
	std::string image = "./dataset/back.png";	// The covered image
private:
	int position_x;
	int position_y;
	int offTable_pix_x;
	int offTable_pix_y;
	int hierarchy;
	bool red;
	bool isPlayerOne;
	bool covered = true;
	bool ontable = true;
	std::string tempImage;	// The uncovered image
};
#endif
