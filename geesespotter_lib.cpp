#include <cstdlib>
#include <iostream>
#include <cctype>
#include <ctime>
#include "geesespotter_lib.h"
#include "geesespotter.h"

int main()
{
  srand(time(NULL));
  game();
  return 0;
}

bool game()
{
  std::size_t xdim {0};
  std::size_t ydim {0};
  unsigned int numgeese {0};
  char * gameBoard {NULL};

  startGame(gameBoard, xdim, ydim, numgeese);

  char currAction {0};
  while(currAction != 'Q')
  {
    switch (currAction)
    {
      case 'S' :  // show
      {
        actionShow(gameBoard, xdim, ydim, numgeese);
        break;
      }
      case 'M' :  // mark
      {
        actionMark(gameBoard, xdim, ydim);
        break;
      }
      case 'R' :  // restart
      {
        std::cout << "Restarting the game." << std::endl;
        startGame(gameBoard, xdim, ydim, numgeese);
        break;
      }
    }
    printBoard(gameBoard, xdim, ydim);
    if (isGameWon(gameBoard, xdim, ydim))
    {
      std::cout << "You have revealed all the fields without disturbing a goose!" << std::endl;
      std::cout << "YOU WON!!!" << std::endl;
      for (std::size_t reveal_row {0}; reveal_row < ydim; reveal_row++)
      {
        for (std::size_t reveal_col {0}; reveal_col < xdim; reveal_col++)
        {
          gameBoard[reveal_row*xdim + reveal_col] = (gameBoard[reveal_row*xdim + reveal_col] & valueMask());
        }
      }
      printBoard(gameBoard, xdim, ydim);
      std::cout << "Resetting the game board." << std::endl;
      startGame(gameBoard, xdim, ydim, numgeese);
    }
    currAction = getAction();
  }

  cleanBoard(gameBoard);
  return true;
}

void startGame(char * & board, std::size_t & xdim, std::size_t & ydim, unsigned int & numgeese)
{
  std::cout << "Welcome to GeeseSpotter!" << std::endl;
  do {
    std::cout << "Please enter the x dimension (max " << xdim_max() << "): ";
    std::cin >> xdim;
  } while(xdim < 1 || xdim > xdim_max());
  do {
    std::cout << "Please enter the y dimension (max " << ydim_max() << "): ";
    std::cin >> ydim;
  } while(ydim < 1 || ydim > ydim_max());

  std::cout << "Please enter the number of geese: ";
  std::cin >> numgeese;
  while (numgeese > xdim * ydim)
  {
    std::cout << "That's too many geese!" << std::endl;
    std::cout << "Please enter the number of geese: ";
    std::cin >> numgeese;
  }

  cleanBoard(board);
  board = createBoard(xdim, ydim);
  spreadGeese(board, xdim, ydim, numgeese);
  
  computeNeighbors(board, xdim, ydim);
  
  hideBoard(board, xdim, ydim);
  }

char getAction()
{
  char action {0};

  std::cout << "Please enter the action ([S]how, [M]ark, [R]estart, [Q]uit): ";
  std::cin >> action;

  if (islower(action))
    action = toupper(action);

  return action;
}

void actionShow(char * & board, std::size_t & xdim, std::size_t & ydim, unsigned int & numgeese)
{
  std::size_t reveal_x {0};
  std::size_t reveal_y {0};
  std::cout << "Please enter the x location to show: ";
  std::cin >> reveal_x;
  std::cout << "Please enter the y location to show: ";
  std::cin >> reveal_y;

  if (reveal_x >= xdim || reveal_y >= ydim)
  {
    std::cout << "Location entered is not on the board." << std::endl;
  }
  else if (board[xdim*reveal_y + reveal_x] & markedBit())
  {
    std::cout << "Location is marked, and therefore cannot be revealed." << std::endl;
    std::cout << "Use Mark on location to unmark." << std::endl;
  }
  else if (reveal(board, xdim, ydim, reveal_x, reveal_y) == 9)
  {
    std::cout << "You disturbed a goose! Your game has ended." << std::endl;
    printBoard(board, xdim, ydim);
    std::cout << "Starting a new game." << std::endl;
    startGame(board, xdim, ydim, numgeese);
  }
}

void actionMark(char * board, std::size_t xdim, std::size_t ydim)
{
  std::size_t mark_x {0};
  std::size_t mark_y {0};
  std::cout << "Please enter the x location to mark: ";
  std::cin >> mark_x;
  std::cout << "Please enter the y location to mark: ";
  std::cin >> mark_y;

  if (mark_x >= xdim || mark_y >= ydim)
  {
    std::cout << "Location entered is not on the board." << std::endl;
  }
  else if (mark(board, xdim, ydim, mark_x, mark_y) == 2)
  {
    std::cout << "Position already revealed, so cannot be marked." << std::endl;
  }
}

std::size_t readSizeT()
{
  return 0;
}

std::size_t xdim_max()
{
  return 60;
}

std::size_t ydim_max()
{
  return 20;
}

char markedBit()
{
  return 0x10;
}

char hiddenBit()
{
  return 0x20;
}

char valueMask()
{
  return 0x0F;
}

void spreadGeese(char * board, std::size_t xdim, std::size_t ydim, unsigned int numgeese)
{
  if (board != NULL)
  {
    for (unsigned int gen_goose {0}; gen_goose < numgeese; gen_goose++)
    {
      std::size_t try_position {0};
      do {
         try_position = rand() % (xdim * ydim);
      } while (board[try_position] != 0);

      board[try_position] = 9;
    }
  }
}

//------------------------------------------------

char * createBoard(std::size_t xdim, std::size_t ydim) {
  char * board = new char[xdim * ydim];
    for (int i=0; i< xdim * ydim; i++) {
        board[i] = 0;
    }
    return board;
}

void computeNeighbors_helper(char *board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc){
	int co {0};
  std::size_t pos = yloc * xdim + xloc;
	if ((board[pos] & valueMask()) == 0x09){
		return;
	}
  else{
    for (int i = -1; i < 2; i++) {
      for (int j = -1; j < 2; j++) {
        if (((yloc+j) >=0) && ((yloc+j)<ydim) && ((xloc+i)>=0) && ((xloc+i)<xdim)) {
          if ((board[(yloc+j)*xdim +xloc+i] & valueMask()) == 0x09){ 
            co++;
          }
        }
      }
  	}
	board[yloc*xdim + xloc] += co;
	return;
  }
}
void computeNeighbors(char *board, std::size_t xdim, std::size_t ydim){
	int i{0};
	for (i = 0; i < xdim*ydim; i++) {
		computeNeighbors_helper(board,xdim,ydim, i%xdim, i/xdim);
	}
	return;
}

void computeNeighbors(char * board, std::size_t xdim, std::size_t ydim) {
  int geese_count {0};
  int cell_val {0};
  for (int x=0; x<xdim; x++) {
    for (int y=0; y<xdim; y++) {
      geese_count = board[(y*xdim + x)];
      if (geese_count!=9) { 
        continue; 
      }
      if (x-1 >= 0) {  
        if (y-1 >=0) {
          cell_val = board[((y-1)*xdim + x-1)];
          if (cell_val!=9) board[((y-1)*xdim + x-1)] = ++cell_val;
      }
        
        cell_val = board[(y*xdim + x-1)];
        if (cell_val!=9) board[(y*xdim + x-1)] = ++cell_val;

        if (y+1 < ydim) {
          cell_val = board[((y+1)*xdim + x-1)];
          if (cell_val!=9) board[((y+1)*xdim + x-1)] = ++cell_val;
        }
      }
      if (y-1 >=0) {
        cell_val = board[((y-1)*xdim + x)]; 
        if (cell_val!=9) board[((y-1)*xdim + x)] = ++cell_val;
      }

      if (y+1 < ydim) {
          cell_val = board[((y+1)*xdim + x)];
          if (cell_val!=9) board[(y+1)*xdim + x] = ++cell_val;
      }

      if (x+1 < xdim) { 
        if (y-1 >=0) {
          cell_val = board[((y-1)*xdim + x+1)];
          if (cell_val!=9) board[((y-1)*xdim + x+1)] = ++cell_val;
        }
        cell_val = board[(y*xdim + x+1)];
        if (cell_val!=9) board[(y*xdim + x+1)] = ++cell_val;
        if (y+1 < ydim) {
          cell_val = board[((y+1)*xdim + x+1)];
          if (cell_val!=9) board[((y+1)*xdim + x+1)] = ++cell_val;
        }
      }
    }
  }
}

void hideBoard(char * board, std::size_t xdim, std::size_t ydim) {
  int cell_val {0};
  for (int i=0; i< xdim * ydim; i++) {
    cell_val = board[i];
    board[i] = cell_val | hiddenBit();
  }  
}

void cleanBoard(char * board) {
  delete[] board;
	board = nullptr;
}

void printBoard(char *board, std::size_t xdim, std::size_t ydim){
	int counter =0;

	for (std::size_t i{0}; i<xdim*ydim ; ++i){
		if (counter == xdim){
			counter = 0;
			std::cout << "" << std::endl;
		}
		if ( (board[i] & markedBit()) == markedBit()){ 
			std::cout << 'M' ;
    }
		else if ( (board[i] & hiddenBit()) == hiddenBit()) {
      std::cout << '*';
    }
		else {
      std::cout << (int) board[i];
    }
		counter++;
	}
}

int reveal(char * board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc){
  int resp = 0;

  int pos = yloc * xdim + xloc;
  int cell_val = board[pos];
  
  if (cell_val & markedBit()){
    resp = 1;
  } else if (cell_val & hiddenBit()) {
    board[pos] = cell_val & valueMask();
    if (board[pos] == 9) {
      resp = 9;
    } else {
      if (board[pos] == 0) {
        if (xloc != 0) { 
          if (yloc != 0) {
            board[((yloc-1)*xdim + xloc-1)] = board[((yloc-1)*xdim + xloc-1)] & valueMask();
          }
          board[(yloc*xdim + xloc-1)] = board[(yloc*xdim + xloc-1)] & valueMask();
          if (yloc+1 < ydim) {
            board[((yloc+1)*xdim + xloc-1)]= board[((yloc+1)*xdim + xloc-1)] & valueMask();
          }
        }
        if (yloc != 0) {
          board[((yloc-1)*xdim + xloc)] = board[((yloc-1)*xdim + xloc)] & valueMask(); 
        }
        if (yloc+1 < ydim) {
            board[((yloc+1)*xdim + xloc)] = board[((yloc+1)*xdim + xloc)] & valueMask();
        }
        if (xloc+1 < xdim) { 
          if (yloc != 0) {
            board[((yloc-1)*xdim + xloc+1)] = board[((yloc-1)*xdim + xloc+1)] & valueMask();
          }
          board[(yloc*xdim + xloc+1)] = board[(yloc*xdim + xloc+1)] & valueMask();
          if (yloc+1 < ydim) {
            board[((yloc+1)*xdim + xloc+1)] = board[((yloc+1)*xdim + xloc+1)] & valueMask();
          }
        }
      } 
    }
  } else {
    resp = 2;
  }
  return resp;
}

int mark(char * board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc) {
  int pos = yloc * xdim + xloc;
  int int_val = board[pos];

  if (int_val < 9) {
    return 2;
  } else if (int_val & markedBit()) {  
    board[pos] = int_val & valueMask();
    board[pos] = board[pos] | hiddenBit();
    return 0;
  } 
  else {
    board[pos] = int_val | markedBit();
    return 0;
  }
}

bool isGameWon(char * board, std::size_t xdim, std::size_t ydim) {
  bool gamewon = true;

  for (int i = 0; i < xdim*ydim; i++) {
   if (board[i] & hiddenBit()) {
     if ((board[i] & valueMask()) != 9){
       gamewon = false;
      }
   }
  }
  return gamewon;
}