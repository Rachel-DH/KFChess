#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

struct MoveAnimation {
    bool active = false;
    int from_r, from_c;
    int to_r, to_c;
    string piece;
    int arrival_time = 0;
};

struct JumpState {
    bool is_airborne = false;
    int landing_time = 0;
};

int current_time = 0;
bool game_over = false;
MoveAnimation active_move;

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
                
                bool is_start_row = false;
                if (color == 'w')
                {
                    if ((board.size() == 4 && r1 == 3) || (board.size() > 4 && r1 == (int)board.size() - 2))
                        is_start_row = true;
                }
                else
                {
                    if ((board.size() == 4 && r1 == 0) || (board.size() > 4 && r1 == 1))
                        is_start_row = true;
                }
                
                if (is_start_row && r2 - r1 == 2 * direction && dc == 0)
                {
                    return board[r1 + direction][c2] == "." && board[r2][c2] == ".";
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

void updateBoardTime(vector<vector<string>> &board, vector<vector<JumpState>> &jump_board)
{
    // 1. קודם כל בודקים הגעה של כלי נע - לפני שמנחיתים כלים קופצים באותו הטיק!
    if (active_move.active && current_time >= active_move.arrival_time)
    {
        int tr = active_move.to_r;
        int tc = active_move.to_c;

        // אם היעד מכיל כלי אויב שעדיין נחשב באוויר בנקודת הזמן הזו
        if (jump_board[tr][tc].is_airborne && board[tr][tc] != "." && board[tr][tc][0] != active_move.piece[0])
        {
            // חוק מיוחד: הכלי שבאוויר לוכד את האויב המגיע. הכלי הנע נמחק ומוסר מהלוח
            board[active_move.from_r][active_move.from_c] = "."; 
            active_move.active = false;
        }
        else
        {
            // תפיסה / הגעה רגילה על הקרקע
            if (board[tr][tc] != "." && board[tr][tc][1] == 'K')
            {
                game_over = true;
            }
            
            string final_piece = active_move.piece;
            if (final_piece[1] == 'P')
            {
                if ((final_piece[0] == 'w' && tr == 0) || 
                    (final_piece[0] == 'b' && tr == (int)board.size() - 1))
                {
                    final_piece[1] = 'Q';
                }
            }

            board[active_move.from_r][active_move.from_c] = ".";
            board[tr][tc] = final_piece;
            active_move.active = false;
        }
    }

    // 2. רק לאחר מכן, מורידים לקרקע את הכלים שזמן הקפיצה שלהם פג
    for (size_t r = 0; r < board.size(); ++r)
    {
        for (size_t c = 0; c < board[r].size(); ++c)
        {
            if (jump_board[r][c].is_airborne && current_time >= jump_board[r][c].landing_time)
            {
                jump_board[r][c].is_airborne = false;
            }
        }
    }
}

void executeClick(vector<vector<string>> &board, vector<vector<JumpState>> &jump_board, int &sel_r, int &sel_c, int r, int c)
{
    updateBoardTime(board, jump_board);

    if (game_over || active_move.active)
    {
        sel_r = -1;
        sel_c = -1;
        return;
    }
    
    if (r < 0 || r >= (int)board.size() || c < 0 || c >= (int)board[0].size()) return;

    if (sel_r == -1)
    {
        if (board[r][c] != "." && !jump_board[r][c].is_airborne)
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
                active_move.active = true;
                active_move.from_r = sel_r;
                active_move.from_c = sel_c;
                active_move.to_r = r;
                active_move.to_c = c;
                active_move.piece = board[sel_r][sel_c];
                
                int distance = max(abs(r - sel_r), abs(c - sel_c));
                active_move.arrival_time = current_time + (distance * 1000); 
            }
        }
        
        if (!active_move.active && board[r][c] != "." && board[sel_r][sel_c][0] == board[r][c][0] && !jump_board[r][c].is_airborne)
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
    vector<vector<JumpState>> jump_board;
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

    if (!board.empty()) {
        jump_board.assign(board.size(), vector<JumpState>(board[0].size()));
    }

    int sel_r = -1, sel_c = -1;
    
    while (getline(cin, line))
    {
        if (line.empty()) continue;

        updateBoardTime(board, jump_board);

        if (line.find("tick") == 0 || line.find("wait") == 0)
        {
            istringstream iss(line);
            string cmd;
            int t;
            if (iss >> cmd >> t)
            {
                current_time += t;
            }
            updateBoardTime(board, jump_board);
        }
        else if (line.find("click") == 0)
        {
            istringstream iss(line);
            string cmd;
            int x, y;
            if (iss >> cmd >> x >> y)
            {
                executeClick(board, jump_board, sel_r, sel_c, y / 100, x / 100);
            }
        }
        else if (line.find("jump") == 0)
        {
            istringstream iss(line);
            string cmd;
            int x, y;
            if (iss >> cmd >> x >> y)
            {
                int r = y / 100;
                int c = x / 100;
                
                if (r >= 0 && r < (int)board.size() && c >= 0 && c < (int)board[0].size() && board[r][c] != ".")
                {
                    bool is_moving = active_move.active && (active_move.from_r == r && active_move.from_c == c);
                    
                    if (!is_moving && !jump_board[r][c].is_airborne)
                    {
                        jump_board[r][c].is_airborne = true;
                        jump_board[r][c].landing_time = current_time + 1000;
                    }
                }
            }
        }
        else if (line == "print board")
        {
            for (size_t r = 0; r < board.size(); ++r)
            {
                for (size_t c = 0; c < board[r].size(); ++c)
                {
                    cout << board[r][c] << (c + 1 < board[r].size() ? " " : "");
                }
                cout << "\n";
            }
        }
    }
    return 0;
}