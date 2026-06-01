/* sand.c - Falling-sand cellular simulation with an emitter.
 * Build: gcc sand.c -o sand */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#define W 80
#define H 40

static volatile int running = 1;
static void stop(int s){ (void)s; running = 0; }

int main(void){
    signal(SIGINT, stop);
    srand((unsigned)time(NULL));
    static int g[H][W];
    memset(g,0,sizeof(g));
    int emit = W/2;
    printf("\033[2J\033[?25l");
    int frame=0;
    while (running){
        g[0][emit] = 1;
        if (frame%3==0){ emit += (rand()%3)-1; if(emit<2)emit=2; if(emit>W-3)emit=W-3; }
        for (int y=H-2;y>=0;y--)
            for (int x=0;x<W;x++) if(g[y][x]){
                if (!g[y+1][x]) { g[y+1][x]=1; g[y][x]=0; }
                else {
                    int l=(x>0&&!g[y+1][x-1]);
                    int r=(x<W-1&&!g[y+1][x+1]);
                    if (l&&r){ if(rand()&1){g[y+1][x-1]=1;}else{g[y+1][x+1]=1;} g[y][x]=0; }
                    else if (l){ g[y+1][x-1]=1; g[y][x]=0; }
                    else if (r){ g[y+1][x+1]=1; g[y][x]=0; }
                }
            }
        printf("\033[H");
        for (int y=0;y<H;y++){
            for (int x=0;x<W;x++) putchar(g[y][x]?'o':' ');
            putchar('\n');
        }
        fflush(stdout);
        frame++;
        usleep(25000);
    }
    printf("\033[0m\033[?25h\033[2J\033[H");
    return 0;
}
