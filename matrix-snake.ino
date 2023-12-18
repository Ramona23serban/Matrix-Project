
#include <LedControl.h>

/* Joystick PINs */
#define VRX     A0
#define  VRY     A1
#define SW      2

const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER

#define SIZE    8
#define ADVANCE_DELAY  20

int snake[SIZE*SIZE][2];
int length;
int food[2], v[2];
bool  is_game_over = false;
long current_time;
long prev_advance;
int blink_count;
const  short messageSpeed = 5;


void init_game() {
  prev_advance = current_time = 0;
  blink_count  = 3;
  int half = SIZE / 2;
  length = SIZE / 3;

  for (int i = 0;  i < length; i++) {
      snake[i][0] = half - 1;
      snake[i][1] = half  + i;
  }

  food[0] = half + 1;
  food[1] = half - 1;

  v[0]  = 0;
  v[1] = -1;
}
/*this function sets up the initial state of the game, 
including the position and length of the snake, 
the position of the food, and the initial velocities
*/

void render() {
  for (int i = 0; i < length;  i++) {
    lc.setLed(0, snake[i][0], snake[i][1], 1);
  }
  lc.setLed(0,  food[0], food[1], 1);
}
//display the snake segments and the food on the LED matrix


void clearScreen() {
  for (int x = 0; x <  SIZE; x++) {
    for (int y = 0; y < SIZE; y++) {
      lc.setLed(0, x, y,  0);
    }
  }
}

/**
 * moves the snake forward
 * returns  true if the game is over
 */
bool advance() {
  int head[2] = {snake[0][0]  + v[0], snake[0][1] + v[1]};//calculates the new position of the snake's head based on its current position and velocity

  if (head[0] < 0 || head[0] >= SIZE) {

            delay(1000);
    showGameOverMessage();
      return true;
  }


  if (head[1] < 0 || head[1] >= SIZE) {

            delay(1000);
    showGameOverMessage();
      return true;
  }


////Check for Collisions with Itself
  for (int i = 0;  i < length; i++) {
      if (snake[i][0] == head[0] && snake[i][1] == head[1])  {
            delay(1000);
        showGameOverMessage();
          return  true;
      }
  }

/*If the new head position coincides with the food position, 
the snake grows in length, a new random position for the food is generated, 
and length is incremented
*/
  bool grow = (head[0] == food[0] && head[1] == food[1]);
  if (grow) {
      length++;  
      randomSeed(current_time);    
      food[0]  = random(SIZE);
      food[1] = random(SIZE);
  }

  for (int i = length  - 1; i >= 0; i--) {
      snake[i + 1][0] = snake[i][0];
      snake[i + 1][1]  = snake[i][1];
  }
  snake[0][0] += v[0];
  snake[0][1] += v[1];
  return  false;
}

void setup() {
  pinMode(SW, INPUT_PULLUP);
  pinMode(VRX,  INPUT);
  pinMode(VRY, INPUT);
  attachInterrupt(digitalPinToInterrupt(SW),  restart, RISING);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);

  init_game();
  render();
}

void loop() {
  if (!is_game_over)  {
    clearScreen();
    render();

    if (current_time - prev_advance  > ADVANCE_DELAY) {
      is_game_over = advance();
      prev_advance = current_time;    
    }
  } else {
    while (blink_count > 0) {
      clearScreen();
      delay(300);
      render();
      delay(300);
      blink_count--;     
         
    }
  }
  readControls();
  current_time++;
}

void  restart() {  
  init_game();
  is_game_over = false;
}

void readControls()  {
  int dx = map(analogRead(VRX), 0, 906, 2, -2);
  int dy = map(analogRead(VRY),  0, 906, -2, 2);
  if (dx != 0) {dx = dx / abs(dx);}
  if (dy != 0) {dy = dy  / abs(dy);}
/*normalize the values to either -1, 0, or 1.
 This step ensures that the snake moves in only one direction at a time
 */

  if (dy != 0 && v[0] != 0) {
    v[0] = 0;
    v[1] =  dy;    
  }

  if (dx != 0 && v[1] != 0) {
    v[0] = dx;
    v[1]  = 0;
  }

}

const PROGMEM bool gameOverMessage[8][90] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,1,1,0,1,0,1,1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,1,1,1,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,1,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0}
};  
//representation of a message or pattern displayed on a grid, possibly for a game over screen

void showGameOverMessage() {
  for (int d = 0; d < sizeof(gameOverMessage[0])  - 7; d++) {
    for (int col = 0; col < 8; col++) {
      delay(messageSpeed);
      for (int row = 0; row < 8; row++) {
        // this reads the byte from  the PROGMEM and displays it on the screen
        lc.setLed(0, row, col, pgm_read_byte(&(gameOverMessage[row][col  + d])));
      }
    }
  }
}
//The function essentially animates the game over message by displaying each frame on the LED matrix with a controlled delay between frames

