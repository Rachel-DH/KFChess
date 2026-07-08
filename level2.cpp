#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>

using namespace std;

static string trim(string s)
{
    size_t first = s.find_first_not_of(" \t\r");
    if (first == string::npos)
        return "";
    size_t last = s.find_last_not_of(" \t\r");
    return s.substr(first, last - first + 1);
}

static string rstripCR(string s)
{
    if (!s.empty() && s.back() == '\r')
        s.pop_back();
    return s;
}

static vector<string> splitTokens(const string &line)
{
    vector<string> tokens;
    istringstream iss(line);
    string tok;
    while (iss >> tok)
        tokens.push_back(tok);
    return tokens;
}

static bool isValidToken(const string &tok)
{
    if (tok == ".")
        return true;
    if (tok.size() != 2)
        return false;
    if (tok[0] != 'w' && tok[0] != 'b')
        return false;
    static const string pieces = "KQRBNP";
    return pieces.find(tok[1]) != string::npos;
}

bool isPathClear(const vector<vector<string>> &board, int r1, int c1, int r2, int c2)
{
    int sr = (r1 == r2) ? 0 : (r2 > r1 ? 1 : -1);
    int sc = (c1 == c2) ? 0 : (c2 > c1 ? 1 : -1);
    int cr = r1 + sr, cc = c1 + sc;
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
    int dr = abs(r2 - r1), dc = abs(c2 - c1);
    if (dr == 0 && dc == 0)
        return false;

    if (piece == 'K')
        return (dr <= 1 && dc <= 1);
    if (piece == 'R')
        return (dr == 0 || dc == 0) && isPathClear(board, r1, c1, r2, c2);
    if (piece == 'B')
        return (dr == dc) && isPathClear(board, r1, c1, r2, c2);
    if (piece == 'Q')
        return ((dr == 0 || dc == 0) || (dr == dc)) && isPathClear(board, r1, c1, r2, c2);
    if (piece == 'N')
        return (dr == 1 && dc == 2) || (dr == 2 && dc == 1);
    if (piece == 'P')
        return (dr == 1 && dc == 0);
    return false;
}

void executeClick(vector<vector<string>> &board, int &sel_r, int &sel_c, int r, int c)
{
    if (r < 0 || r >= (int)board.size() || c < 0 || c >= (int)board[0].size())
        return;

    if (board[r][c] != ".")
    {
        if (sel_r != -1 && board[sel_r][sel_c][0] != board[r][c][0])
        {
            if (isValidMove(board, board[sel_r][sel_c][1], sel_r, sel_c, r, c))
            {
                board[r][c] = board[sel_r][sel_c];
                board[sel_r][sel_c] = ".";
            }
            sel_r = -1;
            sel_c = -1;
        }
        else
        {
            sel_r = r;
            sel_c = c;
        }
    }
    else
    {
        if (sel_r != -1)
        {
            if (isValidMove(board, board[sel_r][sel_c][1], sel_r, sel_c, r, c))
            {
                board[r][c] = board[sel_r][sel_c];
                board[sel_r][sel_c] = ".";
            }
            sel_r = -1;
            sel_c = -1;
        }
    }
}

int main()
{
    string line;
    vector<vector<string>> board;

    bool inBoard = false;
    while (getline(cin, line))
    {
        line = trim(rstripCR(line));        if (line.empty())
            continue;
        if (line == "Board:"){
            inBoard = true;
            continue;
        }
        if (line == "Commands:")
            break;
        
        if (!inBoard) {
            continue;
        }
        vector<string> tokens = splitTokens(line);
        if (!tokens.empty())
            board.push_back(tokens);
    }

    size_t width = board.empty() ? 0 : board[0].size();
    for (const auto &row : board)
    {
        if (row.size() != width)
        {
            cout << "ERROR ROW_WIDTH_MISMATCH\n";
            return 0;
        }
    }

    for (const auto &row : board)
    {
        for (const auto &tok : row)
        {
            if (!isValidToken(tok))
            {
                cout << "ERROR UNKNOWN_TOKEN\n";
                return 0;
            }
        }
    }

    int sel_r = -1, sel_c = -1;
    while (getline(cin, line))
    {
        line = trim(rstripCR(line));
        if (line.empty())
            continue;

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
                    if (i > 0)
                        cout << " ";
                    cout << row[i];
                }
                cout << "\n";
            }
        }
    }

    return 0;
}