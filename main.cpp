#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

void read_file();

int main() {
  bool choice;
	string input, line = "";
	cout << "Skaityti faila(1), ivesti(0):" << endl;
	cin >> choice;

  if (choice) {
    read_file();
  } else {
    cin >> input;
    cout << input << endl;
  }
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