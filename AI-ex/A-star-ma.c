//A* 算法
//f(n) = g(n) + h(n)
//g(n) : 深度
//h(n) : 错误棋子数/曼哈顿距离
#include <stdio.h>
#include <math.h>

#define N 4
#define _MAX_PATH  10000

//function list
int checkZero(int zero[2],int board[N][N]);
int check(int board[N][N],int aim_board[N][N]);
int copy_board(int new_board[N][N],int board[N][N]);
int move_chess(int re,int* de,int depth[_MAX_PATH],int action[_MAX_PATH],int pre_board[N][N],int* pc,int path[_MAX_PATH][N][N]);
int add_path(int* pc,int board[N][N],int path[_MAX_PATH][N][N]);
int find_closest(int res,int result[_MAX_PATH],int pcount,int path[_MAX_PATH][N][N],int aim_board[N][N]);
int find_closest_m(int res,int result[_MAX_PATH],int pcount,int path[_MAX_PATH][N][N],int aim_board[N][N]);
int find_min(int res,int result[_MAX_PATH],int re,int pcount,int dep,int path[_MAX_PATH][N][N],int aim_board[N][N]);
int find_in(int res,int k,int result[_MAX_PATH]);
int find_last(int last_board[N][N],int pcount,int path[_MAX_PATH][N][N],int action[_MAX_PATH],int result[_MAX_PATH]);
int check_same(int pcount,int path[_MAX_PATH][N][N],int aim_board[N][N]);
void print_chess(int board[N][N]);

int main()
{
    //起始
    int board[N][N] = { 0, 1, 4, 8,
                        5, 3, 6, 11,
                        9, 2,10, 7,
                        13,14,15,12 };
    //目标5
    int aim_board[N][N] = { 1, 2, 3, 4,
                            5, 6, 7, 8,
                            9, 10,11,12,
                            13,14,15,0 }; 
    //当前路径点(棋盘)
    int pre_board[N][N];
    //倒推路径选择点
    int last_board[N][N];
    //路径队列
    int path[_MAX_PATH][N][N];
    //路径点个数
    int pcount = 0;
    int* pc = &pcount;
    //每个路径点对应的上一次操作 0:上 1:左 2:下 3:右 
    //下标对应路径点下标,代表该点是由哪一个操作得来的
    int action[_MAX_PATH] = {-1};//初始点的上一次操作值设为-1//因为pcount从1开始记,所以depth也从1开始
    //深度
    int dep = 0;//当前深度
    int* de = &dep;
    int depth[_MAX_PATH] = {0};//路径点深度,从0开始计//因为pcount从1开始记,所以depth也从1开始
    //结果路径集,通过倒推得到结果选择路径
    int result[_MAX_PATH] = {0};//该数组第一遍用来记录已经探索过的路径点
    int res = 1;//第一遍用来做result最先空值下标
    int re = 0;
    copy_board(pre_board,board);
    add_path(pc,pre_board,path);

    while(check(pre_board,aim_board) !=0)
    {
        //循环移动空点添加新的路径点直到花费最小的点
        re = find_min(res,result,re,pcount,dep,path,aim_board);
        result[res++] = re;
        copy_board(pre_board,path[re]);
        move_chess(re,de,depth,action,pre_board,pc,path);

        //h(n) : 错误棋子数
        //re = find_closest(res,result,pcount,path,aim_board);

        //h(n) : 曼哈顿距离
        re = find_closest_m(res,result,pcount,path,aim_board);
        copy_board(pre_board,path[re]);

        if(pcount == _MAX_PATH)
        {
            printf("out of bound!!\n");
            return 0;
        }
    }

    copy_board(last_board,pre_board);
    find_last(last_board,pcount,path,action,result);

    return 0;

}

//初始检查可移动点 0 将坐标放入 zero 
int checkZero(int zero[2],int board[N][N])
{
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < N;j++)
        {
            if(board[i][j] == 0)
            {
                zero[0] = i;
                zero[1] = j;
                break;
            }
        }
    }
    return 1;
}

//返回当前较目标结构的错误棋子数 h(n)
int check(int board[N][N],int aim_board[N][N])
{
    int k = 0;
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < N;j++)
        {
            if(board[i][j] != aim_board[i][j])
            {
                k++;
            }
        }
    }
    return k;
}

//复制棋盘信息到新的内存地址并返回
int copy_board(int new_board[N][N],int pre_board[N][N])
{
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < N;j++)
        {
           new_board[i][j] = pre_board[i][j];
        }
    }
    return 0;
}

//添加路径点
int add_path(int* pc,int board[N][N],int path[_MAX_PATH][N][N])
{
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < N;j++)
        {
            path[*pc][i][j] = board[i][j];
        }
    }
    *pc=*pc + 1;
    return 0;
}

//移动空点形成新的路径点棋盘
//上移
int up_move_chess(int* de,int depth[_MAX_PATH],int action[_MAX_PATH],int pre_board[N][N],int* pc,int path[_MAX_PATH][N][N])
{
    int k = 0;
    int zero[2];
    checkZero(zero,pre_board);
    if(zero[0] > 0)
    {
        int new_board[N][N];
        copy_board(new_board,pre_board);

        new_board[zero[0]][zero[1]] = new_board[zero[0]-1][zero[1]];
        new_board[zero[0]-1][zero[1]] = 0;

        if((k = check_same(*pc,path,new_board)) != -1)
        {
            return -1;
        }
        action[*pc] = 0;
        depth[*pc] = *de;
        add_path(pc,new_board,path);
       
        k = 1;
    }
    return k;
}
//左移
int left_move_chess(int* de,int depth[_MAX_PATH],int action[_MAX_PATH],int pre_board[N][N],int* pc,int path[_MAX_PATH][N][N])
{
    int k = 0;
    int zero[2];
    checkZero(zero,pre_board);
    if(zero[1] > 0)
    {
        int new_board[N][N];
        copy_board(new_board,pre_board);
        
        new_board[zero[0]][zero[1]] = new_board[zero[0]][zero[1]-1];
        new_board[zero[0]][zero[1]-1] = 0;

        if((k = check_same(*pc,path,new_board)) != -1)
        {
            return -1;
        }
        action[*pc] = 1;
        depth[*pc] = *de;
        add_path(pc,new_board,path);
        k = 1;
    }
    return k;
}
//下移
int down_move_chess(int* de,int depth[_MAX_PATH],int action[_MAX_PATH],int pre_board[N][N],int* pc,int path[_MAX_PATH][N][N])
{
    int k = 0;
    int zero[2];
    checkZero(zero,pre_board);
    if(zero[0] < N - 1)
    {
        int new_board[N][N];
        copy_board(new_board,pre_board);
        
        new_board[zero[0]][zero[1]] = new_board[zero[0]+1][zero[1]];
        new_board[zero[0]+1][zero[1]] = 0;

        if((k = check_same(*pc,path,new_board)) != -1)
        {
            return -1;
        }  
        action[*pc] = 2;
        depth[*pc] = *de;
        add_path(pc,new_board,path);

        k = 1;
    }
    return k;
}
//右移
int right_move_chess(int* de,int depth[_MAX_PATH],int action[_MAX_PATH],int pre_board[N][N],int* pc,int path[_MAX_PATH][N][N])
{
    int k = 0;
    int zero[2];
    checkZero(zero,pre_board);
    if(zero[1] < N -1)
    {
        int new_board[N][N];
        copy_board(new_board,pre_board);
        
        new_board[zero[0]][zero[1]] = new_board[zero[0]][zero[1]+1];
        new_board[zero[0]][zero[1]+1] = 0;

        if((k = check_same(*pc,path,new_board)) != -1)
        {
            return -1;
        }
        action[*pc] = 3;
        depth[*pc] = *de;
        add_path(pc,new_board,path);
       
        k = 1;
    }
    return k;
}
//move
int move_chess(int re,int* de,int depth[_MAX_PATH],int action[_MAX_PATH],int pre_board[N][N],int* pc,int path[_MAX_PATH][N][N])
{
    *de = *de +1;
    int last_move = action[re];
    if(last_move != 2)
    {
        up_move_chess(de,depth,action,pre_board,pc,path);
    }
    if(last_move != 3)
    {
        left_move_chess(de,depth,action,pre_board,pc,path);
    }
    if(last_move != 0)
    {
        down_move_chess(de,depth,action,pre_board,pc,path);
    }
    if(last_move != 1)
    {
        right_move_chess(de,depth,action,pre_board,pc,path);
    }
}

//计算所有棋子的曼哈顿距离的和
int manhattan_distance(int pre_board[N][N],int aim_board[N][N])
{
    int k = 0;
    int t = 0;
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < N;j++)
        {

            for(int x = 0;x < N;x++)
            {
                for(int y = 0;y < N;y++)
                {
                    if(aim_board[i][j] == pre_board[x][y])
                    {
                        k = k + abs(i-x) + abs(j-y);
                        t = 1;
                        break;
                    }
                }
                if(t == 1)
                {
                    t = 0;
                    break;
                }
            }
        }
    }
    return k;
}

//用曼哈顿距离作为h(n)
int find_closest_m(int res,int result[_MAX_PATH],int pcount,int path[_MAX_PATH][N][N],int aim_board[N][N])
{
    int min = (N + N - 2) * N;
    int t = 0;
    int k = 0;
    if(pcount == 1)
    {
        return 0;
    }
    for(int i = 0;i < pcount;i++)
    {
        if(!find_in(res,i,result))
        {
            k = manhattan_distance(path[i],aim_board);
            if(k < min)
            {
                min = k;
                t = i;
            }
        }
    }
    return t;
}

//找出当前最接近答案路径点
int find_closest(int res,int result[_MAX_PATH],int pcount,int path[_MAX_PATH][N][N],int aim_board[N][N])
{
    int min = N*N;
    int t = 0;
    int k = 0;
    if(pcount == 1)
    {
        return 0;
    }
    for(int i = 0;i < pcount;i++)
    {
        if(!find_in(res,i,result))
        {
            k = check(path[i],aim_board);
            if(k < min)
            {
                min = k;
                t = i;
            }
        }
    }
    return t;
}

//返回在记录中是否找到该值,找到返回1,否则返回0
int find_in(int res,int k,int result[_MAX_PATH])
{
    for(int i = 0;i < res;i++)
    {
        if(k == result[i])
        {
            return 1;
        }
    }
    return 0;
}

//找出代价最小路径点
int find_min(int res,int result[_MAX_PATH],int re,int pcount,int dep,int path[_MAX_PATH][N][N],int aim_board[N][N])
{
    int min = dep + 1 + 10;//dep+1为当前预计深度,10>最大差异,该和总大于所有可能的情况
    int t = 0;
    int k = 0;
    if(pcount == 1)
    {
        return 0;
    }
    for(int i = 0;i < pcount;i++)
    {
        if(!find_in(res,i,result))
        {
            k = dep + check(path[i],aim_board);//深度+错误数
            if(k < min)
            {
                min = k;
                t = i;
            }
        }
    }
    return t;
}

//在path中查找与目标结构相同的路径点,返回其下标
int check_same(int pcount,int path[_MAX_PATH][N][N],int aim_board[N][N])
{
    for(int i = 0;i < pcount;i++)
    {
        if(check(path[i],aim_board) == 0)
        {
            return i;
        }
    }
    return -1;
}

//返回移动棋子后棋盘的结果
//上移
int up_move(int last_board[N][N])
{
    int zero[2];
    checkZero(zero,last_board);

    last_board[zero[0]][zero[1]] = last_board[zero[0]-1][zero[1]];
    last_board[zero[0]-1][zero[1]] = 0;

    return 1;
}
//左移
int left_move(int last_board[N][N])
{
    int zero[2];
    checkZero(zero,last_board);

    last_board[zero[0]][zero[1]] = last_board[zero[0]][zero[1]-1];
    last_board[zero[0]][zero[1]-1] = 0;

    return 1;
}
//下移
int down_move(int last_board[N][N])
{
    int zero[2];
    checkZero(zero,last_board);

    last_board[zero[0]][zero[1]] = last_board[zero[0]+1][zero[1]];
    last_board[zero[0]+1][zero[1]] = 0;

    return 1;
}
//右移
int right_move(int last_board[N][N])
{
    int zero[2];
    checkZero(zero,last_board);

    last_board[zero[0]][zero[1]] = last_board[zero[0]][zero[1]+1];
    last_board[zero[0]][zero[1]+1] = 0;

    return 1;
}

//输出一个棋盘结构
void print_chess(int board[N][N])
{
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < N;j++)
        {
            printf("%d ",board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

//循环在path中找到上一个路径点,直到找出整个路径
int find_last(int last_board[N][N],int pcount,int path[_MAX_PATH][N][N],int action[_MAX_PATH],int result[_MAX_PATH])
{
    int i = 0;
    int k = pcount;
    int tmp[N][N];
    copy_board(tmp,last_board);

    while(k != 0)
    {
        k = check_same(pcount,path,last_board);
        result[i] = k;
        i++;
        switch (action[k])
        {
        case 0:
            down_move(last_board);
            break;
        case 1:
            right_move(last_board);
            break;
        case 2:
            up_move(last_board);
            break;
        case 3:
            left_move(last_board);
            break;
        }
    }
    for(int j = i-1;j >= 0;j--)
    {
        print_chess(path[result[j]]);
    }
}