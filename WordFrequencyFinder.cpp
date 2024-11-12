/*
 * Name: Evan Shumaker
 * Description: This program reads a text and creates an array of strings with 1 word per entry.
 * It then cleans the string, making all letters lowercase and removing anything that isn't a letter or
 * number. It then generates arrays of structs for all the unigrams and bigrams along with their frequency
 * within the text. It also has an interactive terminal menu to search for unigrams or bigrams and returns
 * their frequency if they exist. Also comes with save and index features.
 * Input: A text file to read through.
 * Output: Sorted lists of unigrams and bigrams within the text
 */

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

using namespace std;

int const ArrayMax = 40000; //Size of the array to store strings into

/*
 * Unigram: An individual word and its frequency
 * static members: word: a single word read from the text.
 *                 frequency: The number of times the word occurs in the text
 */
struct Unigram{
    string word;
    int frequency;
};

/*
 * Bigram: A set of two words next to each other and their frequency
 * static members: word1: The first word in the set from the text.
 *                 word2: The second word in the set from the text.
 *                 frequency: The number of times the words occur in the text
 */
struct Bigram{
    string word1;
    string word2;
    int frequency;
};

//Function prototypes
bool isAlphabetic(string);
int readdata(string, string[], int&);
int getlongestword(string[], int);
void cleanup(string[], int);
int inList(string, Unigram[], int);
int inList(string , string, Bigram[], int);
void sortgms(Unigram[], int);
void sortgms(Bigram[], int);
void generate_unigrams(Unigram[], int&, string[], int);
void generate_bigrams(Bigram[], int&, string[], int);
void write1gmslog(string, Unigram[], int);
void write2gmslog(string, Bigram[], int);

/*
 * makeLowercase: Makes any string passed all lowercase.
 * parameters: The word to be made lowercase
 * return value: void
 */
void makeLowercase(string& input){
    for(size_t i = 0;i<input.size();i++){
        input[i] = tolower(input[i]);
    }
}


/**************************** Main ****************************/
/*
 * main: main command loop.
 * parameters: argc: the number of arguments. argv: contains the file name to read
 * return value: int: returns 1 if there is an error, otherwise 0
 */
int main(int argc, char** argv){
    //first must read text and insert into the fixed array of strings
    //exits if program is used incorrectly
    if(argc != 2){
        cout << "Error: Usage = \n ./a.out <filename>" << endl;
        return 1;
    }
    
    cout << "Array size: "<< ArrayMax << endl;
    
    //we call all the necessary functions before opening the command menu (read cleanup, sort, ect.)
    string words[ArrayMax];
    int wordCount = 0;
    
    readdata(argv[1], words, wordCount);
    cout << "The word count is: " << wordCount << endl;
    
    cleanup(words, ArrayMax);
    int longestIndex = getlongestword(words, wordCount);
    cout << "Longest word: '"<< words[longestIndex] << "' " << endl;
    
    Unigram unis[ArrayMax];
    int unigramCount;
    generate_unigrams(unis, unigramCount, words, wordCount);
    
    Bigram bis[ArrayMax];
    int bigramCount;
    generate_bigrams(bis, bigramCount, words, wordCount);
    
    //The main command loop
    while(true){
        string input;
        cout<<"Commands\n";
        cout<<"Unigram: search for unigram\n";
        cout<<"Bigram : search for bigram\n";
        cout<<"Save   : save 1gms.txt and 2gms.txt\n";
        cout<<"Index  : print index of unigram or bigram\n";
        cout<<"Quit/q : terminate program\n";
        cin >> input;
        
        //executing the command the user wants
        makeLowercase(input);
        if(input == "unigram"){
            cout << "Enter word to search: ";
            cin >> input;
            makeLowercase(input);
            cout << "Searching for " << input << "...\n";
            int index = inList(input, unis, unigramCount);
            if(index != -1){
                cout << "Unigram found at index: " << index << " with frequency of "
                << unis[index].frequency << endl;
            }
            else{
                cout << "Couldn't find " << input << endl;
            }
        }
        else if(input == "bigram"){
            string input1, input2;
            cout << "Enter Word1 to Search: ";
            cin >> input1;
            makeLowercase(input1);
            
            cout << "Enter Word2 to Search: ";
            cin >> input2;
            makeLowercase(input2);
            
            cout << "Searching for "<< input1 << "\t" << input2 << endl;
            int index = inList(input1, input2, bis, bigramCount);
            if(index != -1){
                cout << "Bigram found at index: " << index << " with frequency of "
                << bis[index].frequency << endl;
            }
            else{
                cout << "Couldn't find " << input1 << "\t" << input2 << endl;
            }
        }
        else if(input == "save"){
            write1gmslog("1gms.txt", unis, unigramCount);
            write2gmslog("2gms.txt", bis, bigramCount);
            cout << "Writing log to 1gms.txt and 2gms.txt\n";
        }
        else if(input == "quit" || input == "q"){
            cout<<"Goodbye!\n";
            cout<<"********************************\n";
            break;
        }
        else if(input == "index"){
            int index = 0;
            string s;
            cout << "Enter index: ";
            cin >> s;
            if(isdigit(atoi(s.c_str()))) {
                index = atoi(s.c_str());
            }
            cout << "Unigram or Bigram? ";
            cin >> s;
            makeLowercase(s);
            
            if(s == "unigram"){
                if(index >= 0 && index < unigramCount){
                    cout << "Index " << index << ": '" << unis[index].word << "' with a frequency of "
                    << unis[index].frequency << endl;
                }
                else{
                    cout << "Invalid index: " << index << endl;
                }
            }
            else if(s == "bigram"){
                if(index >= 0 && index < bigramCount){
                    cout << "Index " << index << ": '" << bis[index].word1 << " "<< bis[index].word2
                    << "' with a frequency of " << bis[index].frequency << endl;
                }
                else{
                    cout << "Invalid index: " << index << endl;
                }
            }
            else{
                cout <<"Invalid entry \n";
            }
        }
        else{
            cout<<"ERROR: select one of the options\n";
        }
    }
    
    return 0;
}

/**************************** Function implementations ****************************/

/*
 * isNonAlpha: checks to see if a string is all letters.
 * parameters: The string it will check
 * return value: bool: whether or not it is true that a number was found
 */
bool isNonAlpha(string str){
    bool foundNumber = false;
    
    for(int i = 0;i<str.size();i++){
        if(0 != isdigit(str[i])){
            foundNumber = true;
            break;
        }
    }
    return foundNumber;
}

/*
 * readdata: Reads the file and populates the array with the strings in order.
 * parameters: The name of the file to read, an array of strings that it will populate, and the size
 * of the array
 * return value: int
 */
int readdata(string filename, string words[], int& size){
    cout << "Opening file: " << filename << endl;
    
    ifstream file;
    file.open(filename.c_str());
    
    // checks if file didn't open correctly
    if(!file.good()){
        cout << "ERROR: Could not open file: " << filename << endl;
        return 1;
    }
    
    //count of items read
    int count = 0;
    file >> words[count];
    while(!file.eof()){
        //doesn't count spaces and endlines as strings
        count++;
        file >> words[count];
    }
    size = count;
    
    file.close();
    return 0;
}

/*
 * getlongestword: Searches a string of words and returns the longest one.
 * parameters: The string array to search, and the number of words in that array
 * return value: int: the index of the longest word
 */
int getlongestword(string words[], int wordCount){
    int longestIndex = 0;
    size_t largest = 0;
    
    for(int i = 0; i<wordCount; i++){
        if(words[i].size() > largest){
            largest = words[i].size();
            longestIndex = i;
        }
    }
    return longestIndex;
}

/*
 * cleanup: Removes anything that isn't a letter in each word of the string array.
 * parameters: The array to be cleaned and the number of words in the array.
 * return value: void
 */
void cleanup(string words[], int wordCount){
    cout << "Would you like to log cleanup changes? \n";
    bool saidYes = false;

    string ans;
    
    while(ans != "n" && ans != "y"){
        cout << "Please input a 'Y' or 'N'\n";
        cin >> ans;
        makeLowercase(ans);
        if(ans == "y")
            saidYes = true;
    }
    
    if(saidYes){
        cout << "writing log to 'ast1log.txt'\n";
    }
    
    ofstream changes("ast1log.txt");
    if(saidYes){
        changes << "Nonalphanumeric Replacement Log file: " << endl << endl;
    }
    
    for(int i = 0; i < wordCount; i++){
        string temp = words[i];
        
        //changes all tolower
        for(int l = 0;l<temp.size();l++){
            int testCh = tolower((int)temp[l]);
            temp[l] = testCh;
            
        }
        //this is where it cleans and writes the changes in the log
        words[i] = temp;
        bool removedCh = false;
        for(int j = 0; j<temp.size();j++){
            //removes non-alphanumeric
            if(0 == isalnum(temp[j])){
                temp.erase(j,1);
                j--;
                removedCh = true;
            }
        }
        if(isNonAlpha(temp)){
            temp = "NONALPHA";
        }
        
        if(saidYes && removedCh){
            changes << words[i] << " -> " << temp << endl;
        }
        //putting cleaned word back into array
        words[i] = temp;
    }
    
    changes.close();
}

/*
 * inList: Checks the list to see if a unigram already exists in it, if so, returns the index.
 * parameters: The array of Unigrams to be checked, the number of unigrams in the array, and the word
 * being searched for.
 * return value: int: the index of the word in the unigram list, returns -1 if its not in the list.
 */
int inList(string word, struct Unigram list[], int count){
    int index = -1;
    for(int i = 0;i<count;i++){
        if(word == list[i].word){
            index = i;
            break;
        }
    }
    
    return index;
}

/*
 * inList: Checks the list to see if a bigram already exists in it, if so, returns the index.
 * parameters: The array of Bigrams to be checked, the number of unigrams in the array, and the words
 * being searched for.
 * return value: int: the index of the words in the Bigram list, returns -1 if its not in the list.
 */
int inList(string fWord, string sWord, struct Bigram list[], int count){
    int index = -1;
    
    for(int i = 0;i<count - 1;i++){
        if(list[i].word1 == fWord && list[i].word2 == sWord){
            index = i;
            break;
        }
    }
    
    return index;
}

/*
 * sortgms: Sorts the array of unigrams by frequency.
 * parameters: The array to be sorted and the size of the array.
 * return value: void
 */
void sortgms(Unigram unigrams[], int size){
    int i, j;
    bool isSorted = false;
    Unigram temp;
    
    //counts how many things are sorted
    for(i = 0;i<size && !isSorted;i++){
        isSorted = true;
        //comparing the thing to the right so we -1
        for(j = 0;j< size -i -1;j++){
            // sorting from least to greatest
            if(unigrams[j].frequency < unigrams[j+1].frequency){
                temp = unigrams[j];
                unigrams[j] = unigrams[j+1];
                unigrams[j+1] = temp;
                
                isSorted = false;
            }
        }
    }
}

/*
* sortgms: Sorts the array of bigrams alphabetically.
* parameters: The array to be sorted and the size of the array.
* return value: void
*/
void sortgms(Bigram bigrams[], int size){
    bool didSwap = false;
    
    //will go thru each bigram in the array and bubble sort them
    //on the first word, and then the second word
    do{
        for(int i = 0;i< (size - 1);i++){
            if(bigrams[i].word1 > bigrams[i+1].word1){
                
                Bigram save;
                
                save.frequency = bigrams[i].frequency;
                save.word1 = bigrams[i].word1;
                save.word2 = bigrams[i].word2;
                
                bigrams[i].frequency = bigrams[i+1].frequency;
                bigrams[i].word1 = bigrams[i+1].word1;
                bigrams[i].word2 = bigrams[i+1].word2;
                
                bigrams[i+1].frequency = save.frequency;
                bigrams[i+1].word1 = save.word1;
                bigrams[i+1].word2 = save.word2;
                
                didSwap = true;
            }
            if(bigrams[i].word1 == bigrams[i+1].word1 && bigrams[i].word2 > bigrams[i+1].word2){
                
                Bigram save;
                
                save.frequency = bigrams[i].frequency;
                save.word1 = bigrams[i].word1;
                save.word2 = bigrams[i].word2;
                
                bigrams[i].frequency = bigrams[i+1].frequency;
                bigrams[i].word1 = bigrams[i+1].word1;
                bigrams[i].word2 = bigrams[i+1].word2;
                
                bigrams[i+1].frequency = save.frequency;
                bigrams[i+1].word1 = save.word1;
                bigrams[i+1].word2 = save.word2;
                
                didSwap = true;
            }
        }
        
        // if nothing to sort, stop looping
        if(!didSwap){
            break;
        }
        else{
            didSwap = false;
        }
    } while(!didSwap);
    
}

/*
 * generate_unigrams: Takes the cleaned up array of strings and puts them into the list of
 * unigrams and records frequency.
 * parameters: The array of strings to be input, the number of words in that array, the
 * number of unigrams, and the array of unigrams.
 * return value: void
 */
void generate_unigrams(struct Unigram unigrams[], int& unigramCount, string words[], int wordCount){
    int index;
    unigramCount = 0;
    
    for(int i = 0;i<wordCount;i++){
        index = inList(words[i], unigrams, i);
        if(index == -1){
            unigrams[unigramCount].word = words[i];
            unigrams[unigramCount].frequency = 1;
            unigramCount++;
        }
        else{
            unigrams[index].frequency++;
        }
    }
    
    sortgms(unigrams, unigramCount);
}

/*
 * generate_bigrams: Takes the cleaned up array of strings and puts them into the list of
 * bigrams and records frequency.
 * parameters: The array of words, the number of words in that array, the array of bigrams
 * to be filled, and the number of bigrams in that array.
 * return value: void
 */
void generate_bigrams(Bigram bigrams[], int& bigramCount, string words[], int wordCount){
    int index;
    bigramCount = 0;
    
    for(int i = 0; i < wordCount - 1;i++){
        index = inList(words[i],words[i+1], bigrams, i);
        if(index == -1){
            bigrams[bigramCount].word1 = words[i];
            bigrams[bigramCount].word2 = words[i+1];
            bigrams[bigramCount].frequency = 1;
            bigramCount++;
        }
        else{
            bigrams[index].frequency++;
        }
    }
    sortgms(bigrams, bigramCount);
}

/*
 * write1gmslogs: Takes the array of sorted unigrams and writes it to a file.
 * parameters: The array of unigrams to put in the file, the size of that array, and the
 * name of the file to write.
 * return value: void
 */
void write1gmslog(string filename, Unigram unigrams[], int size){
    ofstream log(filename);
    log << size << endl;
    for(int i = 0;i<size;i++){
        log << unigrams[i].word << "      " << unigrams[i].frequency<<endl;
        
    }
    log.close();
}

/*
 * write2gmslogs: Takes the array of sorted bigrams and writes it to a file.
 * parameters: The array of bigrams to put in the file, the size of that array, and the
 * name of the file to write.
 * return value: void
 */
void write2gmslog(string filename, Bigram bigrams[], int size){
    ofstream log(filename);
    log << size << endl;
    
    for(int i = 0;i<size;i++){
        log << bigrams[i].word1 << " " << bigrams[i].word2 << "      " << bigrams[i].frequency<<endl;
    }
    log.close();
}
