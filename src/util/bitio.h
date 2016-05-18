#ifndef BITIO_H
#define BITIO_H

#include <bitset>

// Buffered bit writer.
//
// Note
//   It writes to file.
//   It use an unsigned character as a buffer.
//
class BitWriter {
private:
    unsigned char  c;
    size_t         c_len;
    std::ofstream& fout;

public:
    // Construct with file.
    //
    // Note
    //   _fout has to be opened in out and binary ("out | binary") mode.
    //
    explicit BitWriter(std::ofstream& _fout): c(0), c_len(0), fout(_fout) {}
    
    // Write bit to file.
    //
    // Example
    //   start
    //     bits: [0 0 1 1 0 0 0 1], bit_len = 5 (LSB)
    //     char: [0 0 0 0 0 0 0 0]
    //   concat
    //     char: [0 0 0 0 0 0 0 0][1 0 0 0 1]
    //   shift
    //     char: [0 0 0 1 0 0 0 1]
    //   
    //   start
    //     bits: [0 0 1 1 0 1 0 1], bit_len = 6 (LSB)
    //     char: [0 0 0 0 0 0 1 0]
    //   concat
    //     char: [0 0 0 0 0 0 1 0][1 1 0 1 0 1]
    //   shift
    //     char: [1 0 1 1 0 1 0 1]
    //     
    template <size_t n>
    void write(const std::bitset<n>& bits, size_t bit_len) {
        // Store at c first, output only if full.
        // Back in front out. out <-- [0 0 0 0 0 0 0 0] <-- in
        while (bit_len > 0) {
            c <<= 1;
            c |= (bits >> (bit_len - 1))[0];
            c_len += 1;

            // If c has been full, output.
            if (c_len == sizeof(unsigned char)*8) {
                fout.put(c);
                c_len = 0;
                c = 0;
            }
            bit_len -= 1;
        }
    }

    // Write bit to file (unsigned int overload).
    void write(unsigned int bits, size_t bit_len) {
        write((std::bitset<sizeof(unsigned int)*8>(bits)), bit_len);
    }

    // Flush remaining bits in buffer.
    //
    // Note
    //   Buffer is reset after that.
    //
    void flush() {
        if (c_len > 0) {
            c <<= sizeof(unsigned char)*8 - c_len;
            fout.put(c);
            c_len = 0;
            c = 0;
        }
    }
};

// Bit reader.
//
// Note
//   It read to bitset.
//   It use an unsigned character as a buffer.
//
class BitReader {
private:
    unsigned char  c;
    int            bit_left;
    std::ifstream& fin;

public:
    // Construct with file.
    //
    // Note
    //   _fin has to be opened in in and binary ("in | binary") mode.
    //
    explicit BitReader(std::ifstream& _fin): c(0), bit_left(0), fin(_fin) {}

    // Read bit from file.
    //
    // Output
    //   True if read successfull.
    //   False otherwise (e.g. EOF)
    //   bits: shift k bits into it from LSB.
    //
    template <size_t n>
    bool read(std::bitset<n>& bits, size_t bit_len) {
        for (size_t i = 0; i < bit_len; ++i) {
            // If c is empty, read a byte from file.
            if (bit_left == 0) {
                char temp;
                if (!fin.get(temp)) {
                    return false;
                }
                c = temp;
                bit_left = 8;
            }
            
            // Fetch from c to temp.
            bits <<= 1;
            bits |= c >> (sizeof(unsigned char)*8 - 1);
            c <<= 1;
            bit_left -= 1;
        }

        return true;
    }

    // Read bit from file (unsigned int overload).
    bool read(unsigned int& bits, size_t bit_len) {
        std::bitset<sizeof(unsigned int)*8> temp(bits);
        bool ret = read(temp, bit_len);
        bits = temp.to_ulong();
        return ret;
    }
};

#endif
