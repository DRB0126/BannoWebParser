// Programmer: David Bippes                                     Date: March 2017
// File: bannoMain.cpp

#include <iostream>
#include <string>
#include <fstream>
#include <curl/curl.h>
using namespace std;

// Global constants
const int maxWordLength = 100;

// Classes
class distinctChar
{
  private:
  	char currChar;
  	int currCharCount;

  public:
    distinctChar()
    {
      currChar = '\0';
      currCharCount = 0;
    }

    ~distinctChar() {}

    char getChar()
    {
    	return currChar;
    }

    int getCount()
    {
    	return currCharCount;
    }

    void setChar(char theChar)
    {
      currChar = theChar;
      return;
    }

    void setCount(int number)
    {
      currCharCount = currCharCount + number;
      return;
    }
};

class distinctCharArray
{
  private:
    int maxSize;
    int size;

  public:
    distinctChar* theArray;

    distinctCharArray()
    {
      maxSize = 0;
      size = 0;
      theArray = new distinctChar[maxSize];
    }

    ~distinctCharArray() {}

    int getSize()
    {
      return size;
    }

    // checks to see if the character is in the array
    // returns the index if character exists
    // returns -1 if character does not exist
    int check(char theChar)
    {
      for (int i = 0; i < size; i++)
      {
        if (theArray[i].getChar() == theChar)
          return i;
      }

      // character has not been found
      return -1;
    }

    // checks for and adds new characters to the array
    void add(char theChar)
    {
      // if the character has occurred before . . .
      int index = check(theChar);
      if (index != -1)
      {
        // if the current array is full . . .
        if (size >= (maxSize - 1))
        {
          maxSize = size * 2;

          distinctChar* tempCharArr = new distinctChar[maxSize];

          for (int i = 0; i < size; i++)
          {
            tempCharArr[i].setChar(theArray[i].getChar());
            tempCharArr[i].setCount(theArray[i].getCount());
          }

          delete[] theArray;
          theArray = new distinctChar[maxSize];
          for (int i = 0; i < size; i++)
          {
            theArray[i].setChar(tempCharArr[i].getChar());
            theArray[i].setCount(tempCharArr[i].getCount());
          }
          delete[] tempCharArr;
        }

        // increments the specific character counter
        theArray[index].setCount(1); // simply adds one to the count
      }

      // if the character has not occurred before . . .
      else
      {
        size++;

        // if the current array is full . . .
        if (size >= (maxSize - 1))
        {
          maxSize = size * 2;

          distinctChar* tempCharArr = new distinctChar[maxSize];

          for (int i = 0; i < size; i++)
          {
            tempCharArr[i].setChar(theArray[i].getChar());
            tempCharArr[i].setCount(theArray[i].getCount());
          }

          delete[] theArray;
          theArray = new distinctChar[maxSize];
          for (int i = 0; i < size; i++)
          {
            theArray[i].setChar(tempCharArr[i].getChar());
            theArray[i].setCount(tempCharArr[i].getCount());
          }
          delete[] tempCharArr;
        }

        // adds the character and increments the counter
        theArray[size].setChar(theChar);
        theArray[size].setCount(1); // simply adds one to the count
      }

      return;
    }
};

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

int main()
{
  const string inputFile = "index.html",
         outputFile = "output.txt",
         webAddress = "https://banno.com/";

  // accessing the webpage
  //--------------------------------------
  CURL* curl_handle;
  static const char* pagefilename = inputFile.c_str();
  FILE* pagefile;
  const char* theURL = webAddress.c_str();

  // preparing curl library
  curl_global_init(CURL_GLOBAL_ALL);
 
  // intializing the curl session
  curl_handle = curl_easy_init();
 
  // setting the URL to fetch
  curl_easy_setopt(curl_handle, CURLOPT_URL, theURL);
 
  // opening the file
  pagefile = fopen(pagefilename, "wb");
  if (pagefile)
  {
    // writing the page body to this file
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
 
    // requesting data
    curl_easy_perform(curl_handle);
 
    // closing the file
    fclose(pagefile);
  }
 
  // cleaning up
  curl_easy_cleanup(curl_handle);
  //--------------------------------------

  // variables for file reading
  ifstream fin;
  ofstream fout;
  int connectCounter;

  // variables for word parsing
  char currentWord[maxWordLength];
  distinctChar currChar[maxWordLength];
  int length;

  // variables for program functioning
  int PNGcounter;
  int FIcounter;
  bool FIfound;
  bool twitterFound;
  char twitterHandle[25];
  int num1Char,
      num2Char,
      num3Char;
  char firstChar,
       secondChar,
       thirdChar;
  distinctCharArray* characters = new distinctCharArray;

  // opening input file for reading
  connectCounter = 0;
  do
  {
    fin.clear();
    fin.open(inputFile);
    connectCounter++;
  } while (!fin && connectCounter <= 10);

  // preparing output file for writing
  do
  {
    fout.clear();
    fout.open(outputFile);
  } while (!fout);

  // initializing relevant variables
  PNGcounter = 0;
  FIcounter = 0;
  length = 0;
  FIfound = false;
  twitterFound = false;

  while (!fin.eof())
  {
    fin >> currentWord;

    length = strlen(currentWord);

    // counting png images
    //--------------------------------------
    // Per the instructions, this block finds the number of .png images *in
    //   the html*, not necessarily the ones displayed on the page.
    if ((currentWord[length - 2] == 'g' && // accounting for pngs surrounded
        currentWord[length - 3] == 'n' &&  //   by quotation marks
        currentWord[length - 4] == 'p' &&
        currentWord[length - 5] == '.')
        ||
        (currentWord[length - 3] == 'g' && // accounting for pngs surrounded
        currentWord[length - 4] == 'n' &&  //   by quotation marks and angle
        currentWord[length - 5] == 'p' &&  //   brackets at the end of a tag
        currentWord[length - 6] == '.'))

    {
      PNGcounter++;
    }
    //--------------------------------------

    // finding the Banno Twitter handle
    //--------------------------------------
    if (!(strcmp(currentWord, "name=\"twitter:site\"")))
    {
      twitterFound = true;
    }

    if (twitterFound &&
        currentWord[0] == 'c' &&
        currentWord[1] == 'o' &&
        currentWord[2] == 'n' &&
        currentWord[3] == 't' &&
        currentWord[4] == 'e' &&
        currentWord[5] == 'n' &&
        currentWord[6] == 't')
    {
      int nameSize = 0;
      for(int i = 9; currentWord[i] != '\"'; i++)
      {
        twitterHandle[i - 9] = currentWord[i];
        nameSize++;
      }

      twitterHandle[nameSize - 1] = '\0';

      twitterFound = false;
    }
    //--------------------------------------

    // searching for "financial institution"
    //--------------------------------------
    if (!(strcmp(currentWord, "financial")))
    {
      FIfound = true;
    }

    if (FIfound &&
        currentWord[0] == 'i' &&
        currentWord[1] == 'n' &&
        currentWord[2] == 's' &&
        currentWord[3] == 't' &&
        currentWord[4] == 'i' &&
        currentWord[5] == 't' &&
        currentWord[6] == 'u' &&
        currentWord[7] == 't' &&
        currentWord[8] == 'i' &&
        currentWord[9] == 'o' &&
        currentWord[10] == 'n')
    {
      FIcounter++;
      FIfound = false;
    }
    //--------------------------------------

    // gathering alphanumerical characters
    //--------------------------------------
    for (int i = 0; i < length; i++)
    {
      characters->add(currentWord[i]);
    }
    //--------------------------------------
  }

  // note: the character counters are case sensitive
  // finding most frequent character
  //--------------------------------------
  num1Char = 0;
  firstChar = '\0';
  for (int i = 0; i < characters->getSize(); i++)
  {
    if (characters->theArray[i].getCount() > num1Char &&
        characters->theArray[i].getChar() != '\0')
    {
      num1Char = characters->theArray[i].getCount();
      firstChar = characters->theArray[i].getChar();
    }
  }
  //--------------------------------------

  // finding second most frequent character
  //--------------------------------------
  num2Char = 0;
  secondChar = '\0';
  for (int i = 0; i < characters->getSize(); i++)
  {
    if (characters->theArray[i].getCount() > num2Char &&
        characters->theArray[i].getChar() != '\0' &&
        characters->theArray[i].getChar() != firstChar)
    {
      num2Char = characters->theArray[i].getCount();
      secondChar = characters->theArray[i].getChar();
    }
  }
  //--------------------------------------

  // finding third most frequent character
  //--------------------------------------
  num3Char = 0;
  thirdChar = '\0';
  for (int i = 0; i < characters->getSize(); i++)
  {
    if (characters->theArray[i].getCount() > num3Char &&
        characters->theArray[i].getChar() != '\0' &&
        characters->theArray[i].getChar() != firstChar &&
        characters->theArray[i].getChar() != secondChar)
    {
      num3Char = characters->theArray[i].getCount();
      thirdChar = characters->theArray[i].getChar();
    }
  }
  //--------------------------------------

  // printing results
  //--------------------------------------
  fout << firstChar << " appears " << num1Char << " times." << endl;
  fout << secondChar << " appears " << num2Char << " times." << endl;
  fout << thirdChar << " appears " << num3Char << " times." << endl;
  fout << webAddress << " contains " << PNGcounter << " png images." << endl;
  fout << "Banno's Twitter handle is " << twitterHandle << "." << endl;
  fout << "The term \"financial institution\" occurs " << FIcounter
       << " times." << endl;
  //--------------------------------------

  cout << "Look for results in the " << outputFile << " file. \n"
       << "Thank you for using this program!" << endl;

  fin.close();
  fout.close();

  delete characters;

  return 0;
}
