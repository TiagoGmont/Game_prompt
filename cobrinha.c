// cobrinha.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "keyboard.h"
#include "screen.h"
#include "timer.h"

#define TAMANHO_INICIAL_COBRA 3

typedef struct {
    int x, y;
} Posicao;

#define TECLA_CIMA    1
#define TECLA_BAIXO   2
#define TECLA_ESQUERDA 3
#define TECLA_DIREITA 4

void telaInicial();
void telaFinal(int pontuacao);
void limparBufferEntrada();

int main() {
    Posicao *cobra = NULL;       
    int tamanho_cobra;            
    int direcao;                  
    int ch;
    int max_x = SCRENDX;
    int max_y = SCRENDY;
    Posicao comida;
    int fim_de_jogo;
    int pontuacao;
    int jogar_novamente = 1;

    
    screenInit(0);
    keyboardInit();
    timerInit(100);

    while (jogar_novamente) {
        
        telaInicial();

        
        tamanho_cobra = TAMANHO_INICIAL_COBRA;
        direcao = TECLA_DIREITA;
        fim_de_jogo = 0;
        pontuacao = 0;

        
        cobra = (Posicao *)malloc(sizeof(Posicao) * tamanho_cobra);
        if (!cobra) {
            fprintf(stderr, "Erro ao alocar memória para a cobra.\n");
            exit(EXIT_FAILURE);
        }

        
        screenHideCursor();

        
        srand(time(NULL));

        
        cobra[0].x = max_x / 2;
        cobra[0].y = max_y / 2;

        cobra[1].x = cobra[0].x - 1;
        cobra[1].y = cobra[0].y;

        cobra[2].x = cobra[1].x - 1;
        cobra[2].y = cobra[1].y;

        
        comida.x = (rand() % (max_x - 1)) + 1;
        comida.y = (rand() % (max_y - 1)) + 1;

        while (!fim_de_jogo) {
            if (timerTimeOver()) {
                screenClear();

                
                screenGotoxy(comida.x, comida.y);
                printf("O");

                
                for (int i = 0; i < tamanho_cobra; i++) {
                    screenGotoxy(cobra[i].x, cobra[i].y);
                    printf("@");
                }

                screenUpdate();

                
                if (keyhit()) {
                    ch = readch();
                    if (ch == 27) {
                        if (keyhit()) {
                            ch = readch();
                            if (ch == '[') {
                                if (keyhit()) {
                                    ch = readch();
                                    switch (ch) {
                                        case 'A':
                                            if (direcao != TECLA_BAIXO)
                                                direcao = TECLA_CIMA;
                                            break;
                                        case 'B':
                                            if (direcao != TECLA_CIMA)
                                                direcao = TECLA_BAIXO;
                                            break;
                                        case 'C':
                                            if (direcao != TECLA_ESQUERDA)
                                                direcao = TECLA_DIREITA;
                                            break;
                                        case 'D':
                                            if (direcao != TECLA_DIREITA)
                                                direcao = TECLA_ESQUERDA;
                                            break;
                                    }
                                }
                            }
                        }
                    } else if (ch == 'q') {
                        fim_de_jogo = 1;
                    }
                }

                
                Posicao nova_cabeca;
                nova_cabeca = cobra[0]; 

                
                switch (direcao) {
                    case TECLA_CIMA:
                        nova_cabeca.y--;
                        break;
                    case TECLA_BAIXO:
                        nova_cabeca.y++;
                        break;
                    case TECLA_ESQUERDA:
                        nova_cabeca.x--;
                        break;
                    case TECLA_DIREITA:
                        nova_cabeca.x++;
                        break;
                }

                
                if (nova_cabeca.x > max_x) nova_cabeca.x = 1;
                if (nova_cabeca.x < 1)     nova_cabeca.x = max_x;
                if (nova_cabeca.y > max_y) nova_cabeca.y = 1;
                if (nova_cabeca.y < 1)     nova_cabeca.y = max_y;

                
                if (nova_cabeca.x == comida.x && nova_cabeca.y == comida.y) {
                    pontuacao += 10; 

                    
                    tamanho_cobra++;
                    Posicao *nova_cobra = (Posicao *)realloc(cobra, sizeof(Posicao) * tamanho_cobra);
                    if (!nova_cobra) {
                        fprintf(stderr, "Erro ao realocar memória para a cobra.\n");
                        free(cobra);
                        exit(EXIT_FAILURE);
                    }
                    cobra = nova_cobra;

                    
                    for (int i = tamanho_cobra - 1; i > 0; i--) {
                        cobra[i] = cobra[i - 1];
                    }
                    cobra[0] = nova_cabeca;

                    
                    comida.x = (rand() % (max_x - 1)) + 1;
                    comida.y = (rand() % (max_y - 1)) + 1;
                } else {
                    
                    for (int i = tamanho_cobra - 1; i > 0; i--) {
                        cobra[i] = cobra[i - 1];
                    }
                    cobra[0] = nova_cabeca;
                }

                
                for (int i = 1; i < tamanho_cobra; i++) {
                    if (cobra[0].x == cobra[i].x && cobra[0].y == cobra[i].y) {
                        screenGotoxy(max_x / 2, max_y / 2);
                        printf("Game Over!");
                        screenUpdate();
                        usleep(2000000);
                        fim_de_jogo = 1;
                        break;
                    }
                }

                timerUpdateTimer(100);
            }
        }

        
        screenShowCursor();

        
        free(cobra);
        cobra = NULL;

        
        telaFinal(pontuacao);

        
        int escolha_feita = 0;
        while (!escolha_feita) {
            if (keyhit()) {
                ch = readch();
                if (ch == '1') {
                    jogar_novamente = 1;
                    escolha_feita = 1;
                } else if (ch == '2') {
                    jogar_novamente = 0;
                    escolha_feita = 1;
                }
            }
        }
    }

    
    timerDestroy();
    keyboardDestroy();
    screenDestroy();

    return 0;
}


void telaInicial() {
    screenClear();
    screenGotoxy(SCRENDX / 2 - 10, SCRENDY / 2 - 2);
    printf("Bem-vindo ao Jogo da Cobrinha!");
    screenGotoxy(SCRENDX / 2 - 12, SCRENDY / 2);
    printf("Pressione '1' para Jogar");
    screenGotoxy(SCRENDX / 2 - 20, SCRENDY / 2 + 2);
    printf("Use as setas do teclado para mover a cobra");
    screenGotoxy(SCRENDX / 2 - 11, SCRENDY / 2 + 4);
    printf("Pressione 'q' para sair durante o jogo");
    screenUpdate();

    
    int iniciar_jogo = 0;
    while (!iniciar_jogo) {
        if (keyhit()) {
            int ch = readch();
            if (ch == '1') {
                iniciar_jogo = 1;
            }
        }
    }
}


void telaFinal(int pontuacao) {
    screenClear();
    screenGotoxy(SCRENDX / 2 - 5, SCRENDY / 2 - 2);
    printf("Fim de Jogo!");
    screenGotoxy(SCRENDX / 2 - 7, SCRENDY / 2);
    printf("Pontuação: %d", pontuacao);
    screenGotoxy(SCRENDX / 2 - 10, SCRENDY / 2 + 2);
    printf("Pressione '1' para Jogar Novamente");
    screenGotoxy(SCRENDX / 2 - 6, SCRENDY / 2 + 4);
    printf("Pressione '2' para Sair");
    screenUpdate();
}
