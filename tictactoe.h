/*
 * tictactoe.h
 *
 *  Created on: Mar 5, 2018
 *      Author: Nick Nesbit
 */

#ifndef TICTACTOE_H_
#define TICTACTOE_H_

//Includes
#include <ctime>

//Declare TicTacToe class
class TicTacToe
{
	private:
		//struct Node
		struct Node {
			int state[16];
			int depth;
			int successors;
			Node** children;
		};

		//Private variables
		Node* root;				//The root node
		bool playerWin;			//Flag for player win
		bool maxWin;			//Flag for maximizer win
		bool minWin;			//Flag for minimizer win
		int board[16];			//Array to hold board state
		int nextMove[2];		//Array to hold the next action
		int gameType;			//Determines whether watching or playing

		//Used for iterative deepening timer
		time_t start;
		time_t end;
		time_t diff;

		//Private member functions
		Node* makeNode(int [16], int, int);
		void insertNode(Node *&, Node *&);
		void destroySubTree(Node*);
		void watchGame(int [8], int [8], int [8], int [8]);
		void playGame(int [8], int [8], int [8], int [8]);
		void printBoard();
		bool checkWin(int [16]);
		int getRandMove(int [8]);
		int getRandPos(int [16]);
		int alphaBeta(Node*, int, int, int, int, int, int[8], int [8], int [8], int [8]);

	public:

		//Public member functions
		TicTacToe();
		void launchGame();

};

#endif /* TICTACTOE_H_ */
