#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

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

int main()
{
    string line;
    vector<vector<string>> board;

    while (getline(cin, line))
    {
        line = rstripCR(line);
        if (line == "Board:")
            continue;
        if (line == "Commands:")
            break;
        board.push_back(splitTokens(line));
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

    while (getline(cin, line))
    {
        line = rstripCR(line);
        if (line == "print board")
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
