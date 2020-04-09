/*	BLOCKS!

	Game from Floppy Software for MESCC - Mike's Enhanced Small C Compiler.

	Copyright (c) 2012 Miguel I. Garcia Lopez, Valencia, Spain.

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 2, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
	
	Author's contact:

		www.floppysoftware.es
		cpm-connections.blogspot.com
		floppysoftware@gmail.com

	To compile with MESCC:

		cc blocks
		ccopt blocks
		zsm blocks
		hextocom blocks

	Usage:

		blocks tty_name

	Revisions:

	02 Jul 2012 : v1.0 : Generic CP/M version 1.0 for 24x80 VT52 screen.
	09 Apr 2020 : v2.0 : Ported to the new KS library.
*/

/* MESCC libraries
*/
#include <mescc.h>   /* MESCC header & runtime */
#include <conio.h>   /* We need puts, putchar */
#include <string.h>  /* We need strlen() */
#include <ctype.h>   /* We need toupper() */
#include <sprintf.h> /* We need sprintf() */

/* KS libraries
*/
#include "ks.h"
#include "ks_cent.h"

/*	Game defs.
*/
#define BOARD_ROW	6	/* Screen position for board */
#define BOARD_COL	25	/* id */

#define BORDER_ROW	5	/* Screen position for border */
#define BORDER_COL	24	/* id */

#define BOARD_ROWS	10	/* Board rows */
#define BOARD_COLS	10	/* Board columns */

#define BLOCK_COLS	3	/* Block size */
#define BLOCK_ROWS	1	/* id */

#define BLOCK_EMPTY	' '	/* Value if block is empty */

#define LEVEL_ROW	2	/* Screen position for level info - used to show blocks killed */
#define LEVEL_COL	2	/* id */

#define SCORE_ROW	2	/* Screen position for score info */
#define SCORE_COL	73	/* id */

#define K_UP		'Q'	/* Key up */
#define K_DOWN		'A'	/* Key down */
#define K_LEFT		'O'	/* Key left */
#define K_RIGHT		'P'	/* Key right */
#define K_EXIT		'X'	/* Key exit */
#define K_SELECT	'S'	/* Key select */
#define K_KILL   	'K'	/* Key kill */
#define K_GRAV_UP 	'1'	/* Key gravity up */
#define K_GRAV_LEFT 	'2'	/* Key gravity left */
#define K_GRAV_RIGHT 	'3'	/* Key gravity right */
#define K_GRAV_DOWN 	'4'	/* Key gravity down */

#define SPR_SEL_LEFT	':'
#define SPR_SEL_RIGHT	':'

/*	Global variables
*/
WORD board[BOARD_ROWS];		/* Array for blocks values <<char *board[] -- MESCC things>> */
WORD board_sel[BOARD_ROWS];	/* Array for blocks selections <<id>> */

int	score,			/* Score */
	blocks,			/* Blocks remaining */
	randindex,		/* Random number used for generate blocks values */
	selected,		/* Number of blocks selected */
	add_to_score,		/* How much can I add to score if I kill selected blocks */
	automode,		/* Non zero if automode is selected */
	ChkPlsCtr,		/* CheckPlease variable */
	ChkPlsMul,		/* id */
	ChkPlsVal;		/* id */
	
int scr_rows,
	scr_cols;

/*	Entry point
*/
main(argc, argv)
int argc, argv[];
{
	int i;
	int tty_n;
	int *tty_s; /* char *[] */

	/* Init KS
	*/
	if(argc != 2) {	
		tty_s = KsGetNames();
		tty_n = KsGetHowMany();
		
		puts("Usage: blocks tty_name\n");
		puts("Supported TTY names:");
		
		for(i = 0; i < tty_n; ++i) {
			putchar('\t'); puts(tty_s[i]);
		}		
		
		return;
	}

	/* Start KS
	*/
	if(KsHello(KsGetCode(argv[1])) == -1) {
		puts("Unknown TTY name.\n\nRun 'blocks' to know supported TTYs.\n");
		return;
	}
	
	/* Setup some things
	*/
	scr_rows = KsGetRows();
	scr_cols = KsGetCols();
	
	board[0] = "1234567890"; board_sel[0] = "1234567890";
	board[1] = "1234567890"; board_sel[1] = "1234567890";
	board[2] = "1234567890"; board_sel[2] = "1234567890";
	board[3] = "1234567890"; board_sel[3] = "1234567890";
	board[4] = "1234567890"; board_sel[4] = "1234567890";
	board[5] = "1234567890"; board_sel[5] = "1234567890";
	board[6] = "1234567890"; board_sel[6] = "1234567890";
	board[7] = "1234567890"; board_sel[7] = "1234567890";
	board[8] = "1234567890"; board_sel[8] = "1234567890";
	board[9] = "1234567890"; board_sel[9] = "1234567890";

	/* Play the game, please!
	*/
	while((i = Menu()))
	{
		if(i == 1)
			Play();
	}

	/* Game is over - best to clean the scren
	*/
	KsClear();
	KsSetCursor(1);

	/* We say good bye to KS environment and say hello to CP/M
	*/
	KsBye();
}

/* Menu: return 0 if we want to quit game, 1 to play game, or 2 to return to menu
*/
Menu()
{
	KsClear();

	KsCenterStr(0, "BLOCKS");
	KsCenterStr(1, "v2.0 - 09 Apr 2020");
	KsCenterStr(3, "(c) 2012-2020 Miguel Garcia / FloppySoftware, Spain.");
	KsCenterStr(4  ,"www.floppysoftware.es\n");
	KsCenterStr(5  ,"floppysoftware@gmail.com\n\n");

	PrintStrAt( 8, 25, "1 : Play game in normal mode");
	PrintStrAt(10, 25, "2 : Play game in automatic mode");
	PrintStrAt(12, 25, "3 : Show help");

	PrintStrAt(14, 25, "X : Exit game");

	KsCenterStr(scr_rows - 2, "Select your choice ... ");

	randindex = 0; /* This is used to generate a random number */

	for(;;)
	{
		while(!KsGetKb())
		{
			if(++randindex == BOARD_ROWS * BOARD_COLS)
				randindex = 0;
		}

		switch(toupper(KsGetCh()))
		{
			case '1' : automode = 0; return 1;
			case '2' : automode = 1; return 1;
			case '3' : Help(); return 2;
			case 'X' : return 0;
		}
	}
}

/*	Show help
*/
Help()
{
	KsClear();

	KsCenterStr( 0, "BLOCKS");

	KsCenterStr( 2, "The object of the game is to remove all the");
	KsCenterStr( 3, "blocks on the board.");
	KsCenterStr( 5, "You can select just one block, or a group of");
	KsCenterStr( 6, "blocks of the same type.");
	KsCenterStr( 8, "Then, if you are in the normal mode, you can place all");
	KsCenterStr( 9, "the blocks against a side (top, bottom, left or right).");
	KsCenterStr(11, "If you are in the automatic mode, the blocks rows will be");
	KsCenterStr(12, "placed on the bottom side of the board, and the columns");
	KsCenterStr(13, "will be placed on the middle of the board.");
	KsCenterStr(15, "The partial score is the square of the number of");
	KsCenterStr(16, "selected blocks.");
	KsCenterStr(18, "The special block 'X', multiplies that result by 10.");
	KsCenterStr(20, "Good luck!");
	KsCenterStr(22, "Press any key ... ");

	KsGetCh();
}

/* Play the game
*/
Play()
{
	int row, col, run, val, key;

	/* Setup variables and board
	*/
	row = col = selected = score = 0; run = 1;

	blocks = BOARD_ROWS * BOARD_COLS;

	SetupBoard();

	/* Draw screen
	*/
	KsClear();

	PrintBox(0, 0, scr_rows-1, scr_cols, NULL); KsCenterStr(0, "| BLOCKS |");

	PrintStrAt(LEVEL_ROW, LEVEL_COL, "BLOCKS"); PrintBlocks();

	PrintStrAt(SCORE_ROW, SCORE_COL, "SCORE"); PrintScore();

	PrintBox(BORDER_ROW, BORDER_COL, BOARD_ROWS*BLOCK_ROWS+2, BOARD_COLS*BLOCK_COLS+2, NULL);

	PaintBoard();

	if(!automode)
	{
		KsCenterStr(BORDER_ROW-1, "1");

		KsCenterStr(BORDER_ROW+BOARD_ROWS+2, "4");

		PrintStrAt(BORDER_ROW+5, BORDER_COL-2, "2");

		PrintStrAt(BORDER_ROW+5, BORDER_COL+BOARD_COLS*BLOCK_COLS+3, "3");
	}

	PrintStrAt(BORDER_ROW+3, 7, "    Q");
	PrintStrAt(BORDER_ROW+4, 7, "    |");
	PrintStrAt(BORDER_ROW+5, 7, "O --x-- P");
	PrintStrAt(BORDER_ROW+6, 7, "    |");
	PrintStrAt(BORDER_ROW+7, 7, "    A");

	PrintStrAt(BORDER_ROW+2, BORDER_COL+BOARD_COLS*BLOCK_COLS+8, automode ? "Automatic mode" : "Normal mode");
	
	PrintStrAt(BORDER_ROW+4, BORDER_COL+BOARD_COLS*BLOCK_COLS+8, "S> SELECT BLOCKS");
	PrintStrAt(BORDER_ROW+5, BORDER_COL+BOARD_COLS*BLOCK_COLS+8, "K> KILL BLOCKS");
	PrintStrAt(BORDER_ROW+6, BORDER_COL+BOARD_COLS*BLOCK_COLS+8, "X> EXIT");

	/* Play
	*/
	while(run)
	{
		/* Draw cursor
		*/
		KsPosCursor(BOARD_ROW+row*BLOCK_ROWS, BOARD_COL+col*BLOCK_COLS+1); KsSetCursor(1);

		/* User action
		*/
		key = toupper(KsGetCh());

		KsSetCursor(0);

		switch(key)
		{
			case K_UP:
					if(row) --row;
					break;

			case K_DOWN:
					if(++row==BOARD_ROWS) --row;
					break;

			case K_LEFT:
					if(col) --col;
					break;

			case K_RIGHT:
					if(++col==BOARD_COLS) -- col;
					break;

			case K_SELECT:
					/* Check for valid blocks
					*/
					val=GetBlock(row, col);

					if(val==BLOCK_EMPTY || val=='X')
						break;

					/* Check if already selected
					*/
					if(TstSelBlock(row, col))
						break;

					/* Unselect previous selection
					*/
					if(selected)
					{
						/* Hey! Too much work if only 1 selected -- FIXME
						*/
						UnSelAllBlocks();
						PaintBoard();
					}

					/* Select
					*/
					selected=CheckPlease(row, col);

					add_to_score=selected*selected*ChkPlsMul;

					PrintSelec();

					break;

			case K_KILL:
					if(selected)
					{
						/* Hey! Too much work if only 1 selected -- FIXME
						*/
						KillSelBlocks();

						score+=add_to_score;

						PrintScore();

						blocks-=selected; PrintBlocks();

						selected=add_to_score=0;

						PrintSelec();

						if(!blocks)
						{
							GameOver(); run=0;
						}
						else if(automode)
						{
							DoAutoMode(); PaintBoard();
						}
					}

					break;

			case K_GRAV_UP:
					if(automode)
						break;

					GravityUp();

					if(selected)
					{
						UnSelAllBlocks(); selected=0;
					}

					PaintBoard();

					break;

			case K_GRAV_DOWN:
					if(automode)
						break;

					GravityDown();

					if(selected)
					{
						UnSelAllBlocks(); selected=0;
					}

					PaintBoard();

					break;

			case K_GRAV_LEFT:
					if(automode)
						break;

					GravityLeft();

					if(selected)
					{
						UnSelAllBlocks(); selected=0;
					}

					PaintBoard();

					break;

			case K_GRAV_RIGHT:
					if(automode)
						break;

					GravityRight();

					if(selected)
					{
						UnSelAllBlocks(); selected=0;
					}

					PaintBoard();

					break;

			case K_EXIT :
					PrintMsg("Are you sure (Y/N)? ");

					if(toupper(KsGetCh())=='Y')
						run=0;
					else
						PrintMsg("");

					break;
		}
	}
}

/*	Game is over
*/
GameOver()
{
	KsCenterStr(BORDER_ROW+4, "*** GAME OVER ***");

	PrintMsg("Press any key ... ");

	KsGetCh();
}

/*	Setup board
*/
SetupBoard()
{
	int r, c; unsigned char *values;

	/*        0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789 (width 100) */
	values = "4321242333123432411123443444432111234332222343331214422444441243444132222311114242423111133222441111";

	for(r=0; r!=BOARD_ROWS; ++r)
	{
		for(c=0; c!=BOARD_COLS; ++c)
		{
			SetBlock(r, c, values[randindex]);

			UnSelBlock(r, c);

			if(++randindex==BOARD_ROWS*BOARD_COLS)
				randindex=0;
		}
	}

	if(randindex<25)
		SetBlock(2, 5, 'X');
	else if(randindex<50)
		SetBlock(9, 4, 'X');
	else if(randindex<75)
		SetBlock(4, 8, 'X');
	else
		SetBlock(7, 3, 'X');
}

/*	Set value to block
*/
SetBlock(row, col, value)
int row, col, value;
{
	char *ptr;

	ptr=board[row];

	ptr[col]=value;
}

/*	Get value from block
*/
GetBlock(row, col)
int row, col;
{
	char *ptr;

	ptr=board[row];

	return ptr[col];
}

/*	Select block
*/
SelBlock(row, col)
int row, col;
{
	char *ptr;

	ptr=board_sel[row];

	ptr[col]=1;
}

/*	Unselect block
*/
UnSelBlock(row, col)
int row, col;
{
	char *ptr;

	ptr=board_sel[row];

	ptr[col]=0;
}

/*	Unselect all blocks
*/
UnSelAllBlocks()
{
	int r, c;

	for(r=0; r!=BOARD_ROWS; ++r)
		for(c=0; c!=BOARD_COLS; ++c)
			UnSelBlock(r, c);
}

/*	Test if a block is selected
*/
TstSelBlock(row, col)
int row, col;
{
	char *ptr;

	ptr=board_sel[row];

	return ptr[col];
}

/*	Kill selected blocks
*/
KillSelBlocks()
{
	int r, c;

	for(r=0; r!=BOARD_ROWS; ++r)
	{
		for(c=0; c!=BOARD_COLS; ++c)
		{
			if(TstSelBlock(r, c))
			{
				SetBlock(r, c, BLOCK_EMPTY);
				UnSelBlock(r, c);
				PaintBlock(r, c);
			}
		}
	}
}

/*	Paint block
*/
PaintBlock(row, col)
int row, col;
{
	int selected;

	KsPosCursor(BOARD_ROW+row*BLOCK_ROWS, BOARD_COL+col*BLOCK_COLS);

	KsPutCh((selected=TstSelBlock(row, col)) ? SPR_SEL_LEFT : ' ');

	switch(GetBlock(row, col))
	{
		case '1' : KsPutCh('$'); break;
		case '2' : KsPutCh('O'); break;
		case '3' : KsPutCh('+'); break;
		case '4' : KsPutCh('#'); break;
		case 'X' : KsPutCh('X'); break;
		case BLOCK_EMPTY : KsPutCh(' '); break;
		default  : KsPutCh('?'); break;
	}

	KsPutCh(selected ? SPR_SEL_RIGHT : ' ');
}

/*	Paint all blocks
*/
PaintBoard()
{
	int r, c;

	for(r=0; r!=BOARD_ROWS; ++r)
		for(c=0; c!=BOARD_COLS; ++c)
			PaintBlock(r, c);
}

/*	Check what blocks can be selected - setup 3 variables
*/
CheckPlease(row, col)
int row, col;
{
	ChkPlsCtr=0; ChkPlsMul=1; ChkPlsVal=GetBlock(row, col);

	ChkPls2(row, col);

	return ChkPlsCtr;
}

ChkPls2(row, col)
int row, col;
{
	int val;

	SelBlock(row, col); PaintBlock(row, col);

	if(GetBlock(row, col)=='X')
		ChkPlsMul=10;

	if(row)
	{
		if(((val=GetBlock(row-1, col))==ChkPlsVal || val=='X') && (!TstSelBlock(row-1, col)))
			ChkPls2(row-1, col);
	}

	if(col<(BOARD_COLS-1))
	{
		if(((val=GetBlock(row, col+1))==ChkPlsVal || val=='X') && (!TstSelBlock(row, col+1)))
			ChkPls2(row, col+1);
	}

	if(row<(BOARD_ROWS-1))
	{
		if(((val=GetBlock(row+1, col))==ChkPlsVal || val=='X') && (!TstSelBlock(row+1, col)))
			ChkPls2(row+1, col);
	}

	if(col)
	{
		if(((val=GetBlock(row, col-1))==ChkPlsVal || val=='X') && (!TstSelBlock(row, col-1)))
			ChkPls2(row, col-1);
	}

	++ChkPlsCtr;
}

/*	Gravity down -- we put blocks on board bottom
*/
GravityDown()
{
	int r, c, i, blk, row[BOARD_ROWS];

	for(c=0; c!=BOARD_COLS; ++c)
	{
		for((r=i=BOARD_ROWS-1); r!=-1; --r)
		{
			if((blk=GetBlock(r, c))!=BLOCK_EMPTY)
				row[i--]=blk;
		}

		while(i!=-1)
			row[i--]=BLOCK_EMPTY;

		for(r=0; r!=BOARD_ROWS; ++r)
			SetBlock(r, c, row[r]);
	}
}

/*	Gravity up -- we put blocks on board top
*/
GravityUp()
{
	int r, c, i, blk, row[BOARD_ROWS];

	for(c=0; c!=BOARD_COLS; ++c)
	{
		for((r=i=0); r!=BOARD_ROWS; ++r)
		{
			if((blk=GetBlock(r, c))!=BLOCK_EMPTY)
				row[i++]=blk;
		}

		while(i!=BOARD_ROWS)
			row[i++]=BLOCK_EMPTY;

		for(r=0; r!=BOARD_ROWS; ++r)
			SetBlock(r, c, row[r]);
	}
}

/*	Gravity left -- we put blocks on board left
*/
GravityLeft()
{
	int r, c, i, blk, col[BOARD_ROWS];

	for(r=0; r!=BOARD_ROWS; ++r)
	{
		for((c=i=0); c!=BOARD_COLS; ++c)
		{
			if((blk=GetBlock(r, c))!=BLOCK_EMPTY)
				col[i++]=blk;
		}

		while(i!=BOARD_COLS)
			col[i++]=BLOCK_EMPTY;

		for(c=0; c!=BOARD_COLS; ++c)
			SetBlock(r, c, col[c]);
	}
}

/*	Gravity right -- we put blocks on board right
*/
GravityRight()
{
	int r, c, i, blk, col[BOARD_ROWS];

	for(r=0; r!=BOARD_ROWS; ++r)
	{
		for((c=i=BOARD_COLS-1); c!=-1; --c)
		{
			if((blk=GetBlock(r, c))!=BLOCK_EMPTY)
				col[i--]=blk;
		}

		while(i!=-1)
			col[i--]=BLOCK_EMPTY;

		for(c=0; c!=BOARD_COLS; ++c)
			SetBlock(r, c, col[c]);
	}
}

/*	Automatic mode -- we put blocks first on board bottom, then we center the columns
*/
DoAutoMode()
{
	int r, c, i, pos, cols;

	GravityDown();

	/* Copy non empty columns on the left
	*/
	for(i=c=0; c!=BOARD_COLS; ++c)
	{
		if(GetBlock(BOARD_ROWS-1, c) != BLOCK_EMPTY)
		{
			if(i!=c)
			{
				for(r=0; r!=BOARD_ROWS; ++r)
					SetBlock(r, i, GetBlock(r, c));
			}

			++i;
		}
	}

	/* Do nothing if all columns are non empty
	*/
	if(i==BOARD_COLS)
		return;

	/* Compute where to put the columns
	*/
	pos=(BOARD_COLS-i)/2;

	/* Copy columns without overlap
	*/
	cols=i;

	for(c=pos+i-1; i!=-1; --c)
	{
		--i;

		for(r=0; r!=BOARD_ROWS; ++r)
			SetBlock(r, c, GetBlock(r, i));
	}

	/* Empty columns to the left
	*/
	for(c=0; c!=pos; ++c)
		for(r=0; r!=BOARD_ROWS; ++r)
			SetBlock(r, c, BLOCK_EMPTY);

	/* Empty columns to the right
	*/
	for(c=pos+cols; c!=BOARD_COLS; ++c)
		for(r=0; r!=BOARD_ROWS; ++r)
			SetBlock(r, c, BLOCK_EMPTY);
}

/*	Print how many remaining blocks we have
*/
PrintBlocks()
{
	char str[6];

	sprintf(str, "%d  ", blocks);

	PrintStrAt(LEVEL_ROW+1, LEVEL_COL, str);
}

/*	Print score
*/
PrintScore()
{
	char str[7];

	sprintf(str, "%5d", score);

	PrintStrAt(SCORE_ROW+1, SCORE_COL, str);
}

/*	Print selection info.
*/
PrintSelec()
{
	char str[7];

	sprintf(str, "x%d  ", selected);

	PrintStrAt(LEVEL_ROW+3, LEVEL_COL, str);

	sprintf(str, "%5d+", add_to_score);

	PrintStrAt(SCORE_ROW+3, SCORE_COL-1, str);
}

/*	Print message text
*/
PrintMsg(str)
char *str;
{
	KsPosCursor(scr_rows-4, 2);
	PrintRpt(' ', scr_cols-4);

	KsCenterStr(scr_rows-4, str);
}

/* Print string at screen position
*/
PrintStrAt(row, col, str)
int row, col; char *str;
{
	KsPosCursor(row, col); KsPutStr(str);
}

/* Print a character n times
*/
PrintRpt(ch, times)
int ch, times;
{
	while(times--)
		KsPutCh(ch);
}

/* Print box
*/
PrintBox(row, col, rows, cols, fill)
int row, col, rows, cols, fill;
{
	int i;

	KsPosCursor(row++, col); KsPutCh('+');
	PrintRpt('-', cols - 2);
	KsPutCh('+');

	for(i = rows - 2; i; --i)
	{
		KsPosCursor(row, col); KsPutCh('|');
		if(fill != NULL)
			PrintRpt(fill, cols - 2);
		KsPosCursor(row++, col + cols - 1); KsPutCh('|');
	}

	KsPosCursor(row, col); KsPutCh('+');
	PrintRpt('-', cols - 2);
	KsPutCh('+');
}