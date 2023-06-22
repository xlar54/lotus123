#ifndef _lotus_
#define _lotus_

#define MAX_COLS        26
#define MAX_ROWS        100
#define INIT_COL_WIDTH  9
#define MIN_COL_WIDTH   1
#define MAX_COL_WIDTH   72

typedef enum {
    READY,
    LABEL,
    VALUE,
    MENU
} modeType;

typedef enum  {
    NUMBER,
    TEXT,
    FUNCTION
} cellType;

struct cellstruct {
    cellType type;
    int *value;
    int locked;
};

struct currentCellStruct {
    int row;
    int col;
} currentCell, firstVisibleCell, lastVisibleCell, screenPos;

struct cursorScreenLocationStruct {
    int row;
    int col;
};

struct columnStruct {
    char label[2];
    int width;
};

struct columnStruct columns[MAX_COLS];
struct cellstruct cell[MAX_ROWS][MAX_COLS];




#endif