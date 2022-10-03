#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <numeric>
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

void read_file(string &input);
bitset<32> rightRotate(string n, unsigned int d);
string add(string a, string b);
void compressionLoop(vector<string> words, long long int m_h[]);
void convertTextToChunksOf512(vector<string> &wordInBits, string &input, int &zeros);
void modifyMessageWords(vector<string> &wordInBits32);
string hashing(string input);
void readByLinesExperiment(int num);
void readLinesForCollisionCheck();
void testAvalancheEffect();

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

const unsigned int sha256_k[64] = 
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
  bool choice;
  string input, hash;

  cout << "Skaityti faila(1), ivesti(0):" << endl;
	cin >> choice;

  if (choice) {
    read_file(input);
  } else {
    getline(cin >> ws, input);
    hash = hashing(input);
    cout << hash << endl;
  }

  // for (int i=1; i<=10; i++)
  //   readByLinesExperiment(pow(2, i));

  // readLinesForCollisionCheck();

  // testAvalancheEffect();

  return 0;
}

void read_file(string &input) {
  string pav, hash;
  cout << "Parasykite failo pavadinima (be .txt galunes):" << endl;
  cin >> pav;
  ifstream rf("./tests/"+pav+".txt");
  stringstream my_buffer;
  my_buffer.clear();

  if (rf.is_open()) {
    my_buffer << rf.rdbuf();
    input = my_buffer.str();
    hash = hashing(input);
    cout << hash << endl;
    rf.close();
  } else {
    cout << "Nepavyko atidaryti failo." << endl;
    exit;
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

string hashing(string input) {
  int zeros = 0;
  vector<string> wordInBits, wordInBits32;
  long long int m_h[105];
  unsigned int message[8];
  stringstream ss;
  
  convertTextToChunksOf512(wordInBits, input, zeros);

  size_t a = 0;
  copy(begin(hashes), end(hashes), begin(message));
  // looping over all chunks of 512bit
  auto start = std::chrono::high_resolution_clock::now();
  for (int j = 0; j < wordInBits.size() / 64; j++) {
    wordInBits32.clear();
    wordInBits32.reserve(64);
    for (int i = 0; i<512/32; i++) {
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

    modifyMessageWords(wordInBits32);

    char letter = 'a';
    for (int i=0; i<8; i++, letter++) {
      m_h[letter] = message[i];
    }

    compressionLoop(wordInBits32, m_h);

    for (int i = 'a', j = 0; i <= 'h'; i++, j++) {
      message[j] += m_h[i];
      while (message[j] >= (unsigned long long) pow(2, 32)) {
        message[j] = message[j] % (unsigned long long) (pow(2, 32));
      }
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = end - start;
  // cout << diff.count() << endl;
  ss.clear();
  for (int i = 0; i < 8; i++) {
    ss << setfill('0') << setw(8) << hex << message[i];
  }
  return ss.str();
}

void convertTextToChunksOf512(vector<string> &wordInBits, string &input, int &zeros) {
  for (std::size_t i = 0; i < input.size(); i++) {
    wordInBits.push_back(bitset<8>(input[i]).to_string());
  }

  while ((wordInBits.size() * 8 + 1 + 64 + zeros) % 512 != 0) {
    zeros++;
  }

  if (zeros) {
    wordInBits.push_back("10000000");
    zeros -= 7;
    while (zeros) {
      wordInBits.push_back(bitset<8>().to_string());
      zeros -= 8;
    }

    string bigEndian = bitset<64>(input.size()*8).to_string();
    for (size_t j = 0; j < 8; j++) {
      string bits = bigEndian.substr(j*8, 8);
      wordInBits.push_back(bits);
    }
  }
}

void modifyMessageWords(vector<string> &wordInBits32) {
  for (int i = 16; i < 64; i++) {
    bitset<32> s0 = (rightRotate(wordInBits32.at(i-15), 7) ^ rightRotate(wordInBits32.at(i-15), 18) ^ (bitset<32>(wordInBits32.at(i-15)) >> 3));
    bitset<32> s1 = (rightRotate(wordInBits32.at(i-2), 17) ^ rightRotate(wordInBits32.at(i-2), 19) ^ (bitset<32>(wordInBits32.at(i-2)) >> 10));
    // apsisaugot reik jeigu po sudeties butu didesnis nei 32??
    bitset<40> modBinary(add(wordInBits32.at(i-16), s0.to_string()));
    while (modBinary.to_ullong() >= (unsigned long long) pow(2, 32)) {
      modBinary = modBinary.to_ullong() % (unsigned long long) (pow(2, 32));
    }
    modBinary = bitset<40>(add(modBinary.to_string(), wordInBits32.at(i-7)));
    while (modBinary.to_ullong() >= (unsigned long long) pow(2, 32)) {
      modBinary = modBinary.to_ullong() % (unsigned long long) (pow(2, 32));
    }
    modBinary = bitset<40>(add(modBinary.to_string(), s1.to_string()));

    while (modBinary.to_ullong() >= (unsigned long long) pow(2, 32)) {
      modBinary = modBinary.to_ullong() % (unsigned long long) (pow(2, 32));
    }

    // convertinam atgal i 32 bitus ir issisaugom
    wordInBits32.at(i) = bitset<32>((modBinary << 8).to_string()).to_string();
  }
}

void compressionLoop(vector<string> words, long long int m_h[]) {
  for (int i=0; i<64; i++) {
    bitset<32> s1 = (((m_h['e'] >> 6)|(m_h['e'] << (32 - 6))) ^ ((m_h['e'] >> 11)|(m_h['e'] << (32 - 11))) ^ ((m_h['e'] >> 25)|(m_h['e'] << (32 - 25))));
    bitset<32> ch = (m_h['e'] & m_h['f']) ^ (~m_h['e'] & m_h['g']);
    bitset<40> temp1(add(bitset<32>(m_h['h']).to_string(), s1.to_string()));
    while (temp1.to_ullong() >= (unsigned long long) pow(2, 32)) {
      temp1 = temp1.to_ullong() % (unsigned long long) (pow(2, 32));
    }
    temp1 = bitset<40>(add(temp1.to_string(), ch.to_string()));
    while (temp1.to_ullong() >= (unsigned long long) pow(2, 32)) {
      temp1 = temp1.to_ullong() % (unsigned long long) (pow(2, 32));
    }
    temp1 = bitset<40>(add(temp1.to_string(), words.at(i)));
    while (temp1.to_ullong() >= (unsigned long long) pow(2, 32)) {
      temp1 = temp1.to_ullong() % (unsigned long long) (pow(2, 32));
    }
    temp1 = bitset<40>(add(temp1.to_string(), bitset<32>(sha256_k[i]).to_string()));
    while (temp1.to_ullong() >= (unsigned long long) pow(2, 32)) {
      temp1 = temp1.to_ullong() % (unsigned long long) (pow(2, 32));
    }
    bitset<32> s0 = (((m_h['a'] >> 2)|(m_h['a'] << (32 - 2))) ^ ((m_h['a'] >> 13)|(m_h['a'] << (32 - 13))) ^ ((m_h['a'] >> 22)|(m_h['a'] << (32 - 22))));
    bitset<32> maj = (m_h['a'] & m_h['b']) ^ (m_h['a'] & m_h['c']) ^ (m_h['b'] & m_h['c']);
    bitset<40> temp2(add(s0.to_string(), maj.to_string()));
    while (temp2.to_ullong() >= (unsigned long long) pow(2, 32)) {
      temp2 = temp2.to_ullong() % (unsigned long long) (pow(2, 32));
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

void readByLinesExperiment(int num) {
  int a=0;
  string line, text="", hash;
  ifstream rf("./tests/konstitucija.txt");
  stringstream my_buffer;

  if (rf.is_open()) {
    my_buffer << rf.rdbuf();
    rf.close();
  }
  while(my_buffer && a < num) {
    getline(my_buffer, line, '\n');
    if (a != 0) {
      text += '\n';
    }
    text += line;
    a++;
  }
  hash = hashing(text);
  cout << hash << endl;
}

void readLinesForCollisionCheck() {
  string line, fileName="check25k1000Symbols.txt", hash="";
  int collisions=0;
  vector<string> hashes;
  hashes.reserve(25000);
  ifstream rf("./tests/" + fileName);
  stringstream my_buffer;

  if (rf.is_open()) {
    my_buffer << rf.rdbuf();
    rf.close();
  }
  while(!my_buffer.eof()) {
    getline(my_buffer, line);
    hash = hashing(line);
    cout << hash << endl;
    hashes.push_back(hash);

    for (int i=0; i<hashes.size() - 1; i++) {
      if (hashes.at(i) == hash)
        collisions++;
    }
  }
  cout << "Collisions count: " << collisions << endl;
}

void testAvalancheEffect() {
  string text, fileName="porosStringu.txt", hash;
  vector<string> hashes1, hashes2;
  vector<double> skirtumai, skirtumai2;
  ifstream rf("./tests/" + fileName);
  stringstream my_buffer;

  if (rf.is_open()) {
    my_buffer << rf.rdbuf();
    rf.close();
  }
  while(!my_buffer.eof()) {
    getline(my_buffer, text, ' ');
    hash = hashing(text);
    hashes1.push_back(hash);
    getline(my_buffer, text, ' ');
    hash = hashing(text);
    hashes2.push_back(hash);
  }

  for (int s = 0; s < hashes1.size(); s++) {
    bitset<512> b1;
    for (int i = 0; i < 64; ++i) {
        char c = hashes1.at(s).at(i);
        for (int j = 7; j >= 0 && c; --j) {
            if (c & 0x1) {
                b1.set(8 * i + j);
            }
            c >>= 1;
        }
    }

    bitset<512> b2;
    for (int i = 0; i < 64; ++i) {
        char c = hashes2.at(s).at(i);
        for (int j = 7; j >= 0 && c; --j) {
            if (c & 0x1) {
                b2.set(8 * i + j);
            }
            c >>= 1;
        }
    }

    double sutapimai = 0, sutapimai2 = 0;

    for(int j = 0; j < 512; j++) if(b1.test(j) != b2.test(j)) sutapimai++;
    for(int j = 0; j < 64; j++) if(hashes1.at(s).at(j) != hashes2.at(s).at(j)) sutapimai2++;

    skirtumai.push_back(sutapimai / 512 * 100);
    skirtumai2.push_back(sutapimai2 / 64 * 100);
  }
  cout << "Bitu lygmenyje:" << endl;
  cout << "Min skirtumo reiksme = " << *min_element(skirtumai.begin(), skirtumai.end()) << "%" << endl;
  cout << "Max skirtumo reiksme = " << *max_element(skirtumai.begin(), skirtumai.end()) << "%" << endl;
  cout << "Vidurkines skirtumo reiksme = " << accumulate( skirtumai.begin(), skirtumai.end(), 0.0) / skirtumai.size() << "%" << endl;
  cout << "Hash'o lygmenyje:" << endl;
  cout << "Min skirtumo reiksme = " << *min_element(skirtumai2.begin(), skirtumai2.end()) << "%" << endl;
  cout << "Max skirtumo reiksme = " << *max_element(skirtumai2.begin(), skirtumai2.end()) << "%" << endl;
  cout << "Vidurkines skirtumo reiksme = " << accumulate( skirtumai2.begin(), skirtumai2.end(), 0.0) / skirtumai2.size() << "%" << endl;
}