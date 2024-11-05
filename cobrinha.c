#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>      // Para usar usleep()
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define MAX_SNAKE_LENGTH 30  // Tamanho máximo da cobra

// posições na tela
typedef struct {
    int x, y;
} Position;

// Direções
#define KEY_UP    1
#define KEY_DOWN  2
#define KEY_LEFT  3
#define KEY_RIGHT 4

int main() {
    Position snake[MAX_SNAKE_LENGTH]; // Array para armazenar as posições do corpo da cobra
    int snake_length = 3;             // Comprimento inicial da cobra
    int direction = KEY_RIGHT;        // Direção inicial da cobra
    int ch;
    int max_x = SCRENDX;              // Máximo valor de x (definido em screen.h)
    int max_y = SCRENDY;              // Máximo valor de y (definido em screen.h)
    Position food;                    // Posição da comida
    int game_over = 0;                // Flag para indicar fim de jogo

    // Inicializa as bibliotecas
    screenInit(0);       // Inicializa a tela sem bordas
    keyboardInit();      // Inicializa a entrada do teclado
    timerInit(100);      // Configura o temporizador para 100 milissegundos

    // Esconde o cursor para melhor visualização
    screenHideCursor();

    // Semente para o gerador de números aleatórios
    srand(time(NULL));

    // Inicializa a posição inicial da cobra
    snake[0].x = max_x / 2; // Cabeça da cobra
    snake[0].y = max_y / 2;

    snake[1].x = snake[0].x - 1; // Segmento do corpo 1
    snake[1].y = snake[0].y;

    snake[2].x = snake[1].x - 1; // Segmento do corpo 2
    snake[2].y = snake[1].y;

    // Gera a comida em posição aleatória
    food.x = (rand() % (max_x - 1)) + 1;
    food.y = (rand() % (max_y - 1)) + 1;

    while (!game_over) {
        // Aguarda o intervalo do temporizador
        if (timerTimeOver()) {
            // Limpa a tela
            screenClear();

            // Desenha a comida
            screenGotoxy(food.x, food.y);
            printf("O");

            // Desenho da cobra
            for (int i = 0; i < snake_length; i++) {
                screenGotoxy(snake[i].x, snake[i].y);
                printf("@");
            }

            // Atualiza a tela
            screenUpdate();

            // Lida com a entrada do usuário sem bloquear
            if (keyhit()) {
                ch = readch();
                if (ch == 27) { // Caractere ESC detectado
                    if (keyhit()) {
                        ch = readch();
                        if (ch == '[') {
                            if (keyhit()) {
                                ch = readch();
                                // Determina qual seta foi pressionada
                                switch (ch) {
                                    case 'A': // Seta para cima
                                        if (direction != KEY_DOWN)
                                            direction = KEY_UP;
                                        break;
                                    case 'B': // Seta para baixo
                                        if (direction != KEY_UP)
                                            direction = KEY_DOWN;
                                        break;
                                    case 'C': // Seta para a direita
                                        if (direction != KEY_LEFT)
                                            direction = KEY_RIGHT;
                                        break;
                                    case 'D': // Seta para a esquerda
                                        if (direction != KEY_RIGHT)
                                            direction = KEY_LEFT;
                                        break;
                                }
                            }
                        }
                    }
                } else if (ch == 'q') {
                    // Encerra o jogo se q for pressionad
                    game_over = 1;
                }
            }

            // Move acobra
            for (int i = snake_length - 1; i > 0; i--) {
                snake[i] = snake[i - 1];
            }

            // Atualiza a cabeça da cobra com base na direção
            switch (direction) {
                case KEY_UP:
                    snake[0].y--;
                    break;
                case KEY_DOWN:
                    snake[0].y++;
                    break;
                case KEY_LEFT:
                    snake[0].x--;
                    break;
                case KEY_RIGHT:
                    snake[0].x++;
                    break;
            }

            // Mantém a cobra dentro dos limites da tela (teletransporte)
            if (snake[0].x > max_x) snake[0].x = 1;
            if (snake[0].x < 1)     snake[0].x = max_x;
            if (snake[0].y > max_y) snake[0].y = 1;
            if (snake[0].y < 1)     snake[0].y = max_y;

            // Verifica se a cobra comeu a comida
            if (snake[0].x == food.x && snake[0].y == food.y) {
                if (snake_length < MAX_SNAKE_LENGTH) {
                    snake_length++; // Aumenta o tamanho da cobra
                }
                // Gera nova comida em posição aleatória
                food.x = (rand() % (max_x - 1)) + 1;
                food.y = (rand() % (max_y - 1)) + 1;
            }

            // Verifica colisão com o próprio corpo
            for (int i = 1; i < snake_length; i++) {
                if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                    // Condição de fim de jogo
                    screenGotoxy(max_x / 2, max_y / 2);
                    printf("Game Over!");
                    screenUpdate();
                    usleep(2000000); // Pausa por 2 segundos
                    game_over = 1;
                    break;
                }
            }

            // Reinicia o temporizador
            timerUpdateTimer(100);
        }
    }

    // Mostra o cursor novamente
    screenShowCursor();

    // Encerra/destroi as bibliotecas inicializadas
    timerDestroy();
    keyboardDestroy();
    screenDestroy();

    return 0;
}