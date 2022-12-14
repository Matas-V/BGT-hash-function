## SHA-256 hash generatorius

# Programos paleidimas
- git clone https://github.com/Matas-V/BGT-hash-function.git
- Atsidaryti projektą VS code aplinkoje
- Konsolėje (CMD) parašyti komandą *make main*
- Konsolėje (CMD) paleisti sukompiliuotą main.exe tipo programą su komanda *main*

# Maišos funkcijos pseudo kodas
<pre>
INPUT text;
// Initialize hash values
unsigned int h[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
vector<string> textInBits;
FOR EACH c in text:
  textInBits.push_back(bitset<8>(c).to_string());
END FOR EACH

int countHowMuchToAddZeros = 0;
WHILE (textInBits.size() * 8 + 1 + 64 + zeros) % 512 != 0:
  countHowMuchToAddZeros++;
END WHILE

IF countHowMuchToAddZeros:
  textInBits.push_back("10000000");
  countHowMuchToAddZeros -= 7;
  WHILE zeros:
    textInBits.push_back(bitset<8>().to_string());
    countHowMuchToAddZeros -= 8;
  END WHILE
  
  string bigEndian = bitset<64>(input.size()*8).to_string();
  FOR int i=0 i<8 i++:
    textInBits.push_back(bigEndian.substr(j*8, 8));
  END FOR
END IF

vector<string> textInBits32;
size_t a = 0;
FOR int j=0; j<textInBits.size() / 64; j++:
  textInBits32.clear();
  textInBits32.reserve(64);
  FOR int i = 0; i<512/32; i++:
    string newBitsSet = "";
    FOR size_t y = a; y < a + 4; y++:
      newBitsSet += textInBits.at(y);
    END FOR
    textInBits32.push_back(newBitsSet);
    a += 4;
  END FOR
  WHILE textInBits32.size() != 64:
    textInBits32.push_back(bitset<32>().to_string());
  END WHILE

  FOR int i=16; i<64; i++:
    s0 = (textInBits32[i-15] rightrotate  7) xor (textInBits32[i-15] rightrotate 18) xor (textInBits32[i-15] rightshift  3);
    s1 = (textInBits32[i- 2] rightrotate 17) xor (textInBits32[i- 2] rightrotate 19) xor (textInBits32[i- 2] rightshift 10);
    textInBits32[i] = textInBits32[i-16] + s0 + textInBits32[i-7] + s1;
  END FOR
  // Initialize working variables to current hash value:
    a = h[0];
    b = h[1];
    c = h[2];
    d = h[3];
    e = h[4];
    f = h[5];
    g = h[6];
    h = h[7];
  
  // Compression loop
  FOR int i=0; i<64; i++:
    S1 = (e rightrotate 6) xor (e rightrotate 11) xor (e rightrotate 25);
    ch = (e and f) xor ((not e) and g);
  // k[i] is the round constant
    temp1 = h + S1 + ch + k[i] + textInBits32[i];
    S0 = (a rightrotate 2) xor (a rightrotate 13) xor (a rightrotate 22);
    maj = (a and b) xor (a and c) xor (b and c);
    temp2 = S0 + maj;
  
    h = g;
    g = f;
    f = e;
    e = d + temp1;
    d = c;
    c = b;
    b = a;
    a = temp1 + temp2;
  END FOR
  h[0] += a;
  h[1] += b;
  h[2] += c;
  h[3] += d;
  h[4] += e;
  h[5] += f;
  h[6] += g;
  h[7] += h;
END FOR

string msg = "";
FOR int i=0; i<8; i++:
  msg += hex << h[i];
</pre>

# Eksperimentinė analizė:
1. Du failai sudaryti tik iš vieno, tačiau skirtingo, simbolio:
- File - oneSymbol1.txt, Hash - ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb
- File - oneSymbol2.txt, Hash - cd0aa9856147b6c5b4ff2b7dfee5da20aa38253099ef1b4a64aced233c9afe29
2. Du failai sudaryti iš daugiau nei 1000 atsitiktinai sugeneruotų simbolių:
- File - 1001Symbols1.txt, Hash - aa6255cac1962e988c9514e83cc80355c86cc714e424b3aa349ad222d03da434
- File - 1001Symbols2.txt, Hash - 9ca449bcee14463ce6ec950cf674d7e1e6cbb01110753ba8a665c99c9b75764e
3. Du failai sudaryti iš daugiau nei 1000 simbolių, bet skiriasi vienu nuo kito tik vienu simboliu:
- File - 1001Symbols3.txt, Hash - 9072798fb3bd3ae0a1f7c0b153104b5d459f9744ab80861c443e706e8a08719b
- File - 1001Symbols4.txt, Hash - 709f4ff0218f4f02e20a56bb77733734e3d427e2e01a33608be5529a731aab36
4. Tuščias failas:
- File - emptyFile.txt, Hash - e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
5. Efektyvumas:
<br/>![image](https://user-images.githubusercontent.com/80633152/193473910-a6faf464-0e49-4fd1-aec9-1be8ffd5b174.png)
6. 100 000 atsitiktinių simbolių eilučių porų:
- 25 000 porų, kurių ilgis 10 simbolių, kolizijų skaičius - 0.
- 25 000 porų, kurių ilgis 100 simbolių, kolizijų skaičius - 0.
- 25 000 porų, kurių ilgis 500 simbolių, kolizijų skaičius - 0.
- 25 000 porų, kurių ilgis 1000 simbolių, kolizijų skaičius - 0.
7. 100 000 atsitiktinių simbolių eilučių porų, 32 simbolių eilučių ilgiu, juos skiria tik vienas simbolis. Įvertinamas gautų hash'ų procentinis "skirtingumas" bitų lygmenyje:
- Bitų lygmenyje:
  - Minimali skirtingumo reikšmė: **0%**
  - Maksimali skirtingumo reikšmė: **43.4%**
  - Vidurinė skirtingumo reikšmė: **33%**
- Hexo lygmenyje:
  - Minimali skirtingumo reikšmė: **0%**
  - Maksimali skirtingumo reikšmė: **100%**
  - Vidurinė skirtingumo reikšmė: **93.8%**

# Išvados:
  - Algoritmas yra greitas
  - Atstatyti išėjimą į įėjimą neįmanoma
  - Funkcija yra deterministinė
  - Minimalus įėjimo pakeitimai absoliučiai pakeičia išėjimą
