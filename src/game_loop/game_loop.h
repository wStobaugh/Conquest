#ifndef H_GAME_LOOP
#define H_GAME_LOOP

// Forward declarations
typedef struct GameHandle GameHandle;

void game_loop(GameHandle *gh);
int initialize_core_services(GameHandle *gh);

#endif // GAME_LOOP_H