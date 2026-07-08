#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

using namespace std;

bool isPathClear(const vector<vector<string>> &board, int r1, int c1, int r2, int c2)
{
    int sr = (r1 == r2) ? 0 : (r2 > r1 ? 1 : -1);
    int sc = (c1 == c2) ? 0 : (c2 > c1 ? 1 : -1);
    int cr = r1 + sr;
    int cc = c1 + sc;
    
    while (cr != r2 || cc != c2)
    {
        if (board[cr][cc] != ".")
            return false;
        cr += sr;
        cc += sc;
    }
    return true;
}

bool isValidMove(const vector<vector<string>> &board, char piece, int r1, int c1, int r2, int c2)
{
    int dr = abs(r2 - r1);
    int dc = abs(c2 - c1);
    
    if (dr == 0 && dc == 0) return false;

    switch (piece)
    {
        case 'K':
            return (dr <= 1 && dc <= 1);
            
        case 'R':
            return (dr == 0 || dc == 0) && isPathClear(board, r1, c1, r2, c2);
            
        case 'B':
            return (dr == dc) && isPathClear(board, r1, c1, r2, c2);
            
        case 'Q':
            return ((dr == 0 || dc == 0) || (dr == dc)) && isPathClear(board, r1, c1, r2, c2);
            
        case 'N':
            return (dr == 1 && dc == 2) || (dr == 2 && dc == 1);
            
        case 'P':
            {
                char color = board[r1][c1][0];
                int direction = (color == 'w') ? -1 : 1;
                
                if (r2 - r1 == direction && dc == 0)
                {
                    return board[r2][c2] == ".";
                }
                
                if (r2 - r1 == direction && dc == 1)
                {
                    return board[r2][c2] != "." && board[r2][c2][0] != color;
                }
                
                return false;
            }
            
        default:
            return false;
    }
}

void executeClick(vector<vector<string>> &board, int &sel_r, int &sel_c, int r, int c)
{
    if (r < 0 || r >= (int)board.size() || c < 0 || c >= (int)board[0].size()) return;

    if (sel_r == -1)
    {
        if (board[r][c] != ".")
        {
            sel_r = r;
            sel_c = c;
        }
    }
    else
    {
        if (board[r][c] == "." || board[sel_r][sel_c][0] != board[r][c][0])
        {
            if (isValidMove(board, board[sel_r][sel_c][1], sel_r, sel_c, r, c))
            {
                board[r][c] = board[sel_r][sel_c];
                board[sel_r][sel_c] = ".";
            }
        }
        
        if (board[r][c] != "." && board[sel_r][sel_c][0] == board[r][c][0])
        {
            sel_r = r;
            sel_c = c;
        }
        else
        {
            sel_r = -1;
            sel_c = -1;
        }
    }
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string line;
    vector<vector<string>> board;
    bool inBoard = false;

    while (getline(cin, line))
    {
        if (line.empty()) continue;
        if (line.find("Board:") != string::npos) { inBoard = true; continue; }
        if (line.find("Commands:") != string::npos) break;
        if (!inBoard) continue;

        vector<string> row;
        string token;
        istringstream iss(line);
        while (iss >> token) row.push_back(token);
        if (!row.empty()) board.push_back(row);
    }

    int sel_r = -1, sel_c = -1;
    
    while (getline(cin, line))
    {
        if (line.empty()) continue;

        if (line.find("click") == 0)
        {
            istringstream iss(line);
            string cmd;
            int x, y;
            if (iss >> cmd >> x >> y)
            {
                executeClick(board, sel_r, sel_c, y / 100, x / 100);
            }
        }
        else if (line == "print board")
        {
            for (const auto &row : board)
            {
                for (size_t i = 0; i < row.size(); ++i)
                {
                    cout << row[i] << (i + 1 < row.size() ? " " : "");
                }
                cout << "\n";
            }
        }
    }
    return 0;
}