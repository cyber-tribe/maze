#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define MAX_LINE 200
#define MAX_COLUMN 200
#define MAX_NUM 200
#define INF 4294967296
typedef struct{
    int x;
    int y;
}Point;
typedef struct{
    Point coordinate;
    int cost;
    int field;
    int done;
    int parent_x;
    int parent_y;
    int isSource;
    int isTarget;
}Maze;
typedef struct{
    Maze data[MAX_NUM];
    int top;
}Stuck;
typedef struct{
    Maze data[MAX_NUM];
    int tail;
}Queue;

int make_rand(int opt);
void init_maze(Maze maze[MAX_LINE][MAX_COLUMN], int lineNum, int columnNum);
int decide_direction(int i, int j, Maze maze[MAX_LINE][MAX_COLUMN], int opt);
void stick_down_method(Maze maze[MAX_LINE][MAX_COLUMN], int lineNum, int columnNum);
void print_maze(Maze maze[MAX_LINE][MAX_COLUMN], int lineNum, int columnNum);
int isFull_s(Stuck stuck);
int isFull_q(Queue queue);
int isEmpty_s(Stuck stuck);
int isEmpty_q(Queue queue);
void push(Stuck *stuck_pt,Maze *maze_pt);
Maze pop(Stuck *stuck_pt);
void enqueue(Queue *queue_pt,Maze *maze_pt);
Maze dequeue(Queue *queue_pt);
void dfs(Maze maze[MAX_LINE][MAX_COLUMN],int lineNum,int columnNum, Point source);
void bfs(Maze maze[MAX_LINE][MAX_COLUMN],int lineNum,int columnNum, Point source);
void reset_done(Maze maze[MAX_LINE][MAX_COLUMN],int lineNum, int columnNum);
void goal(Maze maze[MAX_LINE][MAX_COLUMN],int x,int y);

struct timespec ts;

int main(void){
    ts.tv_sec = 0;
    ts.tv_nsec = 100000000;
    srand(time(NULL));//乱数の種を設定する
    int lineNum, columnNum, opt;
    Point source,target;
    Maze maze[MAX_LINE][MAX_COLUMN];
    while(1){
        printf("0 : 終了\n1 : 迷路生成\n2 : 深さ優先探索\n3 : 幅優先探索\n");
        scanf("%d",&opt);
        system("cls");
        switch(opt){
            case 0:
                printf("EXIT\n");
                return 0;
            case 1:
                printf("縦幅(奇数)："); scanf("%d",&lineNum);
                printf("横幅(奇数)："); scanf("%d",&columnNum);
                if(lineNum%2==0||columnNum%2==0){
                    printf("奇数を入力してください\n");
                    break;}
                init_maze(maze,lineNum,columnNum);
                stick_down_method(maze,lineNum,columnNum);
                print_maze(maze,lineNum,columnNum);
                while(1){
                    printf("始点の横座標："); scanf("%d",&source.x);
                    printf("始点の縦座標："); scanf("%d",&source.y);  
                    printf("終点の横座標："); scanf("%d",&target.x);
                    printf("終点の縦座標："); scanf("%d",&target.y);
                    if(maze[source.y][source.x].field || maze[target.y][target.x].field){
                        printf("通行可能な位置を選択してください\n");
                    }
                    else{break;}
                }
                maze[source.y][source.x].done = 1;
                maze[source.y][source.x].parent_x = 0;
                maze[source.y][source.x].parent_y = 0;
                maze[source.y][source.x].isSource = 1;
                maze[target.y][target.x].isTarget = 1; 
                break;
            case 2:
                reset_done(maze,lineNum,columnNum);
                dfs(maze,lineNum,columnNum,source);
                reset_done(maze,lineNum,columnNum);
                goal(maze,target.x,target.y);
                print_maze(maze,lineNum,columnNum);
                break;
            case 3:
                reset_done(maze,lineNum,columnNum);
                bfs(maze,lineNum,columnNum,source);
                reset_done(maze,lineNum,columnNum);
                goal(maze,target.x,target.y);
                print_maze(maze,lineNum,columnNum);
                break;
        }
    }
    return 0;
}
int make_rand(int opt){
    /*
        1からoptまでの乱整数を生成する
    */
    int r;
    r = rand() % opt + 1;
    return r;
}
void init_maze(Maze maze[MAX_LINE][MAX_COLUMN], int lineNum, int columnNum){
    /*
        棒倒し法の初期設定
        周りをすべて壁にして、2マスおきに壁を作る
    */
    for(int i=0;i<lineNum;i++){
        for(int j=0;j<columnNum;j++){
            maze[i][j].coordinate.x = j;
            maze[i][j].coordinate.y = i;
            maze[i][j].field = 0;
            maze[i][j].isSource = 0;
            maze[i][j].isTarget = 0;
            maze[i][j].parent_x = 0;
            maze[i][j].parent_y = 0;
            maze[i][j].done = 0;
            if(i==0||i==lineNum-1){maze[i][j].field=1;}
            if(j==0||j==columnNum-1){maze[i][j].field=1;}
            if(i%2==0&&j%2==0){maze[i][j].field=1;}
        }
    }
}
int decide_direction(int i, int j, Maze maze[MAX_LINE][MAX_COLUMN], int opt){
    /*
        棒を倒す方向をランダムに決める
    */
    int direction;
    int flag=0;
    while(flag!=1){
        if(opt==3){direction = make_rand(3);}
        else if(opt==4){direction = make_rand(4);}
        else{printf("make_rand requires '3'or'4'");exit(1);}
        switch (direction)
        {
            case 1: //right
                if(maze[i][j+1].field==0){flag=1;maze[i][j+1].field=1;}
                break;
            case 2: //down
                if(maze[i+1][j].field==0){flag=1;maze[i+1][j].field=1;}
                break;
            case 3: //left
                if(maze[i][j-1].field==0){flag=1;maze[i][j-1].field=1;}
                break;
            case 4: //up
                if(maze[i-1][j].field==0){flag=1;maze[i-1][j].field=1;}
                break;
            default:
                break;
        }
    }
    return flag;
}
void stick_down_method(Maze maze[MAX_LINE][MAX_COLUMN], int lineNum, int columnNum){
    /*
        棒倒し法の本体関数
    */
    for(int i=2;i<lineNum-1;i+=2){
        for(int j=2;j<columnNum-1;j+=2){
            if(i==2){decide_direction(i,j,maze,4);}
            else{decide_direction(i,j,maze,3);}
            }
        }
    }
void print_maze(Maze maze[MAX_LINE][MAX_COLUMN], int lineNum, int columnNum){
    /*
        迷路を出力する関数
    */
    for(int i=0;i<lineNum;i++){
        for(int j=0;j<columnNum;j++){
            if(maze[i][j].done){printf(" ●");}
            else if(maze[i][j].isSource || maze[i][j].isTarget){printf(" ◎");}
            else if(maze[i][j].field){printf(" □");}
            else{printf("  ");}
        }
        printf("\n");
    }
    printf("\n\n");
}
int isFull_s(Stuck stuck){
    if(stuck.top>=MAX_NUM-1){return 1;}
    else{return 0;}
}
int isFull_q(Queue queue){
    if(queue.tail>=MAX_NUM-1){return 1;}
    else{return 0;}
}
int isEmpty_s(Stuck stuck){
    if(stuck.top==0){return 1;}
    else{return 0;}
}
int isEmpty_q(Queue queue){
    if(queue.tail==0){return 1;}
    else{return 0;}
}
void push(Stuck *stuck_pt,Maze *maze_pt){
    //printf("push\n");
    if(isFull_s(*stuck_pt)){printf("over-flow\n");}
    else{
        stuck_pt->top++;
        stuck_pt->data[stuck_pt->top] = *maze_pt;
    }
}
Maze pop(Stuck *stuck_pt){
    // printf("pop\n");
    if(isEmpty_s(*stuck_pt)){printf("under-flow\n");}
    else{
        stuck_pt->top--;
        return stuck_pt->data[stuck_pt->top+1];
    }
}
void enqueue(Queue *queue_pt,Maze *maze_pt){
    // printf("enqueue\n");
    if(isFull_q(*queue_pt)){printf("over-flow\n");}
    else{
        queue_pt->data[queue_pt->tail] = *maze_pt;
        queue_pt->tail++;
    }
}
Maze dequeue(Queue *queue_pt){
    // printf("dequeue\n");
    if(isEmpty_q(*queue_pt)){printf("under-flow\n");}
    else{
        Maze tmp = queue_pt->data[0];
        for(int i=0;i<queue_pt->tail;i++){
            queue_pt->data[i] = queue_pt->data[i+1];
        }
        queue_pt->tail--;
        return tmp;
    }
}
void dfs(Maze maze[MAX_LINE][MAX_COLUMN],int lineNum,int columnNum, Point source){
    /* 深さ優先探索 */
    Stuck stuck;
    Maze maze_tmp;
    int x,y;
    stuck.top=0;
    push(&stuck,&maze[source.y][source.x]);
    while(!isEmpty_s(stuck)){
        system("cls");
        print_maze(maze,lineNum,columnNum);
        nanosleep(&ts, NULL);
        maze_tmp = pop(&stuck);
        x = maze_tmp.coordinate.x;
        y = maze_tmp.coordinate.y;
        if(maze_tmp.isTarget){break;}
        if(!maze[y-1][x].field && !maze[y-1][x].done){
            maze[y-1][x].parent_x = x;
            maze[y-1][x].parent_y = y;
            maze[y-1][x].done = 1;
            push(&stuck,&maze[y-1][x]);}
        if(!maze[y][x+1].field && !maze[y][x+1].done){
            maze[y][x+1].parent_x = x;
            maze[y][x+1].parent_y = y;
            maze[y][x+1].done = 1;
            push(&stuck,&maze[y][x+1]);}
        if(!maze[y+1][x].field && !maze[y+1][x].done){
            maze[y+1][x].parent_x = x;
            maze[y+1][x].parent_y = y;
            maze[y+1][x].done = 1;
            push(&stuck,&maze[y+1][x]);}
        if(!maze[y][x-1].field && !maze[y][x-1].done){
            maze[y][x-1].parent_x = x;
            maze[y][x-1].parent_y = y;
            maze[y][x-1].done = 1;
            push(&stuck,&maze[y][x-1]);}
    }
}
void bfs(Maze maze[MAX_LINE][MAX_COLUMN],int lineNum,int columnNum, Point source){
    /* 幅優先探索 */
    Queue queue;
    Maze maze_tmp;
    int x,y;
    queue.tail=0;
    enqueue(&queue,&maze[source.y][source.x]);
    while(!isEmpty_q(queue)){
        system("cls");
        print_maze(maze,lineNum,columnNum);
        nanosleep(&ts, NULL);
        maze_tmp = dequeue(&queue);
        x = maze_tmp.coordinate.x;
        y = maze_tmp.coordinate.y;
        if(maze_tmp.isTarget){break;}
        if(!maze[y-1][x].field && !maze[y-1][x].done){
            maze[y-1][x].parent_x = x;
            maze[y-1][x].parent_y = y;
            maze[y-1][x].done = 1;
            enqueue(&queue,&maze[y-1][x]);}
        if(!maze[y][x+1].field && !maze[y][x+1].done){
            maze[y][x+1].parent_x = x;
            maze[y][x+1].parent_y = y;
            maze[y][x+1].done = 1;
            enqueue(&queue,&maze[y][x+1]);}
        if(!maze[y+1][x].field && !maze[y+1][x].done){
            maze[y+1][x].parent_x = x;
            maze[y+1][x].parent_y = y;
            maze[y+1][x].done = 1;
            enqueue(&queue,&maze[y+1][x]);}
        if(!maze[y][x-1].field && !maze[y][x-1].done){
            maze[y][x-1].parent_x = x;
            maze[y][x-1].parent_y = y;
            maze[y][x-1].done = 1;
            enqueue(&queue,&maze[y][x-1]);}
    }
}
void reset_done(Maze maze[MAX_LINE][MAX_COLUMN],int lineNum, int columnNum){
    for(int i=0;i<lineNum;i++){
        for(int j=0;j<columnNum;j++){
            if(!maze[i][j].isSource){maze[i][j].done = 0;}
        }
    }
}
void goal(Maze maze[MAX_LINE][MAX_COLUMN],int x,int y){
    if(maze[y][x].parent_x!=0 && maze[y][x].parent_y!=0){
        maze[y][x].done = 1;
        goal(maze,maze[y][x].parent_x,maze[y][x].parent_y);
    }
}
