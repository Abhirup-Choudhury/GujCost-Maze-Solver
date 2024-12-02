from floodfill import *
import floodfill
import pygame

pygame.init()

# Dimensions based on pixels
WALL_SIZE = int(0.1 * CELL_SIZE)
DIMENSION = MAZE_SIZE * (CELL_SIZE + WALL_SIZE) + WALL_SIZE
SCREEN_WIDTH, SCREEN_HEIGHT = DIMENSION, DIMENSION

# Colors
WHITE = (255, 255, 255)
BLUE = (0, 0, 255, 0)
CELL_COLOR = (180, 184, 191)
BG_COLOR = (213, 215, 219)

# Create the screen
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Maze Visualization")
clock = pygame.time.Clock()


class Mouse(pygame.sprite.Sprite):
    def __init__(self):
        super(Mouse, self).__init__()
        # self.surf = pygame.image.load("mouse.png").convert_alpha()

        # Load all 4 images
        self.images = [
            pygame.image.load("assets\mouse_up.png").convert_alpha(),  # up
            pygame.image.load("assets\mouse_right.png").convert_alpha(),  # right
            pygame.image.load("assets\mouse_down.png").convert_alpha(),  # down
            pygame.image.load("assets\mouse_left.png").convert_alpha()  # left
        ]
        # Set the surface to initial picture, use self.rotation to keep track.
        self.surf = self.images[0]
        self.rotidx = 0
        self.surf.set_colorkey((0, 0, 0), pygame.RLEACCEL)
        self.rect = self.surf.get_rect()
        self.coords = (0, 0)
        self.draw_at_cell(START_CELL)

    def draw_at_cell(self, next_coord: tuple[int, int]):
        x, y = next_coord
        self.coords = (x, y)
        self.rect.center = (x * (CELL_SIZE + WALL_SIZE) + WALL_SIZE + 0.5*CELL_SIZE,
                            y * (CELL_SIZE + WALL_SIZE) + WALL_SIZE + 0.5*CELL_SIZE)

    def rotate(self, orientation: int):
        # Load the appropriate image based on the current index
        self.surf = self.images[orientation]
        self.rotidx = orientation
        # Update rect to maintain position
        self.rect = self.surf.get_rect(center=self.rect.center)

    # Use this function to move the mouse to specific cell
    # Delays are used only for visual purposes
    def move_to(self, next_coord: tuple[int, int]):
        x, y = next_coord
        curr_x, curr_y = self.coords
        orientation = self.direction_mapping((x-curr_x, y-curr_y))
        if (orientation != -1):
            self.rotate(orientation)
        self.draw_at_cell(next_coord)
        pygame.time.wait(300)

    def direction_mapping(self, diff: tuple[int, int]):
        # Mapping for the directions
        direction_map = {
            (0, -1): 0,  # Up
            (1, 0): 1,   # Right
            (0, 1): 2,   # Down
            (-1, 0): 3   # Left
        }

        # Return -1 if the diff is not valid
        return direction_map.get(diff, -1)

    def render(self, screen: pygame.Surface):
        screen.blit(mouse.surf, mouse.rect)


class MazeCell:
    def __init__(self, x, y, top: bool, right: bool, bottom: bool, left: bool):
        self.coords = (x, y)
        self.top_wall = top
        self.right_wall = right
        self.bottom_wall = bottom
        self.left_wall = left
        self.value = 0
        self.walls_visible = False

    def render(self, screen, font):
        x, y = self.coords
        x = x * (CELL_SIZE + WALL_SIZE)
        y = y * (CELL_SIZE + WALL_SIZE)

        # # Draw walls based if its walls are visible
        if self.walls_visible:

            if self.top_wall:
                pygame.draw.rect(
                    screen, BLUE, (x, y, CELL_SIZE + 2*WALL_SIZE, WALL_SIZE))
            if self.right_wall:
                pygame.draw.rect(
                    screen, BLUE, (x + CELL_SIZE + WALL_SIZE, y, WALL_SIZE, CELL_SIZE + 2*WALL_SIZE))
            if self.bottom_wall:
                pygame.draw.rect(
                    screen, BLUE, (x, y + CELL_SIZE + WALL_SIZE, CELL_SIZE + 2*WALL_SIZE, WALL_SIZE))
            if self.left_wall:
                pygame.draw.rect(
                    screen, BLUE, (x, y, WALL_SIZE, CELL_SIZE + 2*WALL_SIZE))

        pygame.draw.rect(screen, CELL_COLOR,
                         (x + WALL_SIZE, y + WALL_SIZE, CELL_SIZE, CELL_SIZE))

        # Render floodfill value as text
        text = font.render(str(self.value), True, (0, 0, 0))  # Black text
        text_rect = text.get_rect(
            topleft=(x + 20 + WALL_SIZE / 2, y + 20 + WALL_SIZE / 2))
        screen.blit(text, text_rect)


class Maze:
    def __init__(self, size: int, cell_structure: list[list[MazeCell]]):
        self.size = size
        self.cells = cell_structure

    def render(self, screen: pygame.Surface):
        font = pygame.font.Font(None, 36)
        for row in self.cells:
            for cell in row:
                cell.render(screen, font)


cell_structure = [
    [
        MazeCell(0, 0, top=True, right=False, bottom=False, left=True),
        MazeCell(1, 0, top=True, right=False, bottom=True, left=False),
        MazeCell(2, 0, top=True, right=False, bottom=True, left=False),
        MazeCell(3, 0, top=True, right=False, bottom=False, left=False),
        MazeCell(4, 0, top=True, right=True, bottom=False, left=False),
    ],
    [
        MazeCell(0, 1, top=False, right=False, bottom=True, left=True),
        MazeCell(1, 1, top=True, right=False, bottom=False, left=False),
        MazeCell(2, 1, top=True, right=True, bottom=True, left=False),
        MazeCell(3, 1, top=False, right=True, bottom=False, left=True),
        MazeCell(4, 1, top=False, right=True, bottom=False, left=True),
    ],
    [
        MazeCell(0, 2, top=True, right=False, bottom=False, left=True),
        MazeCell(1, 2, top=False, right=True, bottom=False, left=False),
        MazeCell(2, 2, top=True, right=True, bottom=False, left=True),
        MazeCell(3, 2, top=False, right=True, bottom=True, left=True),
        MazeCell(4, 2, top=False, right=True, bottom=False, left=True),
    ],
    [
        MazeCell(0, 3, top=False, right=True, bottom=False, left=True),
        MazeCell(1, 3, top=False, right=True, bottom=False, left=True),
        MazeCell(2, 3, top=False, right=False, bottom=True, left=True),
        MazeCell(3, 3, top=True, right=False, bottom=True, left=False),
        MazeCell(4, 3, top=False, right=True, bottom=False, left=False),
    ],
    [
        MazeCell(0, 4, top=False, right=True, bottom=True, left=True),
        MazeCell(1, 4, top=False, right=False, bottom=True, left=True),
        MazeCell(2, 4, top=True, right=False, bottom=True, left=False),
        MazeCell(3, 4, top=True, right=False, bottom=True, left=False),
        MazeCell(4, 4, top=False, right=True, bottom=True, left=False),
    ]
]

maze = Maze(MAZE_SIZE, cell_structure)
mouse = Mouse()


initialize_floodfill(maze)
# Main loop
start = False
running = True
reached_goal = False
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_RETURN:
                print(f'curr: {curr}')
                print(f'q: {q}')
                # move to next cellv on pressing RETURN key, until reached goal
                if not reached_goal:
                    move_to_next_cell(maze, mouse)
            elif event.key == pygame.K_SPACE:
                # on pressing SPACE, set mouse to starting cell and changed the reached_goal flag.
                mouse.move_to(START_CELL)
                floodfill.curr = START_CELL
                reached_goal = False

    if END_CELL == mouse.coords:
        reached_goal = True
        print("emptying queue...")
        floodfill.q.clear()
        print(f'q: {q}')
        print('--------------------------')

    # Clear screen
    screen.fill(BG_COLOR)
    # Render the maze
    maze.render(screen)
    # Render the mouse
    mouse.render(screen)
    # Update the display
    pygame.display.flip()
    # Control frame rate
    clock.tick(60)

# Quit Pygame
pygame.quit()
