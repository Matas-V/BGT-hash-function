#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <bit>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <bitset>
using namespace std;

void read_file();
bitset<32> rightRotate(string n, unsigned int d);
string addTwoBinaryNum(string bn1, string bn2);
string add(string a, string b);
void compressionLoop(vector<string> words, long long int m_h[]);

const string test = "hello world";
const unsigned int hashes[8] = {
  0x6a09e667,
  0xbb67ae85,
  0x3c6ef372,
  0xa54ff53a,
  0x510e527f,
  0x9b05688c,
  0x1f83d9ab,
  0x5be0cd19
};

const unsigned int sha256_k[64] = //UL = uint32
        {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

int main() {
  size_t a = 0;
  bool choice;
	string input = test, line = "";
  vector<string> wordInBits;
  vector<string> wordInBits32;

  for (std::size_t i = 0; i < input.size(); i++) {
    wordInBits.push_back(bitset<8>(input[i]).to_string());
  }
  wordInBits.push_back("10000000");

  // Pad with 0’s until data is a multiple of 512, less 64 bits (448 bits in our case):
  while (wordInBits.size() != 63) {
    wordInBits.push_back(bitset<8>().to_string());
  }
  // Append 64 bits to the end, where the 64 bits are a big-endian integer representing the length of the original input in binary.
  wordInBits.push_back(bitset<8>(input.size() * 8).to_string());
  // Copy the input data from step 1 into a new array where each entry is a 32-bit word:
  for (int i = 0; i < 512/32; i++) {
    string newSet = "";
    for (size_t y = a; y < a + 4; y++) {
      newSet += wordInBits.at(y);
    }
    wordInBits32.push_back(newSet);
    a += 4;
  }

  while (wordInBits32.size() != 64) {
    wordInBits32.push_back(bitset<32>().to_string());
  }

  for (int i = 16; i < 64; i++) {
    bitset<32> s0 = (rightRotate(wordInBits32.at(i-15), 7) ^ rightRotate(wordInBits32.at(i-15), 18) ^ (bitset<32>(wordInBits32.at(i-15)) >> 3));
    bitset<32> s1 = (rightRotate(wordInBits32.at(i-2), 17) ^ rightRotate(wordInBits32.at(i-2), 19) ^ (bitset<32>(wordInBits32.at(i-2)) >> 10));
    // apsisaugot reik jeigu po sudeties butu didesnis nei 32??
    bitset<34> modBinary(add(wordInBits32.at(i-16), s0.to_string()));
    modBinary = bitset<34>(add(modBinary.to_string(), wordInBits32.at(i-7)));
    modBinary = bitset<34>(add(modBinary.to_string(), s1.to_string()));

    while (modBinary.to_ullong() >= (unsigned long long) pow(2, 32)) {
      modBinary = modBinary.to_ullong() % (unsigned long long) (pow(2, 32));
    }

    wordInBits32.at(i) = modBinary.to_string();
  }

  long long int m_h[105];
  compressionLoop(wordInBits32, m_h);
  long long int message[8];

  for (int i = 'a', j = 0; i <= 'h'; i++, j++) {
    message[j] = bitset<32>(add(bitset<32>(hashes[j]).to_string(), bitset<32>(m_h[i]).to_string())).to_ullong();
    while (message[j] >= (unsigned long long) pow(2, 32)) {
      message[j] = message[j] % (unsigned long long) (pow(2, 32));
    }
    cout << hex << message[j];
  }

	// cout << "Skaityti faila(1), ivesti(0):" << endl;
	// cin >> choice;

  // if (choice) {
  //   read_file();
  // } else {
  //   cin >> input;
  //   cout << input << endl;
  // }
  return 0;
}

void read_file() {
  ifstream rf("message.txt");
  string line;
  stringstream my_buffer;

  my_buffer << rf.rdbuf();

  if (rf.is_open()) {
    rf.close();
    while(!my_buffer.eof()) {
      my_buffer >> line;
      cout << line << endl;
    }
  } else {
    cout << "Nepavyko atidaryti failo." << endl;
  }
}

/*Function to right rotate n by d bits*/
bitset<32> rightRotate(string n, unsigned int d) {
  /* In n>>d, first d bits are 0.
  To put last 3 bits of at
  first, do bitwise or of n>>d
  with n <<(INT_BITS - d) */
  return (bitset<32>(n) >> d)|(bitset<32>(n) << (32 - d));
}

string add(string a, string b) {
  string result = "";
  int temp = 0;
  int size_a = a.size() - 1;
  int size_b = b.size() - 1;
  while (size_a >= 0 || size_b >= 0 || temp == 1){
      temp += ((size_a >= 0)? a[size_a] - '0': 0);
      temp += ((size_b >= 0)? b[size_b] - '0': 0);
      result = char(temp % 2 + '0') + result;
      temp /= 2;
      size_a--; size_b--;
  }
  return result;
}

void compressionLoop(vector<string> words, long long int m_h[]) {
  char letter = 'a';
  for (int i=0; i<8; i++, letter++) {
    m_h[letter] = hashes[i];
  }

  for (int i=0; i <64; i++) {
    bitset<32> s1 = ((m_h['e'] >> 6)|(m_h['e'] << (32 - 6)) ^ (m_h['e'] >> 11)|(m_h['e'] << (32 - 11)) ^ (m_h['e'] >> 25)|(m_h['e'] << (32 - 25)));
    bitset<32> ch = (m_h['e'] & m_h['f']) ^ (~m_h['e'] & m_h['g']);
    bitset<32> temp1(add(bitset<32>(m_h['h']).to_string(), s1.to_string()));
    temp1 = bitset<32>(add(temp1.to_string(), ch.to_string()));
    temp1 = bitset<32>(add(temp1.to_string(), words.at(i)));
    temp1 = bitset<32>(add(temp1.to_string(), bitset<32>(sha256_k[i]).to_string()));
    while (temp1.to_ullong() >= (unsigned long long) pow(2, 32)) {
      temp1 = temp1.to_ullong() % (unsigned long long) (pow(2, 32));
    }
    bitset<32> s0 = ((m_h['a'] >> 2)|(m_h['a'] << (32 - 2)) ^ (m_h['a'] >> 13)|(m_h['a'] << (32 - 13)) ^ (m_h['a'] >> 22)|(m_h['a'] << (32 - 22)));
    bitset<32> maj = (m_h['a'] & m_h['b']) ^ (m_h['a'] & m_h['c']) ^ (m_h['b'] & m_h['c']);
    bitset<32> temp2(add(s0.to_string(), maj.to_string()));
    while (temp2.to_ullong() >= (unsigned long long) pow(2, 32)) {
      temp1 = temp2.to_ullong() % (unsigned long long) (pow(2, 32));
    }
    m_h['h'] = m_h['g'];
    m_h['g'] = m_h['f'];
    m_h['f'] = m_h['e'];
    m_h['e'] = m_h['d'] + temp1.to_ullong();
    while (m_h['e'] >= (unsigned long long) pow(2, 32)) {
      m_h['e'] = m_h['e'] % (unsigned long long) (pow(2, 32));
    }
    m_h['d'] = m_h['c'];
    m_h['c'] = m_h['b'];
    m_h['b'] = m_h['a'];
    m_h['a'] = temp1.to_ullong() + temp2.to_ullong();
    while (m_h['a'] >= (unsigned long long) pow(2, 32)) {
      m_h['a'] = m_h['a'] % (unsigned long long) (pow(2, 32));
    }
  }
}