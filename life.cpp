#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>

//Структура для хранения состояния клетки
struct cell {
  unsigned is_alive{0};
};

//Создание нового мира
cell** create_world(int rows, int cols) {
  cell** new_world = new cell*[rows];
  for (int i = 0; i < rows; i++) {
    new_world[i] = new cell[cols];
  }

  return new_world;
}

//Удаление мира
void delete_world(cell** world, int rows) {
  for (int i{0}; i < rows; i++) {
    delete[] world[i];
  }
  delete[] world;
}

//Вывод мира на экран
void print_world(cell **arr, int rows, int cols) {
  for (int i{0}; i < rows; i++) {
    for (int j{0}; j < cols; j++) {
      std::cout << (arr[i][j].is_alive ? '*' : '-') << ' ';
    }
    std::cout << std::endl;
  }
}

//Копирование мира для сохранения состояния предыдущего поколения
void copy_world(cell **src, cell **dest, int rows, int cols) {
  for (int i{0}; i < rows; i++) {
    for (int j{0}; j < cols; j++) {
      dest[i][j] = src[i][j];
    }
  }
}

//Сравнение состояний текущего и прошлого поколений
int cmp_world(cell **world, cell **prev_world, int rows, int cols) {
  for (int i{0}; i < rows; i++) {
    for (int j{0}; j < cols; j++) {
      if (world[i][j].is_alive != prev_world[i][j].is_alive) {
        return -1;
      }
    }
  }
  return 0;
}

//Подсчёт всех живых клеток
int get_live_count(cell **world, int rows, int cols) {
  int count{0};
  for (int i{0}; i < rows; i++) {
    for (int j{0}; j < cols; j++) {
      if (world[i][j].is_alive == 1) {
        count++;
      }
    }
  }
  return count;
}

//Получение координат соседей клетки в окрестности Мура первого порядка
void read_cell_neighbors(int nbs[][2], int x, int y) {
  int k{0};

  for (int i = x - 1; i <= x + 1; i++) {
    for (int j = y - 1; j <= y + 1; j++) {
      if (i == x && j == y) {
        continue;
      }
      nbs[k][0] = i;
      nbs[k][1] = j;
      k++;
    }
  }
}

//Подсчёт живых соседей
int count_live_neighbors(cell **world, int rows, int cols, int x, int y) {
  int count{ 0 };
  int nbs[8][2];
  int nb_x{}, nb_y{};

  read_cell_neighbors(nbs, x, y);

  for (int i{ 0 }; i < 8; i++) {
    nb_x = nbs[i][0];
    nb_y = nbs[i][1];


    if (nb_x < 0 || nb_y < 0) {
      continue;
    }

    if (nb_x >= rows || nb_y >= cols) {
      continue;
    }

    if (world[nb_x][nb_y].is_alive == 1) {
      count++;
    }
  }

  return count;
}

//Создание следующего поколения
void next_gen(cell **world, cell **prev_world, int rows, int cols) {
  int live_nbs{};
  cell c{};

  for (int i{0}; i < rows; i++) {
    for (int j{0}; j < cols; j++) {
      c = prev_world[i][j];
      live_nbs = count_live_neighbors(prev_world, rows, cols, i, j);
      if (c.is_alive == 0) {
        if (live_nbs == 3) {
          world[i][j].is_alive = 1;
        }
      }
      else {
        if (live_nbs < 2 || live_nbs > 3) {
          world[i][j].is_alive = 0;
        }
      }
    }
  }
}


int main() { 
  std::ifstream fin("in.txt");
  if (!fin.is_open()) {
    std::cout << "Не удалось открыть файл!" << std::endl;
    return EXIT_FAILURE;
  }

  int world_height{}, world_width{};
  fin >> world_height >> world_width;

  cell **world = create_world(world_height, world_width);
  cell **prev_world = create_world(world_height, world_width);

  //Инициализация первого поколения игры
  int i{}, j{};
  while(fin >> i >> j) {
    world[i][j].is_alive = 1;
  }

  fin.close();

  int gen_number{1};
  int live_cells{0};
  bool is_stagnating{false};

  do {
    print_world(world, world_height, world_width);
    live_cells = get_live_count(world, world_height, world_width);

    std::cout << "Generation: " << gen_number << ". Alive cells: " << live_cells << std::endl;

    copy_world(world, prev_world, world_height, world_width);
    next_gen(world, prev_world, world_height, world_width);

    is_stagnating = cmp_world(world, prev_world, world_height, world_width) == 0;

    if (is_stagnating) {
      std::cout << "The world has stagnated. Game over." << std::endl;
    }

    if (live_cells == 0) {
      std::cout << "All cells are dead. Game over." << std::endl;
    }

    gen_number++;
    sleep(1);
    (void)std::system("clear");
  } while (live_cells != 0 && !is_stagnating);

  delete_world(world, world_height);
  delete_world(prev_world, world_height);

  return EXIT_SUCCESS;
}
