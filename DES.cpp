#include <iostream>
#include "tables.cpp"
#include <string> 
#include <bitset>
#include <stdlib.h>
#include <cstdio>

using namespace std;

//Binary Conversion
string Conv(int num)
{
    string result = "";

    while (num/2 != 0)
    {
        result = result.insert(0,to_string(num%2));
        num = num/2;
    }

    result = result.insert(0,to_string(num%2));

   while (result.size() != 4)
   {
        result.insert(0,"0");
   }
    
    return result;
}

#include <string>

string hexToBin(const string& hexString)
{
    static const char* const lut[] = {"0000", "0001", "0010", "0011",
                                      "0100", "0101", "0110", "0111",
                                      "1000", "1001", "1010", "1011",
                                      "1100", "1101", "1110", "1111"};

    string binaryString;
    binaryString.reserve(hexString.length() * 4);

    for (const char c : hexString)
    {
        if (isdigit(c))
        {
            int value = c - '0';
            binaryString.append(lut[value]);
        }
        else
        {
            int value = tolower(c) - 'a' + 10;
            binaryString.append(lut[value]);
        }
    }

    return binaryString;
}

#include <string>

std::string binToHex(const std::string& binaryString)
{
    static const char* const lut[] = {"0", "1", "2", "3",
                                      "4", "5", "6", "7",
                                      "8", "9", "A", "B",
                                      "C", "D", "E", "F"};

    std::string hexString;
    hexString.reserve((binaryString.length() + 3) / 4);

    for (std::size_t i = 0; i < binaryString.length(); i += 4)
    {
        int value = 0;
        for (std::size_t j = 0; j < 4 && i + j < binaryString.length(); ++j)
        {
            value = (value << 1) + (binaryString[i + j] - '0');
        }
        hexString += lut[value];
    }

    return hexString;
}


//Permutation
string perm(string input, int box[], int new_size)
{
    //Create a new strings and adds the character at the specified position of the input to it
    string out = "";
    for (int i = 0; i < new_size; i++)
    {
        out += input[box[i] - 1];
    }

    return out;
}

//Shift 
string Shift(string str, int x)
{
    //Gets the last chracter in the string and adds it to the front
    for (int i = 0; i < x; i++)
    {
        string temp = "";
        temp = temp + str.front();
        str.erase(str.begin());
        str.append(temp);
    }

    return str;
}

//BitSwap
string Swap(string str)
{
    string Left = str.substr(0,32);
    string Right = str.substr(32,63);

    str = Right + Left;

    return str;    
}

//Creates subkeys
string* subkeys(string str, string key[])
{
    //Split the string into 2 halves
    string C = str.substr(0,28);
    string D = str.substr(28,55);

    //Shift the halves, concatenate, then permutate for 16 rounds
    for (int i = 0; i < 16; i++)
    {
        C = Shift(C, shiftTable[i]); 
        D = Shift(D, shiftTable[i]); 
        
        string pair = C + D;

        pair = perm(pair, PC2,48);
        key[i] = pair;
    
    }
    return key;
}
//XOR
string XOR (string str1, string str2)
{
    //for the length of the string it compares and if they bits are the same its get a 0 else it gets 1
    string str;
    if (str1.size() == str2.size())
    {
        for (int i = 0; i < str1.size(); i++)
        {
            if (str1[i] == str2[i])
            {
                str = str + '0';
            }
            else
            {
                str = str + '1';
            }
        }
            return str;
    }
    else
        {
            return "0";
        }

    
}

//S Box and permutation 
string SBox (string group [])
{
    int result;
    string key;
    //Get the row and column from groups of 6 and gets the 4 digit number in binary 
    for (int i = 0; i < 8; i++)
    {
        string temp = "";
        //first two digits for row 
        temp = temp + group[i].front() + group[i].back();
        int row  = stoul(temp,0,2);

        //gets the middle digit for the columun
        int columun = stoul(group[i].substr(1,4),0,2);
        result = S[i][row][columun];

        //Converts the number and put the 4 binary digits in an array        
        group[i] = Conv(result);
        
    }

    //Puts the 8 groups of 4 binary digits into a string
    for (int i = 0; i < 8; i++)
    {
        key = key + group[i];
    }
    //Permuate the string
    key = perm(key, P, 32);
    
    return key;
}
//Round for get the cipher text
string Round(string text,string subkey)
{
    //splits the 64 bit cipher text in halfs
    string result;
    string Left = text.substr(0,32);
    string Right = text.substr(32,63);
    //Expands the Right Half and XOR with the subkey 
    string Key =  XOR(subkey,perm(Right,Exp,48));
    string groups [8];

    
    //Put the 6 digits each in 8 groups
    for(int i = 0; i < 8; i++)
    {
        groups[i] = Key.substr((i)*6 , 6 );
    }

    
    Key = SBox(groups);
    
    Left = XOR(Left,Key);

    result = result + Right + Left;

    return result;
}

string Encrypt (string Text, string Key)
{
    string subKeys[16];
    
    Key = perm(Key,PC,56);
    subkeys(Key, subKeys);

    Text = perm(Text,IP,64);

    for (int i = 0; i < 16; i++)
    {
        Text = Round(Text,subKeys[i]);
    }

    Text = Swap(Text);
    Text = perm(Text,invIP,64);

    return Text;
}


int main()
{
    string Text;
    string Key;
    
    cout << "Enter Message in Hex: ";
    cin >> Text;

    cout << "Enter Key in Hex: ";
    cin >> Key;

    
    Text = hexToBin(Text);
    Key = hexToBin(Key);

    cout << "Ciphertext: "<< binToHex(Encrypt(Text, Key));
    
    return 0;
}