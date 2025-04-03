#include "floodfill.h"

#define button 13
float sensorValue[3];
int sensorPins[3] = { A2, A1, A0 };  // Left, Front, Right
#define THRESHOLD 9

// Remove this function later
void printMatrix(short matrix[6][6]) {
  Serial.println("flood_values:");
  for (size_t i = 0; i < 6; i++) {
    for (size_t j = 0; j < 6; j++) {
      Serial.print(matrix[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
}
void printVWalls(short matrix[6][6 - 1]) {
  Serial.println("v walls:");
  for (size_t i = 0; i < 6; i++) {
    for (size_t j = 0; j < 6; j++) {
      Serial.print(matrix[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
}
void printHWalls(short matrix[6 - 1][6]) {
  Serial.println("h walls:");
  for (size_t i = 0; i < 6; i++) {
    for (size_t j = 0; j < 6; j++) {
      Serial.print(matrix[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

// MazeCell cellStructure[MAZE_SIZE][MAZE_SIZE] = {
//     {
//         MazeCell(0, 0, true, false, false, true),
//         MazeCell(1, 0, true, false, true, false),
//         MazeCell(2, 0, true, false, true, false),
//         MazeCell(3, 0, true, false, false, false),
//         MazeCell(4, 0, true, true, false, false),
//     },
//     {
//         MazeCell(0, 1, false, false, true, true),
//         MazeCell(1, 1, true, false, false, false),
//         MazeCell(2, 1, true, true, true, false),
//         MazeCell(3, 1, false, true, false, true),
//         MazeCell(4, 1, false, true, false, true),
//     },
//     {
//         MazeCell(0, 2, true, false, false, true),
//         MazeCell(1, 2, false, true, false, false),
//         MazeCell(2, 2, true, true, false, true),
//         MazeCell(3, 2, false, true, true, true),
//         MazeCell(4, 2, false, true, false, true),
//     },
//     {
//         MazeCell(0, 3, false, true, false, true),
//         MazeCell(1, 3, false, true, false, true),
//         MazeCell(2, 3, false, false, true, true),
//         MazeCell(3, 3, true, false, true, false),
//         MazeCell(4, 3, false, true, false, false),
//     },
//     {
//         MazeCell(0, 4, false, true, true, true),
//         MazeCell(1, 4, false, false, true, true),
//         MazeCell(2, 4, true, false, true, false),
//         MazeCell(3, 4, true, false, true, false),
//         MazeCell(4, 4, false, true, true, false),
//     }
// };


void readWall() {
  for (int i = 0; i < 3; i++) {
    int rawValue = analogRead(sensorPins[i]);
    float voltage = rawValue * (5.0 / 1023.0);
    sensorValue[i] = 12.08 / (voltage - 0.2);
    if (!(sensorValue[i] >= 4.0 && sensorValue[i] <= 32.0)) {
      sensorValue[i] = 100.0;
    }
  }
}

WallData getWallData() {  // wall data from ir sensor.
  Serial.println("Reading wall");
  readWall();
  Serial.println("Getting wall data.");
  bool hasWall[3] = { sensorValue[0] < THRESHOLD, sensorValue[1] < THRESHOLD, sensorValue[2] < THRESHOLD };
  WallData wallData = { hasWall[0], hasWall[1], hasWall[2] };
  return wallData;
}

void moveBotForward() {  //Move bot forward for a specified distance. Direct commands to motors.
  Serial.println("Moving forward");
}
void rotateBot(short target_orientation)  //Add commands to motors for specific rotations
{
  short diff = (target_orientation - ROTATION_IDX + 4) % 4;  //in range [0, 3]

  if (diff == 1) {
    // Rotate 90 degrees clockwise (right)
    Serial.println("Rotating Right");
    // Add motor commands for 90-degree clockwise rotation here
  } else if (diff == 3) {
    // Rotate 90 degrees counterclockwise (left)
    Serial.println("Rotating Left");
    // Add motor commands for 90-degree counterclockwise rotation here
  } else if (diff == 2) {
    // Rotate 180 degrees (backward)
    Serial.println("Rotating Backward");
    // Add motor commands for 180-degree rotation here
  } else if (diff == 0) {
    // No rotation needed
    Serial.println("No Rotation Needed");
  }

  ROTATION_IDX = target_orientation;
}

void moveBot(Point next) {
  Serial.print("currently at ");
  Serial.println(prev.x, prev.y);
  short orientation = mapDirection(next.x - prev.x, next.y - prev.y);
  if (orientation != -1) {
    rotateBot(orientation);
    moveBotForward();
  } else
    Serial.println("Invalid orientation");
}
short mapDirection(short x, short y) {  //any idea how to not use if statements?
  // (0, -1): 0/Up   (1, 0): 1/Right   (0, 1): 2/Down    (-1, 0): 3/Left
  //helper function for moveBot()
  if (x == 0 && y == -1)
    return 0;
  else if (x == 1 && y == 0)
    return 1;
  else if (x == 0 && y == 1)
    return 2;
  else if (x == -1 && y == 0)
    return 3;
  else
    return -1;
}
int trial = 1;
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) pinMode(sensorPins[i], INPUT);
  pinMode(button, INPUT_PULLUP);
  // while (!digitalRead(button)) {}
  //need to call this function initially!!
  // the next 2 lines to be used when IR is used.
  // WallData wallData = getWallData();
  Serial.println("Initializing flood fill");
  initialize_floodfill(getWallData());
  printVWalls(v_walls);
  printHWalls(h_walls);
  // initialize_floodfill(cellStructure[START_CELL.y][START_CELL.x]);
}

// void loop() {
//   while (!digitalRead(button)) {}
//   WallData wallData = getWallData();
//   Serial.print("Left: ");
//   Serial.println(wallData.has_left_wall);
//   // Serial.println(sensorValue[0]);
//   Serial.print("Front: ");
//   Serial.println(wallData.has_front_wall);
//   // Serial.println(sensorValue[1]);
//   Serial.print("Right: ");
//   Serial.println(wallData.has_right_wall);
//   // Serial.println(sensorValue[2]);
//   delay(500);
// }


void loop() {
  if (trial <= 3) {
    if (curr != END_CELL) {
      Serial.print("trial:");
      Serial.println(trial);
      while (!digitalRead(button)) {}
      // Point coord = move_to_next_cell(cellStructure[curr.y][curr.x], curr);
      Point coord = move_to_next_cell(curr);
      moveBot(coord);
      printVWalls(v_walls);
      printHWalls(h_walls);
    } else {
      curr = START_CELL;
      trial++;
    }
  } else if (trial == 4) {
    printMatrix(flood_values);
    trial++;
  }
}
