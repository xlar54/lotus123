#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <c128.h>
#include "main.h"

#define CRSR_UP     145
#define CRSR_DWN    17
#define CRSR_RIGHT  29
#define CRSR_LEFT   157

void init();
void welcomeScreen();
void screen_display();
void cursor_update(int row, int col, int onoff, char* value);
void controlPanel_update();
void colHeaders_update();
void rowHeaders_update(int start);
void cursor_MoveRight();
void cursor_MoveLeft();

modeType currentMode = READY;

void main(void)
{
    init();
    screen_display();
}

void init() 
{
    int t=0;

    fast();
    welcomeScreen();

    //set initial screen position (actual 80x24)
    screenPos.row = 0;
    screenPos.col = 0;

    // set initial active cell (will xlate to A1)
    currentCell.row = 0;
    currentCell.col = 0;

    // set first visible cell
    firstVisibleCell.row = 0;
    firstVisibleCell.col = 0;

    //set initial column widths
    for(t=0;t<MAX_COLS;t++) {
        columns[t].width = INIT_COL_WIDTH;
        columns[t].label[0]=97+t;
        if(t==0) columns[t].width=3;
        if(t==9) columns[t].width=12;
        columns[t].label[1]=32;
    }

    revers(1);
    cputsxy(0,3,"                                                                           ");   
    revers(0);    

}

void welcomeScreen()
{
    clrscr();

    cputsxy(35,7,"1 - 2 - 3");
    cputsxy(28, 9, "Copyright (C) 1982,1983");
    cputsxy(25, 10, "Lotus Development Corporation");
    cputsxy(30, 11, "All Rights Reserved");
    cputsxy(34, 12, "Release 1A");
    cputsxy(27, 16, "(Press Any Key To Continue)");

    cgetc();
}

void screen_display()
{
    int c=0; 

    clrscr();
    controlPanel_update();
    colHeaders_update(); 
    rowHeaders_update(firstVisibleCell.row);

    cursor_update(currentCell.col, currentCell.row,1, NULL);
    
    while(1) 
    {
        c = cgetc();

        switch(c) 
        {
            case CRSR_RIGHT:
                    cursor_MoveRight();
                break;
            case CRSR_LEFT:
                    cursor_MoveLeft();
                break;
            case CRSR_DWN:
                //visibleStartRow++;
                //rowHeaders_update(visibleStartRow);
                break;
            case CRSR_UP:
                //if(visibleStartRow>1)
                //{
                //    visibleStartRow--;
                //    rowHeaders_update(visibleStartRow);    
                //}
                break;
        }
    }
}

void cursor_update(int row, int col, int rvs, char* value)
{
    int t=0;
    int sc = 4, sr=4;
    char *cursor;
    revers(rvs);

    // allocate memory for the current cell width
    cursor = (char *)malloc(columns[col].width+1);

    for(t=0;t<columns[col].width;t++)
        cursor[t]=32;

    cursor[t]=0;

    //calculate screen position x,y from column/row
    for(t=firstVisibleCell.col;t<col;t++) {
        sc += columns[t].width;
    }

    for(t=0;t<row;t++) {
        sr++;
    }
    
    screenPos.col = sc;
    screenPos.row = sr;


    cputsxy(screenPos.col,screenPos.row,cursor);

    free(cursor);
    revers(0);

    controlPanel_update();
}

void controlPanel_update()
{
    char currentCellLabel[10];

    sprintf(currentCellLabel, "%d%d:%d%d", currentCell.col, currentCell.row, lastVisibleCell.col, lastVisibleCell.row);
    

    cputsxy(0,0,currentCellLabel);

    revers(1);
    switch(currentMode)
    {
        case READY:
            cputsxy(75,0,"READY");
            break;
        case LABEL:
            cputsxy(75,0,"LABEL");
            break;
        case VALUE:
            cputsxy(75,0,"VALUE");
            break;
        case MENU:
            cputsxy(75,0,"MENU ");
            break;
    }
    revers(0);
}

void colHeaders_update() 
{
    int t=0;
    char headerrow[80];
    int linePos=4;
    int col=0;
    int lastcol = firstVisibleCell.col;

    // determine the last column to display
    while(1)
    {
        if(linePos + columns[lastcol].width > 75)
        {
            if(linePos + columns[lastcol].width > 76)
                lastcol--;
            linePos=4;
            break;
        }
        linePos += columns[lastcol].width;
        lastcol++;
    }

    headerrow[0] = ' ';
    headerrow[1] = ' ';
    headerrow[2] = ' ';
    headerrow[3] = ' ';

    for(col=firstVisibleCell.col;col<=lastcol;col++)
    {
        // loop over the column width
        for(t=0;t<columns[col].width;t++) {

            //if the current column width position equals half the column width,
            //this is where we put the column label
            if(t==columns[col].width/2) {
                headerrow[linePos++]=columns[col].label[0];
                headerrow[linePos++]=columns[col].label[1];
                t++;
            }
            else
                headerrow[linePos++] = ' ';
        }
    }

    while(linePos < 76)
        headerrow[linePos++]='-';
        
    headerrow[76]=0;
    lastVisibleCell.col=lastcol;

    revers(1);
    cputsxy(0,3, headerrow);
    revers(0);
}

void rowHeaders_update(int start)
{
    int t=1;
    char rh[5];

    revers(1);

    for(t=0;t<20;t++)
    {
        sprintf(rh, "%-4d",start+t+1);
        cputsxy(0,4+t,rh);
    }

    lastVisibleCell.row=start+t;
        
}

void cursor_MoveRight() 
{
    if(currentCell.col == MAX_COLS-1)
        return;

    if(currentCell.col < lastVisibleCell.col)
    {
        cursor_update(currentCell.row,currentCell.col,0,NULL);
        currentCell.col++;
        cursor_update(currentCell.row,currentCell.col,1,NULL);
    }
    else 
    {
        cursor_update(currentCell.row,currentCell.col,0,NULL);
        firstVisibleCell.col++;
        colHeaders_update();

        currentCell.col++;
        cursor_update(currentCell.row,currentCell.col,1,NULL);
        
    }
}

void cursor_MoveLeft() 
{
    if(screenPos.col > 4)
    {
        cursor_update(currentCell.row,currentCell.col,0,NULL);
        currentCell.col--;
        cursor_update(currentCell.row,currentCell.col,1,NULL);
        return;
    }
    
    if(screenPos.col == 4 && firstVisibleCell.col > 0) 
    {
        cursor_update(currentCell.row,currentCell.col,0,NULL);
        
        firstVisibleCell.col--;
        colHeaders_update();

        currentCell.col=firstVisibleCell.col;
        cursor_update(currentCell.row,currentCell.col,1,NULL);
    }
}

void cursor_show()
{
    
}