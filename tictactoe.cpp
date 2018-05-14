/*
 * tictactoe.cpp
 *
 *  Created on: Mar 5, 2018
 *      Author: Nick Nesbit
 */

//Includes
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <ctime>

#include "tictactoe.h"

//Using namespace std
using namespace std;

/*****************************************
 * TicTacToe() - Class Constructor       *
 *****************************************/
TicTacToe::TicTacToe()
{
	//Seed the random number generator
	srand(time(0));

	//Set root to NULL
	root = NULL;

	//Initialize the board
	for (int i = 0; i < 16; i++)
		board[i] = 0;

	//Initialize the next move
	nextMove[0] = -1;
	nextMove[1] = -1;

	//Initialize game type variable
	gameType = -1;

	//Initialize timer variables
	start = 0;
	end = 0;
	diff = 0;

	//Initialize outcome variables
	playerWin = false;
	maxWin = false;
	minWin = false;
}

/*************************************************
 * destroySubTree(Node*) - Tree Destructor       *
 *************************************************/
void TicTacToe::destroySubTree(Node *nodePtr)
{
	//Recursivley delete nodes
	if (nodePtr) {
		for (int i = 0; i < nodePtr->successors; i++)
			if (nodePtr->children[i])
				destroySubTree(nodePtr->children[i]);
		//printf("Node %p deleted at depth %d\n", nodePtr, nodePtr->depth);
		delete nodePtr;
	}
}

/*************************************************
 * launchGame(Node*) - Sets up game detail       *
 *************************************************/
void TicTacToe::launchGame()
{
	//String used to hold user input
	string input;

	//Get gametype
	do {
		cin.clear();
		cout << "Enter 0 to watch or enter 1 to play: ";
		cin >> input;
		cin.ignore();
	} while ( (input[0] != '0' && input[0] != '1') || !(isdigit(input[0])) || (input.length() != 1) );

	//If watching:
	if (input[0] == '0') {
		//Setup moves and call watchGame()
		gameType = 0;
		int odds[8] = {1, 3, 5, 7, 9, 11, 13, 15};
		int oddFlags[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		int evens[8] = {2, 4, 6, 8, 10, 12, 14, 16};
		int evenFlags[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		watchGame(odds, oddFlags, evens, evenFlags);
	}
	//Else if playing:
	else {
		//Set up flags
		gameType = 1;
		int playFlags[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		int aiFlags[8] = {0, 0, 0, 0, 0, 0, 0, 0};

		//Get move set
		do {
			cin.clear();
			cout << "Enter 1 for odd numbers (first move) or 2 for even numbers (last move): ";
			cin >> input;
			cin.ignore();
		} while ( (input[0] != '1' && input[0] != '2') || !(isdigit(input[0])) || (input.length() != 1) );

		//Set up moves and call playGame()
		if (input[0] == '1') {
			int playMoves[8] =  {1, 3, 5, 7, 9, 11, 13, 15};
			int aiMoves[8] = {2, 4, 6, 8, 10, 12, 14, 16};
			playGame(playMoves, playFlags, aiMoves, aiFlags);
		}
		else if (input[0] == '2') {
			int playMoves[8] = {2, 4, 6, 8, 10, 12, 14, 16};
			int aiMoves[8] = {1, 3, 5, 7, 9, 11, 13, 15};
			playGame(playMoves, playFlags, aiMoves, aiFlags);
		}
	}

	//Display game outcome
	if (playerWin == true)
		cout << "Game Over. Player won." << endl;
	else if (maxWin == true)
		cout << "Game Over. Maximizer AI won." << endl;
	else if (minWin == true)
		cout << "Game Over. Minimizer AI won." << endl;
	else
		cout << "Game Over. Tie Game." << endl;
}

/********************************************************************************
 * playGame(int [8], int [8], int [8], int [8]) - Method for playing against AI *
 ********************************************************************************/
void TicTacToe::playGame(int pMoves[8], int pFlgs[8], int aiMoves[8], int aiFlgs[8])
{
	//Initialize the turn to AI
	int turn = 0;

	//Change turn to player if player is odds
	if (pMoves[0] == 1)
		turn = 3;

	//Variables to hold player move
	int pos;
	int move;

	//Variables used for iterative deepening
	int limit = 0;
	int depth = 0;

	//alpha and Beta
	int a;
	int b;

	//Print starting board
	printBoard();

	//While the game is not over:
	while (checkWin(board) == false && depth < 16) {
		//AI turn
		if (turn == 0) {
			//Set alpha and beta to "infinity"
			a = -10;
			b = 10;

			//Compute number of moves for AI
			int moves = 0;
			if (turn == 0) {
				for (int i = 0; i < 8; i++)
					if (aiFlgs[i] == 0)
						moves++;
			}

			//Prompt to continue
			cout << "Press Enter to Continue";
			cin.ignore(std::numeric_limits<streamsize>::max(),'\n');

			//Start the iterative deepening timer
			start = time(0);

			//While depth limit is feasible:
			while (limit <= (16-depth)) {
				//Create and insert a new root node
				Node* node = makeNode(board, depth, moves);
				insertNode(root, node);

				//Get the best next move with alphaBeta method
				alphaBeta(node, depth+1, limit, a, b, turn, aiFlgs, pFlgs, aiMoves, pMoves);

				//Increment the limit
				limit++;

				//Destroy the current tree
				destroySubTree(root);
				root = NULL;

				//Break if more than two seconds have passed
				if (diff > 2)
					break;
			}

			//Perfom the move
			cout << "Move " << depth << endl;
			cout << nextMove[0] << ", " << nextMove[1] << endl;
			board[nextMove[1]] = nextMove[0];						//nextMove[2] is a class member and is set in alphaBeta() method

			//Set the flag for the played move
			for (int i = 0; i < 8; i++) {
				if (aiMoves[i] == nextMove[0]) {
					aiFlgs[i] = 1;
					break;
				}
			}

			//Reset utility variables
			limit = 0;
			diff = 0;

			//Increment the depth
			depth++;

			//Print the current board
			printBoard();

			//Check for win
			if (checkWin(board) == true)
				maxWin = true;
		}
		//User turn
		else if (turn == 3) {
			//Display available moves
			cout << "Available numbers: " << endl;
			for (int i = 0; i < 8; i++)
				if (pFlgs[i] == 0)
					cout << pMoves[i] << " ";
			cout << endl;

			//Get the desired board position
			do {
				cin.clear();
				cout << "Enter board postition (0-15): ";
				cin >> pos;
				cin.ignore();
			} while ( pos < 0 || pos > 15 || board[pos] > 0);

			//Get the desired move number
			int checkNum = 0;
			while (checkNum == 0) {
				do {
					cin.clear();
					cout << "Enter a move number: ";
					cin >> move;
					cin.ignore();
				} while ( move < pMoves[0] || move > pMoves[7] );

				//Set the flag for the chosen move
				for (int i = 0; i < 8; i++) {
					if (pMoves[i] == move && pFlgs[i] == 0) {
						checkNum = 1;
						pFlgs[i] = 1;
						break;
					}
				}
			}

			//Perform the move
			cout << "Move " << depth << endl;
			cout << move << ", " << pos << endl;
			board[pos] = move;

			//Increment the depth
			depth++;

			//Print the current board
			printBoard();

			//Check for win
			if (checkWin(board) == true)
				playerWin = true;
		}

		//Alternate the turn
		if ( turn == 0 )
			turn = 3;
		else
			turn = 0;
	}
}

/********************************************************************************
 * watchGame(int [8], int [8], int [8], int [8]) - Method for watching AIs      *
 ********************************************************************************/
void TicTacToe::watchGame(int odds[8], int oddFlgs[8], int evens[8], int evenFlgs[8])
{
	//Initialize utility variables
	int limit = 0;
	int depth = 0;
	int turn = 0;
	int a = 0;
	int b = 0;

	//Print the starting board
	printBoard();

	//While the game is not over:
	while (checkWin(board) == false && depth < 16) {
		//Set alpha and beta to "infinity"
		a = -10;
		b = 10;

		//Get the number of moves for the current player
		int moves = 0;
		if (turn == 0){
			for (int i = 0; i < 8; i++)
				if (oddFlgs[i] == 0)
					moves++;
		}
		else {
			for (int i = 0; i < 8; i++)
				if (evenFlgs[i] == 0)
					moves++;
		}

		//Prompt user to continue
		cout << "Press Enter to Continue";
		cin.ignore(std::numeric_limits<streamsize>::max(),'\n');

		//Start the iterative deepening timer
		start = time(0);

		//While the depth limit is feasible
		while (limit <= (16-depth)) {
			//Create and insert a new root node
			Node* node = makeNode(board, depth, moves);
			insertNode(root, node);

			//Get the best next move with alphabeta() method
			alphaBeta(node, depth+1, limit, a, b, turn, oddFlgs, evenFlgs, odds, evens);

			//Increment the limit
			limit++;

			//Destroy the current tree
			destroySubTree(root);
			root = NULL;

			//Break if more than 2 seconds have passed
			if (diff > 2)
				break;
		}

		//Perfrom the move
		cout << "Move " << depth << endl;
		cout << nextMove[0] << ", " << nextMove[1] << endl;
		board[nextMove[1]] = nextMove[0];

		//Set the flag for the played made
		for (int i = 0; i < 8; i++) {
			if (odds[i] == nextMove[0]) {
				oddFlgs[i] = 1;
				break;
			}
			else if (evens[i] == nextMove[0]) {
				evenFlgs[i] = 1;
				break;
			}
		}

		//Reset utility variables
		limit = 0;
		diff = 0;

		//Increment the depth
		depth++;

		//Print the current board
		printBoard();

		//Check for win
		if (checkWin(board) == true) {
			if (turn == 0)
				maxWin = true;
			else
				minWin = true;
		}

		//Alternate players
		if ( turn == 0 )
			turn = 1;
		else
			turn = 0;
	}
}

/**************************************************************************
 * makeNode(int [16], int, int) - Method for creating a new tree node     *
 **************************************************************************/
TicTacToe::Node* TicTacToe::makeNode(int state[16], int depth, int moves)
{
	//Create a new node
	Node *newNode;
	newNode = new Node;

	//Set the state
	for (int i = 0; i < 16; i++)
		newNode->state[i] = state[i];

	//Set the depth, util, and pos
	newNode->depth = depth;

	//Set number of successors
	newNode->successors = moves * (16 - depth);

	//Initialize the children
	newNode->children = new Node* [newNode->successors];
	for (int i = 0; i < newNode->successors; i++)
		newNode->children[i] = NULL;

	//printf("%p\n", newNode);

	//Return the new node
	return newNode;
}

/**********************************************************************
 * insetNode(Node*&, Node*&) - Method for building a depth first tree *
 **********************************************************************/
void TicTacToe::insertNode(Node*& nodePtr, Node*& newNode)
{
	//Recursivley insert a node to the tree
	if (nodePtr == NULL) {
		nodePtr = newNode;
		//printf("Search node: %p, Depth: %d\n", nodePtr, nodePtr->depth);
	}
	else {
		for (int i = 0; i < nodePtr->successors; i++) {
			if (nodePtr->children[i] == NULL) {
				insertNode(nodePtr->children[i], newNode);
				break;
			}
		}
	}
}

/*********************************************************************
 * getRandMove(int [8]) - Method for getting a valid random move     *
 *********************************************************************/
int TicTacToe::getRandMove(int flags[8])
{
	int num;

	do {
		num = rand()%8;
	} while (flags[num] != 0);

	return num;
}

/************************************************************************
 * getRandPos(int [8]) - Method for getting a valid random position     *
 ************************************************************************/
int TicTacToe::getRandPos(int state[16])
{
	int pos;

	do {
		pos = rand()%16;
	} while (state[pos] != 0);

	return pos;
}

/************************************************************************************************************************
 * alphaBeta(Node*, int, int, int, int, int, int [8], int [8] ,int [8], int[8]) - Method for getting the next best move *
 ************************************************************************************************************************/
int TicTacToe::alphaBeta(Node* node, int depth, int limit, int alpha, int beta, int player, int maxFlgs[8], int minFlgs[8], int maxMovs[8], int minMovs[8])
{
	//Check base cases
	if (checkWin(node->state) && player == 0)
		return -1;
	if(checkWin(node->state) && player == 1)
		return 1;
	if (limit == 0)
		return 0;

	//Set local state array equal to the state of the node parameter
	int state[16];
	for (int i = 0; i < 16; i++)
		state[i] = node->state[i];

	//Calculate the number of move for each player
	int eMoves = 0;
	int oMoves = 0;
	for (int i = 0; i < 8; i++) {
		if (minFlgs[i] == 0)
			eMoves++;
		if (maxFlgs[i] == 0)
			oMoves++;
	}

	//Temp variables to hold next move information
	int tmp1;
	int tmp2;

	//Maximizer:
	if (player == 0) {
		//Let v equal "negative infinity"
		int v = -10;

		//Initialize the temp variables to random values
		tmp1 = getRandMove(maxFlgs);
		tmp2 = getRandPos(state);

		//For all actions in current state:
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 16; j++) {
				//If the action is valid:
				if (maxFlgs[i] == 0 && state[j] == 0) {
					//Set the move flag and update the state
					maxFlgs[i] = 1;
					state[j] = maxMovs[i];

					//Create and insert a new node
					Node* newNode = makeNode(state, depth, oMoves);
					insertNode(node, newNode);

					//Recursively call the alphaBeta function, alternating players
					int newAlpha = alphaBeta(newNode, depth+1, limit-1, alpha, beta, 1, maxFlgs, minFlgs, maxMovs, minMovs);

					//If the newAlpha is greater than or equal to v
					if (newAlpha >= v) {
						//Update v and update temp variables
						v = newAlpha;
						tmp1 = i;
						tmp2 = j;
					}

					//Reset the move flag and state position
					maxFlgs[i] = 0;
					state[j] = 0;

					//Prune if possible
					if (v >= beta) {
						//Return
						nextMove[0] = maxMovs[tmp1];			//nextMove[0] holds the next move number
						nextMove[1] = tmp2;						//nextMove[1] holds the next position
						return v;
					}

					//Update alpha
					alpha = max(alpha, v);
				}

				//Check time duration
				end = time(0);
				diff = end - start;
				if (diff >= 2) {
					//Return
					nextMove[0] = maxMovs[tmp1];
					nextMove[1] = tmp2;
					return v;
				}
			}
		}
		//Return
		nextMove[0] = maxMovs[tmp1];
		nextMove[1] = tmp2;
		return v;
	}

	//Minimizer
	else if (player == 1) {
		//Let v equal "positive infinity"
		int v = 10;

		//Initialize the temp variables to random values
		tmp1 = getRandMove(minFlgs);
		tmp2 = getRandPos(state);
		//For all actions in current state:
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 16; j++) {
				//If the action is valid:
				if (minFlgs[i] == 0 && state[j] == 0) {
					//Set the move flag and update the state
					minFlgs[i] = 1;
					state[j] = minMovs[i];

					//Create and insert a new node
					Node* newNode = makeNode(state, depth, eMoves);
					insertNode(node, newNode);

					//Recursively call the alphaBeta function, alternating players
					int newBeta = alphaBeta(newNode, depth+1, limit-1, alpha, beta, 0, maxFlgs, minFlgs, maxMovs, minMovs);

					//If the newBeta is less than or equal to v
					if (newBeta <= v) {
						//Update v and temp variables
						v = newBeta;
						tmp1 = i;
						tmp2 = j;
					}

					//Reset the move flag and state position
					minFlgs[i] = 0;
					state[j] = 0;

					//Prune if possible
					if (v <= alpha) {
						//Return
						nextMove[0] = minMovs[tmp1];			//nextMove[0] holds the next move number
						nextMove[1] = tmp2;						//netMove[1] holds the next position
						return v;
					}

					//Update beta
					beta = min(beta, v);
				}

				//Check time duration
				end = time(0);
				diff = end - start;
				if (diff >= 2) {
					//Return
					nextMove[0] = minMovs[tmp1];
					nextMove[1] = tmp2;
					return v;
				}
			}
		}
		//Return
		nextMove[0] = minMovs[tmp1];
		nextMove[1] = tmp2;
		return v;
	}

	//Something went wrong if we end up here
	return -1;
}

/**************************************************************
 * printBoard() - Method for printing the current board       *
 **************************************************************/
void TicTacToe::printBoard()
{
	for (int i = 0; i < 16; i++) {
		cout << setw(4) << board[i];
		if ( ((i+1) % 4) == 0)
			cout << endl;
		if (i == 15)
			cout << endl;
	}
}

/**********************************************************************************
 * checkWin(int [16]) - Method for checking if a given state is a winning state   *
 **********************************************************************************/
bool TicTacToe::checkWin(int state[16])
{
	//There is probably a better way to do this
	if ( ( (state[0] + state[1] + state[2] + state[3] == 34)  && ( (state[0] > 0) && (state[1] > 0) && (state[2] > 0) && (state[3] > 0) ) )  ||
		   ( (state[4] + state[5] + state[6] + state[7] == 34) && ( (state[4] > 0) && (state[5] > 0) && (state[6] > 0) && (state[7] > 0) ) ) ||
		   ( (state[8] + state[9] + state[10] + state[11] == 34) && ( (state[8] > 0) && (state[9] > 0) && (state[10] > 0) && (state[11] > 0) ) ) ||
		   ( (state[12] + state[13] + state[14] + state[15] == 34) && ( (state[12] > 0) && (state[13] > 0) && (state[14] > 0) && (state[15] > 0) ) ) ||
		   ( (state[0] + state[4] + state[8] + state[12] == 34) && ( (state[0] > 0) && (state[4] > 0) && (state[8] > 0) && (state[12] > 0) ) ) ||
		   ( (state[1] + state[5] + state[9] + state[13] == 34) && ( (state[1] > 0) && (state[5] > 0) && (state[9] > 0) && (state[13] > 0) ) ) ||
		   ( (state[2] + state[6] + state[10] + state[14] == 34) && ( (state[2] > 0) && (state[6] > 0) && (state[10] > 0) && (state[14] > 0) ) ) ||
		   ( (state[3] + state[7] + state[11] + state[15] == 34) && ( (state[3] > 0) && (state[7] > 0) && (state[11] > 0) && (state[15] > 0) ) ) ||
		   ( (state[0] + state[5] + state[10] + state[15] == 34) && ( (state[0] > 0) && (state[5] > 0) && (state[10] > 0) && (state[15] > 0) ) ) ||
		   ( (state[3] + state[6] + state[9] + state[12] == 34) && ( (state[3] > 0) && (state[6] > 0) && (state[9] > 0) && (state[12] > 0) ) ) ) {
	   //cout << "Win" << endl;
	   return true;
   }
   //cout << "Not win" << endl;
   return false;
}
