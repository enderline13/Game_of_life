#include <SDL.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

//Инициализация констант экрана
int SCREEN_HEIGHT = 1920;
int SCREEN_WIDTH = 1080;
int CELL_SIZE = 10;
int GRID_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;
int GRID_WIDTH = SCREEN_WIDTH / CELL_SIZE;
int TIME_MSEC = 1000;

//Создание сетки
std::vector<std::vector<bool>> grid;
std::vector<std::vector<bool>> temp;

//Функция подсчета соседей клетки
int getLivingNeighbours(int i, int j) {
    int num = 0;
    if (i > 0) {
        if (temp[i - 1][j] == 1) num++;
    }
    if (i < GRID_HEIGHT - 1) {
        if (temp[i + 1][j] == 1) num++;
    }
    if (j > 0) {
        if (temp[i][j - 1] == 1) num++;
    }
    if (j < GRID_WIDTH - 1) {
        if (temp[i][j + 1] == 1) num++;
    }
    if (i > 0 && j > 0) {
        if (temp[i - 1][j - 1] == 1) num++;
    }
    if (i > 0 && j < GRID_WIDTH - 1) {
        if (temp[i - 1][j + 1] == 1) num++;
    }
    if (i < GRID_HEIGHT - 1 && j > 0) {
        if (temp[i + 1][j - 1] == 1) num++;
    }
    if (i < GRID_HEIGHT - 1 && j < GRID_WIDTH - 1) {
        if (temp[i + 1][j + 1] == 1) num++;
    }
    return num;
}

//Функция обновления состояния сетки
void updateGrid() {
    temp = grid;

    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            int neighbours = getLivingNeighbours(i, j);
            if (grid[i][j] == 0) {
                if (neighbours == 3) {
                    grid[i][j] = 1;
                }
            }
            else {
                if (neighbours < 2 || neighbours > 3) {
                    grid[i][j] = 0;
                }
            }
        }
    }
}

//Отображение сетки на экране
void drawGrid(SDL_Renderer* ren) {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            if (grid[i][j] == 1) {
                SDL_Rect rect = { j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                SDL_RenderFillRect(ren, &rect);
            }
        }
    }
    SDL_RenderPresent(ren);
}



int main(int argc, char* argv[]) {
    
    //Получение аргументов командной строки
    if (argc > 1) {
        TIME_MSEC = std::stoi(argv[1]);
    }

    if (argc == 4) {
        SCREEN_HEIGHT = std::stoi(argv[2]);
        SCREEN_WIDTH = std::stoi(argv[3]);
        GRID_HEIGHT = std::stoi(argv[2]) / CELL_SIZE;
        GRID_WIDTH = std::stoi(argv[3]) / CELL_SIZE;
    }
    grid = std::vector<std::vector<bool>>(GRID_HEIGHT, std::vector<bool>(GRID_WIDTH, 0));
    temp = grid;

    // Инициализация SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Создание окна
    SDL_Window* win = SDL_CreateWindow("Game of life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_HEIGHT, SCREEN_WIDTH, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Создание рендерера
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr) {
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Создание пустого поля
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);

    bool GameState = false;
    bool quit = false;

    //Обработка событий пользователя
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN: {
                SDL_Keycode key = event.key.keysym.sym;
                switch (key) {
                case SDLK_SPACE: {
                    GameState = !GameState;
                    break;
                }
                case SDLK_ESCAPE: { quit = true; break; }
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                if (!GameState) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    x /= CELL_SIZE;
                    y /= CELL_SIZE;
                    grid[y][x] = !grid[y][x];

                    drawGrid(ren);
                }
            }
            }
        }
        if (GameState) {
            updateGrid();
        }
        drawGrid(ren);
        if(GameState) SDL_Delay(TIME_MSEC);
    }

    // Очистка ресурсов
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}