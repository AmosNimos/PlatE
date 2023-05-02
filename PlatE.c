#include <stdbool.h> // To have access to boolean variables
#include <SDL2/SDL.h> // To have graphics
#include <stdio.h>
#include <SDL_ttf.h>

// TODO:
// add viewport
// add sound
// fix collision glitch and clipping
// add score gui and lifes
// addaptive level size based on the .lvl file
// add multiple level
// add a main menu (optional)
// Level editor
// sprites
// fix coin so that they are removed and not just hidden
// add moving enemys
// add moving platforms
// some power ups.




// Fixed value variables :

#define LEVEL_WIDTH 20 // Number of tiles wide
#define LEVEL_HEIGHT 15 // Number of tiles high
#define TILE_SIZE 32 // Pixel size of each tile
#define MAX_OBJECTS 300 // Maximum number of objects allowed in the game
#define GRAVITY 0.5f
#define JUMP_FORCE 12.0f

// Initiallize the game window

SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;

typedef struct {
  float x, y; // Position of the object
  float width, height; // Size of the object rectangle
  float dx, dy; // delta speed aka the velocity
  const char* name; // Use to identify the object type
  bool solid; // Is the object solid or not
  SDL_Rect rect; // Shape of the object
  SDL_Color color; // Color of the object
} Object;

Object objects_array[MAX_OBJECTS]; // Fixed array size at 64 objects
int num_objects = 0; // Keep track of how many objects are in the objects_array
int player_index = 0; // Default player index in the objects_array
int file_width=20, file_height=15;
char* game_title = "Game Title";
float player_width=TILE_SIZE-8;
float player_height=(TILE_SIZE*2)-8;


// Colors
SDL_Color green = {0, 255, 0, 255};
SDL_Color red = {255, 0, 0, 255};
SDL_Color blue = {0, 0, 255, 255};
SDL_Color cyan = {0, 255, 255, 255};
SDL_Color yellow = {255,255,0,255};
SDL_Color white = {255, 255, 255, 255};
SDL_Color black = {0, 0, 0, 255};

Object player;

// Use to initialize the object
Object create_object(float x, float y, float width, float height, float dx, float dy, const char* name, bool solid, SDL_Color color) {
  if (num_objects < MAX_OBJECTS) {
    Object obj = {x, y, width, height, dx, dy, name, solid}; // Set solid to true by default if it is not passed as an argument
    obj.rect.x = (int)x;
    obj.rect.y = (int)y;
    obj.rect.w = (int)width;
    obj.rect.h = (int)height;
    obj.color=color;
    objects_array[num_objects] = obj;
    num_objects++;
  } else {
    printf("Error: object array is full\n");
  }
}

void game_over() {
  // Game over
  printf("Game over\n");
  exit(0);
}

void win_game() {
  // Victory screen
  printf("You won!\n");
  exit(0);
}

void get_file_dimensions(FILE* fp, int* width, int* height) {
    int w = 0, h = 0;
    int max_width = 0;
    char c;

    // Iterate through the file character by character
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            // End of line, update the maximum width and reset the width counter
            if (w > max_width) {
                max_width = w;
            }
            w = 0;
            h++;
        } else {
            // Regular character, increment the width counter
            w++;
        }
    }

    // Handle the last line, which may not end with a newline
    if (w > max_width) {
        max_width = w;
    }

    // Update the output parameters
    *width = max_width;
    *height = h;
}

void read_level_file(char* filename){
  FILE* file = fopen(filename, "r");
  if (!file) {
      printf("Error: could not open file %s\n", filename);
      return;
  }

  //int width, height;
  //get_file_dimensions(file, &width, &height);
  //file_width=width;
  //file_height=height;
  printf("file_width: %i file_height: %i\n", file_width, file_height);

  int c;
  float x, y;
  for (int row = 0; row < file_height; row++) {
      y = row * TILE_SIZE;
      for (int col = 0; col < file_width+1; col++) {
          x = col * TILE_SIZE;
          c = fgetc(file);
          if (c == EOF) {
              printf("Warning: unexpected end of file at row: %i col: %i\n", row, col);
              printf("character: %c\n",c);
              break;
          }

          switch (c) {
              case '@': // Player
                  player_index=num_objects; // Update player index in the objects_array
                  create_object(x, y, player_width, player_height, 0, 0, "player", true, green);
                  player=objects_array[player_index];
                  break;
              case '#': // Solid block
                  create_object(x, y, TILE_SIZE, TILE_SIZE, 0, 0, "block", true, blue);
                  break;
              case 'X': // Solid block
                  create_object(x, y, TILE_SIZE, TILE_SIZE, 0, 0, "spike", true, red);
                  break;
              case 'G': // Goal block
                  create_object(x, y, TILE_SIZE, TILE_SIZE, 0, 0, "goal", true, cyan);
                  break;
              case 'C': // Coin block
                  create_object(x, y, TILE_SIZE, TILE_SIZE, 0, 0, "coin", true, yellow);
                  break;
              case '_': // Platform
                  create_object(x, y, TILE_SIZE, TILE_SIZE, 0, 0, "platform", true, white);
                  break;
              default: // Ignore other characters
                  break;
          }
      }
  }

  fclose(file);
}

void update_player() {
    // Apply gravity to the player's vertical velocity
    player.dy += GRAVITY;

    // Move the player horizontally
    player.x += player.dx;
    player.y+=player.dy;

    // Check collision between the player and other solid objects
    for (int i = 0; i < num_objects; i++) {
        Object obj = objects_array[i];
        if (obj.solid && SDL_HasIntersection(&player.rect, &obj.rect)) {
            // Collision detected
            if (player.dy > 0 && player.rect.y < obj.rect.y) {
                // Player is falling and collided with the top of the object
                player.dy = 0;
                player.y = obj.rect.y - player.rect.h;
                if (obj.name == "spike"){
                  game_over();
                } else if (obj.name == "goal") {
                  win_game();
                } else if (obj.name == "coin") {
                  // Remove the coin from the array
                  /*
                  for (int j = i; j < num_objects - 1; j++) {
                      objects_array[j] = objects_array[j + 1];
                  }
                  num_objects--;
                  i--; // We need to decrement i because we just removed an element from the array
                  */
                  obj.solid=false;
                  obj.color=black;
                  objects_array[i]=obj;  
                  // TODO: add logic to increase the player's score or do something else with the coin
                }
            }
            else if (player.dy < 0 && player.rect.y > obj.rect.y) {
                // Player is jumping and collided with the bottom of the object
                if (obj.name != "platform"){
                  player.dy = 0;
                  player.y = obj.rect.y + obj.rect.h;
                }
                if (obj.name == "spike"){
                  game_over();
                } else if (obj.name == "goal") {
                  win_game();
                } else if (obj.name == "coin") {
                  obj.solid=false;
                  obj.color=black;
                  objects_array[i]=obj;  
                }
            }
            else if (player.dx > 0 && player.rect.x < obj.rect.x) {
                // Player is moving right and collided with the left side of the object
                if (player.y+TILE_SIZE > obj.rect.y){
                  player.dx =- player.dx/2;
                  player.x = obj.rect.x - player.rect.w;
                  if (obj.name == "spike"){
                    game_over();
                  } else if (obj.name == "goal") {
                    win_game();
                  } else if (obj.name == "coin") {
                    obj.solid=false;
                    obj.color=black;
                    objects_array[i]=obj;  

                  }
                }
            }
            else if (player.dx < 0 && player.rect.x > obj.rect.x) {
                // Player is moving left and collided with the right side of the object
                player.dx =- player.dx/2;
                player.x = obj.rect.x + obj.rect.w;
                if (obj.name == "spike"){
                  game_over();
                } else if (obj.name == "goal") {
                  win_game();
                } else if (obj.name == "coin") {
                  obj.solid=false;
                  obj.color=black;
                  objects_array[i]=obj;  

                }
            }
        }
    }
    
    if (player.rect.y + player.rect.h > file_height*TILE_SIZE) {
      // Player has fallen off the screen
      game_over();
    }

    // Update the player's rectangle
    player.rect.x = player.x;
    player.rect.y = player.y;
    objects_array[player_index]=player;
}

void handle_input() {
    const Uint8* state = SDL_GetKeyboardState(NULL);

    // Move the player left or right
    player = objects_array[player_index];
    if (state[SDL_SCANCODE_LEFT]) {
        player.dx = -5.0f;
    }
    else if (state[SDL_SCANCODE_RIGHT]) {
        player.dx = 5.0f;
    }
    else {
        player.dx = 0.0f;
    }

    // Make the player jump if there is a solid object below them
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (state[SDL_SCANCODE_SPACE] && objects_array[i].solid && SDL_HasIntersection(&player.rect, &objects_array[i].rect) && player.rect.y + player.rect.h <= objects_array[i].rect.y + 1) {
            player.dy = -JUMP_FORCE;
        }
    }
    objects_array[player_index]=player;
}

void render() {
    // Clear the screen
    SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_renderer);

    // Render all objects in the object array
    for (int i = 0; i < MAX_OBJECTS; i++) {
        Object object = objects_array[i];
        SDL_SetRenderDrawColor(g_renderer, object.color.r, object.color.g, object.color.b, object.color.a);
        SDL_RenderFillRect(g_renderer, &object.rect);
    }

    for (int i = 0; i < MAX_OBJECTS; i++) {
        Object object = objects_array[i];
        if (object.name == "player"){
          SDL_SetRenderDrawColor(g_renderer, object.color.r, object.color.g, object.color.b, object.color.a);
          SDL_RenderFillRect(g_renderer, &object.rect);
        }
    }


    // Swap buffers
    SDL_RenderPresent(g_renderer);
}

void cleanup() {
    // Destroy renderer and window
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);

    // Quit SDL subsystems
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    read_level_file(argv[1]);
    printf("file_width: %i file_height: %i\n", file_width, file_height);

    // Create a window and renderer
    int gui_height = 64;
    g_window = SDL_CreateWindow(game_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, file_width*TILE_SIZE, file_height*TILE_SIZE+gui_height, 0);
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);

    // Main game loop
    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Handle player input
        handle_input();

        // Update the game state
        update_player();

        // Render the game
        render();

        // Delay to control frame rate
        SDL_Delay(16);
    }

    // Clean up resources
    cleanup();

    return 0;
}


