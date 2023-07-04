#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>

#define ROWS 20
#define COLS 11
#define TRUE 1
#define FALSE 0

char Table[ROWS][COLS] = {0};

suseconds_t timer = 500000; //half second

typedef struct {
    char GameOn;
    char restartOption;
    int score;
} gameOptions;

gameOptions menu;



typedef struct {
    int biggest;
    int lower;
} scoreMarks;

scoreMarks scoreBoard; 

    
typedef struct {
    char nome[100];
    int idade;
    int score;
} usuario;

typedef struct {
    char **array;
    int width, row, col;
} Shape;
Shape current;

const Shape ShapesArray[7]= {
    {(char *[]){(char []){0,1,1},(char []){1,1,0}, (char []){0,0,0}}, 3},                           //S_shape     
    {(char *[]){(char []){1,1,0},(char []){0,1,1}, (char []){0,0,0}}, 3},                           //Z_shape     
    {(char *[]){(char []){0,1,0},(char []){1,1,1}, (char []){0,0,0}}, 3},                           //T_shape     
    {(char *[]){(char []){0,0,1},(char []){1,1,1}, (char []){0,0,0}}, 3},                           //L_shape     
    {(char *[]){(char []){1,0,0},(char []){1,1,1}, (char []){0,0,0}}, 3},                           //ML_shape    
    {(char *[]){(char []){1,1},(char []){1,1}}, 2},                                                   //SQ_shape
    {(char *[]){(char []){0,0,0,0}, (char []){1,1,1,1}, (char []){0,0,0,0}, (char []){0,0,0,0}}, 4} //R_shape
};

Shape CopyShape(Shape shape){
    Shape new_shape = shape;
    char **copyshape = shape.array;
    new_shape.array = (char**)malloc(new_shape.width * sizeof(char*));
    int i, j;
    for(i = 0; i < new_shape.width; i++){
        new_shape.array[i] = (char*)malloc(new_shape.width * sizeof(char));
        for(j=0; j < new_shape.width; j++) {
            new_shape.array[i][j] = copyshape[i][j];
        }
    }
    return new_shape;
}

void DeleteShape(Shape shape){
    int i;
    for(i = 0; i < shape.width; i++){
        free(shape.array[i]);
    }
    free(shape.array);
}

int CheckPosition(Shape shape){ //Check the position of the copied shape
    char **array = shape.array;
    int i, j;
    for(i = 0; i < shape.width;i++) {
        for(j = 0; j < shape.width ;j++){
            if((shape.col+j < 0 || shape.col+j >= COLS || shape.row+i >= ROWS)){ //Out of borders
                if(array[i][j]) //but is it just a phantom?
                    return FALSE;
            }
            else if(Table[shape.row+i][shape.col+j] && array[i][j])
                return FALSE;
        }
    }
    return TRUE;
}

void GetNewShape(){ //returns random shape
    Shape new_shape = CopyShape(ShapesArray[rand()%7]);

    new_shape.col = rand()%(COLS-new_shape.width+1);
    new_shape.row = 0;
    DeleteShape(current);
    current = new_shape;
    if(!CheckPosition(current)){
        menu.GameOn = FALSE;
    }
}

void RotateShape(Shape shape){ //rotates clockwise
    Shape temp = CopyShape(shape);
    int i, j, k, width;
    width = shape.width;
    for(i = 0; i < width ; i++){
        for(j = 0, k = width-1; j < width ; j++, k--){
                shape.array[i][j] = temp.array[k][i];
        }
    }
    DeleteShape(temp);
}

void WriteToTable(){
    int i, j;
    for(i = 0; i < current.width ;i++){
        for(j = 0; j < current.width ; j++){
            if(current.array[i][j])
                Table[current.row+i][current.col+j] = current.array[i][j];
        }
    }
}

void Halleluyah_Baby() {
    int i, j, sum, count = 0;
    int lineCount = 0; // Count of lines cleared at once
    for (i = 0; i < ROWS; i++) {
        sum = 0;
        for (j = 0; j < COLS; j++) {
            sum += Table[i][j];
        }
        if (sum == COLS) {
            count++;
            int l, k;
            for (k = i; k >= 1; k--)
                for (l = 0; l < COLS; l++)
                    Table[k][l] = Table[k - 1][l];
            for (l = 0; l < COLS; l++)
                Table[k][l] = 0;
        }
    }
    
    if (count > 0) {
        lineCount = count;
        menu.score += 100 * count; // Bonus points for multiple line clears
    }

    // Additional bonus points based on the number of lines cleared at once
    if (lineCount >= 2) {
        menu.score += lineCount * 50;
    }
    
    timer -= 1000;
}


void PrintTable(scoreMarks scoreBoard){
    char Buffer[ROWS][COLS] = {0};
    int i, j;
    for(i = 0; i < current.width ;i++){
        for(j = 0; j < current.width ; j++){
            if(current.array[i][j])
                Buffer[current.row+i][current.col+j] = current.array[i][j];
        }
    }
    clear();
    for(i = 0; i < ROWS ;i++){
        for(j = 0; j < COLS ; j++){
            printw("%c ", (Table[i][j] + Buffer[i][j])? 'O': '.');
        }
        printw("\n");
    }
    printw("\nScore: %d\nHighest Score: %d\nLowest Score: %d\n", menu.score, scoreBoard.biggest, scoreBoard.lower);

}

void pause() {
    int gabriel = 'a';
    while (gabriel != 'p') {
        gabriel = getch();
    }
}

void ManipulateCurrent(int action){
    Shape temp = CopyShape(current);
    switch(action){
        case 's':
            temp.row++;  //move down
            if(CheckPosition(temp))
                current.row++;
            else {
                WriteToTable();
                Halleluyah_Baby(); //check full lines, after putting it down
                GetNewShape();
            }
            break;
        case 'd':
            temp.col++;  //move right
            if(CheckPosition(temp))
                current.col++;
            break;
        case 'a':
            temp.col--;  //move left
            if(CheckPosition(temp))
                current.col--;
            break;
        case 'w':
            RotateShape(temp);  //yes
            if(CheckPosition(temp))
                RotateShape(current);
            break;
        case 'p':
            pause();
            break;
    }
    DeleteShape(temp);
    PrintTable(scoreBoard);
}

void ClearTable() {
    int i, j;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            Table[i][j] = 0;
        }
    }
}


int main() {
    
    usuario newPlayer;
    menu.GameOn = TRUE;
    menu.score = 0;
    scoreBoard.lower = 0;
    scoreBoard.biggest = 0;
    
    printf("Bem-vindo ao Tetris! Para começar a jogar, entre com seu nome e idade.\n");
    printf("Nome: ");
    scanf("%s", newPlayer.nome);
    while ( getchar() != '\n' ); //Limpar buffer
    printf("\nIdade: ");
    scanf("%d", &newPlayer.idade);
    fflush(stdin); //Limpar buffer
    while ( getchar() != '\n' ); //Limpar buffer

    
    do {
        menu.GameOn = true;
        srand(time(0));
        struct timeval before, after;
        int c;
        
        initscr();
        halfdelay(1);
        menu.score = 0;
        ClearTable();
        gettimeofday(&before, NULL);
        GetNewShape();
        PrintTable(scoreBoard);
        while (menu.GameOn) {
            if ((c = getch()) != ERR) {
                ManipulateCurrent(c);
            }
            gettimeofday(&after, NULL);
            if (((double)after.tv_sec * 1000000 + (double)after.tv_usec) - ((double)before.tv_sec * 1000000 + (double)before.tv_usec) > timer) {
                before = after;
                ManipulateCurrent('s');
            }
        }
        printw("\nGame over\n");
        
        //checa se o score foi maior que o ultimo do player, se foi, atribui ao score da struct.
        if(menu.score > newPlayer.score){
            newPlayer.score = menu.score;
        }
        
        if(menu.score > scoreBoard.biggest){
            scoreBoard.biggest = menu.score;
        }
        
        if(menu.score < scoreBoard.lower || scoreBoard.lower == 0){
            scoreBoard.lower = menu.score;
        }
        
        printw("Do you want to restart? (y/n): ");
        refresh();
        menu.restartOption = getchar();
        clear();
    } while (menu.restartOption == 'y' || menu.restartOption == 'Y');
    endwin();
    DeleteShape(current);
    return 0;
}
