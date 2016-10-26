#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void draw_grid(char **grid, int height, int width);
int is_cell_completed(int cellRow, int cellColumn, 
        int maxHeight, int maxWidth, char **grid);
int is_game_complete(char **grid, int height, int width);

/*
 * Function: check_arguments
 *
 * check the number of arguments, grid dimensions, player count is correct
 *
 * argc: number of arguments
 * argv[]: array of arguments  
 * height: the height of grid
 * width: the width of grid
 * numOfPlayer: player count
 *
 * return void
 */
void check_arguments(int argc, char *argv[], int *height,
        int *width, int *numOfPlayer) {
    char *ptr;

    if(argc != 4 && argc != 5) {
        fprintf(stderr, "Usage: boxes height width playercount [filename]\n");
        exit(1);  
    }
    
    if((int) strtol(argv[1], &ptr, 10) <= 1 || *ptr 
            || (int) strtol(argv[1], (char **)NULL, 10) >= 1000) {
        fprintf(stderr, "Invalid grid dimensions\n");
        exit(2);
    }
    
    
    if((int) strtol(argv[2], &ptr, 10) <= 1 || *ptr
            || (int) strtol(argv[2], (char **)NULL, 10) >= 1000) {
        fprintf(stderr, "Invalid grid dimensions\n");
        exit(2);
    }
    
    *numOfPlayer = (int) strtol(argv[3], (char **)NULL, 10);
    
    if((int) strtol(argv[3], &ptr, 10) <= 1 || *ptr
            || (int) strtol(argv[3], (char **)NULL, 10) >= 101) {
        fprintf(stderr, "Invalid player count\n");
        exit(3);
    }
    
    *height = (int) strtol(argv[1], (char **)NULL, 10);
    *width = (int) strtol(argv[2], (char **)NULL, 10);
}

/*
 * Function: init_grid
 *
 * initial the empty grid which only have "+" symbols
 *
 * grid: the grid we initialise 
 * height: the height of grid
 * width: the width of grid
 * 
 * return void
 */
void init_grid(char **grid, int height, int width) {
    int j, k;
    for(j = 0; j < 2 * height + 1; j++) {
        for(k = 0; k < 2 * width + 1; k++) {
            if(j % 2 == 0 && k % 2 == 0) {
                grid[j][k] = '+';
            } else {
                grid[j][k] = ' ';
            }
        }
    }
}

/*
 * Function: read_cell
 *
 * when a file is loaded, this function read the cell part
 * and determine which cell should be filled with player name
 *
 * buf: the array to save the content loaded
 * grid: the grid we work on 
 * row: the row number in the file we work on
 * width: the width of grid
 * numOfPlayer: player count
 * players: an array to put all the players 
 *
 * return void
 */
void read_cell(char *buf, char **grid, int row,
        int width, int numOfPlayer, int *players) {
    char *pos;
    int i;
    int num;
    i = 0;
    pos = buf;
    
    while(*pos && i < width) {
        num = strtol(pos, &pos, 10);
        if(num < 0 || num > numOfPlayer) {
            fprintf(stderr, "Error reading grid contents\n");
            exit(5);
        }
        
        if (*pos != ',' && *pos != 0 && *pos != '\n') {
            fprintf(stderr, "Error reading grid contents\n");
            exit(5);                    
        }
        pos++;
        if (num != 0) {
            grid[2 * row + 1][2 * i + 1] = 'A' + num - 1;
            players[num - 1]++;
        }
        i++;
    }
}

/*
 * Function: read_edge
 *
 * when a file is loaded, this function read the edge part
 * and fill in all "-" and "|" edge
 *
 * buf: the array to save the content loaded
 * lineCounter: to record which line we read
 * emptyGrid: the grid we work on 
 * width: the width of grid
 *
 * return void
 */
void read_edge(char *buf, int lineCounter, char **emptyGrid, int width) {
    int i;
    
    if(lineCounter % 2 == 0) {
        for(i = 0; i < width; i++) {
            if(buf[i] == '1') {
                emptyGrid[lineCounter - 2][2 * i + 1] = '-';
            } else if(buf[i] != '0') {
                fprintf(stderr, "Error reading grid contents\n");
                exit(5);
            }
        }
        if(buf[i] != 0 && buf[i] != '\n') {
            fprintf(stderr, "Error reading grid contents\n");
            exit(5);
        }
    } else {
        for(i = 0; i < width + 1; i++) {
            if(buf[i] == '1') {
                emptyGrid[lineCounter - 2][2 * i] = '|';
            } else if(buf[i] != '0') {
                fprintf(stderr, "Error reading grid contents\n");
                exit(5);
            }                         
        }
        if(buf[i] != 0 && buf[i] != '\n') {
            fprintf(stderr, "Error reading grid contents\n");
            exit(5);
        }                    
    }
}    

/*
 * Function: read_file
 *
 * read the given file
 *
 * fileName: the file we read
 * emptyGrid: the grid we work on 
 * height: the height of grid
 * width: the width of grid
 * numOfPlayer: player count
 * currentPlayer: to record the player of next turn
 * players: an array to save all players 
 *
 * return void
 */
void read_file(char *fileName, char **emptyGrid, int height, int width,
        int numOfPlayer, char *currentPlayer, int *players) {
    int lineCounter;
    char buf[2048];

    

    FILE* f = fopen(fileName, "r");
    lineCounter = 1;
    if(!f)
    {
        fprintf(stderr, "Invalid grid file\n");
        exit(4);        
    }
    
    while(fgets(buf, 2048, f) != NULL) {
        if(lineCounter == 1) {
            if(atoi(buf) > numOfPlayer || atoi(buf) < 1) {
                fprintf(stderr, "Error reading grid contents\n");
                exit(5);
            }
            *currentPlayer = atoi(buf) + 'A' - 1;
        } else if(lineCounter > 1 && lineCounter < 2 * height + 3) {
            read_edge(buf, lineCounter, emptyGrid, width);
        } else if(lineCounter > 2 * height + 2 &&
                lineCounter < 3 * height + 3) {
            read_cell(buf, emptyGrid, lineCounter - 2 * height - 3,
                    width, numOfPlayer, players);
        } else {
            fprintf(stderr, "Error reading grid contents5\n");
            exit(5);
        }
        lineCounter++;

    } 
    
    fclose(f);
}

/*
 * Function: display_winners
 *
 * when game is over, display who wins the game
 *
 * currentPlayer: to record the player of next turn
 * players: an array to save all players 
 *
 * return void
 */
void display_winners(int numOfPlayer, int *players) {
    int t;
    int maxScore;
    int multipleWinnerFlag;
    maxScore = -1; 
    multipleWinnerFlag = 0;
    
    for(t = 0; t < numOfPlayer; t++) {
        if(players[t] > maxScore) {
            maxScore = players[t];
        }
    }
    
    for(t = 0; t < numOfPlayer; t++) {
        if(players[t] == maxScore && multipleWinnerFlag == 0) {
            multipleWinnerFlag = 1;
            
            printf("Winner(s): %c", 'A' + t);
            t++;
            
        }
        if(players[t] == maxScore && multipleWinnerFlag == 1) {
            printf(", %c", 'A' + t);
        }
        
    }
    printf("\n");
    exit(0);
}

/*
 * Function: write_cell
 *
 * save the cell part of grid into a file
 *
 * outputFile: the file we save to
 * emptyGrid: the grid we work on 
 * height: the height of grid
 * width: the width of grid 
 *
 * return void
 */
void write_cell(int height, int width, char** emptyGrid, FILE *outputFile) {
    int i, j;
       
    for(i = 0; i < height; i++) {
        for(j = 0; j < width; j++) {
            if(emptyGrid[2 * i + 1][2 * j + 1] == ' ') {
                fprintf(outputFile, "0");
            } else {
                fprintf(outputFile, "%d",
                        emptyGrid[2 * i + 1][2 * j + 1] - 'A' + 1);
            }
            
            if (j < width - 1) {
                fprintf(outputFile, ",");
            } else {
                fprintf(outputFile, "\n");
            }
        }
    }
}

/*
 * Function: write_file
 *
 * save the grid into a file
 *
 * str: the string we work on
 * emptyGrid: the grid we work on
 * currentPlayer: to record the player of next turn 
 * height: the height of grid
 * width: the width of grid 
 *
 * return void
 */
void write_file(char *str, char currentPlayer,
        char **emptyGrid, int height, int width) { 
    int *hPosition, *vPosition;
    FILE *outputFile;    
    int len, j, k, p;
    int printHorizentalFlag = 1;
    hPosition = (int *) malloc(width);
    vPosition = (int *) malloc(width + 1);
    len = strlen(str);
    if(str[len - 1] == '\n') {
        str[len - 1] = 0;
    } 
    if ((outputFile = fopen(str + 2, "w")) == NULL) {
        fprintf(stderr, "Can not open file for write\n");
        free(hPosition);
        return;        
    }
    fprintf(outputFile, "%d\n", currentPlayer - 'A' + 1);
    for(j = 0; j < 2 * height + 1; j++) {
        if(printHorizentalFlag) { 
            for(k = 1; k < 2 * width + 1; k += 2) {
                if(emptyGrid[j][k] == '-') {
                    hPosition[k / 2] = 1;
                } else {
                    hPosition[k / 2] = 0;
                }
            }
            for(p = 0; p < width; p++) {
                fprintf(outputFile, "%d", hPosition[p]);
                printHorizentalFlag = 0;
            }
            fprintf(outputFile, "\n");
        } else {
            for(k = 0; k < 2 * width + 1; k += 2) {
                if(emptyGrid[j][k] == '|') {
                    vPosition[k / 2] = 1;
                } else {
                    vPosition[k / 2] = 0;
                } 
            }
            for(p = 0; p < width + 1; p++) {
                fprintf(outputFile, "%d", vPosition[p]);
                printHorizentalFlag = 1;
            }
            fprintf(outputFile, "\n");
        }
    }
    write_cell(height, width, emptyGrid, outputFile);
    fclose(outputFile);
    fprintf(stderr, "Save complete\n");
    free(hPosition);
}

/*
 * Function: change_grid
 *
 * in game process, update the grid when input is given
 *
 * x: the row we work on
 * y: the column we work on
 * type: the type of edge(horizental or vertical)
 * emptyGrid: the grid we work on
 * currentPlayer: to record the player of next turn 
 * height: the height of grid
 * width: the width of grid
 * players: an array to save all players
 *
 * return 0 if there is error
 * return 1 if an edge is added, but no cell is completed
 * return 2 if an edge is added, and there is
 * at least one cell is completed
 */
int change_grid(int x, int y, char type, char **emptyGrid,
        int height, int width, char currentPlayer, int *players) {
    int cellCompleteFlag;
    cellCompleteFlag = 0;   
    if(type == 'h') {
        if (x < 0 || x > height || y < 0 || y >= width) {
            return 0;
        }
        
        emptyGrid[2 * x][2 * y + 1] = '-';
        if(is_cell_completed(x, y, height, width, emptyGrid)) {
            emptyGrid[2 * x + 1][2 * y + 1] = currentPlayer;
            cellCompleteFlag = 1;
            players[currentPlayer - 'A']++;
        }
        if(is_cell_completed(x - 1, y, height, width, emptyGrid)) {
            emptyGrid[2 * (x - 1) + 1][2 * y + 1] = currentPlayer;
            cellCompleteFlag = 1;
            players[currentPlayer - 'A']++;
        }    
    } else if(type == 'v') {
        if (x < 0 || x >= height || y < 0 || y > width) {
            return 0;
        }

        emptyGrid[2 * x + 1][2 * y] = '|';
        if(is_cell_completed(x, y, height, width, emptyGrid)) {
            emptyGrid[2 * x + 1][2 * y + 1] = currentPlayer;
            cellCompleteFlag = 1;
            players[currentPlayer - 'A']++;
        }
        if(is_cell_completed(x, y - 1, height, width, emptyGrid)) {
            emptyGrid[2 * x + 1][2 * (y - 1) + 1] = currentPlayer;
            cellCompleteFlag = 1;
            players[currentPlayer - 'A']++;
        } 
    } else {
        return 0;
    }
    
    if(cellCompleteFlag == 0) {
        return 1;
    } else {
        return 2;
    }  
}

/*
 * Function: play_game
 *
 * deal with the whole game process
 *
 * emptyGrid: the grid we work on
 * currentPlayer: to record the player of next turn 
 * height: the height of grid
 * width: the width of grid
 * players: an array to save all players
 * numOfPlayer: player count
 *
 * return void
 */
void play_game(char **emptyGrid, int height, int width,
        int *players, int numOfPlayer, char currentPlayer) {
    int ret;
    int inputRow;
    int inputColumn;
    char type;
    char str[2048];
    
        
    while(1) {
        printf("%c> ", currentPlayer);
        
        if (fgets(str, 2048, stdin) == NULL) {
            fprintf(stderr, "End of user input\n");
            exit(6);
        }
        
        if(strncmp("w ", str, 2) == 0) {
            /* save file */
            write_file(str, currentPlayer, emptyGrid, height, width);
        } else {
            if (sscanf(str, "%d %d %c", &inputRow, &inputColumn, &type) == 3) {

                printf("%d %d %c\n", inputRow, inputColumn, type);
                ret = change_grid(inputRow, inputColumn, type,
                        emptyGrid, height, width, currentPlayer, players);
                
                if(ret != 0) {
                    draw_grid(emptyGrid, height, width);
                }            
    
                if (ret == 1) {
                    currentPlayer++;
                    if(currentPlayer - 'A' >= numOfPlayer) {
                        currentPlayer = 'A';
                    }
                }
    
                if(is_game_complete(emptyGrid, height, width)) {
                    display_winners(numOfPlayer, players);
                }
            }       
        }
    }
}

/*
 * Function: main
 *
 * the main function, here we start the game and
 * call other functions
 *
 * argc: number of arguments
 * argv[]: array of arguments 
 *
 * return 0 
 */
int main(int argc, char * argv[]) {
    int height;
    int numOfPlayer;
    int width;
    char currentPlayer;
    int *players;
    char **emptyGrid;
    int i;

    check_arguments(argc, argv, &height, &width, &numOfPlayer);
        
    emptyGrid = (char **) malloc((2 * height + 1) * sizeof(char *));
    for(i = 0; i < 2 * height + 1; i++) {
        emptyGrid[i] = (char *) malloc((2 * width + 1) * sizeof(char));
    }
    players = (int *) malloc(numOfPlayer * sizeof(int));
    currentPlayer = 'A';
    init_grid(emptyGrid, height, width);   
    if (argc == 5) {
        read_file(argv[4], emptyGrid, height, width,
                numOfPlayer, &currentPlayer, players);
    }
    draw_grid(emptyGrid, height, width);
    
    play_game(emptyGrid, height, width, players, numOfPlayer, currentPlayer);
    
    return 0;                                                        
}

/*
 * Function: graw_grid
 *
 * draw the grid we have
 *
 * emptyGrid: the grid we work on 
 * height: the height of grid
 * width: the width of grid
 *
 * return void
 */
void draw_grid(char **grid, int height, int width) {
    int j;
    int k;
    for(j = 0; j < 2 * height + 1; j++) {
        for(k = 0; k < 2 * width + 1; k++) {
            printf("%c", grid[j][k]);
        }
        printf("\n");
    }
}

/*
 * Function: is_cell_completed
 *
 * to judge whether a cell is completed
 *
 * cellRow: the row we work on
 * cellColumn: the column we work on
 * maxHeight: the max height of grid
 * maxWidth: the max width of grid
 * grid: the grid we work on
 *
 * return 0 if no cell is completed
 * return 1 if cell is completed
 */
int is_cell_completed(int cellRow, int cellColumn,
        int maxHeight, int maxWidth, char **grid) {
    if(cellRow >= maxHeight || cellColumn >= maxWidth
            || cellRow < 0 || cellColumn < 0) {
        return 0;
    }
    
    if(grid[2 * cellRow][2 * cellColumn + 1] == '-' 
            && grid[2 * cellRow + 1][2 * cellColumn] == '|' 
            && grid[2 * cellRow + 2][2 * cellColumn + 1] == '-'
            && grid[2 * cellRow + 1][2 * cellColumn + 2] == '|') {
        return 1;
    }
    return 0;
}

/*
 * Function: is_game_complete
 *
 * to judge whether game is over
 *
 * height: the height of grid
 * width: the width of grid
 * grid: the grid we work on
 *
 * return 0 if game is not over
 * return 1 if game is over
 */    
int is_game_complete(char **grid, int height, int width) {
    int j;
    int k;
    for(j = 0; j < 2 * height + 1; j++) {
        for(k = 0; k < 2 * width + 1; k++) {
            if(grid[j][k] == ' ') {
                return 0;
            }
        }
    }
    return 1;
}





    
     