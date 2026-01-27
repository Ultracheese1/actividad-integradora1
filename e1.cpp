#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>

using namespace std;

// ----------------------------------- Parte 1 -----------------------------------
// Búsqueda de subsecuencias

// KMP prefix
vector<int> prefix_function(const string &s) {
    int n = s.size();
    vector<int> pi(n);
    for (int i = 1; i < n; i++) {
        int j = pi[i - 1];
        while (j > 0 && s[i] != s[j])
            j = pi[j - 1];
        if (s[i] == s[j])
            j++;
        pi[i] = j;
    }
    return pi;
}

// KMP para buscar si un patron esta en un texto
bool KMP_search(const string &text, const string &pattern, int &position) {
    if (pattern.empty() || text.empty())
        return false;

    vector<int> pi = prefix_function(pattern);
    int j = 0;
    for (int i = 0; i < (int)text.size(); i++) {
        while (j > 0 && text[i] != pattern[j])
            j = pi[j - 1];
        if (text[i] == pattern[j])
            j++;
        if (j == (int)pattern.size()) {
            position = i - pattern.size() + 2; // para base 1
            return true;
        }
    }
    return false;
}


// ----------------------------------- Parte 2 -----------------------------------
// Busqueda de palíndromos

// Manacher
pair<int, int> longestPalindromeManacher(const string &s) {
    // transformar la cadena con # separador
    string t = "#";
    for (char c : s) {
        t += c;
        t += '#';
    }

    int n = t.size();
    vector<int> p(n, 0); // radio del palíndromo centrado en cada posición
    int center = 0, right = 0;
    int maxLen = 0, centerIndex = 0;

    for (int i = 0; i < n; i++) {
        int mirror = 2 * center - i;
        if (i < right)
            p[i] = min(right - i, p[mirror]);

        // extender al rededor de i
        int a = i + 1 + p[i];
        int b = i - 1 - p[i];
        while (a < n && b >= 0 && t[a] == t[b]) {
            p[i]++;
            a++;
            b--;
        }

        // limites
        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }

        // guardar el máximo
        if (p[i] > maxLen) {
            maxLen = p[i];
            centerIndex = i;
        }
    }

    // convertir índices a la cadena original
    int start = (centerIndex - maxLen) / 2;
    int end = start + maxLen - 1;

    return {start + 1, end + 1}; // para base 1
}


// para leer el contenido completo de un archivo
string readFile(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filename << endl;
        exit(1);
    }
    string content, line;
    while (getline(file, line))
        content += line; // se ignoran saltos de línea
    return content;
}

// ----------------------------------- Parte 3 -----------------------------------
// Substring común más largo

// (DP O(n*m))
pair<int, int> longestCommonSubstring(const string &t1, const string &t2) {
    int n = t1.size();
    int m = t2.size();
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));

    int maxLen = 0;
    int endPos = 0; // posición final en t1

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (t1[i - 1] == t2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                if (dp[i][j] > maxLen) {
                    maxLen = dp[i][j];
                    endPos = i; // posición final en t1
                }
            }
        }
    }

    int start = endPos - maxLen + 1;
    return {start, endPos}; // base 1
}


// ----------------------------------- Parte 4 -----------------------------------
// Huffman Coding

struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode *left, *right;

    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    HuffmanNode(HuffmanNode* l, HuffmanNode* r)
        : ch('\0'), freq(l->freq + r->freq), left(l), right(r) {}
};

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

// construir el árbol de Huffman
HuffmanNode* buildHuffmanTree(const string &text) {
    unordered_map<char, int> freq;
    for (char c : text)
        freq[c]++;

    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
    for (auto &p : freq)
        pq.push(new HuffmanNode(p.first, p.second));

    while (pq.size() > 1) {
        HuffmanNode* a = pq.top(); pq.pop();
        HuffmanNode* b = pq.top(); pq.pop();
        pq.push(new HuffmanNode(a, b));
    }

    return pq.top();
}

// obtener longitudes de códigos de Huffman
void getCodeLengths(HuffmanNode* node, int depth,
                    unordered_map<char, int> &lengths) {
    if (!node) return;

    if (!node->left && !node->right) {
        lengths[node->ch] = depth;
        return;
    }

    getCodeLengths(node->left, depth + 1, lengths);
    getCodeLengths(node->right, depth + 1, lengths);
}

// longitud promedio esperada
double averageCodeLength(const unordered_map<char,int> &freq,
                          const unordered_map<char,int> &lengths,
                          int total) {
    double avg = 0.0;
    for (auto &p : freq) {
        double prob = (double)p.second / total;
        avg += prob * lengths.at(p.first);
    }
    return avg;
}

// longitud codificada de cualquier string
int encodedLength(const string &s,
                  const unordered_map<char,int> &lengths) {
    int total = 0;
    for (char c : s)
        total += lengths.at(c);
    return total;
}


// ----------------------------------- Main -----------------------------------
int main() {
    string t1 = readFile("transmission1.txt");
    string t2 = readFile("transmission2.txt");
    
    vector<string> mcodes = {
        readFile("mcode1.txt"),
        readFile("mcode2.txt"),
        readFile("mcode3.txt")
    };

    // Parte 1
    vector<string> transmissions = {t1, t2};

    for (int i = 0; i < (int)transmissions.size(); i++) {
        for (int j = 0; j < (int)mcodes.size(); j++) {
            int pos = -1;
            bool found = KMP_search(transmissions[i], mcodes[j], pos);
            if (found)
                // cout << "true " << pos << " " << mcodes[j] << "\n";
                cout << "true " << pos << "\n";

            else
                cout << "false\n";
        }
    }

    // Parte 2
    auto p1 = longestPalindromeManacher(t1);
    auto p2 = longestPalindromeManacher(t2);

    cout << p1.first << " " << p1.second << endl;
    cout << p2.first << " " << p2.second << endl;

    // Parte 3
    pair<int, int> res = longestCommonSubstring(t1, t2);
    cout << res.first << " " << res.second << endl;

    // Parte 4 – Huffman

    for (int i = 0; i < 2; i++) {
        const string &t = transmissions[i];

        // frecuencias
        unordered_map<char,int> freq;
        for (char c : t) freq[c]++;

        HuffmanNode* root = buildHuffmanTree(t);

        unordered_map<char,int> lengths;
        getCodeLengths(root, 0, lengths);

        double avgLen = averageCodeLength(freq, lengths, t.size());

        for (const string &mcode : mcodes) {
            int realLen = encodedLength(mcode, lengths);
            double expected = avgLen * mcode.size();

            if (realLen > expected * 1.5)
                cout << "sospechoso " << realLen << "\n";
            else
                cout << "no-sospechoso " << realLen << "\n";
        }
    }

    return 0;
}