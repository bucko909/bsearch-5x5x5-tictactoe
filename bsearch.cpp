#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#define SIZE 4
#define add(l, d) l.x += d[0]; l.y += d[1]
#define printm(m) printf("(%i,%i)\n",m.x,m.y)
#define printd(m) printf("(%i,%i)\n",m[0],m[1])
using namespace std;
class loc {
	public:
	int x;
	int y;
	loc() {
		x=y=0;
	}
};
class node {
	public:
	int state[SIZE][SIZE];
	vector<node> children;
	int visits;
	int wins;
	int winner;
	bool allowpass;
	node() {
		winner = visits = wins = 0;
	}
};
int dir[8][2] = {
	{-1, -1},
	{0, -1},
	{1, -1},
	{-1, 0},
	{1, 0},
	{-1, 1},
	{0, 1},
	{1, 1} };
void blank(int a[][SIZE]) {
	for(int i=0; i<SIZE; i++) for(int j=0; j<SIZE; j++)
		a[i][j] = 0;
	int s = (SIZE-1)/2;
	a[s][s] = a[s+1][s+1] = 1;
	a[s+1][s] = a[s][s+1] = -1;
}
void printb(int b[][SIZE]) {
	loc l;
	for(l.y=0; l.y<SIZE; l.y++) {
		for(l.x=0; l.x<SIZE; l.x++) {
			int c = b[l.x][l.y];
			if (c == 0)
				cout << ' ';
			else if (c == 1)
				cout << 'x';
			else
				cout << 'o';
		}
		cout << endl;
	}
}
bool _check(int x, int d) {
	return x+d >= 0 && x+d<SIZE;
}
bool check(loc l, int d[2]) {
	return _check(l.x, d[0]) && _check(l.y, d[1]);
}
void copy(int a[][SIZE], int b[][SIZE]) {
	loc l;
	for(l.x=0; l.x<SIZE; l.x++) for(l.y=0; l.y<SIZE; l.y++)
		b[l.x][l.y] = a[l.x][l.y];
}

vector<loc> moves(int b[][SIZE], int col, bool allowpass) {
	vector<loc> temp;
	loc l;
	for(l.x=0; l.x<SIZE; l.x++) for(l.y=0; l.y<SIZE; l.y++) {
		if (b[l.x][l.y] != 0) continue;
		for(int d=0; d<8; d++) {
			int c = 0;
			loc myl = l;
			while(1) {
				if (!check(myl,dir[d]))
					break;
				add(myl,dir[d]);
				int c2 = b[myl.x][myl.y];
				if (c2 == 0) {
					break;
				} else if (c2 == col) {
					if (c) {
						temp.push_back(l);
						d=8;
					}
					break;
				} else
					c = -col;
			}
		}
	}
	if (allowpass && temp.size() == 0) {
		loc pass;
		pass.x = -1;
		temp.push_back(pass);
	}
	return temp;
}

void play(int b[][SIZE], loc m, int col) {
	if (m.x == -1)
		return;
	b[m.x][m.y] = col;
	for(int d=0; d<8; d++) {
		loc myl = m;
		int c = 0;
		while(1) {
			if (!check(myl,dir[d]))
				break;
			add(myl,dir[d]);
			int c2 = b[myl.x][myl.y];
			if (c2 == 0)
				break;
			else if (c2 == col) {
				if (c) {
					myl = m;
					add(myl,dir[d]);
					while(b[myl.x][myl.y] == -col) {
						b[myl.x][myl.y] = col;
						add(myl,dir[d]);
					}
				}
				break;
			} else
				c = -col;
		}
	}
}

int score(int b[][SIZE], int c) {
	int s[3] = {0, 0, 0};
	for(int i=0; i<SIZE; i++) for(int j=0; j<SIZE; j++)
		s[b[i][j]+1]++;
//	if (s[1])
//		return -c;
	return s[2] + 7 > s[0] ? 1 : -1;
}

int playrandom(int b[][SIZE], int c) {
	//printf("playrandom\n");
	bool allowpass = true;
	while(1) {
		vector<loc> m = moves(b, c, allowpass);
		if (m.size() == 0)
			break;
		int i=rand() % m.size();
		play(b, m[i], c);
		allowpass = m[i].x != -1;
		c *= -1;
	}
	int r = score(b, c);
	//printb(b);
	//cout << r << " won" << endl;
	return r;
}

int depth=0;
int explore(node &n, int c) {
	depth++;
	int r;
	if (n.winner)
		r = n.winner;
	else if (n.visits == 0) {
		vector<loc> m = moves(n.state, c, n.allowpass);
		if (m.size()) {
			for(int i=0; i<m.size(); i++) {
				node newn;
				copy(n.state, newn.state);
				play(newn.state, m[i], c);
				newn.allowpass = m[i].x != -1;
				n.children.push_back(newn);
			}
			int b[SIZE][SIZE];
			copy(n.state, b);
			r = playrandom(b, c);
		} else {
			n.winner = r = score(n.state, c);
		}
		//printb(n.state);
		//cout << r << " won at depth " << depth << endl;
	} else {
		int maxi = -1;
		double max = -10000000;
		int winners = 0;
		int winneri = -1;
		for(int i=0; i<n.children.size(); i++) {
			double rat = n.children[i].visits ? (double)n.children[i].wins / n.children[i].visits : 1;
			if (n.children[i].winner == c)
				winneri = i;
			if (n.children[i].winner)
				winners++;
			double bonus = (double)n.children[i].visits / n.visits;
			double val = sqrt(rat) - log(bonus);
			if (val > max && !n.children[i].winner) {
				max = val;
				maxi = i;
			}
		}
		if (winneri > -1)
			winners = n.children.size();
		if (n.children.size() == winners) {
			if (winneri > -1) {
				cout << "Short circuit win to " << c << endl;
				printb(n.children[winneri].state);
				n.wins = n.visits;
				n.winner = r = c;
				n.children[0] = n.children[winneri];
			} else {
				cout << "Short circuit lose for " << c << endl;
				printb(n.state);
				n.winner = r = -c;
				n.wins = 0;
			}
			n.children.resize(1);
		} else {
			//cout << "pick node " << maxi << " / " << n.children.size() << endl;
			r = explore(n.children[maxi], -c);
		}
	}
	n.visits++;
	if (r == c)
		n.wins++;
	depth--;
	return r;
}

int main() {
	srand(time(NULL));
	node init;
	blank(init.state);
	printb(init.state);
	long wins=0, plays=0;
	while(1) {
		plays++;
		if (explore(init, 1) == 1)
			wins++;
		if (plays % 100 == 0)
			cout << wins << " / " << plays << " = " << (double)wins / plays << endl;
	}
}
