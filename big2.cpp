#include <bits/stdc++.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STEP_UP  true
#define TIME_LM 20000
#define WIN_RTO   2.2

#define WIN_WID   157
#define WIN_HEI    72
#define MAX_LEN    15
#define LEN_s       1
#define LEN_p       2
#define LEN_gciC    5
#define LEN_d      13
short   BTN_X = WIN_WID - 15;
short   BTN_Y = (WIN_HEI >> 1) - 8;

using namespace std;
using namespace std::chrono;

string int2str(long long int_in) {
	stringstream ss;
	ss << int_in;
	return ss.str();
}

string short2str(short short_in) {
	stringstream ss;
	ss << short_in;
	return ss.str();
}

struct Mouse
{
    // char op;
	short x, y;
    Mouse(/*char op_, */short x_, short y_): /*op(op_), */x(x_), y(y_) {}
};

struct Card
{
    short num, suit;
	Card *L, *R;
	bool sel;
    Card(short num_, short suit_): num(num_), suit(suit_), L(NULL), R(NULL), sel(false) {}
};

struct Operation
{
    char type;
	short priority_1;
	short priority_2;
	vector<short> play;
    Operation(char type_, vector<short> play_): type(type_), priority_1(-1), priority_2(-1), play(play_) {}
};

MEVENT event;
WINDOW *mouse_win;
string player, name, table, guest[4];
short rnd/*round*/, master_rnd, virtual_rnd, virtual_rnd_prev;
short rid/*round id*/, continuous_pass;
bool r_pass/*already pass this round*/, is_op, is_exit;
short num_cards[4], num_card, num_sel;
long long now;
bool is_host;
short  cards_x[13] = {4, 14, 24, 34, 44, 54, 64, 74, 84, 94, 104, 114, 124};
short  ctrl_seq[4] = {27, 91, 77, 32};
string suit_clr[4] = {"\33[94m", "\33[31m", "\33[31m", "\33[94m"};
string nums[5][13] = {{"⣿⣿⣿⣿⣿⣿⣆", "⣿⣿   ⣿⣿", "⣿⣿⣿⣿⣿⣿⣿", "⢠⣶⣿⣿⣿⣿ ", "⣿⣿⣿⣿⣿⣿⣿", "⣰⣿⣿⣿⣿⣿⣆", "⣰⣿⣿⣿⣿⣿⣆", "⢸⣿ ⣶⣿⣷⡆", "     ⣿⣿", "⢠⣶⣿⣿⣿⣶⡄", "⣿⣿  ⢀⣿⡿", "⢠⣶⣿⣿⣿⣶⡄", "⣿⣿⣿⣿⣿⣿⣆"},\
					  {"     ⣿⣿", "⣿⣿   ⣿⣿", "⣿⣿     ", "⣿⣿⠁    ", "    ⣰⣿⠏", "⣿⣿   ⣿⣿", "⣿⣿   ⣿⣿", "⣿⣿⢸⣿⠁⢹⣿", "     ⣿⣿", "⣿⣿⠁ ⠈⣿⣿", "⣿⣿ ⢀⣿⡿ ", "⣿⣿⠁ ⠈⣿⣿", "     ⣿⣿"},\
					  {" ⣿⣿⣿⣿⣿⣏", "⣿⣿⣿⣿⣿⣿⣿", "⣿⣿⣿⣿⣿⣿⣆", "⣿⣿⣿⣿⣿⣿⣆", "   ⣰⣿⠏ ", "⣹⣿⣿⣿⣿⣿⣏", "⠹⣿⣿⣿⣿⣿⣿", "⢸⣿⢸⣿ ⢸⣿", "     ⣿⣿", "⣿⣿   ⣿⣿", "⣿⣿⣿⣿⣿  ", "⣿⣿⣶⣶⣶⣿⣿", "⣰⣿⣿⣿⣿⣿⠏"},\
					  {"     ⣿⣿", "     ⣿⣿", "     ⣿⣿", "⣿⣿   ⣿⣿", "  ⢠⣿⡟  ", "⣿⣿   ⣿⣿", "    ⢀⣿⣿", "⢸⣿⢸⣿⡀⣸⣿", "⣿⣿⡀ ⢀⣿⣿", "⣿⣿⡀⢻⣧⣿⣿", "⣿⣿ ⠈⣿⣷ ", "⣿⣿⠉⠉⠉⣿⣿", "⣿⣿     "},\
					  {"⣿⣿⣿⣿⣿⣿⠏", "     ⣿⣿", "⣿⣿⣿⣿⣿⣿⠏", "⠹⣿⣿⣿⣿⣿⠏", "  ⢸⣿⡇  ", "⠹⣿⣿⣿⣿⣿⠏", " ⣿⣿⣿⣿⠿⠃", "⢸⣿ ⠿⣿⡿⠇", "⠘⠿⣿⣿⣿⠿⠃", "⠘⠿⣿⣿⣿⢿⣆", "⣿⣿  ⠈⣿⣷", "⣿⣿   ⣿⣿", "⣿⣿⣿⣿⣿⣿⣿"}};
string suits[4][4] = {{"  ⣾⣿⣷  ", "  ⣠⣿⣄  ", "⢠⣶⣦⣀⣴⣶⡄", "  ⣠⣿⣄  "},\
					  {"⣠⣤⣹⣿⣏⣤⣄", "⣠⣾⣿⣿⣿⣷⣄", "⢻⣿⣿⣿⣿⣿⡟", "⢀⣾⣿⣿⣿⣷⡀"},\
					  {"⢿⣿⡿⣿⢿⣿⡿", "⠙⢿⣿⣿⣿⡿⠋", " ⠻⣿⣿⣿⠟ ", "⢻⣿⡿⣿⢿⣿⡟"},\
					  {"  ⣠⣿⣄  ", "  ⠙⣿⠋  ", "  ⠈⠿⠁  ", "  ⣠⣿⣄  "}};
string num4[5][14] = {{"┌──────┐", "┌──────┐", "┌──────┐", "┌──────┐", "┌──────┐", "┌──────┐", "┌──────┐", "┌──────┐", "┌──────┐", "┌──────┐", "┌──────┐", "┌──────┐", "┌──────┐", "┌──────┐"},\
					  {"│ ╔══╗ │", "│  ╔╗  │", "│ ═══╗ │", "│ ╔══╗ │", "│ ╦  ╦ │", "│ ╔═══ │", "│ ╔══  │", "│ ╔══╗ │", "│ ╔══╗ │", "│ ╔══╗ │", "│ ╗╔═╗ │", "│ ╗  ╗ │", "│ ╗╔═╗ │", "│ ╗╔═╗ │"},\
					  {"│ ║  ║ │", "│   ║  │", "│ ╔══╝ │", "│  ══╣ │", "│ ╚══╣ │", "│ ╚══╗ │", "│ ╠══╗ │", "│    ║ │", "│ ╠══╣ │", "│ ╚══╣ │", "│ ║║ ║ │", "│ ║  ║ │", "│ ║╔═╝ │", "│ ║ ═╣ │"},\
					  {"│ ╚══╝ │", "│  ═╩╝ │", "│ ╚═══ │", "│ ╚══╝ │", "│    ╝ │", "│ ═══╝ │", "│ ╚══╝ │", "│    ╩ │", "│ ╚══╝ │", "│    ╩ │", "│ ╩╚═╝ │", "│ ╩  ╩ │", "│ ╩╚══ │", "│ ╩╚═╝ │"},\
					  {"└──────┘", "└──────┘", "└──────┘", "└──────┘", "└──────┘", "└──────┘", "└──────┘", "└──────┘", "└──────┘", "└──────┘", "└──────┘", "└──────┘", "└──────┘", "└──────┘"}};

inline string To(short y, short x) {
	return "\33[" + short2str(y) + ";" + short2str(x) + "H";
}

inline void GoTo(short y, short x) {
	cout << To(y, x);
}

inline bool Parse_Arguments(int argc, char* argv[]) {
	bool is_name_valid = true, is_window_valid = true;
	if (argc < 2) {
		cout << "- Too few arguments. (Format: './big2 <PLAYER_NAME>')\n"; is_name_valid = false; }
	else {
		player = argv[1];
		if (player.length() > MAX_LEN) {
			cout << "- Your player name is too long. (Yours: " << player.length() << ", Max length = " << MAX_LEN << ")\n"; is_name_valid = false; }
		if (player.find('.') != std::string::npos || \
			player.find(' ') != std::string::npos || \
			player.find('/') != std::string::npos) {
			cout << "- Do not use special characters in your name.\n"; is_name_valid = false; }
	}
	struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	if (w.ws_col < WIN_WID + 2) {
		cout << "- Your terminal width is too short. (Yours: " << w.ws_col << ", need " << WIN_WID + 2 << ")\n"; is_window_valid = false; }
	if (w.ws_row < (WIN_HEI >> 1) + 2) {
		cout << "- Your terminal height is too short. (Yours: " << w.ws_row << ", need " << (WIN_HEI >> 1) + 2 << ")\n"; is_window_valid = false; }
	return (is_name_valid && is_window_valid);
}

inline void Init_Game(int argc, char* argv[]) {
	now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	srand(now);
	initscr();
	clear();
	noecho();
	cbreak(); //Line buffering disabled.
	mousemask(ALL_MOUSE_EVENTS, NULL);
	mouse_win = newwin(WIN_HEI >> 1, WIN_WID, 0, 0);
	// mouse_win = newwin(0, 0, 0, 0);
	mouse_win->_delay = 10;
	wgetch(mouse_win);
	printf("\33[?25l\33[0;37m");
	flushinp();
}

inline void Print_Window(bool mode, short clr) {
	// Mode:
	//  ├─ True  => Print Block
	//  └─ False => Print Edge Only
	printf("\33[%dm", clr);
	string str_e, str_s;
	if (clr == 37) {
		for (short i = 0; i < WIN_WID; i++) { str_e += "─"; str_s += " "; }
		for (short i = 1; i <= 2 + (WIN_HEI >> 1); i++)
			if (i == 1)
				cout << To(i, 1) + "┌" + str_e + "┐\n";
			else if (i == 2 + (WIN_HEI >> 1))
				cout << To(i, 1) + "└" + str_e + "┘\n";
			else if (mode)
				cout << To(i, 1) + "│" + str_s + "│\n";
			else
				cout << To(i, 1) + "│" + To(i, 2 + WIN_WID) + "│\n";
	}
	else {
		for (short i = 0; i < WIN_WID; i++) { str_e += "═"; str_s += " "; }
		for (short i = 1; i <= 2 + (WIN_HEI >> 1); i++)
			if (i == 1)
				cout << To(i, 1) + "╔" + str_e + "╗\n";
			else if (i == 2 + (WIN_HEI >> 1))
				cout << To(i, 1) + "╚" + str_e + "╝\n";
			else if (mode)
				cout << To(i, 1) + "║" + str_s + "║\n";
			else
				cout << To(i, 1) + "║" + To(i, 2 + WIN_WID) + "║\n";
	}
	printf("\33[37m");
}

inline void Print_Matching_Players(short y, short x, bool mode) {
	// Mode:
	//  ├─ True  => Print
	//  └─ False => Erase
	if (mode == true) {
		cout << To(y    , x) + "___  ___      _       _     _                ______ _                                 \n";
		cout << To(y + 1, x) + "|  \\/  |     | |     | |   (_)               | ___ \\ |                                \n";
		cout << To(y + 2, x) + "| .  . | __ _| |_ ___| |__  _ _ __   __ _    | |_/ / | __ _ _   _  ___ _ __ ___       \n";
		cout << To(y + 3, x) + "| |\\/| |/ _` | __/ __| '_ \\| | '_ \\ / _` |   |  __/| |/ _` | | | |/ _ \\ '__/ __|      \n";
		cout << To(y + 4, x) + "| |  | | (_| | || (__| | | | | | | | (_| |   | |   | | (_| | |_| |  __/ |  \\__ \\_ _ _ \n";
		cout << To(y + 5, x) + "\\_|  |_/\\__,_|\\__\\___|_| |_|_|_| |_|\\__, |   \\_|   |_|\\__,_|\\__, |\\___|_|  |___(_|_|_)\n";
		cout << To(y + 6, x) + "                                     __/ |                   __/ |                    \n";
		cout << To(y + 7, x) + "                                    |___/                   |___/                     \n";
	}
	else for (int i = 0; i < 8; i++)
		cout << To(y + i, x) + "                                                                                      \n";
}

inline void Print_Start(short y, short x, bool mode) {
	// Mode:
	//  ├─ True  => Print
	//  └─ False => Erase
	if (mode == true) {
		cout << To(y    , x) + " _____                           _____ _             _   \n";
		cout << To(y + 1, x) + "|  __ \\                         /  ___| |           | |  \n";
		cout << To(y + 2, x) + "| |  \\/ __ _ _ __ ___   ___     \\ `--.| |_ __ _ _ __| |_ \n";
		cout << To(y + 3, x) + "| | __ / _` | '_ ` _ \\ / _ \\     `--. \\ __/ _` | '__| __|\n";
		cout << To(y + 4, x) + "| |_\\ \\ (_| | | | | | |  __/    /\\__/ / || (_| | |  | |_ \n""";
		cout << To(y + 5, x) + " \\____/\\__,_|_| |_| |_|\\___|    \\____/ \\__\\__,_|_|   \\__|\n";
	}
	else for (int i = 0; i < 6; i++)
		cout << To(y + i, x) + "                                                         \n";
}

inline void Print_Players() {
	cout << To(2, 4) + "┌──────────────────────────────────────────┐  ┌──────────────────────────────────────────┐  ┌──────────────────────────────────────────┐\n";
	cout << To(3, 4) + "│                                          │  │                                          │  │                                          │\n";
	cout << To(4, 4) + "│                                          │  │                                          │  │                                          │\n";
	cout << To(5, 4) + "│                                          │  │                                          │  │                                          │\n";
	cout << To(6, 4) + "│                                          │  │                                          │  │                                          │\n";
	cout << To(7, 4) + "│                                          │  │                                          │  │                                          │\n";
	cout << To(8, 4) + "└──────────────────────────────────────────┘  └──────────────────────────────────────────┘  └──────────────────────────────────────────┘\n";
	for (short i = 0; i < 4; i++) {
		string guest_ = guest[(rid + i) & 0x0003];
		guest_ = guest_.substr(guest_.find('.') + 1);
		cout << (i ? To(5, 145 - 46 * i) : To(BTN_Y - 3, BTN_X)) << guest_ << "\n";
	}
}

inline void Print_Pass(short y, short x, bool mode) {
	// Mode:
	//  ├─ True  => Print
	//  └─ False => Erase
	if (mode == true) {
		cout << To(y++, x) + " ┌──────────────────────────────┐ \n";
		cout << To(y++, x) + " |  ______  ___   _____ _____   | \n";
		cout << To(y++, x) + " |  | ___ \\/ _ \\ /  ___/  ___|  | \n";
		cout << To(y++, x) + " |  | |_/ / /_\\ \\ `--.\\ `--.    | \n";
		cout << To(y++, x) + " |  |  __/|  _  | `--. \\`--. \\  | \n";
		cout << To(y++, x) + " |  | |   | | | |/\\__/ /\\__/ /  | \n";
		cout << To(y++, x) + " |  \\_|   \\_| |_/\\____/\\____/   | \n";
		cout << To(y++, x) + " |                              | \n";
		cout << To(y++, x) + " └──────────────────────────────┘ \n";
	}
	else for (int i = 0; i < 9; i++)
		cout << To(y + i, x) + "                                  \n";
}

inline void Print_Button(short y, short x, short clr, string mode) {
	// Mode:
	//  ├─ True  => PLAY
	//  └─ False => PASS
	printf("\33[%dm", clr);
	if (mode == "PASS") {
		cout << To(y++, x) + "┌──────────────┐\n";
		cout << To(y++, x) + "│ ╔═╗╔═╗╔═╗╔═╗ │\n";
		cout << To(y++, x) + "│ ╠═╝╠═╣╚═╗╚═╗ │\n";
		cout << To(y++, x) + "│ ╩  ╩ ╩╚═╝╚═╝ │\n";
		cout << To(y++, x) + "└──────────────┘\n";
	}
	else if (mode == "PLAY") {
		cout << To(y++, x) + "┌──────────────┐\n";
		cout << To(y++, x) + "│ ╔═╗╦  ╔═╗╦ ╦ │\n";
		cout << To(y++, x) + "│ ╠═╝║  ╠═╣╚╦╝ │\n";
		cout << To(y++, x) + "│ ╩  ╩═╝╩ ╩ ╩  │\n";
		cout << To(y++, x) + "└──────────────┘\n";
	}
	else if (mode == "no_hand") {
		for (short i = 0; i < 3; i++)
			cout << To(y + i, x) + "                \n";
	}
	else if (mode == "d") {
		cout << To(y    , x) + "┌──────────────┐\n";
		cout << To(y + 1, x) + "│ Straight A-K │\n";
		cout << To(y + 2, x) + "└──────────────┘\n";
	}
	else if (mode == "C") {
		cout << To(y    , x) + "┌──────────────┐\n";
		cout << To(y + 1, x) + "│Straight Flush│\n";
		cout << To(y + 2, x) + "└──────────────┘\n";
	}
	else if (mode == "i") {
		cout << To(y    , x) + "┌──────────────┐\n";
		cout << To(y + 1, x) + "│Four of a kind│\n";
		cout << To(y + 2, x) + "└──────────────┘\n";
	}
	else if (mode == "g") {
		cout << To(y    , x) + "┌──────────────┐\n";
		cout << To(y + 1, x) + "│  Full House  │\n";
		cout << To(y + 2, x) + "└──────────────┘\n";
	}
	else if (mode == "c") {
		cout << To(y    , x) + "┌──────────────┐\n";
		cout << To(y + 1, x) + "│   Straight   │\n";
		cout << To(y + 2, x) + "└──────────────┘\n";
	}
	else if (mode == "p") {
		cout << To(y    , x) + "┌──────────────┐\n";
		cout << To(y + 1, x) + "│     Pair     │\n";
		cout << To(y + 2, x) + "└──────────────┘\n";
	}
	else if (mode == "s") {
		cout << To(y    , x) + "┌──────────────┐\n";
		cout << To(y + 1, x) + "│    Single    │\n";
		cout << To(y + 2, x) + "└──────────────┘\n";
	}
	else if (mode == "PRESS_START") {
		cout << To(y++, x) + "┌──────────────────────┐\n";
		cout << To(y++, x) + "│                      │\n";
		cout << To(y++, x) + "│   ╔═╗╦═╗╔═╗╔═╗╔═╗    │\n";
		cout << To(y++, x) + "│   ╠═╝╠╦╝║╣ ╚═╗╚═╗    │\n";
		cout << To(y++, x) + "│   ╩  ╩╚═╚═╝╚═╝╚═╝    │\n";
		cout << To(y++, x) + "│    ╔═╗╔╦╗╔═╗╦═╗╔╦╗   │\n";
		cout << To(y++, x) + "│    ╚═╗ ║ ╠═╣╠╦╝ ║    │\n";
		cout << To(y++, x) + "│    ╚═╝ ╩ ╩ ╩╩╚═ ╩    │\n";
		cout << To(y++, x) + "│                      │\n";
		cout << To(y++, x) + "└──────────────────────┘\n";
	}
	else if (mode == "QUIT_GAME") {
		cout << To(y++, x) + "┌──────────────┐\n";
		cout << To(y++, x) + "│              │\n";
		cout << To(y++, x) + "│ ╔═╗ ╦ ╦ ╦╔╦╗ │\n";
		cout << To(y++, x) + "│ ║═╬╗║ ║ ║ ║  │\n";
		cout << To(y++, x) + "│ ╚═╝╚╚═╝ ╩ ╩  │\n";
		cout << To(y++, x) + "│ ╔═╗╔═╗╔╦╗╔═╗ │\n";
		cout << To(y++, x) + "│ ║ ╦╠═╣║║║╠╣  │\n";
		cout << To(y++, x) + "│ ╚═╝╩ ╩╩ ╩╚═╝ │\n";
		cout << To(y++, x) + "│              │\n";
		cout << To(y++, x) + "└──────────────┘\n";
	}
	printf("\33[37m");
}

inline void Print_Card(short idx, vector<Card*> *card, short mode, short clr) {
	// Mode:
	//  ├─ 0 => Print A Single Card Violently
	//  ├─ 1 => Enable Basic Edge Trimming
	//  └─ 2 =>
	short x = 4 + 10 * idx;
	short y = (WIN_HEI >> 1) - 10;
	short num  = (*card)[idx]->num;
	short suit = (*card)[idx]->suit;
	string clr_ = "\33[" + short2str(clr) + "m";
	cout << clr_;
	cout << To(y     , x) + "⣾⠟⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠻⣷\n";
	cout << To(y +  1, x) + "⣿ " + suit_clr[suit] + nums[0][num]   + clr_ + "      ⣿\n";
	cout << To(y +  2, x) + "⣿ " + suit_clr[suit] + nums[1][num]   + clr_ + "      ⣿\n";
	cout << To(y +  3, x) + "⣿ " + suit_clr[suit] + nums[2][num]   + clr_ + "      ⣿\n";
	cout << To(y +  4, x) + "⣿ " + suit_clr[suit] + nums[3][num]   + clr_ + "      ⣿\n";
	cout << To(y +  5, x) + "⣿ " + suit_clr[suit] + nums[4][num]   + clr_ + "      ⣿\n";
	cout << To(y +  6, x) + "⣿              ⣿\n";
	cout << To(y +  7, x) + "⣿ " + suit_clr[suit] + suits[0][suit] + clr_ + "      ⣿\n";
	cout << To(y +  8, x) + "⣿ " + suit_clr[suit] + suits[1][suit] + clr_ + "      ⣿\n";
	cout << To(y +  9, x) + "⣿ " + suit_clr[suit] + suits[2][suit] + clr_ + "      ⣿\n";
	cout << To(y + 10, x) + "⣿ " + suit_clr[suit] + suits[3][suit] + clr_ + "      ⣿\n";
	cout << To(y + 11, x) + "⣿              ⣿\n";
	if (mode == 1) cout << To(y, x - 1) + "⠋\n";
	printf("\33[37m");
}

inline void Print_Card_Select(vector<Card*> *card, short idx, short clr) {
	short x = 4 + 10 * idx;
	short y = (WIN_HEI >> 1) - ((*card)[idx]->sel ? 14 : 10);
	short num  = (*card)[idx]->num;
	short suit = (*card)[idx]->suit;
	string clr_ = "\33[" + short2str(clr) + "m";
	cout << clr_;
	cout << To(y     , x) + "⣾⠟⠛⠛⠛⠛⠛⠛⠛⠛\n";
	cout << To(y +  1, x) + "⣿ " + suit_clr[suit] + nums[0][num]   + clr_ + " \n";
	cout << To(y +  2, x) + "⣿ " + suit_clr[suit] + nums[1][num]   + clr_ + " \n";
	cout << To(y +  3, x) + "⣿ " + suit_clr[suit] + nums[2][num]   + clr_ + " \n";
	cout << To(y +  4, x) + "⣿ " + suit_clr[suit] + nums[3][num]   + clr_ + " \n";
	cout << To(y +  5, x) + "⣿ " + suit_clr[suit] + nums[4][num]   + clr_ + " \n";
	cout << To(y +  6, x) + "⣿         \n";
	cout << To(y +  7, x) + "⣿ " + suit_clr[suit] + suits[0][suit] + clr_ + " \n";
	cout << To(y +  8, x) + "⣿ " + suit_clr[suit] + suits[1][suit] + clr_ + " \n";
	cout << To(y +  9, x) + "⣿ " + suit_clr[suit] + suits[2][suit] + clr_ + " \n";
	cout << To(y + 10, x) + "⣿ " + suit_clr[suit] + suits[3][suit] + clr_ + " \n";
	cout << To(y + 11, x) + "⣿         \n";
	if ((*card)[idx]->sel) {
		cout << To(y + 12, x) + "⢿⣦⣤⣤⣤⣤⣤⣤⣤⣤\n";
		for (short i = 13; i < 16; i++)
			cout << To(y + i, x) + "          \n";
		if (idx != 0) {
			if ((*card)[idx - 1]->sel) {
				cout << To(y, x - 1) + "⠋\n" + To(y + 12, x - 1) + "⣄\n";
			}
			else {
				cout << To(y + 4, x - 1) + "⠃\n";
				for (short i = 13; i < 16; i++)
					cout << To(y + i, x + 5) + "⣿\n";
			}
		}
		x += 10;
		if (idx != num_card - 1) {
			if ((*card)[idx + 1]->sel) {
				for (short i = 13; i < 16; i++)
					cout << To(y + i, x) + "      \n";
				cout << To(y, x - 1) + "⠋\n" + To(y + 12, x - 1) + "⣄\n";
			}
			else {
				cout << To(y, x) + "⠛⠛⠛⠛⠻⣷\n";
				for (short i = 1; i < 4; i++)
					cout << To(y + i, x + 5) + "⣿\n";
				cout << To(y + 12, x - 1) + "⡄\n";
			}
		}
		else {
			cout << To(y, x) + "⠛⠛⠛⠛⠻⣷\n";
			for (short i = 1; i < 12; i++)
				cout << To(y + i, x) + "     ⣿\n";
			cout << To(y + 12, x) + "⣤⣤⣤⣤⣴⡿\n";
			for (short i = 13; i < 16; i++)
				cout << To(y + i, x) + "      \n";
		}
	}
	else {
		for (short i = 4; i > 0; i--)
			cout << To(y - i, x) + "          \n";
		if (idx != 0) {
			if ((*card)[idx - 1]->sel) {
				cout << To(y - 4, x - 1) + "⠛⠛⠛⠛⠛⠻⣷\n";
				for (short i = 3; i > 0; i--)
					cout << To(y - i, x + 5) + "⣿\n";
				cout << To(y + 8, x - 1) + "⡄\n";
			}
			else {
				cout << To(y, x - 1) + "⠋\n";
			}
		}
		x += 10;
		if (idx != num_card - 1) {
			if ((*card)[idx + 1]->sel) {
				cout << To(y, x - 1) + "⠃\n";
				for (short i = 9; i < 12; i++)
					cout << To(y + i, x + 5) + "⣿\n";
			}
			else {
				for (short i = 4; i > 0; i--)
					cout << To(y - i, x) + "      \n";
				cout << To(y, x - 1) + "⠋\n";
			}
		}
		else {
			for (short i = 4; i > 0; i--)
				cout << To(y - i, x) + "      \n";
			cout << To(y, x) + "⠛⠛⠛⠛⠻⣷\n";
			for (short i = 1; i < 12; i++)
				cout << To(y + i, x) + "     ⣿\n";
		}
	}
	// if (mode == 1) cout << To(y, x - 1) + "⠋\n";
	printf("\33[37m");
}

inline void Print_4P_Cards(short virtual_rnd_, short clr) {
	printf("\33[%dm", clr);
	if (virtual_rnd_) {
		short x = virtual_rnd_ == 3 ? 24 : virtual_rnd_ == 2 ? 70 : 116;
		short num_ = num_cards[virtual_rnd_];
		GoTo(3, x); for (short i = 13; i >= 0; i--) cout << (i == num_ ? num4[0][num_] : i < num_ ? "┐" : " "); cout << "\n";
		GoTo(4, x); for (short i = 13; i >= 0; i--) cout << (i == num_ ? num4[1][num_] : i < num_ ? "│" : " "); cout << "\n";
		GoTo(5, x); for (short i = 13; i >= 0; i--) cout << (i == num_ ? num4[2][num_] : i < num_ ? "│" : " "); cout << "\n";
		GoTo(6, x); for (short i = 13; i >= 0; i--) cout << (i == num_ ? num4[3][num_] : i < num_ ? "│" : " "); cout << "\n";
		GoTo(7, x); for (short i = 13; i >= 0; i--) cout << (i == num_ ? num4[4][num_] : i < num_ ? "┘" : " "); cout << "\n";
	}
	else Print_Window(false, clr);
	printf("\33[37m");
}

inline void Print_Win_Lose(short virtual_rnd_, string result, bool mode) {
	short clr = (result == "win") ? (mode ? 32 : 37) : (mode ? 37 : 31);
	string str_clr = "\33[" + short2str(clr) + "m";;
	cout << str_clr;
	if (virtual_rnd_) {
		short y = 3, x = 145 - 46 * virtual_rnd_;
		if (result == "win") {
			cout << To(y    , x) << "                                      \n";
			cout << To(y + 1, x) << "          ╦  ╦  ╦  ╦  ╔═╗  ╔          \n";
			cout << To(y + 2, x) << "          ╚╗╔╩╗╔╝  ║  ║ ╚╗ ║          \n";
			cout << To(y + 3, x) << "           ╚╝ ╚╝   ╩  ╝  ╚═╝          \n";
			cout << To(y + 4, x) << "                                      \n";
		}
		else if (result == "lose") {
			cout << To(y    , x) << "                                      \n";
			cout << To(y + 1, x) << "        ╦     ╔══╗  ╔══╗  ╔══╗        \n";
			cout << To(y + 2, x) << "        ║     ║  ║  ╚══╗  ╠═╣         \n";
			cout << To(y + 3, x) << "        ╩══╝  ╚══╝  ╚══╝  ╚══╝        \n";
			cout << To(y + 4, x) << "                                      \n";
		}
	}
	else {
		string str_suits = suit_clr[3] + "♠ " + suit_clr[2] + "♥ " + suit_clr[1] + "♦ " + suit_clr[0] + "♣" + str_clr;
		short y = (WIN_HEI >> 1) - 9, x = 42;
		if (result == "win") {
			cout << To(y    , x) << "     __   __               _    _ _            _ _ _ _      \n";
			cout << To(y + 1, x) << "     \\ \\ / /              | |  | (_)          | | | | |     \n";
			cout << To(y + 2, x) << "      \\ V /___  _   _     | |  | |_ _ __      | | | | |     \n";
			cout << To(y + 3, x) << "       \\ // _ \\| | | |    | |/\\| | | '_ \\     | | | | |     \n";
			cout << To(y + 4, x) << "       | | (_) | |_| |    \\  /\\  / | | | |    |_|_|_|_|     \n";
			cout << To(y + 5, x) << "       \\_/\\___/ \\__,_|     \\/  \\/|_|_| |_|    (_|_|_|_)     \n";
			cout << To(y + 6, x) << "                                                            \n";
		}
		else if (result == "lose") {
			cout << To(y    , x) << "  __   __               _                         _ _ _ _   \n";
			cout << To(y + 1, x) << "  \\ \\ / /              | |                       | | | | |  \n";
			cout << To(y + 2, x) << "   \\ V /___  _   _     | |     ___  ___  ___     | | | | |  \n";
			cout << To(y + 3, x) << "    \\ // _ \\| | | |    | |    / _ \\/ __|/ _ \\    | | | | |  \n";
			cout << To(y + 4, x) << "    | | (_) | |_| |    | |___| (_) \\__ \\  __/    |_|_|_|_|  \n";
			cout << To(y + 5, x) << "    \\_/\\___/ \\__,_|    \\_____/\\___/|___/\\___|    (_|_|_|_)  \n";
			cout << To(y + 6, x) << "                                                            \n";
		}
	}
	printf("\33[37m");
}

inline void Menu() {
	Print_Window(false, 37);
	short btn_y_ = (WIN_HEI >> 2) - 3, btn_x_ = (WIN_WID >> 1) - 10;
	Print_Button(btn_y_, btn_x_, 32, "PRESS_START");
	for (short ctrl_idx = 0; ctrl_idx < 4; ) {
		if (wgetch(mouse_win) == ctrl_seq[ctrl_idx] && ++ctrl_idx == 4) {
			ctrl_idx = 0;
			short x = wgetch(mouse_win) - 32;
			short y = wgetch(mouse_win) - 32;
			if (x >= btn_x_ + 1 && x < btn_x_ + 23 && y >= btn_y_ + 1 && y < btn_y_ + 9) break;
		}
	}
	Print_Window(true, 37);
	Print_Matching_Players((6 + WIN_HEI >> 2) - 4, (3 + WIN_WID >> 1) - 42, true);
	system(("perl ./join.pl " + int2str(now) + " " + player).c_str());
	name = int2str(now) + "." + player;
	string str_in;
	ifstream fin("table/" + name + ".play");
	getline(fin, str_in); table = "table/" + str_in + ".table";
	getline(fin, str_in); is_host = str_in == "host";
	fin.close();
	Print_Matching_Players((6 + WIN_HEI >> 2) - 4, (3 + WIN_WID >> 1) - 42, false);
	flushinp();
}

inline void Shuffle() {
	if (!is_host) return;
	// Fisher-Yates Shuffle Cards
	char cards[52];
	for (char i = 0; i < 52; i++) cards[i] = i;
	for (char i = 52; i > 0; i--) swap(cards[rand()%i], cards[i - 1]);
	// for (char i = 0; i < 52; i++) cards[i] = (i % 13) * 4 + i / 13; //// DEL ////
	// Deal & Sort Cards
	string guest_[4];
	ifstream fin(table);
	flushinp();
	guest_[0] = name;
	for (char i = 1; i < 4; i++)
		getline(fin, guest_[i]);
	fin.close();
	// ifstream fin(table);
	for (char i = 0; i < 52; i += 13) {
		ofstream fout("table/" + guest_[i / 13] + ".card");
		// Insertion Sort Cards
		for (char j = i; j < i + 13; j++)
			for (char k = j; k > i; k--)
				if (cards[k] > cards[k - 1]) break;
				else swap(cards[k], cards[k - 1]);
		if (cards[i] == 0) master_rnd = i / 13;
		// Deal Cards & Round Info
		for (char j = i; j < i + 13; j++)
			fout << (short)cards[j] << ' ';
		for (char j = 0; j < 4; j++)
			fout << "\n" << guest_[j];
		fout << "\n" << i / 13 << "\n";
		fout.close();
	}
	// fin.close();
	// Send Shuffled Signal
	ofstream fout(table);
	fout << "shuffled";
	fout.close();
}

inline void Get_Decks(vector<Card*> *card) {
	// Wait Shuffled Signal
	for (bool sig = false; !sig; ) {
		ifstream fin(table);
		flushinp();
		for (string str_in; getline(fin, str_in); )
			if (str_in == "shuffled") { sig = true; break; }
		fin.close();
	}
	// Get Cards
	ifstream fin("table/" + name + ".card");
	flushinp();
	for (short cid, j = 0; j < 13; j++) {
		fin >> cid;
		(*card)[j] = new Card(cid >> 2, cid & 0x0003);
	}
	for (short cid, j = 0; j < 13; j++) {
		(*card)[j]->L = j !=  0 ? (*card)[j - 1] : NULL;
		(*card)[j]->R = j != 12 ? (*card)[j + 1] : NULL;
	}
	for (char j = 0; j < 4; j++)
		fin >> guest[j];
	fin >> rid;
	fin.close();
	// Animation
	Print_Start((6 + WIN_HEI >> 2) - 4, (3 + WIN_WID >> 1) - 28, true);
	usleep(750000);
	for (short j = 0; j < 13; j++) {
		Print_Card(j, card, j ? 1 : 0, 37);
		usleep(200000);
	}
	usleep(500000);
	Print_Start((6 + WIN_HEI >> 2) - 4, (3 + WIN_WID >> 1) - 28, false);
	flushinp();
	// Send Got Signal (-1)
	ofstream fout("table/" + name + ".play");
	fout << "-1 x\n";
	fout.close();
}

inline void Start_Game() {
	// Wait Got Signal (-1)
	Print_Players();
	Print_Button(BTN_Y    , BTN_X, 37, "PLAY");
	Print_Button(BTN_Y + 5, BTN_X, 37, "PASS");
	if (is_host) {
		ofstream fout(table); // flushinp(); //// DEL ////
		fout << master_rnd << " x\n";
		fout.close();
	}
	for (bool sig[4] = { 0 }; !(sig[0] & sig[1] & sig[2] & sig[3]); ) {
		for (short i = 0; i < 4; i++) {
			ifstream fin("table/" + guest[i] + ".play");
			flushinp();
			short rnd_; fin >> rnd_;
			sig[i] = (rnd_ == -1);
			fin.close();
	}	}
	for (string str_in = "shuffled"; str_in == "shuffled"; ) {
		ifstream fin(table); flushinp(); fin >> str_in; fin.close();
	}
	for (short i = 0; i < 4; i++) num_cards[i] = 13;
	num_card = 13;
	num_sel = 0;
	rnd = -1;
	r_pass = false;
	is_exit = false;
	flushinp();
}

inline void Send_Next_Round() {
	if (!is_host) return;
	// Check Message from Guests
	ifstream fin("table/" + guest[rnd & 0x0003] + ".play");
	short rnd_; fin >> rnd_;
	if (rnd_ < master_rnd) {
		fin.close(); return; }
	// Confirm Message, Send Next Round
	ofstream fout(table);
	// ofstream fout("table/" + guest[rnd & 0x0003] + ".play");
	// fout << "-1"; fout.close();
	// fout.open(table);
	char type_; fin >> type_;
	if (type_ != '_') {
		continuous_pass = 0;
		fout << ++master_rnd << ' ' << type_;
		short priority_1_, priority_2_; fin >> priority_1_ >> priority_2_;
		fout << ' ' << priority_1_ << ' ' << priority_2_ << "\n";
		for (short play_; fin >> play_; ) fout << play_ << ' ';
	}
	else fout << ++master_rnd << ' ' << ((++continuous_pass == 3) ? '0' : '_');
	fin.close(); fout.close();
	// cout << To(7, 1) + "Confirm master_rnd -> " << master_rnd << ' ' << duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() << "\n"; //// DEL ////
}

inline void Wait_Next_Round(Operation *op, bool *en_print_hand, bool *en_print_task, bool *en_check_play, bool *en_check_pass, short *t_iter) {
	// Check Current Round
	ifstream fin(table);
	short rnd_; fin >> rnd_;
	if (rnd_ <= rnd) {
		fin.close(); return; }
	// Confirm New Round, Get New Task
	virtual_rnd = ((rnd = rnd_) - rid + 4) & 0x0003;
	virtual_rnd_prev = (virtual_rnd + 3) & 0x0003;
	*en_print_task = *en_print_hand = *en_check_play = *en_check_pass = true;
	*t_iter = (TIME_LM > 0 && virtual_rnd == 0) ? 0 : -1;
	char type_; fin >> type_;
	if (type_ == '0' || type_ == 'x') {
		op->type = type_;
		op->priority_1 = virtual_rnd ? 10 : 0;
		op->priority_2 = 0;
	}
	else if (type_ != '_') {
		op->type = type_;
		fin >> op->priority_1 >> op->priority_2;
		op->play.clear();
		for (short play_; fin >> play_; op->play.push_back(play_))
			num_cards[virtual_rnd_prev]--;
		if (num_cards[virtual_rnd_prev] == 0) is_exit = true;
	}
	else ;
	is_op = (type_ != '0' && type_ != '_');
	fin.close();
	// cout << To(8, 1) + "Confirm rnd -> " << rnd << ' ' << duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() << "\n"; //// DEL ////
	// cout << To(2, 3) << '  ' << op->type << ' ' << op->priority_1 << ' ' << op->priority_2 << "    \n"; //// DEL ////
}

inline void Send_Card_Operation(bool mode, Operation *op_try, vector<Card*> *card) {
	// Mode:
	//  ├─ True  => Play
	//  └─ False => Pass
	ofstream fout("table/" + name + ".play");
	if (mode) {
		fout << rnd << ' ' << op_try->type << ' ' << op_try->priority_1 << ' ' << op_try->priority_2 << "\n";
		for (short i = 0; i < num_sel; i++) fout << op_try->play[i] << ' ';
		for (short i = 0, j = 0; i < num_card; i++) {
			if ((*card)[i]->sel) delete (*card)[i];
			else {
				(*card)[j] = (*card)[i];
				Print_Card(j, card, j ? 1 : 0, 37);
				j++;
			}
		}
		(*card).resize(num_card -= num_sel);
		string str_u(136, ' '), str_r(10 * num_sel, ' ');
		short x = 10 + 10 * num_card, y = (WIN_HEI >> 1) - 10;
		if (num_card == 0) { x -= 6; str_r += "      "; }
		for (short i = -4; i < 12; i++)
			(i < 0) ? (cout << To(y + i, 4) + str_u + "\n") : (cout << To(y + i, x) + str_r + "\n");
		num_sel = 0;
	}
	else fout << rnd << " _";
	fout.close();
}

inline void Compute_Priority(Operation *op_try, vector<Card*> *card) {
	vector<short> play(num_sel), num_(num_sel);
	short idx = num_sel;
	for (short i = num_card; --i >= 0; )
		if ((*card)[i]->sel) {
			play[--idx] = ((*card)[i]->num << 2) + (*card)[i]->suit;
			num_[  idx] = (*card)[i]->num;
		}
	op_try->play = play;
	if (num_sel == LEN_s) {
		op_try->type = 's'; op_try->priority_1 = 1; op_try->priority_2 = play[0];
	}
	else if (num_sel == LEN_p) {
		if (play[0] >> 2 == play[1] >> 2) {
			op_try->type = 'p'; op_try->priority_1 = 1; op_try->priority_2 = play[1];
		}
		else {
			op_try->type = 'x'; op_try->priority_1 = -1; op_try->priority_2 = -1;
		}
	}
	else if (num_sel == LEN_gciC) {
		if (num_[0] == num_[1] && num_[3] == num_[4] && (num_[1] == num_[2] || num_[2] == num_[3])) {
			op_try->type = 'g'; op_try->priority_1 = 1; op_try->priority_2 = num_[2];
		}
		else if (play[0] + 4 == play[1] && play[1] + 4 == play[2] && play[2] + 4 == play[3] && play[3] + 4 == play[4]) {
			if (num_[0] != 8) {
				op_try->type = 'C'; op_try->priority_1 = 3; op_try->priority_2 = play[4] - 12;
			}
			else {
				op_try->type = 'x'; op_try->priority_1 = -1; op_try->priority_2 = -1;
			}
		}
		else if (num_[0] == 0 && play[0] + 4 == play[1] && play[0] + 8 == play[2] && play[0] + 48 == play[4]) {
			if (play[0] + 44 == play[3]) {
				op_try->type = 'C'; op_try->priority_1 = 3; op_try->priority_2 = play[4] - 48;
			}
			else if (play[0] + 12 == play[3]) {
				op_try->type = 'C'; op_try->priority_1 = 3; op_try->priority_2 = play[4] - 12;
			}
		}
		else if (num_[0] + 1 == num_[1] && num_[1] + 1 == num_[2] && num_[2] + 1 == num_[3] && num_[3] + 1 == num_[4]) {
			if (num_[0] != 8) {
				op_try->type = 'c'; op_try->priority_1 = 1; op_try->priority_2 = play[4] - 12;
			}
			else {
				op_try->type = 'x'; op_try->priority_1 = -1; op_try->priority_2 = -1;
			}
		}
		else if (num_[0] == 0 && num_[1] == 1 && num_[2] == 2 && num_[4] == 12) {
			if (num_[3] == 11) {
				op_try->type = 'c'; op_try->priority_1 = 1; op_try->priority_2 = play[4] - 48;
			}
			else if (num_[3] == 3) {
				op_try->type = 'c'; op_try->priority_1 = 1; op_try->priority_2 = play[4] - 12;
			}
		}
		else if (num_[1] == num_[2] && num_[2] == num_[3] && (num_[0] == num_[1] || num_[3] == num_[4])) {
			op_try->type = 'i'; op_try->priority_1 = 2; op_try->priority_2 = num_[2];
		}
		else {
			op_try->type = 'x'; op_try->priority_1 = -1; op_try->priority_2 = -1;
		}
	}
	else if (num_sel == LEN_d) {
		op_try->type = 'd'; op_try->priority_1 = 4; op_try->priority_2 = play[12];
		for (short i = 0; i < LEN_d; i++)
			if (num_[i] != i) {
				op_try->type = 'x'; op_try->priority_1 = -1; op_try->priority_2 = -1;
			}
	}
	else {
		op_try->type = 'x'; op_try->priority_1 = -1; op_try->priority_2 = -1;
	}
	//// DEL ////
	// cout << To(3, 3) << num_sel << ' ' << op_try->type << ' ' << op_try->priority_1 << ' ' << op_try->priority_2 << "    \n";
	// cout << To(2, 1);
	// for (short i = 0; i < num_sel; i++) cout << ' ' << num_[i];
	// cout << "            \n";
	//// DEL ////
}

inline void Update_Play(bool valid) {
	Print_Button(BTN_Y, BTN_X, valid ? (virtual_rnd ? 33 : 32) : 37, "PLAY");
}

inline void Update_Pass(bool valid, short clr) {
	Print_Button(BTN_Y + 5, BTN_X, clr ? clr : valid ? 32 : 37, "PASS");
}

inline void Print_Hand(bool *en/*, bool *valid*/, Operation *op/*, Operation *op_try*/, vector<Card*> *card) {
	if (!(*en) || num_card == 0) return;
	*en = false;
	short num_suit[13] = { 0 }, max_suit[13] = { 0 };
	bool card_map[13][4] = { 0 };
	for (short i = 0; i < num_card; i++) {
		num_suit[(*card)[i]->num]++;
		max_suit[(*card)[i]->num] = (*card)[i]->suit;
		card_map[(*card)[i]->num][(*card)[i]->suit] = 1;
	}

	bool is_hand, is_sendable;

	// Straight A-K
	is_hand = is_sendable = true;
	for (short i = 13; --i >= 0; )
		if (!num_suit[i]) { is_hand = is_sendable = false; break; }
	is_hand ? Print_Button(2, BTN_X, is_sendable ? (virtual_rnd ? 33 : 32) : 37, "d") \
			: Print_Button(2, BTN_X, 37, "no_hand");
	
    // Straight Flush
	is_hand = is_sendable = false;
	bool is_sA2345[4] = { 0 }, is_s23456[4] = { 0 };
	for (short j = 4; --j >= 0; )
		if (card_map[12][j] && card_map[0][j] && card_map[1][j] && card_map[2][j]) {
			is_sA2345[j] = (card_map[11][j]);
			is_s23456[j] = (card_map[ 3][j]);
		}
	for (short i = 13, i_mark[4] = {12, 12, 12, 12}; --i >= -1; ) {
		if (i == 12) {
			for (short j = 4; --j >= 0; ) {
				if (is_s23456[j] && (is_hand = true) && (op->type != 'x' || j == 0)) {
					is_sendable = (op->priority_1 < 3 || (op->priority_1 == 3 && 36 + j > op->priority_2));
					i = -1; break;
		}	}	}
		else if (i != -1) {
			for (short j = 4; --j >= 0; ) {
				if (!card_map[i][j]) i_mark[j] = i;
				else if (i <= i_mark[j] - 5 && (is_hand = true) && (op->type != 'x' || (i | j) == 0)) {
					is_sendable = (op->priority_1 < 3 || (op->priority_1 == 3 && (i + 1 << 2) + j > op->priority_2));
					i = -1; break;
		}	}	}
		else {
			for (short j = 4; --j >= 0; ) {
				if (is_sA2345[j] && (is_hand = true) && (op->type != 'x' || j == 0)) {
					is_sendable = (op->priority_1 < 1 || (op->type == 'c' && max_suit[12] > op->priority_2));
					i = -1; break;
		}	}	}
	}
	is_hand ? Print_Button(5, BTN_X, is_sendable ? (virtual_rnd ? 33 : 32) : 37, "C") \
			: Print_Button(5, BTN_X, 37, "no_hand");
    
	// Four of a kind
	is_hand = false;
	for (short i = 13; --i >= 0; )
		if (num_suit[i] == 4 && num_card > 4) {
			is_sendable = (op->priority_1 < 2 || (op->priority_1 == 2 && i > op->priority_2));
			is_hand = true; break;
		}
	is_hand ? Print_Button(8, BTN_X, is_sendable ? (virtual_rnd ? 33 : 32) : 37, "i") \
			: Print_Button(8, BTN_X, 37, "no_hand");
	
	// Full House
	is_hand = is_sendable = false;
	for (short i = 13; --i >= 0; )
		if (num_suit[i] >= 3)
			for (short j = 13; --j >= 0; )
				if (i != j && num_suit[j] >= 2 && (is_hand = true) && (op->type != 'x' || i == 0 || j == 0)) {
					is_sendable = (op->priority_1 < 1 || (op->type == 'g' && i > op->priority_2));
					break;
				}
	is_hand ? Print_Button(11, BTN_X, is_sendable ? (virtual_rnd ? 33 : 32) : 37, "g") \
			: Print_Button(11, BTN_X, 37, "no_hand");
	
    // Straight
	is_hand = is_sendable = false;
	bool is_A2345 = false, is_23456 = false;
	if (num_suit[12] && num_suit[0] && num_suit[1] && num_suit[2]) {
		is_A2345 = (num_suit[11] != 0);
		is_23456 = (num_suit[ 3] != 0);
	}
	for (short i = 12, i_mark = 12; --i >= 0; ) {
		if (num_suit[i] == 0) i_mark = i;
		else if (is_23456 && i == 11 && (is_hand = true)) {
			is_sendable = (op->priority_1 < 1 || (op->type == 'c' && 36 + max_suit[12] > op->priority_2));
			break;
		}
		else if (i <= i_mark - 5 && (is_hand = true) && (op->type != 'x' || i == 0)) {
			is_sendable = (op->priority_1 < 1 || (op->type == 'c' && (i + 1 << 2) + max_suit[i + 4] > op->priority_2));
			break;
		}
		else if (is_A2345 && i == 0 && (is_hand = true)) {
			is_sendable = (op->priority_1 < 1 || (op->type == 'c' && max_suit[12] > op->priority_2));
			break;
		}
	}
	is_hand ? Print_Button(14, BTN_X, is_sendable ? (virtual_rnd ? 33 : 32) : 37, "c") \
			: Print_Button(14, BTN_X, 37, "no_hand");
	
    // Pair
	is_hand = is_sendable = false;
	for (short i = 13; --i >= 0; )
		if (num_suit[i] >= 2 && (is_hand = true) && (op->type != 'x' || i == 0)) {
			is_sendable = (op->priority_1 < 1 || (op->type == 'p' && (i << 2) + max_suit[i] > op->priority_2));
			break;
		}
	is_hand ? Print_Button(17, BTN_X, is_sendable ? (virtual_rnd ? 33 : 32) : 37, "p") \
			: Print_Button(17, BTN_X, 37, "no_hand");
	
    // Single
	is_sendable = (op->priority_1 < 1 || (op->type == 's' && ((*card)[num_card - 1]->num << 2) + (*card)[num_card - 1]->suit > op->priority_2));
	is_hand = (num_card != 0);
	is_hand ? Print_Button(20, BTN_X, is_sendable ? (virtual_rnd ? 33 : 32) : 37, "s") \
			: Print_Button(20, BTN_X, 37, "no_hand");
	
	flushinp();
}

inline void Print_Task(bool *en/*, bool *valid*/, Operation *op/*, Operation *op_try, vector<Card*> *card*/) {
	if (!(*en)) return;
	*en = false;
	if (op->type == 'x')
		for (short i = 1; i <= 3; i++) Print_4P_Cards(i, 37);
	else {
		short x = (WIN_WID - (op->play.size() * 10 + 18)) >> 1;
		short y = (WIN_HEI >> 1) - 27;
		if (is_op == false) {
			Print_Pass((6 + WIN_HEI >> 2) - 8, (3 + WIN_WID >> 1) - 25, true);
			usleep(1000000);
			Print_Pass((6 + WIN_HEI >> 2) - 8, (3 + WIN_WID >> 1) - 25, false);
		}
		if (op->type == '0') {
			string str_e(136, ' ');
			for (short i = 0; i < 13; i++)
				cout << To(y + i, 4) + str_e + "\n";
		}
		else for (short j = 0; j < op->play.size(); j++) {
			if (virtual_rnd_prev) Print_4P_Cards(virtual_rnd_prev, 33);
			short x_   = x + 10 * j;
			short num  = op->play[j] >> 2;
			short suit = op->play[j] & 0x0003;
			cout << To(y     , x_) + "⣾⠟⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠻⣷\n";
			cout << To(y +  1, x_) + "⣿ " + suit_clr[suit] + nums[0][num]   + "\33[37m      ⣿\n";
			cout << To(y +  2, x_) + "⣿ " + suit_clr[suit] + nums[1][num]   + "\33[37m      ⣿\n";
			cout << To(y +  3, x_) + "⣿ " + suit_clr[suit] + nums[2][num]   + "\33[37m      ⣿\n";
			cout << To(y +  4, x_) + "⣿ " + suit_clr[suit] + nums[3][num]   + "\33[37m      ⣿\n";
			cout << To(y +  5, x_) + "⣿ " + suit_clr[suit] + nums[4][num]   + "\33[37m      ⣿\n";
			cout << To(y +  6, x_) + "⣿              ⣿\n";
			cout << To(y +  7, x_) + "⣿ " + suit_clr[suit] + suits[0][suit] + "\33[37m      ⣿\n";
			cout << To(y +  8, x_) + "⣿ " + suit_clr[suit] + suits[1][suit] + "\33[37m      ⣿\n";
			cout << To(y +  9, x_) + "⣿ " + suit_clr[suit] + suits[2][suit] + "\33[37m      ⣿\n";
			cout << To(y + 10, x_) + "⣿ " + suit_clr[suit] + suits[3][suit] + "\33[37m      ⣿\n";
			cout << To(y + 11, x_) + "⣿              ⣿\n";
			cout << To(y + 12, x_) + "⢿⣦⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣤⣴⡿\n";
			if (j) cout << To(y, x_ - 1) + "⠋\n" + To(y + 12, x_ - 1) + "⣄\n";
		}
		if (is_op == true) usleep(1000000);
	}
	Print_4P_Cards(virtual_rnd_prev, 37);
	Print_4P_Cards(virtual_rnd, 33);
	flushinp();
	return;
}

inline void Check_Play(bool *en, bool *valid, Operation *op, Operation *op_try, vector<Card*> *card) {
	if (!(*en)) return;
	*en = false;
	Compute_Priority(op_try, card);
	if (op->type == 'x' && (*card)[0]->sel == false)
		{ op_try->type = 'x'; op_try->priority_1 = -1; op_try->priority_2 = -1; }
	
	if (!(STEP_UP & r_pass) && ((op_try->priority_1 > op->priority_1) || (op_try->priority_1 == op->priority_1 && op_try->type == op->type && op_try->priority_2 > op->priority_2)))
		Update_Play(*valid = true);
	else Update_Play(*valid = false);
}

inline void Check_Pass(bool *en, bool *valid, Operation *op, Operation *op_try, vector<Card*> *card) {
	if (!(*en)) return;
	if (op->type == '0') { r_pass = false; }
	if (virtual_rnd == 0 && op->type != '0' && op->type != 'x') {
		if (STEP_UP & r_pass) {
			for (short i = 0; i < 8; i++) {
				Update_Pass(*valid = true, i % 2 ? 33 : 37);
				if (i == 7) break; usleep(225000);
			}
			Send_Card_Operation(false, op_try, card);
			flushinp();
			*en = false; return;
		}
		else { Update_Pass(*valid = true, 0); *en = false; return; }
	}
	else { Update_Pass(*valid = false, 0); *en = false; return; }
}

inline bool Check_Time(short *t_iter, double *t_lm, float *t_stripe_unit, float *t_stripe) {
	if (*t_iter == -1) return false;
	// cout << To(2, 2); for (int i = 0; i < WIN_WID; i++) cout << i % 10; cout << "\n"; while(1); //// DEL ////
	double t_lm_ = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	if (*t_iter == 0) {
		*t_iter        = (WIN_WID + 2 << 1) + WIN_HEI;
		*t_stripe_unit = TIME_LM / (float)(WIN_WID + WIN_HEI + 2 << 1);
		*t_stripe      = *t_stripe_unit * ((1 + WIN_RTO) / 2);
		*t_lm          = t_lm_ + *t_stripe;
	}
	if (t_lm_ < *t_lm) return false;
	if (*t_iter == (WIN_WID + 2 << 1) + WIN_HEI) {
		cout << To((WIN_HEI >> 1) + 2, WIN_WID + 2) << "┘\n";
		*t_stripe = *t_stripe_unit * WIN_RTO;
	}
	else if (*t_iter > (WIN_WID << 1) + (WIN_HEI >> 1) + 3) {
		cout << To(*t_iter - ((WIN_WID << 1) + (WIN_HEI >> 1) + 3) + 1, WIN_WID + 2) << "│\n";
		if (*t_iter - ((WIN_WID << 1) + (WIN_HEI >> 1) + 3) == 1)
			*t_stripe = *t_stripe_unit * ((1 + WIN_RTO) / 2);
	}
	else if (*t_iter == (WIN_WID << 1) + (WIN_HEI >> 1) + 3) {
		cout << To(1, WIN_WID + 2) << "┐\n";
		*t_stripe = *t_stripe_unit;
	}
	else if (*t_iter > WIN_WID + 2 + (WIN_HEI >> 1)) {
		cout << To(1, *t_iter - (WIN_WID + 2 + (WIN_HEI >> 1)) + 1) << "─\n";
		if (*t_iter - (WIN_WID + 2 + (WIN_HEI >> 1)) == 1)
			*t_stripe = *t_stripe_unit * ((1 + WIN_RTO) / 2);
	}
	else if (*t_iter == WIN_WID + 2 + (WIN_HEI >> 1)) {
		cout << To(1, 1) << "┌\n";
		*t_stripe = *t_stripe_unit * WIN_RTO;
	}
	else if (*t_iter > WIN_WID + 1) {
		cout << To((WIN_HEI >> 1) + 3 - *t_iter + WIN_WID, 1) << "│\n";
		if (*t_iter - (WIN_WID + 1) == 1)
			*t_stripe = *t_stripe_unit * ((1 + WIN_RTO) / 2);
	}
	else if (*t_iter == WIN_WID + 1) {
		cout << To((WIN_HEI >> 1) + 2, 1) << "└\n";
		*t_stripe = *t_stripe_unit;
	}
	else {
		cout << To((WIN_HEI >> 1) + 2, WIN_WID + 2 - *t_iter) << "─\n";
		if (*t_iter == 1) {
			*t_iter = -1;
			return true;
		}
	}
	*t_lm   += *t_stripe;
	*t_iter -= 1;
	return false;
}

inline void Mouse_Click(short x, short y, Operation *op_try, vector<Card*> *card, bool *en_check_play, bool *valid_play, bool *valid_pass) {
	if (virtual_rnd == 0 && x >= BTN_X + 1 && x < BTN_X + 15) {
		if (y >= BTN_Y + 1 && y < BTN_Y + 4) {
			if (*valid_play) {
				Update_Play(*valid_play = false);
				Send_Card_Operation(true, op_try, card);
				*en_check_play = true;
		}	}
		else if (y >= BTN_Y + 6 && y < BTN_Y + 9) {
			if (*valid_pass) {
				Update_Pass(*valid_pass = false, 0);
				Send_Card_Operation(false, op_try, card);
				r_pass = *en_check_play = true;
		}	}
	}
	else if (x >= 4 && x <= num_card * 10 + 9 && y >= (WIN_HEI >> 1) - 14 && y <= (WIN_HEI >> 1) + 1) {
		short upper = x < (num_card - 1) * 10 + 4 ? (x - 4) / 10 : num_card - 1;
		short lower = x > 19 ? (x - 10) / 10 :  0;
		short sel_idx = -1;
		if (((*card)[upper]->sel && y <= (WIN_HEI >> 1) - 2) || (!(*card)[upper]->sel && y >= (WIN_HEI >> 1) - 10))
			sel_idx = upper;
		else if (lower != upper && (((*card)[lower]->sel && y <= (WIN_HEI >> 1) - 2) || (!(*card)[lower]->sel && y >= (WIN_HEI >> 1) - 10)))
			sel_idx = lower;
		if (sel_idx != -1) {
			(*card)[sel_idx]->sel = !(*card)[sel_idx]->sel;
			(*card)[sel_idx]->sel ? ++num_sel : --num_sel;
			Print_Card_Select(card, sel_idx, 37);
		}
		*en_check_play = true;
	}
}

inline void End_Game() {
	system(("perl ./del.pl " + guest[0] + " " + guest[1] + " " + guest[2] + " " + guest[3]).c_str());
	double t_lm_, t_lm = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	bool mode = false;
	Print_Window(false, 37);
	Print_Button(BTN_Y, BTN_X, 33, "QUIT_GAME");
	for (short ctrl_idx = 0, i = 0; ctrl_idx < 4; ) {
		if (wgetch(mouse_win) == ctrl_seq[ctrl_idx] && ++ctrl_idx == 4) {
			ctrl_idx = 0;
			short x = wgetch(mouse_win) - 32;
			short y = wgetch(mouse_win) - 32;
			if (i == 6 && x >= BTN_X + 1 && x < BTN_X + 15 && y >= BTN_Y + 1 && y < BTN_Y + 9) break;
		}
		t_lm_ = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		if (t_lm_ < t_lm) continue;
		t_lm += 500; mode = !mode;
		if (i < 6 && ++i == 6) Print_Button(BTN_Y, BTN_X, 32, "QUIT_GAME");
		for (short j = 0; j < 4; j++)
			Print_Win_Lose(j, j == virtual_rnd_prev ? "win" : "lose", mode);
	}
	printf("\033[?25h");
	endwin();
}

int main(int argc, char* argv[])
{
	if (!Parse_Arguments(argc, argv)) return 0;

    Mouse *mouse = new Mouse(0, 0);
	vector<Card*> card(13);
	vector<short> play_(13);
	Operation *op     = new Operation('x', play_);
	Operation *op_try = new Operation('x', play_);

	Init_Game(argc, argv);
	Menu();
	Shuffle();
	Get_Decks(&card);
	Start_Game();

	bool en_print_hand = true;
	bool en_print_task = true;
	bool en_check_play = true; bool valid_play = false;
	bool en_check_pass = true; bool valid_pass = false;
	short t_iter; double t_lm; float t_stripe_unit, t_stripe;
	for (short ctrl_idx = 0; ctrl_idx < 4; ) {
		Send_Next_Round();
		Wait_Next_Round(op, &en_print_hand, &en_print_task, &en_check_play, &en_check_pass, &t_iter);
		Print_Hand(&en_print_hand/*, &valid_pass*/, op/*, op_try*/, &card);
		Print_Task(&en_print_task/*, &valid_pass*/, op/*, op_try, &card*/);
		if (is_exit) break;
		else if (num_card == 0) continue;
		Check_Play(&en_check_play, &valid_play, op, op_try, &card);
		Check_Pass(&en_check_pass, &valid_pass, op, op_try, &card);
		if (Check_Time(&t_iter, &t_lm, &t_stripe_unit, &t_stripe)) {
			ctrl_idx = 0;
			if (valid_pass) Mouse_Click(BTN_X + 1, BTN_Y + 6, op_try, &card, &en_check_play, &valid_play, &valid_pass);
			else {
				// for (short i = 0; i < num_card; i++)
				// 	if ((i == 0) ^ card[i]->sel)
				// 		Mouse_Click(cards_x[i], (WIN_HEI >> 1) - 6, op_try, &card, &en_check_play, &valid_play, &valid_pass);
				for (short i = card[0]->sel = en_check_play = num_sel = 1; i < num_card; ) card[i++]->sel = 0;
				Check_Play(&en_check_play, &valid_play, op, op_try, &card);
				Mouse_Click(BTN_X + 1, BTN_Y + 1, op_try, &card, &en_check_play, &valid_play, &valid_pass);
			}
		}
		else if (wgetch(mouse_win) == ctrl_seq[ctrl_idx]) {
			if (++ctrl_idx == 4) {
				ctrl_idx = 0;
				mouse->x = wgetch(mouse_win) - 32;
				mouse->y = wgetch(mouse_win) - 32;
				Mouse_Click(mouse->x, mouse->y, op_try, &card, &en_check_play, &valid_play, &valid_pass);
			}
		}
	}

	End_Game();
	return 0;
}