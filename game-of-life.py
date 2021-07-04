import pygame
import random


GRID_SIZE = 200
CELL_SIZE = 10


def get_neighbours(grid, x, y):
    num_neighbours = grid[(y - 1) % GRID_SIZE][(x - 1) % GRID_SIZE]
    num_neighbours += grid[(y - 1) % GRID_SIZE][x]
    num_neighbours += grid[(y - 1) % GRID_SIZE][(x + 1) % GRID_SIZE]
    num_neighbours += grid[y][(x - 1) % GRID_SIZE]
    num_neighbours += grid[y][(x + 1) % GRID_SIZE]
    num_neighbours += grid[(y + 1) % GRID_SIZE][(x - 1) % GRID_SIZE]
    num_neighbours += grid[(y + 1) % GRID_SIZE][x]
    num_neighbours += grid[(y + 1) % GRID_SIZE][(x + 1) % GRID_SIZE]

    return num_neighbours


def draw_grid(win, grid):
    for y in range(GRID_SIZE):
        for x in range(GRID_SIZE):
            # Any live cell with two or three live neighbours survives.
            if grid[y][x] == 1:
                pygame.draw.rect(
                    win,
                    (0, 255, 0),
                    (
                        x * CELL_SIZE,
                        y * CELL_SIZE,
                        CELL_SIZE,
                        CELL_SIZE,
                    ),
                )


def update_grid(win, source_grid, destination_grid):
    for y in range(GRID_SIZE):
        for x in range(GRID_SIZE):
            num_neighbours = get_neighbours(source_grid, x, y)

            next_generation = 0

            # Any live cell with two or three live neighbours survives.
            if source_grid[y][x] == 1:
                # Any live cell with two or three live neighbours survives.
                if (num_neighbours > 1) and (num_neighbours < 4):
                    next_generation = 1
            else:
                # Any dead cell with three live neighbours becomes a live cell.
                if num_neighbours == 3:
                    next_generation = 1

            # All other live cells die in the next generation. Similarly, all other dead cells stay dead.
            destination_grid[y][x] = next_generation


def main():
    pygame.init()
    win = pygame.display.set_mode((GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE))
    pygame.display.set_caption("Game Of Life")

    grids = [[], []]
    for y in range(GRID_SIZE):
        grids[0].append([])
        grids[1].append([])
        for x in range(GRID_SIZE):
            grids[0][y].append(random.randrange(2))
            grids[1][y].append(0)

    run = 0
    source = 0
    while run < 1000:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False

        background = (0, 0, 0)
        win.fill(background)

        update_grid(win, grids[source], grids[source ^ 1])
        draw_grid(win, grids[source])
        pygame.display.flip()

        source = (source + 1) % 2
        run += 1

    pygame.quit()


if __name__ == "__main__":
    main()
