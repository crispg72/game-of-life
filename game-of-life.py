import pygame
import random


GRID_SIZE = 50
CELL_SIZE = 20


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


def update_grid(source_grid):
    destination_grid = []
    for y in range(GRID_SIZE):
        destination_grid.append([])
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
            destination_grid[y].append(next_generation)

    return destination_grid


def main():
    pygame.init()
    win = pygame.display.set_mode((GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE))
    pygame.display.set_caption("Game Of Life")

    grid = []
    for y in range(GRID_SIZE):
        grid.append([])
        for x in range(GRID_SIZE):
            grid[y].append(random.randrange(2))

    run = True
    while run:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False

        background = (0, 0, 0)
        win.fill(background)

        for y in range(GRID_SIZE):
            for x in range(GRID_SIZE):
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

        pygame.display.flip()

        grid = update_grid(grid)
        pygame.time.Clock().tick(60)

    pygame.quit()


if __name__ == "__main__":
    main()
