#include "geesespotter_lib.h"

char * createBoard(std::size_t xdim, std::size_t ydim) {
  char * board = new char[xdim * ydim];
    for (int i=0; i < xdim * ydim; i++) {
        board[i] = 0;
    }
    return board;
}

void neighborsCount(char *board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc){
	int co {0};
  std::size_t pos = yloc * xdim + xloc;
	if ((board[pos] & valueMask()) == 0x09){
		return;
	}
  else{
    for (int i = -1; i < 2; i++) {
      for (int j = -1; j < 2; j++) {
        if (((yloc + j) >= 0) && ((yloc + j) < ydim) && ((xloc + i) >= 0) && ((xloc + i) < xdim)) {
          if ((board[(yloc + j) * xdim + xloc + i] & valueMask()) == 0x09) { 
            co++;
          }
        }
      }
  	}
	board[yloc * xdim + xloc] += co;
	return;
  }
}

void computeNeighbors(char *board, std::size_t xdim, std::size_t ydim){
	int i{0};
	for (i = 0; i < xdim * ydim; i++) {
		neighborsCount(board, xdim, ydim, i%xdim, i/xdim);
	}
	return;
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