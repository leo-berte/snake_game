#include <LedControl.h>
#include <binary.h>
#define N 16 // rows
#define S 35 // speed

// Pin 10 is connected to the DATA IN-pin of the first MAX7221
// Pin 12 is connected to the CLK-pin of the first MAX7221
// Pin 11 is connected to the LOAD(/CS)-pin of the first MAX7221
 
LedControl lc = LedControl(10, 12, 11, 4); 
int xPin = A1;
int yPin = A0;
int yPosition = 0;
int xPosition = 0;

typedef char Grid[N][N];

typedef struct {
    int i;
    int j;
} Cell;

int left = 0, right = 0, up = 0, down = 0, k = 0, z = 0, value, length;

// countdown animation
byte three[8] = {B00011100, B00100010, B00000010, B00011100, B00000010, B00100010, B00011100, B00000000}; // 3
byte two[8] = {B00111000, B01000100, B01000100, B00100000, B00010000, B00001000, B01111100, B00000000};   // 2
byte one[8] = {B00001000, B00011000, B00101000, B00001000, B00001000, B00001000, B00111110, B00000000};   // 1

// functions
void printGrid(Grid grid);
void initializeGrid(Grid grid);
int checkBounds(int i, int j);
void generateApple(Grid g);
void addToEnd(Cell *list, int a, int b, int length);
void removeHead(Cell *list, Grid g, int length);
int checkCollision(Cell *list, int a, int b, int length);
void updateDirection(void);
void gameOver(int points);
int getLength(Cell *list);
void displaySadFace(void);
void countdown(void);

void setup() {

    pinMode(yPin, INPUT);
    pinMode(xPin, INPUT);
   
    for (int a = 0; a < 4; a++) {
        lc.shutdown(a, false);
        lc.setIntensity(a, 6); // Set brightness to a medium value
        lc.clearDisplay(a);   // Clear the display 
    }
  
    randomSeed(analogRead(A1)); // unconnected pin
    delay(20);
}

void loop() { 

    Grid g;  
    Cell list[25];
    int i = 3, j = 2;

    initializeGrid(g);
    generateApple(g);
  
    // init snake
    g[i][j] = 'O';
    addToEnd(list, i, j, 0);  
    g[i][++j] = 'O';
    addToEnd(list, i, j, 1); 
    g[i][++j] = 'O';
    addToEnd(list, i, j, 2); 
    g[i][++j] = 'O';
    addToEnd(list, i, j, 3);
  
    list[4].i = -5;  // condition to indicate end of list
  
    countdown();    // 3... 2... 1...

    for (int a = 0; a < 4; a++)  lc.clearDisplay(a);  
                            
    right = 1; // snake starts moving without pressing any button
  
    while (checkBounds(i, j)) {  
        updateDirection(); 
    
        if (right) ++j;      
        else if (down) ++i;
        else if (up) --i;
        else if (left) --j;

        length = getLength(list);

        printGrid(g);
  
        if (checkCollision(list, i, j, length)) i = -2; // exits the while(checkBounds) loop
  
        if (g[i][j] == '#') {
            generateApple(g);
            k++;
        }
  
        g[i][j] = 'O';

        addToEnd(list, i, j, length);  
  
        if (!k) {
            removeHead(list, g, length); 
            list[length].i = -5; // condition to indicate end of list
        } else {
            list[length + 1].i = -5;
        }

        k = 0;  

        delay(S); 
    }

    gameOver(getLength(list));
}

void updateDirection(void) {
    xPosition = analogRead(xPin);
    delay(2);
    yPosition = analogRead(yPin);          
    delay(2);

    if (yPosition >= 450 && yPosition <= 600 && up == 1) {
        up = 1; down = 0; right = 0; left = 0;
    }
    if (yPosition >= 450 && yPosition <= 600 && down == 1) {
        down = 1; up = 0; right = 0; left = 0;
    }
    if (yPosition < 450 && yPosition >= 0) {
        down = 0; up = 1; right = 0; left = 0;
    } else if (yPosition > 600 && yPosition < 1024) {
        up = 0; down = 1; right = 0; left = 0;
    }
  
    if (xPosition >= 450 && xPosition <= 600 && right == 1) {
        right = 1; down = 0; up = 0; left = 0;
    }
    if (xPosition >= 450 && xPosition <= 600 && left == 1) {
        left = 1; down = 0; up = 0; right = 0;
    }
    if (xPosition < 450 && xPosition >= 0) {
        left = 0; right = 1; down = 0; up = 0;
    } else if (xPosition > 600 && xPosition < 1024) {
        right = 0; left = 1; down = 0; up = 0;
    }
}

int getLength(Cell *list) {
    int count = 0;
    while (list[count].i != -5) count++;
    return count;
}

void initializeGrid(Grid grid) {        
    for (int i = 0; i < N; i++) 
        for (int j = 0; j < N; j++)
            grid[i][j] = ' ';
}

void printGrid(Grid grid) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] != ' ') {
                if (i < 8 && j < 8)
                    lc.setLed(0, i, j, HIGH);
                else if (i < 8 && j > 7)
                    lc.setLed(2, map(i, 0, 7, 7, 0), map(j - 8, 0, 7, 7, 0), HIGH);
                else if (i > 7 && j < 8)
                    lc.setLed(1, i - 8, j, HIGH);
                else
                    lc.setLed(3, map(i - 8, 0, 7, 7, 0), map(j - 8, 0, 7, 7, 0), HIGH);
            } else {
                if (i < 8 && j < 8)
                    lc.setLed(0, i, j, LOW);
                else if (i < 8 && j > 7)
                    lc.setLed(2, map(i, 0, 7, 7, 0), map(j - 8, 0, 7, 7, 0), LOW);
                else if (i > 7 && j < 8)
                    lc.setLed(1, i - 8, j, LOW);
                else
                    lc.setLed(3, map(i - 8, 0, 7, 7, 0), map(j - 8, 0, 7, 7, 0), LOW);
            }
        }  
    }
}

int checkBounds(int i, int j) {
    return i >= 0 && i < N && j >= 0 && j < N;  
}

void generateApple(Grid g) {
    int n, m;   
     
    do {
        n = random(0, N); // random from 0 to N-1
        m = random(0, N); // random from 0 to N-1
    } while (g[n][m] == 'O' || g[n][m] == '#');
    
    g[n][m] = '#';  
}

void addToEnd(Cell *list, int a, int b, int length) {
    list[length].i = a;
    list[length].j = b;
}   
   
void removeHead(Cell *list, Grid g, int length) {
    g[list[0].i][list[0].j] = ' ';

    for (int n = 0; n < length; n++) {
        list[n].i = list[n + 1].i;
        list[n].j = list[n + 1].j;
    }
}  

int checkCollision(Cell *list, int a, int b, int length) {
    for (int i = 0; i < length; i++) 
        if (a == list[i].i && b == list[i].j) 
            return 1;

    return 0;
}

void gameOver(int points) {
    for (int a = 0; a < 4; a++) lc.clearDisplay(a);  
  
    displaySadFace();
    delay(1000);

    for (int a = 0; a < 4; a++) lc.clearDisplay(a);  

    int counter = points;
    if (counter < 10) {
        while (counter != 0) {
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++)
                    if (j < 8)
                        lc.setLed(0, i, j, HIGH);
                    else
                        lc.setLed(1, i, j - 8, HIGH);

            delay(40);
            for (int a = 0; a < 4; a++) lc.clearDisplay(a); 
            delay(40);
            counter--;
        }
    }
    for (int a = 0; a < 4; a++) lc.clearDisplay(a);  
    delay(500);

    if (points < 100) {
        for (int i = 0; i < 8; i++)
            lc.setRow(2, i, one[i]);  // 1
        for (int i = 0; i < 8; i++)
            lc.setRow(0, i, one[i]);  // 1
    }
    if (points >= 100 && points < 1000) {
        for (int i = 0; i < 8; i++)
            lc.setRow(0, i, one[i]);  // 1
        for (int i = 0; i < 8; i++)
            lc.setRow(1, i, two[i]);  // 2
    }
    if (points >= 1000) {
        for (int i = 0; i < 8; i++)
            lc.setRow(1, i, three[i]);  // 3
        for (int i = 0; i < 8; i++)
            lc.setRow(0, i, three[i]);  // 3
    }
    delay(1500);
    for (int a = 0; a < 4; a++) lc.clearDisplay(a);  
}

void displaySadFace(void) {
    byte eye = B01100110;
    byte mouth = B00111100;
  
    for (int i = 0; i < 8; i++) lc.setRow(0, i, 0);
    lc.setRow(0, 2, eye);
    lc.setRow(0, 3, eye);
    lc.setRow(0, 5, mouth);
    lc.setRow(0, 6, B00011000);
}

void countdown(void) {
    for (int i = 0; i < 8; i++) lc.setRow(2, i, three[i]);  
    for (int i = 0; i < 8; i++) lc.setRow(0, i, three[i]);  
    delay(500);

    for (int i = 0; i < 4; i++) lc.clearDisplay(i);  

    for (int i = 0; i < 8; i++) lc.setRow(2, i, two[i]);  
    for (int i = 0; i < 8; i++) lc.setRow(0, i, two[i]);  
    delay(500);
  
    for (int i = 0; i < 4; i++) lc.clearDisplay(i);  

    for (int i = 0; i < 8; i++) lc.setRow(2, i, one[i]);  
    for (int i = 0; i < 8; i++) lc.setRow(0, i, one[i]);  
    delay(500);

    for (int i = 0; i < 4; i++) lc.clearDisplay(i);  
}