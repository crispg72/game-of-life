import pygame
import random


GRID_SIZE = 50
CELL_SIZE = 20

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
        pygame.time.Clock().tick(60)

    pygame.quit()


if __name__ == "__main__":
    main()
