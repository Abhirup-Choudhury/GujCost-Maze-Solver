import numpy as np
from collections import deque

# Screen dimensions and grid settings
q = deque(maxlen=10)
START_CELL = (0, 4)
END_CELL = (4, 0)
MAZE_SIZE = 5
CELL_SIZE = 100

v_walls = np.zeros((MAZE_SIZE, MAZE_SIZE-1))
h_walls = np.zeros((MAZE_SIZE-1, MAZE_SIZE))
flood_values = np.zeros((MAZE_SIZE, MAZE_SIZE), dtype=np.int)
visited = np.zeros((MAZE_SIZE, MAZE_SIZE), dtype=np.int)
curr = START_CELL


def initialize_floodfill(maze):
    update_walls(curr[0], curr[1], maze.cells[curr[1]][curr[0]])
    for i in range(MAZE_SIZE):
        for j in range(MAZE_SIZE):
            # Unpack the cell coordinates
            x, y = i, j
            end_x, end_y = END_CELL
            # calculate Mahattan Distance
            distance = abs(end_x-x) + abs(end_y-y)
            flood_values[j][i] = distance
            maze.cells[j][i].value = distance


def update_walls(curr_x, curr_y, cell):
    # update the wall values in the array if the current cell is not visited.
    if curr_y != MAZE_SIZE-1 and cell.bottom_wall:
        # if it isnt the bottom row then update that cell
        h_walls[curr_y][curr_x] = 1
    if curr_y != 0 and cell.top_wall:
        # if cell has top wall, update the upper value.
        # but if the cell is the top row, dont do it. it will give error.
        h_walls[curr_y-1][curr_x] = 1

    if curr_x != MAZE_SIZE-1 and cell.right_wall:
        # if it isnt the rightmost col then update that cell
        v_walls[curr_y][curr_x] = 1
    if curr_x != 0 and cell.left_wall:
        # if cell has left wall, update the left value.
        # but if the cell is the leftmost col, dont do it. it will give error.
        v_walls[curr_y][curr_x-1] = 1

    cell.walls_visible = True
    # print("Horizontal walls\n")
    # print(h_walls)
    # print("\nVertical Walls\n")
    # print(v_walls)


def check_neighbours(x: int, y: int):
    neighbours = []
    # Check all neighbours
    if y < MAZE_SIZE - 1 and h_walls[y][x] == 0:  # Down
        neighbours.append(((x, y + 1), flood_values[y + 1][x]))
    if x > 0 and v_walls[y][x - 1] == 0:  # Left
        neighbours.append(((x - 1, y), flood_values[y][x - 1]))
    if y > 0 and h_walls[y - 1][x] == 0:  # Up
        neighbours.append(((x, y - 1), flood_values[y - 1][x]))
    if x < MAZE_SIZE - 1 and v_walls[y][x] == 0:  # Right
        neighbours.append(((x + 1, y), flood_values[y][x + 1]))

    return neighbours


def floodfill(maze):
    print("inside floodfill()")
    print(f'curr: {curr}')
    """ push the neighbour cells to a queue
        update the floodfill values"""
    global q
    # append the current cell to q
    current_cell = (curr, flood_values[curr[1]][curr[0]])
    q.append(current_cell)
    # iterate as long is q isn't empty
    while len(q) != 0:
        print(f'q: {q}')
        # breakpoint()
        # front_el refers to element at the front of q
        front_el = q.popleft()
        print(f'front_el: {front_el}')
        # breakpoint()
        (x, y) = front_el[0]
        neighbours = check_neighbours(x, y)
        print(f'neighbours: {neighbours}')
        # finding min value among neighbours
        min_value = float('inf')
        for _, value in neighbours:
            if value < min_value:
                min_value = value
        """ if value of front_el is less than min of neighbours
            add +1 to the front_el cell
            and push all neighbours into the q """
        if front_el[1] <= min_value:
            for cell in neighbours:
                if cell not in q:
                    q.append(cell)
                    print(f'Appending: {cell}')
            # breakpoint()
            print(f'value of ({x,y}): {front_el[1]} ----> {front_el[1]+1}')
            flood_values[y][x] += 1
            maze.cells[y][x].value += 1  # updating for visual purposes


def move_to_next_cell(maze, mouse):
    # check for walls in front,left,right of the mouse.
    # move to the cell with lower flood value
    # if no lower floodvalue availble, do floodfill()
    # found implies that was a valid neighbour found so that we can move to it.
    found = False
    global curr  # Current position of the mouse
    print(f'after global curr: {curr}')

    x, y = curr

    # To store possible moves with their flood values
    neighbours = check_neighbours(x, y)
    # find the direction with the lowest flood value
    min_value = float('inf')
    next_cell = None
    for cell, value in neighbours:
        if value < min_value and value < flood_values[y][x]:
            min_value = value
            next_cell = cell
            found = True

    if found:
        # Move to the next cell
        curr = next_cell
        visited[curr[1]][curr[0]] = 1  # Mark as visited
        mouse.move_to(curr)
        # Update walls for the new position
        update_walls(curr[0], curr[1], maze.cells[curr[1]][curr[0]])
        return
    else:
        # If no valid move, call floodfill and try again
        floodfill(maze)
