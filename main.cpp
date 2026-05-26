/* 
 * File:   main.cpp
 * Author: Matthew Madrigal
 * Purpose: To recreate the mastermind game
 */

//System Libraries
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <cstring>
using namespace std;

//User Libraries
enum Color {RED, GREEN, BLUE, YELLOW, BROWN, ORANGE, BLACK, WHITE};

//Global Constants - Math/Science/Conversions only

//Function Prototypes
string getColorStr(const char color);                                      // Converts the color enum to a string
char getColor(string input);                                               // Converts a string to a color enum
bool valColor(string input);                                                // Verifies the string input can be converted to a color
void getSetting(int &length, bool &duplicate, bool &statTest);                              // Gets settings from the user (length of code, duplicates)
char* genCode(const int length, const bool duplicate, const bool statTest);                     // Creates the code using the provided settings
bool playGame(const int length, const char* code);                         // Main game loop. Allows the user to play and checks what the user got right
void checkColors(const int length, const char* code, const char* guess, int &corrPlc, int &incPlc);  // Checks how many colors are in the correct location and how many are in the incorrect location. returned as an array of 2 ints (correct, misplaced)
void results(const bool won, const char* code, const int length);          // Tells the user if they won or lost and what the code was
void statTestPlay(const int length, const char* code);

//Execution Begins Here
int main(int argc, char** argv) {
    //Set random seed
    srand(time(0));
    //Declare Variables
    int length = -1;
    bool duplicate, won, statTest;
    char* code;
    
    //Initialize Variables
    getSetting(length, duplicate, statTest);
    code = genCode(length, duplicate, statTest);
    
    //The Process -> Map Inputs to Outputs
    if(!statTest) {
         won = playGame(length, code);
        //Display Inputs/Outputs
        results(won, code, length);
    } else {
        statTestPlay(length, code);
    }

    //Exit the Program
    delete[] code;
    return 0;
}
string getColorStr(const char color) {
    switch(color - '0') {
        case RED:
            return "red";
        case GREEN:
            return "green";
        case BLUE:
            return "blue";
        case YELLOW:
            return "yellow";
        case BROWN:
            return "brown";
        case ORANGE:
            return "orange";
        case BLACK:
            return "black";
        case WHITE:
            return "white";
        default:
            return "unknown";
    }
}

char getColor(string input) {
    // Converts to lower to ignore case
    for(int i = 0; i < input.size(); i++) {
        input[i] = tolower(input[i]);
    }
    if(input == "red") return RED + '0';
    else if(input == "green") return GREEN + '0';
    else if(input == "blue") return BLUE + '0';
    else if(input == "yellow") return YELLOW + '0';
    else if(input == "brown") return BROWN + '0';
    else if(input == "orange") return ORANGE + '0';
    else if(input == "black") return BLACK + '0';
    else return WHITE + '0';
}

bool valColor(string input) {
    // Converts to lower to ignore case
    for(int i = 0; i < input.size(); i++) {
        input[i] = tolower(input[i]);
    }
    return input == "red" || input == "green" || input == "blue" || input == "yellow" || input == "brown" || input == "orange" || input == "black" || input == "white";
}

void getSetting(int &length, bool &duplicate, bool &statTest) {
    const int DEFAULT_LENGTH = 4;
    const bool DEFAULT_DUPLICATE = true;    // Right now the program can't do the statTest with duplicates disabled
    cout << "Would you like to run the statistic test instad of playing? (y/N): ";
    char answer;
    cin >> answer;
    if(tolower(answer) == 'y') {
        length = DEFAULT_LENGTH;
        duplicate = DEFAULT_DUPLICATE;
        statTest = true;
        cout << endl;   // This is here incase this code is having its output routed to a file. (I had to reroute the output during testing because the output was so long)
        cout << "Using Preset settings: code length: " << DEFAULT_LENGTH << ", duplicates allowed: " << (DEFAULT_DUPLICATE? "TRUE" : "FALSE") << endl;
        return;
    } else statTest = false;

    do {
        cout << "Enter the length of the code (4, 6, 8): ";
        cin >> length;
    } while(!(length == 4 || length == 6 || length == 8));
    bool valid;
    int response;
    do {
        cout << "Should there be duplicates? (1 for yes or 2 for no): ";
        cin >> response;
        if(response < 1 || response > 2) {
            valid = false;
        } else {
            valid = true;
            if(response == 1) duplicate = true;
            else if(response == 2) duplicate = false;
        }
    } while(!valid);
}

char* genCode(const int length, const bool duplicate, const bool statTest) {
    char* code = new char[length];
    int numColor = statTest? 10:8;  // Quick way to add the ability to use 0-9 instead of the colors
    // The array below allows the program to keep track of what colors have been used incase it needs to not have duplicates.
    int options[8] = {RED, GREEN, BLUE, YELLOW, BROWN, ORANGE, BLACK, WHITE};
    for(int i = 0; i < length; i++) {
        int chosen = rand() % numColor;
        if(statTest) code[i] = chosen + '0';
        else code[i] = options[chosen] + '0';
        // If duplicates are not allowed the program shifts the used color to the end of the array and forgets it exists (-1 size)
        if(!duplicate) {
            for(int j = chosen; j < numColor-1; j++) {
                int tmp = options[j+1];
                options[j+1] = options[j];
                options[j] = tmp;
            }
            numColor--;
        }
    }
    return code;
}

bool playGame(const int length, const char* code) {
    // Edit this constant to give the users more or less tries to guess
    const int MAX_GUESS = 10;
    int numGuess = 0;
    string *guess = new string[length];
    // This variable makes checking the color easier since it will be an enum of only allowed colors
    char *guessCol = new char[length];
    bool valid;
    bool won = false;
    // Main loop
    while(numGuess < MAX_GUESS) {
        cout << "Enter what colors you think the code is in text. The code is " << length << " colors long: ";
        for(int i = 0; i < length; i++) {
            cin >> guess[i];
        }
        valid = true;
        // Verifys the colors the user inputed can be recognized
        for (int i = 0; i < length; i++) {
            if(!valColor(guess[i])) {
                valid = false;
            }
        }
        if(valid) {
            // converts the strings to the correct enum color
            for(int i = 0; i < length; i++) {
                guessCol[i] = getColor(guess[i]);
            }
            // Checks how many are in the correct location and how many are in the wrong location. Returns as a int[2] (correct, misplaced)
            int correct, incorrect;
            checkColors(length, code, guessCol, correct, incorrect);
            // If the returned correct is the length then all of them are correct and the user won
            if(correct == length) {
                numGuess = MAX_GUESS;
                won = true;
            } else {    // The user got 1 or more wrong and a guess is used
                cout << "You got " << correct << " in the correct spot and " << incorrect << " in the wrong spot!" << endl;
                numGuess++;
            }
        } else {
            cout << "You didn't enter valid colors!" << endl;
        }
    }
    delete[] guess;
    delete[] guessCol;
    return won;
}
void statTestPlay(const int length, const char* code) {
    int max = 0;
    const int colLength = 22;

    for(int i = 0; i < length; i++) {
        max *= 10;
        max += 9;
    }
    // Header
    cout << setw(colLength) << "Code" << setw(colLength) << "Guess" << setw(colLength) << "#right" << setw(colLength) << "#right in wrong spot" << setw(colLength) << "Sum" << endl;
    char* guess = new char[length];
    char* guessDisp = new char[length+1];
    char* codeDisp = new char[length+1];
    strncpy(codeDisp, code, length);
    codeDisp[length] = '\0';
    guessDisp[length] = '\0';

    for(int i = 0; i <= max; i++) {
        int current = i;
        for(int j = length - 1; j >= 0; j--) {
            guess[j] = (current%10) + '0';
            guessDisp[j] = (current%10) + '0';
            current/=10;
        }
        int correct, incorrect;
        checkColors(length, code, guess, correct, incorrect);
        cout << setw(colLength) << codeDisp << setw(colLength) << guessDisp << setw(colLength) << correct << setw(colLength) << incorrect << setw(colLength) << correct + incorrect << endl;
    }
    delete[] guess;
    delete[] guessDisp;
    delete[] codeDisp;
}


void checkColors(const int length, const char* code, const char* guess, int &corrPlc, int &incPlc) {
    // First number is number of colors in correct place. 2nd number is number of colors in wrong place.
    corrPlc = 0;
    incPlc = 0;
    vector<char> wrongGuess;
    vector<char> wrongCode;
    // Checks what colors are correct. If it is not correct, its put in another array (code and guess) to check if its just misplaced
    for(int i = 0; i < length; i++) {
        if(code[i] == guess[i]) {
            corrPlc++;
        } else {
            wrongGuess.push_back(guess[i]);
            wrongCode.push_back(code[i]);
        }
    }
    // Checks if any that were wrong are just misplaced
    for(int i = 0; i < wrongCode.size(); i++) {
        for(int j = 0; j < wrongGuess.size(); j++) {
            if(wrongCode[i] == wrongGuess[j]) {
                wrongGuess.erase(wrongGuess.begin() + j);
                incPlc++;
                break;
            }
        }
    }
}

void results(const bool won, const char* code, const int length) {
    if(won) cout << "You won!" << endl;
    else {
        cout << "You Lost. The code was ";
        for(int i = 0; i < length; i++) cout << getColorStr(code[i]) << " ";
        cout << endl;
    }
}