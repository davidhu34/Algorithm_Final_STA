#include <cassert>
#include <vector>
#include <queue>
#include <bitset>
#include <unordered_map>
#include <algorithm>

#include "hc.h"
#include "bitio.h"

typedef unsigned int  Uint;
typedef unsigned char Uchar;

static constexpr size_t char_set_n = 256;

typedef std::bitset<char_set_n> Bits;

namespace {
    struct Node {
        Uchar c;     // represented character
        Uint  f;     // frequency
        Node* left;  // left child
        Node* right; // right child
    };

    // Compare Node according to frequency.
    //
    // Input
    //   n1: (Pointer to) Node 1.
    //   n2: (Pointer to) Node 2.
    //
    // Output
    //   True if frequency of n1 < frequency of n2.
    //   False otherwise.
    //
    struct CompNode {
        bool operator()(const Node* n1, const Node* n2) const {
            return n1->f < n2->f;
        }
    };

    // Compare symbol (character) according to its code length.
    //
    // Input
    //   code_len[]: Map character to its code length.
    //   a         : A character.
    //   b         : Another character.
    //
    // Output
    //   True if code length of a < code length of b.
    //   False otherwise.
    //
    struct CompSym {
        const size_t* code_len;

        explicit CompSym(const size_t* _code_len): code_len(_code_len) {}
        bool operator()(Uchar a, Uchar b) const {
            return code_len[a] < code_len[b];
        }
    };
}

// Calculate frequency of characters appeared in the input file.
//
// Input
//   fin: A file (stream).
//
// Output
//   freq[]: Map character to its frequency of appearance in the file.
//
// Note
//   It set fin to initial state before return.
//   
static void calc_freq(std::ifstream& fin, Uint freq[]) {
    for (size_t i = 0; i < char_set_n; ++i) {
        freq[i] = 0;
    }

    char c;
    while (!fin.get(c)) {
        freq[(Uchar)c] += 1;
    }

    fin.seekg(0);
    fin.clear();
}

// Use recursive method to calculate code length of all characters according 
// to huffman coding tree.
//
// Input
//   root: Root of huffman coding tree.
//
// Output
//   code_len[]: Map symbol (character) to its code length.
//
// Note
//   Initialize len with 0.
//
static void calc_code_len(size_t len, Node* node, size_t code_len[]) {
    if (node->left == nullptr && node->right == nullptr) { // is leaf
        code_len[node->c] = len;
    }
    else {
        if (node->left) {
            calc_code_len(len + 1, node->left, code_len);
        }
        if (node->right) {
            calc_code_len(len + 1, node->right, code_len);
        }
    }
}

// Create mapping of character to code.
//
// Input
//   code_len[]: Map character to its code length.
//   sym[]     : Array of character which has freq > 0, 
//               sorted by length, then by its value
//   sym_n     : Size of sym[].
//   
// Output
//   code[]: Map character to code. Initially all reset.
//
static void create_code(Bits code[], const size_t code_len[], 
                        const Uchar sym[], size_t sym_n) {
    code[sym[0]].reset();
    Bits temp;
    for (size_t i = 1; i < sym_n; ++i) {
        temp.set(0); // temp |= 1;
        temp <<= (code_len[sym[i]] - code_len[sym[i - 1]]);
        code[sym[i]] = temp;
    }
}

// Build a coding table.
//
// Input
//   root: Root of a tree representing a huffman coding tree.
//
// Output
//   code[]    : Map character to code.
//   code_len[]: Map character to its code length.
//   sym[]     : Array of character which has freq > 0, 
//               sorted by length, then by its value
//   sym_n     : Size of sym[].
//
static void build_table(Node* root, Bits code[], size_t code_len[], 
                        Uchar sym[], size_t& sym_n) {
    // Calculate code length of symbols.
    for (size_t i = 0; i < char_set_n; ++i) {
        code_len[i] = 0;
    }
    calc_code_len(0, root, code_len);

    // Collect those symbols with code length > 0.
    sym_n = 0;
    for (size_t i = 0; i < char_set_n; ++i) {
        if (code_len[i] != 0) {
            sym[sym_n] = (Uchar)i;
            sym_n += 1;
        }
    }

    // Sort them by length.
    std::stable_sort(sym, sym + sym_n, (CompSym(code_len)));

    // Create their code.
    create_code(code, code_len, sym, sym_n);
}

void hc_c(std::ifstream& fin, std::ofstream& fout) {
    typedef std::priority_queue<Node*, std::vector<Node*>, CompNode> Heap;

    // Calculate frequency.
    Uint freq[char_set_n];
    calc_freq(fin, freq);

    // Build a priority queue.
    Node nodes[char_set_n*2];
    size_t nodes_n = char_set_n;
    for (size_t i = 0; i < nodes_n; ++i) {
        nodes[i] = {(Uchar)i, freq[i], nullptr, nullptr};
    }
    
    Node* pnodes[char_set_n]; // a helper for building queue
    for (size_t i = 0; i < char_set_n; ++i) {
        pnodes[i] = &nodes[i];
    }
    Heap q(pnodes, pnodes + char_set_n);

    // Build the coding tree.
    for (size_t i = 1; i < char_set_n; ++i) {
        Node* z = &nodes[nodes_n];
        nodes_n += 1;
        assert(nodes_n <= char_set_n*2);

        z->left  = q.top(); q.pop();
        z->right = q.top(); q.pop();
        z->f = z->left->f + z->right->f;
        q.push(z);
    }
    assert(q.size() == 1);

    // Build the conversion table.
    Bits   code[char_set_n];
    size_t code_len[char_set_n]; // length of each code in table

    Uchar  sym[char_set_n];
    size_t sym_n = 0;
    
    build_table(q.top(), code, code_len, sym, sym_n);
    
    // Output table.
    //   _ = 8 bits
    //
    //   _ _ _ _ _ _ _ _ 255 x _ _ _ _ _ _ _ _ x 
    //     ^             ^         ^           ^
    //     number of     divider   character   divider
    //     symbol that             with code
    //     is 2-bit                length > 0
    //     length                  sort by len
    //
    size_t count[char_set_n] = {};
    size_t count_n = 0;

    for (size_t i = 0; i < char_set_n; ++i) {
        count[code_len[i]] += 1;
        if (code_len[i] > count_n) {
            count_n = code_len[i];
        }
    }

    for (size_t i = 1; i < count_n; ++i) {
        fout.put((Uchar)count[i]);
    }
    fout.put((Uchar)255); // divider
    for (size_t i = 0; i < sym_n; ++i) {
        fout.put(sym[i]);
    }
    fout.put(sym[0]); // divider

    // Output encoding.
    BitWriter writer(fout);
    char c;
    while (!fin.get(c)) {
        writer.write(code[(Uchar)c], code_len[(Uchar)c]);
    }
    writer.flush();
    
    // Done.
}

// Create mapping of code to character.
//
// Input
//   code_len[]: Map character to its code length.
//   sym[]     : Array of character which has freq > 0, 
//               sorted by length, then by its value
//   sym_n     : Size of sym[].
//   
// Output
//   character[]: Map code to character. Initially all reset.
//
static void create_character(std::unordered_map<Bits, Uchar>& character, 
                             const size_t code_len[],
                             const Uchar sym[], size_t sym_n)  {
    Bits code[char_set_n];
    create_code(code, code_len, sym, sym_n);

    for (size_t i = 0; i < sym_n; ++i) {
        character[code[sym[i]]] = sym[i];
    }
}

void hc_x(std::ifstream& fin, std::ofstream& fout) {
    // Read code length count.
    size_t count[char_set_n] = {};
    size_t count_n = 1;

    char c;
    while (!fin.get(c) && (Uchar)c != 255) {
        count[count_n] = c;
        count_n += 1;
    }
    assert(!fin);

    // Read symbol.
    Uchar  sym[char_set_n];
    size_t sym_n = 0;

    fin.get(c);
    sym[0] = c;
    sym_n += 1;

    while (!fin.get(c) && (Uchar)c != sym[0]) {
        sym[sym_n] = c;
        sym_n += 1;
    }
    assert(!fin);

    // Create mapping of character to code length.
    size_t code_len[char_set_n] = {};
    size_t k = 0;
    for (size_t i = 0; i < count_n; ++i) {
        for (size_t j = 0; j < count[i]; ++j) {
            code_len[sym[k]] = i;
            k += 1;
        }
    }
    assert(k == sym_n);

    // Create mapping of character to code.
    std::unordered_map<Bits, Uchar> character;
    create_character(character, code_len, sym, sym_n);

    // Start decoding.
    BitReader reader(fin);
    Bits temp;

    while (reader.read(temp, 1)) {
        if (character.count(temp)) {
            fout.put((char)(character[temp]));
            temp.reset();
        }
    }
    
    // Done.
}
