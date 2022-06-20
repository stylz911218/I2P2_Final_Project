#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <vector>
#include <deque>
#include <set>
#include <limits.h>

using namespace std;

struct Point
{
    int x, y;

    bool operator<(const Point &rhs) const
    {
        if (rhs.x < x)
            return -1;
        else if (rhs.x > x)
            return 1;
        else
        {
            if (rhs.y < y)
                return -1;
            else if (rhs.y > y)
                return 1;
            else
                return 0;
        }
    }
};

struct node
{
    int value;
    Point pt;
    vector<node *> nextmove;
};

enum SPOT_STATE
{
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

int player;
const int SIZE = 15;
std::array<std::array<int, SIZE>, SIZE> board;
int black_count = 0, white_count = 0;

set<Point> checked()
{
    set<Point> reg;
    int dx[8] = {0, 1, 0, -1, -1, 1, 1, -1};
    int dy[8] = {1, 0, -1, 0, 1, -1, 1, -1};
    int tmp_x, tmp_y;

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] != 0)
            {
                for (int k = 0; k < 8; k++)
                {
                    tmp_x = i + dx[k];
                    tmp_y = j + dy[k];

                    if (tmp_x < 0 || tmp_y < 0 || tmp_x >= SIZE || tmp_y >= SIZE)
                        continue;

                    if (board[tmp_x][tmp_y] != 0)
                        continue;

                    Point tmp;
                    tmp.x = tmp_x, tmp.y = tmp_y;
                    reg.insert(tmp);
                }
            }
        }
    }

    return reg;
}

void generator(node *n)
{
    set<Point> range = checked();

    for (auto i : range)
    {
        node *tmp = new node;
        tmp->pt.x = i.x;
        tmp->pt.y = i.y;
        n->nextmove.emplace_back(tmp);
    }
}

void read_board(std::ifstream &fin)
{
    fin >> player;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            fin >> board[i][j];
        }
    }
}

void state_value(node *n)
{
    int cur_x = n->pt.x, cur_y = n->pt.y;
    n->value = 0;
    int pos[36] = {0};
    pos[1] = board[cur_x - 4][cur_y], pos[2] = board[cur_x - 3][cur_y], pos[3] = board[cur_x - 2][cur_y], pos[4] = board[cur_x - 1][cur_y],                         //直上
        pos[5] = board[cur_x + 1][cur_y], pos[6] = board[cur_x + 2][cur_y], pos[7] = board[cur_x + 3][cur_y], pos[8] = board[cur_x + 4][cur_y],                     //直下
        pos[9] = board[cur_x - 4][cur_y + 4], pos[10] = board[cur_x - 3][cur_y + 3], pos[11] = board[cur_x - 2][cur_y + 2], pos[12] = board[cur_x - 1][cur_y + 1],  //左斜上
        pos[13] = board[cur_x + 1][cur_y - 1], pos[14] = board[cur_x + 2][cur_y - 2], pos[15] = board[cur_x + 3][cur_y - 3], pos[16] = board[cur_x + 4][cur_y - 4], //左斜下
        pos[17] = board[cur_x][cur_y + 4], pos[18] = board[cur_x][cur_y + 3], pos[19] = board[cur_x][cur_y + 2], pos[20] = board[cur_x][cur_y + 1],                 //中右
        pos[21] = board[cur_x][cur_y - 1], pos[22] = board[cur_x][cur_y - 2], pos[23] = board[cur_x][cur_y - 3], pos[24] = board[cur_x][cur_y - 4],                 //中左
        pos[25] = board[cur_x + 4][cur_y + 4], pos[26] = board[cur_x + 3][cur_y + 3], pos[27] = board[cur_x + 2][cur_y + 2], pos[28] = board[cur_x + 1][cur_y + 1],
    pos[29] = board[cur_x - 1][cur_y - 1], pos[30] = board[cur_x - 2][cur_y - 2], pos[31] = board[cur_x - 3][cur_y - 3], pos[32] = board[cur_x - 4][cur_y - 4];

    //我方連線
    for (int i = 0; i < 4; i++)
    {
        for (int j = 1; j <= 5; j++)
        {
            if (pos[i * 8 + j] == player && pos[1 + i * 8 + j] == player && pos[2 + i * 8 + j] == player && pos[3 + i * 8 + j] == player)
            {
                n->value += 1000000;
                return;
            }
        }
    }

    //敵方下步下這個就將連線
    for (int i = 0; i < 4; i++)
    {
        for (int j = 1; j <= 5; j++)
        {
            if (pos[i * 8 + j] == 3 - player && pos[1 + i * 8 + j] == 3 - player && pos[2 + i * 8 + j] == 3 - player && pos[3 + i * 8 + j] == 3 - player)
            {
                n->value += 700000;
                return;
            }
        }
    }

    //我方活四
    for (int i = 0; i < 4; i++)
    {
        for (int j = 2; j <= 5; j++)
        {
            if (pos[i * 8 + j] == player && pos[1 + i * 8 + j] == player && pos[2 + i * 8 + j] == player)
            {
                n->value += 300000;
                return;
            }
        }
    }

    //敵方活三 || 斷三
    for (int i = 0; i < 4; i++)
    {
        for (int j = 1; j <= 4; j++)
        {
            if (pos[i * 8 + j] == EMPTY && pos[1 + i * 8 + j] == 3 - player && pos[2 + i * 8 + j] == 3 - player && pos[3 + i * 8 + j] == 3 - player)
            {
                n->value += 200000;
                return;
            }
        }
    }

    //我方活三
    for (int i = 0; i < 4; i++)
    {
        for (int j = 2; j <= 4; j++)
        {
            if (pos[i * 8 + j] == EMPTY && pos[1 + i * 8 + j] == player && pos[2 + i * 8 + j] == player && pos[3 + i * 8 + j] == EMPTY)
            {
                n->value += 5000;
            }
        }
    }

    //我方死四
    for (int i = 0; i < 4; i++)
    {
        for (int j = 1; j <= 4; j++)
        {
            if ((pos[i * 8 + j] == 3 - player || pos[4 + i * 8 + j] == 3 - player) && pos[1 + i * 8 + j] == player && pos[2 + i * 8 + j] == player && pos[3 + i * 8 + j] == player)
            {
                n->value += 2000;
            }
        }
    }

    //我方斷三
    for (int i = 0; i < 4; i++)
    {
        for (int j = 3; j <= 4; j++)
        {
            if (pos[i * 8 + j] == player && pos[1 + i * 8 + j] == EMPTY && pos[2 + i * 8 + j] == player)
            {
                n->value += 200;
            }
        }
    }

    //周遭有兄弟
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (pos[4 * (2 * i + 1) + j] == player)
            {
                n->value += 10;
            }
        }
    }

    //沒兄弟 後手的第一格
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (pos[4 * (2 * i + 1) + j] == 3 - player)
            {
                n->value += 2;
            }
        }
    }
}

pair<Point, int> minimax(node *n, int depth)
{
    if (depth == 0)
    {
        state_value(n);
        cout << "(" << n->pt.x << "," << n->pt.y << ")"
             << ":" << n->value << endl;
        return pair<Point, int>(n->pt, n->value);
    }

    pair<Point, int> tmp;

    if (depth % 2)
    {
        n->value = INT_MIN;
        generator(n);
        for (auto i : n->nextmove)
        {
            board[i->pt.x][i->pt.y] = player;
            tmp = minimax(i, depth - 1);
            board[i->pt.x][i->pt.y] = 0;
            if (tmp.second > n->value)
            {
                n->value = tmp.second;
                n->pt.x = i->pt.x;
                n->pt.y = i->pt.y;
            }
        }
        return pair<Point, int>(n->pt, n->value);
    }
    else
    {
        n->value = INT_MAX;
        generator(n);
        for (auto i : n->nextmove)
        {
            board[i->pt.x][i->pt.y] = 3 - player;
            tmp = minimax(i, depth - 1);
            board[i->pt.x][i->pt.y] = 0;
            if (tmp.second < n->value)
            {
                n->value = tmp.second;
                n->pt.x = i->pt.x;
                n->pt.y = i->pt.y;
            }
        }
        return pair<Point, int>(n->pt, n->value);
    }
}

void write_valid_spot(std::ofstream &fout)
{
    srand(time(NULL));
    int x, y;
    bool first = true;
    node *cur = new node;

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] != 0)
            {
                first = false;
                break;
            }
        }
        if (!first)
            break;
    }

    if (!first)
    {
        pair<Point, int> ans;

        ans = minimax(cur, 1);
        x = ans.first.x;
        y = ans.first.y;
    }
    else
    {
        x = y = 7;
    }

    fout << x << " " << y << endl;
    fout.flush();
}

/*pair<Point, int> alphabeta(node *n, int depth, int alpha, int beta)
{
}*/

int main(int, char **argv)
{
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
