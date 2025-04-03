#include "queues.h"
// set these 3 values based on maze design
const Point START_CELL = {0, 5};
const Point END_CELL = {5, 0};
const short MAZE_SIZE = 6;
short ROTATION_IDX = 1; //(0 = Up, 1 = Right, 2 = Down, 3 = Left) set ROTATION_IDX based on maze

short v_walls[MAZE_SIZE][MAZE_SIZE - 1] = {0};
short h_walls[MAZE_SIZE - 1][MAZE_SIZE] = {0};
short flood_values[MAZE_SIZE][MAZE_SIZE] = {0};

// below 3 lines are changing variables and must be defined in the main file.
Queue q(12);
Queue neighbours(4);
Point curr = START_CELL;
Point prev = {0,0};


// will get currWallData from sensor which will be passed to the functions.
// WallData currWallData = {true, false, false};
struct WallData
{
    bool has_left_wall;
    bool has_front_wall;
    bool has_right_wall;
};
WallData getWallData();
// the above function shall be written in the main
//  file wrt to the bot's components.

// A complicated lookup table that I found from somewhere after a lot of research.
// Needs to be used to gather data from actual ir sensors.
struct WallOffsets
{
    short front_dx;
    short front_dy;
    short left_dx;
    short left_dy;
    short right_dx;
    short right_dy;
};
// Table of offsets for each rotation (0 = Up, 1 = Right, 2 = Down, 3 = Left)
WallOffsets rotation_offsets[] = {
    {0, -1, -1, 0, 1, 0}, // Facing Up (ROTATION_IDX = 0)
    {1, 0, 0, -1, 0, 1},  // Facing Right (ROTATION_IDX = 1)
    {0, 1, 1, 0, -1, 0},  // Facing Down (ROTATION_IDX = 2)
    {-1, 0, 0, 1, 0, -1}  // Facing Left (ROTATION_IDX = 3)
};
// wallData will be provided by the sensors, containing 3 booleans
// roation_idx must be maintained using mod4 arightmatic while performing rotations.
// we will use the look up table to find the coordinates of the neighbouring
// based on wallData and update the wall arrays.

// USE THIS FUNCTION WHEN WE WILL USE IR
void update_walls(const short ROTATION_IDX, const Point curr, const WallData wallData)
{
    // Get the appropriate offsets based on the ROTATION_IDX
    WallOffsets offsets = rotation_offsets[ROTATION_IDX];

    // Update front wall
    if (wallData.has_front_wall)
    {
        short front_x = curr.x + offsets.front_dx;
        short front_y = curr.y + offsets.front_dy;

        // Check for boundaries. front_wall coords should be b/w [0,MAZE_SIZE)
        if (front_x >= 0 && front_x < MAZE_SIZE && front_y >= 0 && front_y < MAZE_SIZE)
        {
            // Update the current cell's front wall (horizontal or vertical)
            if (offsets.front_dy == 0)
                v_walls[curr.y][curr.x] = 1; // Vertical wall
            else
                h_walls[curr.y][curr.x] = 1; // Horizontal wall
        }
    }

    // Update left wall
    if (wallData.has_left_wall)
    {
        short left_x = curr.x + offsets.left_dx;
        short left_y = curr.y + offsets.left_dy;

        if (left_x >= 0 && left_x < MAZE_SIZE && left_y >= 0 && left_y < MAZE_SIZE)
        {
            if (offsets.left_dy == 0)
                v_walls[curr.y][curr.x] = 1; // Vertical wall (affects v_walls)
            else
                h_walls[curr.y][curr.x] = 1; // Horizontal wall (affects h_walls)
        }
    }

    // Update right wall
    if (wallData.has_right_wall)
    {
        short right_x = curr.x + offsets.right_dx;
        short right_y = curr.y + offsets.right_dy;

        if (right_x >= 0 && right_x < MAZE_SIZE && right_y >= 0 && right_y < MAZE_SIZE)
        {
            // Update the current cell's right wall (horizontal or vertical)
            if (offsets.right_dy == 0)
                v_walls[curr.y][curr.x] = 1; // Vertical wall (affects v_walls)
            else
                h_walls[curr.y][curr.x] = 1; // Horizontal wall (affects h_walls)
        }
    }
}

// Remove this class when using IR sensors!!
class MazeCell
{
public:
    // Coordinates of the cell
    Point coords;
    // Wall indicators
    bool top_wall;
    bool right_wall;
    bool bottom_wall;
    bool left_wall;
    // Constructor
    MazeCell(short x, short y, bool top, bool right, bool bottom, bool left)
        : coords({x, y}), top_wall(top), right_wall(right),
          bottom_wall(bottom), left_wall(left) {}
};

// for simulating a maze
void _update_walls(const Point curr, const MazeCell cell) // use the other update_walls function not this one.
{
    // If it isn't the bottom row and the cell has a bottom wall, update h_walls.
    if (curr.y != MAZE_SIZE - 1 && cell.bottom_wall)
    {
        h_walls[curr.y][curr.x] = 1;
    }

    // If it isn't the top row and the cell has a top wall, update h_walls for the row above.
    if (curr.y != 0 && cell.top_wall)
    {
        h_walls[curr.y - 1][curr.x] = 1;
    }

    // If it isn't the rightmost column and the cell has a right wall, update v_walls.
    if (curr.x != MAZE_SIZE - 1 && cell.right_wall)
    {
        v_walls[curr.y][curr.x] = 1;
    }

    // If it isn't the leftmost column and the cell has a left wall, update v_walls for the column to the left.
    if (curr.x != 0 && cell.left_wall)
    {
        v_walls[curr.y][curr.x - 1] = 1;
    }
}

// custom absolute value function made by me
// short abs(short x)
// {
//     return (x >= 0) ? x : -x;
// };
// initialize the floodvalues at the start.
// void initialize_floodfill(MazeCell cell) // use this for simulating maze
void initialize_floodfill(WallData wallData)
{
    // update_walls(curr, cell);
    // WallData wallData = getWallData();
    update_walls(ROTATION_IDX, curr, wallData);
    for (short i = 0; i < MAZE_SIZE; i++)
    {
        for (short j = 0; j < MAZE_SIZE; j++)
        {
            short distance = abs(END_CELL.x - i) + abs(END_CELL.y - j);
            flood_values[j][i] = distance;
        }
    }
}

void getNeighbours(Queue& neighbours, const Point curr)
{
    // clear the neighbours queue before proceeding.
    neighbours.flush();

    // Check Down neighbor (y + 1)
    if (curr.y < MAZE_SIZE - 1 && h_walls[curr.y][curr.x] == 0)
    {
        Cell downCell = {{curr.x, curr.y + 1}, flood_values[curr.y + 1][curr.x]};
        neighbours.push(downCell);
    }

    // Check Left neighbor (x - 1)
    if (curr.x > 0 && v_walls[curr.y][curr.x - 1] == 0)
    {
        Cell leftCell = {{curr.x - 1, curr.y}, flood_values[curr.y][curr.x - 1]};
        neighbours.push(leftCell);
    }

    // Check Up neighbor (y - 1)
    if (curr.y > 0 && h_walls[curr.y - 1][curr.x] == 0)
    {
        Cell upCell = {{curr.x, curr.y - 1}, flood_values[curr.y - 1][curr.x]};
        neighbours.push(upCell);
    }

    // Check Right neighbor (x + 1)
    if (curr.x < MAZE_SIZE - 1 && v_walls[curr.y][curr.x] == 0)
    {
        Cell rightCell = {{curr.x + 1, curr.y}, flood_values[curr.y][curr.x + 1]};
        neighbours.push(rightCell);
    }
}

void floodfill(Queue& q, const Point curr)
{
    Cell currentCell = {curr, flood_values[curr.y][curr.x]};
    q.push(currentCell);

    while (!q.isEmpty())
    {
        Cell front = q.pop();
        short x = front.point.x;
        short y = front.point.y;
        getNeighbours(neighbours, {x, y});

        short min_value = 100;
        for (short i = 0; i < neighbours.getItemCount(); i++)
        {
            short value = neighbours[i].floodValue;
            if (value < min_value)
            {
                min_value = value;
            }
        }

        if (front.floodValue <= min_value)
        {
            for (short i = 0; i < neighbours.getItemCount(); i++)
            {
                Cell cell = neighbours[i];
                if (q.find(cell) == -1)
                {
                    q.push(cell);
                }
            }

            flood_values[y][x] += 1;
        }
    }
}

// Point move_to_next_cell(MazeCell currentCell, Point &curr)
// UNCOMMENT below line for ir reading
Point move_to_next_cell(Point &curr)
{
    // update_walls(curr, currentCell);
    // UNCOMMENT below line for ir reading
     update_walls(ROTATION_IDX, curr, getWallData());

    // Check for walls in front, left, right of the mouse.
    // Move to the cell with lower flood value.
    // If no lower flood value available, call floodfill.
    bool found = false;
    // Get possible moves (neighbors with flood values)
    getNeighbours(neighbours, curr); // Use a custom Queue to store possible moves
    Point next_cell = {0, 0};        // Default point in case no valid move is found.
    // Find the direction with the lowest flood value
    short min_value = 100;
    for (short i = 0; i < neighbours.getItemCount(); i++)
    {
        short value = neighbours[i].floodValue;
        if (value < min_value && value < flood_values[curr.y][curr.x])
        {
            min_value = value;
            next_cell = neighbours[i].point;
            found = true;
        }
    }

    if (found)
    {
        // Move to the next cell
        prev = curr;
        curr = next_cell;
        return next_cell;
    }
    else
    {
        // If no valid move, call floodfill and try again
        floodfill(q, curr);
    }
}
