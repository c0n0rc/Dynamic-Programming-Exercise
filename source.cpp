#include <string.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <sstream>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>   
#include <unistd.h>

using namespace std;

typedef struct card_details {
	string name;
	int sale_price;
	int market_price;
	int profit;
	card_details(int i) : sale_price(i), market_price(i), profit(i) { }
} card;

//=======GLOBALS========\\
extern int C;
extern int N;
int C = 0;
int N = 0;


string calc_algo_time(timeval &starttime, timeval &endtime);
int compare_prices(vector<card> &market_cards, vector<card> &available_cards, int index);
void dynamic_prog(ofstream &file, vector<card> &cards);
void output_results(ofstream &file, int &profit, timeval &starttime, timeval &endtime);

int main (int argc, char* argv[]) {
	int market_cards   = 0,
		spending_money = 0,
		element 	   = 0,
		input_size     = 0;
	string market_list = "",
		output_file	   = "",
		price_list     = "",
		algo_choice    = "",
		buff           = "";
	vector<card> cards, market_deck;

	//command line: $> ./program4 –m <market-price-file> -p <price-list-file> -o <output-file-name> [3]
	if (argc != 8) {
		cout << "Incorrect command line args." << endl;
		return 1;
	}

	market_list = argv[2];
	price_list = argv[4];
	output_file = argv[6];
	algo_choice =argv[7];
	element = atoi(algo_choice.c_str());
	if (element != 3) {
		cout << "Incorrect command line args." << endl;
		return 1;
	}

	ifstream marketfile;
	marketfile.open(market_list.c_str());
	ifstream pricefile;
	pricefile.open(price_list.c_str());
	ofstream output;
	output.open(output_file.c_str());
	
	if(!(output.is_open())) {
		cout << "Error opening output file." << endl;
		return 1;
	}

	if(!(pricefile.is_open() || marketfile.is_open())) {
		cout << "File name provided does not exist." << endl;
		return 1;
	}

	int firstrun = 1;
	int indexcount = 0;
	int linecount = 0;
   	stringstream linestream;
	while (getline(marketfile, buff)) {
    	linestream.str(buff);
    	if (linecount == 0) {
    		linestream >> market_cards;
    		linestream.clear();
    	 	linecount++;
    	} else {
			card market_card(0);
	     	linestream >> market_card.name >> market_card.market_price;
	     	market_deck.push_back(market_card);
	     	linestream.clear();
	       	linecount++;
		}  
	}	

	//get all cards per problem. call dynamic_prog.
	linecount = 0;
	while (getline(pricefile, buff)) {
    	linestream.str(buff);
    	if (linecount == 0) {
    		linestream >> input_size >> spending_money;
    		C = spending_money;
    		N = input_size;
    		linestream.clear();
    	 	linecount++;
    	} else {
	    	card baseball(0);
	     	linestream >> baseball.name >> baseball.sale_price;
	     	linestream.clear();
	    	cards.push_back(baseball);
	       	linecount++;
			if (linecount == input_size + 1) {			
				if (!firstrun) indexcount+= input_size-1;
				if (compare_prices(market_deck, cards, indexcount)) break;
				dynamic_prog(output, cards);
				linecount = 0;
				cards.clear();
				firstrun = 0;
			}
		}  
	}
	output.close();
	marketfile.close();
	pricefile.close();
	return 0;
}

// *** DYNAMIC PROGRAMMING CODE STARTS HERE *** 
//Implements  algorithm described in  class lecture notes.  Stores only two rows of the dynamic programming table in memory
void dynamic_prog(ofstream &file, vector<card> &cards) {
	int table[2][C+1];
	int p = 0;
	struct timeval starttime, endtime;
	gettimeofday(&starttime, 0);
	for (int i = 0; i <= C; i++) {
		table[0][i] = 0;				//initialize first row to 0
	}
	int index = 1;
	for (int i = 0; i < N; i++) {
		if (index > 0 && index % 2 == 0) index = 0;
		table[index][0] = 0;			//initialize first column to 0
		for (int j = 1; j <= C; j++) {
			if (cards.at(i).sale_price <= j) {
				if (index == 0) {
					table[index][j] = max((table[index+1][j-cards.at(i).sale_price] + cards.at(i).profit), table[index+1][j]);
				} else { 
					table[index][j] = max((table[index-1][j-cards.at(i).sale_price] + cards.at(i).profit), table[index-1][j]);
				}
			} else { 
				if (index == 0){ 
					table[index][j] = table[index+1][j];   
				} else {
					table[index][j] = table[index-1][j];   
				}
			}
		}
		index++;
	}
	p = table[index-1][C];
	gettimeofday(&endtime, 0);
	output_results(file, p, starttime, endtime);
}

void output_results(ofstream &file, int &profit, timeval &starttime, timeval &endtime) {
	time_t sec = endtime.tv_sec - starttime.tv_sec;
	double microsec = (endtime.tv_usec - starttime.tv_usec);
	file << "Dynamic Programming: "  << N << " " << profit << " " <<  sec << "." << abs(microsec) << endl;
	cout << "Dynamic Programming: "  << N << " " << profit << " " <<  sec << "." << abs(microsec) << endl;
}

//matches the cards in the price list with cards in the market list.
int compare_prices(vector<card> &market_cards, vector<card> &cards, int index) {
	int i = 0;
	while (i < cards.size()) {
		if (cards.at(i).name == (market_cards.at(index)).name ) {
			(cards.at(i)).market_price = (market_cards.at(index)).market_price;
			(cards.at(i)).profit = ( (market_cards.at(index)).market_price - (cards.at(i)).sale_price );
			index++;
			i++;
		} else {
			cout << "Error: " << (cards.at(i)).name << " not found in market file. " << endl;
			return 1;	
		}
	}
	return 0;
}

