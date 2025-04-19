#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <ctime>
#include <windows.h>

using namespace std;

// ANSI color codes
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define BLUE "\033[34m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define MAGENTA "\033[35m"
#define YELLOW "\033[33m"

class HangmanGame {
private:
    string selectedWord;
    string hint;
    string display;
    string userId;
    int score;
    int maxGuesses;
    int guessesLeft;
    int hintCount;
    int gameCheck;
    int complete;
    static int highestScore;

    void enableAnsi() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }

    void playSound(const string& soundType) {
        if (soundType == "correct") {
            Beep(600, 300);
        } else if (soundType == "incorrect") {
            Beep(400, 500);
        } else if (soundType == "gameOver") {
            Beep(200, 800);
        } else if (soundType == "victory") {
            Beep(1000, 400);
        }
    }

    void drawHangman(int guesses) {
        cout << RED "*************\n";

        switch (guesses) {
            case 6:
                cout << YELLOW "*  _______  *\n";
                cout << YELLOW "*  |        *\n";
                cout << YELLOW "*  |        *\n";
                cout << YELLOW "*  |        *\n";
                cout << YELLOW "*  |        *\n";
                cout << YELLOW "*__|_______ *\n";
                cout << RED "*************\n";
                cout << BLUE "I'm still safe! Let's keep it that way!\n";
                break;
            case 5:
                cout << YELLOW "*  _______   *\n";
                cout << YELLOW "*  |     |   *\n";
                cout << YELLOW "*  |     o   *\n";
                cout << YELLOW "*  |         *\n";
                cout << YELLOW "*  |         *\n";
                cout << YELLOW "*__|_______  *\n";
                cout << RED "*************\n";
                cout << BLUE "Careful now! You can do it!\n";
                break;
            case 4:
                cout << YELLOW "*  _______   *\n";
                cout << YELLOW "*  |     |   *\n";
                cout << YELLOW "*  |     o   *\n";
                cout << YELLOW "*  |     |   *\n";
                cout << YELLOW "*  |         *\n";
                cout << YELLOW "*__|_______  *\n";
                cout << RED "*************\n";
                cout << BLUE "Uh-oh... Stay focused!\n";
                break;
            case 3:
                cout << YELLOW "*  _______   *\n";
                cout << YELLOW "*  |     |   *\n";
                cout << YELLOW "*  |     o   *\n";
                cout << YELLOW "*  |    /|   *\n";
                cout << YELLOW "*  |         *\n";
                cout << YELLOW "*__|_______  *\n";
                cout << RED "*************\n";
                cout << BLUE "Please... don't give up on me!\n";
                break;
            case 2:
                cout << YELLOW "*  _______    *\n";
                cout << YELLOW "*  |     |    *\n";
                cout << YELLOW "*  |     o    *\n";
                cout << YELLOW "*  |    /|\\   *\n";
                cout << YELLOW "*  |          *\n";
                cout << YELLOW "*__|_______   *\n";
                cout << RED "**************\n";
                cout << BLUE "I'm getting nervous! Hurry up!\n";
                break;
            case 1:
                cout << YELLOW "*  _______     *\n";
                cout << YELLOW "*  |     |     *\n";
                cout << YELLOW "*  |     o     *\n";
                cout << YELLOW "*  |    /|\\    *\n";
                cout << YELLOW "*  |    /      *\n";
                cout << YELLOW "*__|_______    *\n";
                cout << RED "***************\n";
                cout << BLUE "This is too close! You can still save me!\n";
                break;
            case 0:
                cout << YELLOW "*  _______     *\n";
                cout << YELLOW "*  |     |     *\n";
                cout << YELLOW "*  |     o     *\n";
                cout << YELLOW "*  |    /|\\    *\n";
                cout << YELLOW "*  |    / \\    *\n";
                cout << YELLOW "*__|_______    *\n";
                cout << RED "***************\n";
                cout << BLUE "AAAHHH! It's over... The word was too tough.\n";
                break;
            default:
                cout << RED "* Invalid number of guesses. *\n";
                cout << BLUE "*************\n";
        }
    }

    void scoreSave(int score) {
        ifstream inFile("highestscore.txt");
        int highestScore = 0;

        if (inFile) {
            inFile >> highestScore;
            inFile.close();
        }

        if (score > highestScore) {
            ofstream outFile("highestscore.txt");
            if (outFile) {
                outFile << score;
                outFile.close();
                cout << BLUE "HURRAY!! New high score! = " << score;
            } else {
                cout << RED "Error: Unable to open the file for writing.\n";
            }
        }
    }

    void quitGame() {
        cout << RED "\n GAME ENDED";
        playSound("gameOver");
        exit(0);
    }

    void saveGame(const string& userId, int score, const string& selectedWord, 
                 const string& display, int guesses, int hintCount, const string& hint) {
        fstream file("resume.txt", ios::in | ios::out);
        if (!file.is_open()) {
            file.open("resume.txt", ios::out);
            if (!file.is_open()) {
                cout << "Error opening file.\n";
                return;
            }
        }

        string line;
        streampos foundPos = -1;
        while (getline(file, line)) {
            if (line == userId) {
                foundPos = file.tellg();
                break;
            }
        }

        string gameData = userId + "\n" + to_string(score) + "\n" + selectedWord + "\n" + 
                         display + "\n" + to_string(guesses) + "\n" + 
                         to_string(hintCount) + "\n" + hint + "\n";

        if (foundPos != -1) {
            file.seekp(foundPos - streampos(line.length() + 1));
            file << gameData;
            cout << "GAME SAVED...\n";
        } else {
            file.seekp(0, ios::end);
            file << gameData;
            cout << "GAME SAVED...\n";
        }

        file.close();
    }

    void chooseCategory(int categoryChoice) {
        const char* animals[20] = {
            "Lion", "Tiger", "Elephant", "Giraffe", "Zebra",
            "Kangaroo", "Penguin", "Whale", "Dolphin", "Shark",
            "Bear", "Wolf", "Fox", "Deer", "Eagle",
            "Owl", "Crocodile", "Rabbit", "Horse", "Monkey"
        };

        const char* animalsH[20] = {
            "Big cat with a mane.",
            "Striped big cat.",
            "Huge animal with a trunk.",
            "Tall animal with a long neck.",
            "Black and white stripes.",
            "It hops and has a pouch.",
            "A bird that waddles and swims.",
            "The biggest sea animal.",
            "Friendly sea animal, jumps out of water.",
            "A scary fish with lots of teeth.",
            "Big, furry, and hibernates in winter.",
            "Howls at the moon.",
            "Known for being sly and red.",
            "Bambi is one of these.",
            "A bird that soars high and has sharp eyes.",
            "Hoots at night.",
            "Big reptile with sharp teeth, lives in water.",
            "Small, cute, and hops around.",
            "Rides in races and pulls carriages.",
            "Loves bananas and swings from trees."
        };

        const char* fruits[20] = {
            "Apple", "Banana", "Orange", "Grapes", "Mango",
            "Pineapple", "Strawberry", "Blueberry", "Peach", "Cherry",
            "Watermelon", "Papaya", "Kiwi", "Pomegranate", "Plum",
            "Guava", "Lemon", "Lychee", "Pear", "Dragonfruit"
        };

        const char* fruitsH[20] = {
            "Red, often associated with doctors and teachers.",
            "Yellow and curved, a favorite of monkeys.",
            "Citrus fruit that's a great source of Vitamin C.",
            "Small, round, and come in bunches.",
            "Known as the king of fruits, tropical and juicy.",
            "Has a spiky exterior and sweet, yellow flesh.",
            "Small red fruit with seeds on the outside.",
            "Tiny, round, and blue, often in pancakes.",
            "Soft and fuzzy, with a sweet, juicy interior.",
            "Small, red, and often put on top of desserts.",
            "Large, green on the outside, red and juicy inside.",
            "Tropical fruit, orange flesh, lots of seeds inside.",
            "Small, brown, with green flesh and tiny seeds.",
            "Red fruit with many seeds inside, very juicy.",
            "Small, round, purple, with a pit inside.",
            "Green, often with a rough surface and sweet taste.",
            "Yellow and sour, often used in drinks.",
            "Small, round, tropical, with a juicy white interior.",
            "Green or yellow, sweet, soft inside.",
            "Exotic fruit with a spiky skin and pink flesh."
        };

        const char* vegetables[20] = {
            "Carrot", "Broccoli", "Spinach", "Potato", "Tomato",
            "Cucumber", "Onion", "Garlic", "Peas", "Cauliflower",
            "Lettuce", "Cabbage", "Pumpkin", "Zucchini", "Radish",
            "Celery", "Mushroom", "BellPepper", "Beetroot", "Eggplant"
        };

        const char* vegetablesH[20] = {
            "Orange, crunchy, and good for eyesight.",
            "Green, tree-like vegetable, rich in fiber.",
            "Leafy green, often used in salads and smoothies.",
            "Brown outside, soft and starchy inside.",
            "Red or yellow, technically a fruit but used as a vegetable.",
            "Green, long, and refreshing, often in salads.",
            "Pungent bulb, used in almost every cuisine.",
            "Small, white, and has a strong flavor.",
            "Green, round, and often in pods.",
            "White, with a texture similar to broccoli.",
            "Leafy green, often used in salads and sandwiches.",
            "Green, round, and often used in coleslaw.",
            "Large, orange, and popular for Halloween.",
            "Green, elongated, and similar to cucumber.",
            "Small, red, and has a spicy kick.",
            "Crunchy stalks, often used in soups and salads.",
            "Fungi, usually white or brown, with an earthy flavor.",
            "Comes in various colors, crisp and sweet.",
            "Red root vegetable, often used in salads and juices.",
            "Purple and spongy, used in many cuisines."
        };

        const char* countries[20] = {
            "USA", "Canada", "Brazil", "India", "China",
            "Japan", "Germany", "France", "Australia", "Russia",
            "Italy", "Mexico", "SouthAfrica", "Argentina", "Egypt",
            "Spain", "UnitedKingdom", "SouthKorea", "Nigeria", "SaudiArabia"
        };

        const char* countriesH[20] = {
            "Known for Hollywood, technology, and fast food.",
            "Known for its maple syrup and polite people.",
            "Famous for the Amazon rainforest and carnival.",
            "Diverse culture, spices, and the Taj Mahal.",
            "The most populous country, famous for the Great Wall.",
            "Land of anime, sushi, and Mount Fuji.",
            "Famous for cars, beer, and Oktoberfest.",
            "Known for the Eiffel Tower and fine cuisine.",
            "Known for unique wildlife like kangaroos and koalas.",
            "The largest country in the world by area.",
            "Famous for pasta, pizza, and the Colosseum.",
            "Known for tacos, mariachi, and vibrant culture.",
            "Known for safaris, wildlife, and Nelson Mandela.",
            "Famous for tango, beef, and football.",
            "Home to the Pyramids and ancient civilization.",
            "Known for flamenco, tapas, and sunny beaches.",
            "Known for the Queen, tea, and Big Ben.",
            "Famous for K-pop, technology, and kimchi.",
            "The most populous African country, rich in culture.",
            "Known for its deserts, oil, and Mecca."
        };

        const char* cities[20] = {
            "NewYork", "Tokyo", "Paris", "London", "Sydney",
            "Mumbai", "Beijing", "Berlin", "Moscow", "RiodeJaneiro",
            "Toronto", "Dubai", "LosAngeles", "Rome", "MexicoCity",
            "Seoul", "BuenosAires", "CapeTown", "Istanbul", "Bangkok"
        };

        const char* citiesH[20] = {
            "Known as the 'Big Apple', famous for Times Square and Central Park.",
            "Capital of Japan, known for its skyscrapers and cherry blossoms.",
            "City of lights and romance, famous for the Eiffel Tower.",
            "Home of Big Ben and the London Eye, a historic British city.",
            "Famous for its Opera House and Harbour Bridge, an Australian gem.",
            "The heart of Bollywood, a bustling Indian metropolis.",
            "Capital of China, home to the Forbidden City and Great Wall.",
            "Capital of Germany, known for the Berlin Wall and vibrant arts.",
            "Russian capital, famous for the Kremlin and Red Square.",
            "Brazilian city known for Carnival and Copacabana Beach.",
            "Largest city in Canada, famous for CN Tower and multiculturalism.",
            "Luxurious Middle Eastern city, known for skyscrapers and malls.",
            "City of stars in California, famous for Hollywood.",
            "Italian capital, known for ancient ruins like the Colosseum.",
            "One of the largest cities in Latin America, vibrant and busy.",
            "South Korean capital, known for K-pop and technology hubs.",
            "Argentinian capital, known for tango and rich culture.",
            "South African city famous for Table Mountain and beaches.",
            "Turkish city known for its historic sites and Bosphorus Strait.",
            "Capital of Thailand, famous for vibrant street life and temples."
        };

        const char* items[20] = {
            "Phone", "Laptop", "Wallet", "Watch", "Keys",
            "Pen", "Notebook", "Glasses", "Backpack", "Shoes",
            "Umbrella", "WaterBottle", "Headphones", "Charger", "Toothbrush",
            "Towel", "Sunglasses", "Mug", "Chair", "Spoon"
        };

        const char* itemsH[20] = {
            "A device used to make calls and browse the internet.",
            "Portable computer for work and entertainment.",
            "Holds cash, cards, and IDs.",
            "Worn on the wrist to tell time.",
            "Used to unlock doors or start a car.",
            "Used for writing, often with ink.",
            "Used for taking notes and jotting down ideas.",
            "Helps improve vision or protects eyes from sun.",
            "Bag used to carry books, laptops, and supplies.",
            "Worn on feet, comes in many styles.",
            "Protects you from rain.",
            "Used to carry drinking water.",
            "Plays music directly into your ears.",
            "Powers up electronic devices.",
            "Used to clean your teeth.",
            "Used to dry off after a shower or swim.",
            "Protects eyes from bright sunlight.",
            "Cup with a handle, used for hot drinks.",
            "Used to sit on.",
            "Utensil for eating soups or stirring."
        };

        const char* professions[20] = {
            "Doctor", "Engineer", "Teacher", "Lawyer", "Architect",
            "Nurse", "Scientist", "PoliceOfficer", "Firefighter", "Chef",
            "Accountant", "Electrician", "Plumber", "Artist", "Pilot",
            "Journalist", "Farmer", "Dentist", "SoftwareDeveloper", "Musician"
        };

        const char* professionsH[20] = {
            "Heals patients and prescribes medicine.",
            "Designs and builds structures or machines.",
            "Educates students on various subjects.",
            "Practices law and represents clients.",
            "Designs buildings and other structures.",
            "Cares for patients and assists doctors.",
            "Conducts experiments and research.",
            "Maintains public order and enforces laws.",
            "Responds to emergencies and extinguishes fires.",
            "Cooks and prepares meals in restaurants.",
            "Manages finances and prepares financial records.",
            "Installs and repairs electrical systems.",
            "Fixes water systems and pipes.",
            "Creates visual or performance art.",
            "Operates aircraft and flies passengers.",
            "Writes news stories and reports events.",
            "Grows crops and raises livestock.",
            "Cares for and treats teeth.",
            "Develops computer software and applications.",
            "Plays musical instruments or sings."
        };

        const char* colors[20] = {
            "Red", "Blue", "Green", "Yellow", "Purple",
            "Orange", "Black", "White", "Pink", "Brown",
            "Gray", "Cyan", "Magenta", "Gold", "Silver",
            "Teal", "Maroon", "Beige", "Violet", "Navy"
        };

        const char* colorsH[20] = {
            "Color of fire and passion.",
            "Color of the sky and ocean.",
            "Color of grass and nature.",
            "Bright and cheerful, like the sun.",
            "A mix of red and blue, often royal.",
            "A blend of red and yellow, like sunsets.",
            "Absence of color, dark and mysterious.",
            "Color of purity and simplicity.",
            "A light red, often associated with romance.",
            "Earthy color, often seen in wood.",
            "Neutral, often seen in clouds or stones.",
            "Bright blue-green, often associated with water.",
            "Vibrant pinkish-purple, used in printing.",
            "Shiny and metallic, associated with wealth.",
            "Metallic gray, often reflective.",
            "Blue-green color, calming and fresh.",
            "Deep red, often associated with elegance.",
            "Light, sandy color, often used in interiors.",
            "Soft purple, close to lavender.",
            "Deep, dark blue, like the night sky."
        };

        srand(time(NULL));
        int randomIndex = rand() % 20;

        switch (categoryChoice) {
            case 1:
                selectedWord = animals[randomIndex];
                hint = animalsH[randomIndex];
                break;
            case 2:
                selectedWord = fruits[randomIndex];
                hint = fruitsH[randomIndex];
                break;
            case 3:
                selectedWord = countries[randomIndex];
                hint = countriesH[randomIndex];
                break;
            case 4:
                selectedWord = vegetables[randomIndex];
                hint = vegetablesH[randomIndex];
                break;
            case 5:
                selectedWord = cities[randomIndex];
                hint = citiesH[randomIndex];
                break;
            case 6:
                selectedWord = items[randomIndex];
                hint = itemsH[randomIndex];
                break;
            case 7:
                selectedWord = professions[randomIndex];
                hint = professionsH[randomIndex];
                break;
            case 8:
                selectedWord = colors[randomIndex];
                hint = colorsH[randomIndex];
                break;
            default:
                cout << "Invalid category choice.\n";
                break;
        }
    }

    void blanking() {
        display = string(selectedWord.length(), '_');
    }

    void onePlayer() {
        int categoryChoice;
        cout << MAGENTA "\nChoose a category:\n1. Animals\n2. Fruits\n3. Countries\n4. Vegetables\n5. Cities\n6. Items\n7. Professions\n8. Colors\n";
        cout << GREEN "Enter your choice: ";
        cin >> categoryChoice;
        chooseCategory(categoryChoice);
        blanking();

        cout << BLUE "\nStarting One Player Game...\n";
        gameCheck = 1;
        playGame();
    }

    void twoPlayer() {
        gameCheck = 1;

        cout << BLUE "PLAYER 1 IS HOST.\n";
        cout << GREEN "ENTER YOUR WORD FOR PLAYER 2 TO GUESS: ";
        cin >> selectedWord;

        cin.ignore();

        cout << GREEN "ENTER YOUR HINT FOR THE WORD: ";
        getline(cin, hint);

        blanking();
        system("cls");

        playGame();
    }

    void newGame() {
        int choice = 0;
        cout << YELLOW "Enter 1 for 1 player game\n ";
        cout << YELLOW "Enter 2 for 2 player game\n ";
        cin >> choice;
        if (choice == 1) {
            onePlayer();
        } else if (choice == 2) {
            twoPlayer();
        } else {
            cout << RED "Invalid input.\n TRY AGAIN";
            playSound("gameOver");
        }
    }

    void playGame() {
        guessesLeft = maxGuesses;
        hintCount = 1;
        complete = 0;

        while (guessesLeft > 0) {
            if (display == selectedWord) {
                score += 50;
                playSound("victory");
                cout << GREEN "\nCongratulations! You guessed the word: " << selectedWord << endl;
                cout << MAGENTA "You scored a completion bonus of 50 points!\n";
                cout << GREEN "TOTAL SCORE: " << score << endl;
                scoreSave(score);
                complete = 1;
                break;
            }

            cout << "\n==============================\n";
            if (hintCount == 1) {
                cout << BLUE "Enter ? for acquiring a hint COST=-20 points.\n";
            }
            cout << YELLOW "Enter * to quit and save game.\n";
            cout << "\n==============================\n";
            drawHangman(guessesLeft);
            cout << endl;

            char guessedChar;
            cout << GREEN "Current Word: " << display << endl;
            cout << BLUE "Guess a letter: ";
            cin >> guessedChar;
            //cin.ignore(numeric_limits<streamsize>::max(), '\n');

            guessedChar = tolower(guessedChar);

            if (guessedChar == '?') {
                if (hintCount > 0) {
                    cout << MAGENTA "\nHINT: " << hint << endl;
                    score -= 20;
                    cout << RED "\n20 POINTS LOST..\nTOTAL SCORE= " << score << endl;
                    hintCount--;
                } else {
                    cout << BLUE "\nSorry, no more hints available!\n";
                }
                cout << YELLOW "\nPress Enter to continue......\n";
                cin.ignore();
            } else if (guessedChar == '*') {
                cout << MAGENTA "\nEnter your ID to resume the game (user***): ";
                cin >> userId;
               // cin.ignore(numeric_limits<streamsize>::max(), '\n');
                saveGame(userId, score, selectedWord, display, guessesLeft, hintCount, hint);
                break;
            } else {
                bool correctGuess = false;

                for (size_t i = 0; i < selectedWord.length(); i++) {
                    if (tolower(selectedWord[i]) == guessedChar && display[i] == '_') {
                        display[i] = selectedWord[i];
                        correctGuess = true;
                    }
                }

                if (correctGuess) {
                    score += 10;
                    playSound("correct");
                    cout << GREEN "\nCORRECT GUESS! 10 POINTS GAINED\n";
                    cout << MAGENTA "GUESSES LEFT: " << guessesLeft << endl;
                    cout << GREEN "TOTAL SCORE: " << score << endl;
                } else {
                    guessesLeft--;
                    score -= 5;
                    playSound("incorrect");
                    cout << RED "\nWRONG GUESS! 5 POINTS LOST\n";
                    cout << MAGENTA "GUESSES LEFT: " << guessesLeft << endl;
                    cout << RED "TOTAL SCORE: " << score << endl;
                }
                cout << BLUE "\nPress Enter to continue......\n";
                cin.ignore();
            }
        }

        if (guessesLeft == 0 && complete == 0) {
            drawHangman(guessesLeft);
            cout << RED "You are out of guesses. Try again later.";
            quitGame();
            playSound("gameOver");
        }
    }

    void resumeGame() {
        string id;
        int found = 0;

        cout << MAGENTA "Enter your ID to resume the game (user***): ";
        cin >> userId;
        cin.ignore();

        ifstream file("resume.txt");
        if (!file.is_open()) {
            cout << RED "Error: Unable to open file 'resume.txt'.\n";
            playSound("gameOver");
            return;
        }

        while (getline(file, id)) {
            if (id == userId) {
                found = 1;
                if (!(file >> score) ||
                    !(file.ignore(), getline(file, selectedWord)) ||
                    !getline(file, display) ||
                    !(file >> guessesLeft) ||
                    !(file >> hintCount) ||
                    !(file.ignore(), getline(file, hint))) {
                    cout << RED "Error: Malformed file data for ID: " << userId << endl;
                    file.close();
                    return;
                }
                break;
            } else {
                string temp;
                for (int i = 0; i < 6; i++) {
                    if (!getline(file, temp)) break;
                }
            }
        }
        file.close();

        if (!found) {
            cout << RED "\nNo game found for ID: " << userId << ". Please start a new game.\n";
            playSound("gameOver");
            return;
        }

        while (guessesLeft > 0) {
            if (display == selectedWord) {
                score += 50;
                playSound("victory");
                cout << GREEN "\nCongratulations! You guessed the word: " << selectedWord << endl;
                cout << MAGENTA "You scored a completion bonus of 50 points!\n";
                cout << GREEN "TOTAL SCORE: " << score << endl;
                scoreSave(score);
                break;
            }

            cout << "\n==============================\n";
            if (hintCount == 1) {
                cout << BLUE "Enter ? for acquiring a hint COST=-20 points.\n";
            }
            cout << YELLOW "Enter * to quit and save game.\n";
            cout << "\n==============================\n";
            drawHangman(guessesLeft);
            cout << endl;

            char guessedChar;
            cout << GREEN "Current Word: " << display << endl;
            cout << BLUE "Guess a letter: ";
            cin >> guessedChar;
            //cin.ignore(numeric_limits<streamsize>::max(), '\n');

            guessedChar = tolower(guessedChar);

            if (guessedChar == '?') {
                if (hintCount > 0) {
                    cout << "\nHINT: " << hint << endl;
                    score -= 20;
                    cout << "\n20 POINTS LOST..\nTOTAL SCORE= " << score << endl;
                    hintCount--;
                } else {
                    cout << "\nSorry, no more hints available!\n";
                }
                cout << "\nPress Enter to continue......\n";
                cin.ignore();
            } else if (guessedChar == '*') {
                string saveId = userId;
                cout << MAGENTA "\nEnter your ID to save the game (user***): ";
                cin >> saveId;
                //cin.ignore(numeric_limits<streamsize>::max(), '\n');
                saveGame(saveId, score, selectedWord, display, guessesLeft, hintCount, hint);
                break;
            } else {
                bool correctGuess = false;

                for (size_t i = 0; i < selectedWord.length(); i++) {
                    if (tolower(selectedWord[i]) == guessedChar && display[i] == '_') {
                        display[i] = selectedWord[i];
                        correctGuess = true;
                    }
                }

                if (correctGuess) {
                    score += 10;
                    playSound("correct");
                    cout << GREEN "\nCORRECT GUESS! 10 POINTS GAINED\n";
                    cout << MAGENTA "GUESSES LEFT: " << guessesLeft << endl;
                    cout << GREEN "TOTAL SCORE: " << score << endl;
                } else {
                    guessesLeft--;
                    score -= 5;
                    playSound("incorrect");
                    cout << RED "\nWRONG GUESS! 5 POINTS LOST\n";
                    cout << MAGENTA "GUESSES LEFT: " << guessesLeft << endl;
                    cout << RED "TOTAL SCORE: " << score << endl;
                }
                cout << BLUE "\nPress Enter to continue......\n";
                cin.ignore();
            }
        }

        if (guessesLeft == 0) {
            drawHangman(guessesLeft);
            cout << RED "You are out of guesses. Try again later.\n";
            quitGame();
            playSound("gameOver");
        }
    }

    int menu() {
        int temp = 0;
        cout << BOLD_RED "\n------ HANGMAN GAME ------\n";
        Beep(800, 500);
        cout << GREEN "1. New Game\n";
        cout << GREEN "2. Resume\n";
        cout << GREEN "3. Exit\n";
        cout << MAGENTA "Enter your choice: ";

        cin >> temp;

        return temp;
    }

public:
    HangmanGame() : maxGuesses(6), score(0), gameCheck(0), complete(0) {
        enableAnsi();
    }

    void run() {
        int choice = menu();

        while (choice < 4) {
            switch (choice) {
                case 1:
                    newGame();
                    break;
                case 2:
                    resumeGame();
                    break;
                case 3:
                    quitGame();
                    break;
            }

            choice = menu();
        }

        system("COLOR 0C");
        cout << "Invalid choice, please try again.\n";
    }
};

int HangmanGame::highestScore = 0;

int main() {
    HangmanGame game;
    game.run();
    return 0;
}