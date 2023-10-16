#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
 
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

bool gameOver = false;
bool quit = false;
int score[2] = {0, 0};
char* scoreImages[6] = {"img/0.png", "img/1.png", "img/2.png", "img/3.png", "img/4.png", "img/5.png"};
SDL_Texture* scoreTextures[2];

typedef struct{

    SDL_Rect body;
    int acceleration;
} Player;

typedef struct{

    SDL_Rect body;
    int acceleration;
} Enemy;

typedef struct 
{
    SDL_Rect body;
    int accelerationX, accelerationY;
} Ball;


Player initPlayer(){

    Player player;
    SDL_Rect rect = {60, (SCREEN_HEIGHT/2 - 50), 10, 100};

    player.body = rect;
    player.acceleration = 5;

    return player;
}

Enemy initEnemy(int acceleration){

    Enemy enemy;
    SDL_Rect rect = {SCREEN_WIDTH - 60, (SCREEN_HEIGHT/2 - 50), 10, 100};

    enemy.body = rect;
    enemy.acceleration = acceleration;

    return enemy;
}

Ball initBall(){

    Ball ball;
    SDL_Rect rect = {(SCREEN_WIDTH/2), (SCREEN_HEIGHT/2 - 50), 10, 10};

    ball.body = rect;

    ball.accelerationX = -4;
    ball.accelerationY = -4;

    return ball;
}

int check_collision(SDL_Rect* a, SDL_Rect* b) {

	return 1 * ((a->x < (b->x + b->w))) * (((a->x + a->w) > b->x)) * (!(a->y > (b->y + b->h))) * (!((a->y + a->h) < b->y));
}

void invertBallX(Ball *ball){
    ball->accelerationX = - ball->accelerationX;
}

void invertBallY(Ball *ball){
    ball->accelerationY = - ball->accelerationY;
}

void invertBall(Ball *ball, int objectY, int objectH){
    
    int hitPos = (objectY + objectH) - ball->body.y;

    if(ball->accelerationX < 0){

        if(ball->accelerationX >= 20){
            ball->accelerationX--;
        }

    }else{
                
        if(ball->accelerationX <= 20){
            ball->accelerationX++;
        }

    }

    invertBallX(ball);

    int test = rand() % 2;

    if(ball->accelerationY > 0){
        ball->accelerationY = - test + (2 - hitPos/7) % 5;
    }else{
        ball->accelerationY = test + (2 - hitPos/7) % 5;
    }
    
    

}

void restart(Player* player, Enemy* enemy, Ball *ball){

    player->body.x = 60;
    player->body.y = SCREEN_HEIGHT/2 - 50;

    enemy->body.x = SCREEN_WIDTH - 60;
    enemy->body.y = SCREEN_HEIGHT/2 - 50;

    ball->body.x = SCREEN_WIDTH/2;
    ball->body.y = SCREEN_HEIGHT/2 - 50;
    
    if(rand() % 2 == 0){
        ball->accelerationX = 4;
    }else{
        ball->accelerationX = -4;
    }

    if(rand() % 2 == 0){
        ball->accelerationY = 4;
    }else{
        ball->accelerationY = -4;
    }


    
    
}

void updateScores(SDL_Renderer *renderer){

    SDL_Surface *scoreSurface;

    if(score[0] <= 5){
        scoreSurface = IMG_Load(scoreImages[score[0]]);
        scoreTextures[0] = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    }

    if(score[1] <= 5){
        scoreSurface = IMG_Load(scoreImages[score[1]]);
        scoreTextures[1] = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    }

    SDL_FreeSurface(scoreSurface);
;
}

void moveBall(Ball *ball, Player *player, Enemy *enemy, SDL_Renderer* renderer){

    if(ball->body.x >= SCREEN_WIDTH || ball->body.x <= 0){
        
        if(ball->body.x <= 0){
            score[0]++;
        }else{
            score[1]++;
        }

        updateScores(renderer);
        restart(player, enemy, ball);
        
    }else{


        if(check_collision(&ball->body, &player->body)){
            invertBall(ball, player->body.y, player->body.h);
        }

        if(check_collision(&ball->body, &enemy->body)){
            invertBall(ball, enemy->body.y, enemy->body.h);
        }
        
        if(ball->body.y >= SCREEN_HEIGHT) invertBallY(ball);
        if(ball->body.y <= 0) invertBallY(ball);

        ball->body.x += ball->accelerationX;
        ball->body.y += ball->accelerationY;
    }


}

void moveEnemy(Enemy* enemy, Ball* ball){

	if (enemy->body.y < ball->body.y) {

        if(enemy->body.y + enemy->body.h >= SCREEN_HEIGHT){
            enemy->body.y = SCREEN_HEIGHT - enemy->body.h;
        }else{
            enemy->body.y += enemy->acceleration;
        }
        
    
    } else {
        enemy->body.y -= enemy->acceleration;	
    }


}

void renderInterface(SDL_Renderer * renderer){

    SDL_Rect rect = {SCREEN_WIDTH/2, SCREEN_HEIGHT - 30, 5, 20};

    for(int c =-0; c < 15; c++){
        SDL_RenderFillRect(renderer, &rect);
        rect.y -= 40;
    }
    
}

void renderScore(SDL_Renderer* renderer){

    SDL_Rect rect = {SCREEN_WIDTH - 200, 100, 100, 100};

    SDL_RenderCopy(renderer, scoreTextures[0], NULL, &rect);
    rect.x = 100;
    SDL_RenderCopy(renderer, scoreTextures[1], NULL, &rect);
}


int main(int argc, char *arqv[]){

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    SDL_Event event;

    srand(time(NULL));   // Initialization, should only be called once.
    

    //create a window and a renderer for it, accelerated and sync to set the refresh rate to the same as the monitor, -1 set driver 
    SDL_Window *window =  SDL_CreateWindow("pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Surface  *endGameSurface;
    SDL_Texture * endGameTexture;

    //load scores images
    updateScores(renderer);

    Player player = initPlayer();
    int dificulty = 2;
    Enemy enemy = initEnemy(dificulty);
    Ball ball = initBall();

    
    while(!(quit)){

        //look if the quit button was pressed
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = true;
            }
        }
        
        SDL_PumpEvents();
        const Uint8 *keyState = SDL_GetKeyboardState(NULL);

        if(keyState[SDL_SCANCODE_ESCAPE]){
            quit = true;
        }

        if(!gameOver){

            if(keyState[SDL_SCANCODE_UP] || keyState[SDL_SCANCODE_W]){
                
                if(player.body.y <= 0){
                    player.body.y = 0;
                }else{
                    player.body.y -= player.acceleration;
                }
                
                
            }
            
            if(keyState[SDL_SCANCODE_DOWN] || keyState[SDL_SCANCODE_S]){

                if(player.body.y >= SCREEN_HEIGHT - player.body.h){
                    player.body.y = SCREEN_HEIGHT - player.body.h;
                }else{
                    player.body.y += player.acceleration;
                }
            }



            moveEnemy(&enemy, &ball);
            moveBall(&ball, &player, &enemy, renderer);
            
            //set background black 
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);

            //check if game won 
            if(score[1] > 5){
                dificulty += 2;
                enemy.acceleration = dificulty;

                score[0] = score[1] = 0;
                updateScores(renderer);
            }


            //render background
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            renderScore(renderer);
            renderInterface(renderer);


            //render gameObjects
            SDL_RenderFillRect(renderer, &player.body);
            SDL_RenderFillRect(renderer, &enemy.body);
            SDL_RenderFillRect(renderer, &ball.body);
            
            if(score[0] > 5 || dificulty > 5){

                if(dificulty > 5){
                    endGameSurface = IMG_Load("img/gameWin.png");

                }else if(score[0] > 5){
                    endGameSurface = IMG_Load("img/gameOver.png");

                }

                gameOver = true;
                endGameTexture = SDL_CreateTextureFromSurface(renderer, endGameSurface);
                SDL_FreeSurface(endGameSurface);
                SDL_RenderCopy(renderer, endGameTexture, NULL, NULL);

            }

            //update window
            SDL_RenderPresent(renderer);

        }

    
    } 

    SDL_DestroyTexture(scoreTextures[0]);
    SDL_DestroyTexture(scoreTextures[1]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();


}