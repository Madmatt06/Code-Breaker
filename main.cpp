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
using namespace std;

//User Libraries
enum Color {RED, GREEN, BLUE, YELLOW, BROWN, ORANGE, BLACK, WHITE};

//Global Constants - Math/Science/Conversions only

//Function Prototypes
string getColorStr(const Color color);                                      // Converts the color enum to a string
Color getColor(string input);                                               // Converts a string to a color enum
bool valColor(string input);                                                // Verifies the string input can be converted to a color
void getSetting(int &length, bool &duplicate);                              // Gets settings from the user (length of code, duplicates)
Color* genCode(const int length, const bool duplicate);                     // Creates the code using the provided settings
bool playGame(const int length, const Color* code);                         // Main game loop. Allows the user to play and checks what the user got right
int* checkColors(const int length, const Color* code, const Color* guess);  // Checks how many colors are in the correct location and how many are in the incorrect location. returned as an array of 2 ints (correct, misplaced)
void results(const bool won, const Color* code, const int length);          // Tells the user if they won or lost and what the code was

//Execution Begins Here
int main(int argc, char** argv) {
    //Set random seed
    srand(time(0));
    //Declare Variables
    int length = -1;
    bool duplicate, won;
    Color* code;
    
    //Initialize Variables
    getSetting(length, duplicate);
    code = genCode(length, duplicate);
    
    //The Process -> Map Inputs to Outputs
    won = playGame(length, code);
    
    //Display Inputs/Outputs
    results(won, code, length);

    //Exit the Program
    delete[] code;
    return 0;
}
string getColorStr(const Color color) {
    switch(color) {
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

Color getColor(string input) {
    // Converts to lower to ignore case
    for(int i = 0; i < input.size(); i++) {
        input[i] = tolower(input[i]);
    }
    if(input == "red") return RED;
    else if(input == "green") return GREEN;
    else if(input == "blue") return BLUE;
    else if(input == "yellow") return YELLOW;
    else if(input == "brown") return BROWN;
    else if(input == "orange") return ORANGE;
    else if(input == "black") return BLACK;
    else return WHITE;
}

bool valColor(string input) {
    // Converts to lower to ignore case
    for(int i = 0; i < input.size(); i++) {
        input[i] = tolower(input[i]);
    }
    return input == "red" || input == "green" || input == "blue" || input == "yellow" || input == "brown" || input == "orange" || input == "black" || input == "white";
}

void getSetting(int &length, bool &duplicate) {
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

Color* genCode(const int length, const bool duplicate) {
    Color* code = new Color[length];
    int numColor = 8;
    // The array below allows the program to keep track of what colors have been used incase it needs to not have duplicates.
    Color options[8] = {RED, GREEN, BLUE, YELLOW, BROWN, ORANGE, BLACK, WHITE};
    for(int i = 0; i < length; i++) {
        int chosen = rand() % numColor;
        code[i] = options[chosen];
        // If duplicates are not allowed the program shifts the used color to the end of the array and forgets it exists (-1 size)
        if(!duplicate) {
            for(int j = chosen; j < numColor-1; j++) {
                Color tmp = options[j+1];
                options[j+1] = options[j];
                options[j] = tmp;
            }
            numColor--;
        }
    }
    return code;
}

bool playGame(const int length, const Color* code) {
    // Edit this constant to give the users more or less tries to guess
    const int MAX_GUESS = 10;
    int numGuess = 0;
    string *guess = new string[length];
    // This variable makes checking the color easier since it will be an enum of only allowed colors
    Color *guessCol = new Color[length];
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
            int* result = checkColors(length, code, guessCol);
            // If the returned correct is the length then all of them are correct and the user won
            if(result[0] == length) {
                numGuess = MAX_GUESS;
                won = true;
            } else {    // The user got 1 or more wrong and a guess is used
                cout << "You got " << result[0] << " in the correct spot and " << result[1] << " in the wrong spot!" << endl;
                numGuess++;
            }
            // Clean up
            delete[] result;
        } else {
            cout << "You didn't enter valid colors!" << endl;
        }
    }
    delete[] guess;
    delete[] guessCol;
    return won;
}

int* checkColors(const int length, const Color* code, const Color* guess) {
    // First number is number of colors in correct place. 2nd number is number of colors in wrong place.
    int* result = new int[2];
    result[0] = 0;
    result[1] = 0;
    vector<Color> wrongGuess;
    vector<Color> wrongCode;
    // Checks what colors are correct. If it is not correct, its put in another array (code and guess) to check if its just misplaced
    for(int i = 0; i < length; i++) {
        if(code[i] == guess[i]) {
            result[0]++;
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
                result[1]++;
                break;
            }
        }
    }
    return result;
}

void results(const bool won, const Color* code, const int length) {
    if(won) cout << "You won!" << endl;
    else {
        cout << "You Lost. The code was ";
        for(int i = 0; i < length; i++) cout << getColorStr(code[i]) << " ";
        cout << endl;
    }
}