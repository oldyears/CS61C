#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  // TODO: Implement this function.
  struct game_state_t* default_state = malloc(sizeof(game_state_t));
  if (default_state == NULL) {
    printf("failed to create defalut state\n");
    return NULL;
  }

  default_state->num_rows = 18;
  default_state->num_snakes = 1;

  default_state->board = malloc(sizeof(char*) * default_state->num_rows);
  if (default_state->board == NULL) {
    printf("failed to get enough memory for defalut state's board\n");
    return NULL;
  }

  for (int i = 0; i < default_state->num_rows; i++) {
    default_state->board[i] = malloc(sizeof(char) * 21);
    if (default_state->board[i] == NULL) {
      printf("failed to create borad %d", i);
      return NULL;
    }
  }

  strcpy(default_state->board[0], "####################");
  strcpy(default_state->board[17], "####################");
  
  for (int i = 1; i < default_state->num_rows - 1; i++) {
    if (i == 2) {
      strcpy(default_state->board[i], "# d>D    *         #");
    }
    else {
      strcpy(default_state->board[i], "#                  #");
    }
  }

  set_board_at(default_state, 2, 9, '*');
  
  default_state->snakes = malloc(sizeof(snake_t));
  default_state->snakes[0].live = true;
  default_state->snakes[0].head_col = 4;
  default_state->snakes[0].head_row = 2;
  default_state->snakes[0].tail_col = 2;
  default_state->snakes[0].tail_row = 2;
 
  return default_state;
}

/* Task 2 */
void free_state(game_state_t *state) {
  // TODO: Implement this function.
  // free snakes
  free(state->snakes);

  // free board
  for (int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }
  free(state->board);

  // free state
  free(state);

  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i++) {
    fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x') {
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  char* snakeWords = "wasd^<v>WASDx";
  for (int i = 0; i < strlen(snakeWords); i++) {
    if (c == snakeWords[i]) {
      return true;
    }
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if (c == '^') {
    return 'w';
  }
  else if (c == '<') {
    return 'a';
  }
  else if (c == 'v') {
    return 's';
  }
  else if (c == '>') {
    return 'd';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W') {
    return '^';
  }
  else if (c == 'A') {
    return '<';
  }
  else if (c == 'S') {
    return 'v';
  }
  else if (c == 'D') {
    return '>';
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  }
  else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  }
  else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  char head = get_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col);
  unsigned int next_row = get_next_row(state->snakes[snum].head_row, head);
  unsigned int next_col = get_next_col(state->snakes[snum].head_col, head);

  return get_board_at(state, next_row, next_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  // update the head on the board
  char head = get_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col);
  unsigned int next_row = get_next_row(state->snakes[snum].head_row, head);
  unsigned int next_col = get_next_col(state->snakes[snum].head_col, head);

  set_board_at(state, next_row, next_col, head);
  set_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col, head_to_body(head));

  // update the snake infomation
  state->snakes[snum].head_row = next_row;
  state->snakes[snum].head_col = next_col;

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  // update the hail on the board
  char tail = get_board_at(state, state->snakes[snum].tail_row, state->snakes[snum].tail_col);
  unsigned int next_row = get_next_row(state->snakes[snum].tail_row, tail);
  unsigned int next_col = get_next_col(state->snakes[snum].tail_col, tail);

  char next_body = get_board_at(state, next_row, next_col);
  set_board_at(state, state->snakes[snum].tail_row, state->snakes[snum].tail_col, ' ');
  set_board_at(state, next_row, next_col, body_to_tail(next_body));
  
  // update the snake information
  state->snakes[snum].tail_row = next_row;
  state->snakes[snum].tail_col = next_col;

  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // TODO: Implement this function.
  for (unsigned int i = 0; i < state->num_snakes; i++) {
    // check the snake live
    if (state->snakes[i].live) {
      char next_char = next_square(state, i);

      // crash into the body of a snake fo a wall
      if (next_char == '#' || (next_char != ' ' && next_char != '*')) {
        state->snakes[i].live = false;
        set_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col, 'x');
        continue;
      }   

      // update the snake on the board
      update_head(state, i);
      if (next_char != '*') {
        update_tail(state, i);  // eat fruit then don't update the hail
      }
      else {
        add_food(state);        // add new one fruit on the board
      }
    }
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  size_t buffer_size = 100;
  char* line = malloc(buffer_size * sizeof(char));
  if (line == NULL) {
    printf("failed to create line");
    return NULL;
  }

  if (fgets(line, (int)buffer_size, fp) != NULL) {
    // check the end
    if (strchr(line, '\n') != NULL) {
      // realloc the char*
      size_t real_length = (size_t)(strchr(line, '\n') - line + 1);
      line = realloc(line, (real_length + 1) * sizeof(char));
      if (line == NULL) {
        printf("Memory reallocation failed\n");
        return NULL;
      }
      line[real_length] = '\0';
      return line;
    }
  }
  return NULL;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  struct game_state_t* state = malloc(sizeof(game_state_t));
  if (state == NULL) {
    printf("Memory mallocation failed\n");
    return NULL;
  }

  char* line;
  unsigned int line_count = 0;
  size_t capacity = 10;
  state->board = malloc(capacity * sizeof(char*));
  if (state->board == NULL) {
    printf("Memory mallocation failed\n");
    return NULL;
  }

  while ((line = read_line(fp)) != NULL) {
    state->board[line_count] = malloc(strlen(line) * sizeof(char));
    if (state->board[line_count] == NULL) {
      printf("Memory mallocation failed\n");
      return NULL;
    }

    strcpy(state->board[line_count], line);
    line_count++;

    if (line_count >= capacity) {
      capacity *= 2;
      state->board = realloc(state->board, capacity * sizeof(char*));
      if (state->board == NULL) {
        printf("Memory reallocation failed\n");
        return NULL;
      }
    }
  }

  state->board = realloc(state->board, (size_t)line_count * sizeof(char*));
  if (state->board == NULL) {
    printf("Memory reallocation failed\n");
    return NULL;
  }
  state->num_rows = line_count;
  state->num_snakes = 0;
  state->snakes = NULL;
  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // TODO: Implement this function.
  return NULL;
}
