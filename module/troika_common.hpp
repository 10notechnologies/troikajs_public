#pragma once
//==============================================================================
//
// Copyright (c) 2015 Smells Like Donkey Software Inc.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//==============================================================================

#include <stdint.h>
#include <string>
#include <stdexcept>

//==============================================================================
//==============================================================================

#ifndef DEBUG_LOG
    #define DEBUG_LOG 0
#endif

//==============================================================================
//==============================================================================

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

//==============================================================================
//==============================================================================

#define RULE_PASS_CARDS 0
#define RULE_NO_TRUMP_BIDOUT 52
#define RULE_MINIMUM_BID 7

//==============================================================================
//==============================================================================

enum CardFace {
    CARD_2 = 2,
    CARD_3 = 3,
    CARD_4 = 4,
    CARD_5 = 5,
    CARD_6 = 6,
    CARD_7 = 7,
    CARD_8 = 8,
    CARD_9 = 9,
    CARD_T = 10,
    CARD_J = 11,
    CARD_Q = 12,
    CARD_K = 13,
    CARD_A = 14
};

enum CardSuit {
    SUIT_H = 0,
    SUIT_S = 1,
    SUIT_D = 2,
    SUIT_C = 3,
    NUM_SUITS = 4
};

enum TrumpSuit {
    TRUMP_H = 0,
    TRUMP_S = 1,
    TRUMP_D = 2,
    TRUMP_C = 3,
    TRUMP_N = 4,
    TRUMP_UNKNOWN = 5
};

// Types
typedef uint8_t Card;

struct Hand {
    uint8_t     num_cards;
    Card        cards[8];
};

struct Bid {
    Bid() {}
    Bid(uint8_t bid_, TrumpSuit trump_)
        :   bid (bid_),
            trump (trump_)
        {}

    uint8_t     bid;
    TrumpSuit   trump;
};

inline bool operator == (const Bid &a, const Bid &b) {
    return a.bid == b.bid && a.trump == b.trump;
}

// Constants
const uint32_t NUM_CARDS_IN_HAND = 8;

//==============================================================================
// Utility functions
//==============================================================================

inline Card card (CardFace cf, CardSuit cs) 
{
    return (uint8_t) cf << 4 | (uint8_t) cs;
}

inline CardFace card_to_face (Card c)
{
    return (CardFace) (c >> 4);
}

inline CardSuit card_to_suit (Card c)
{
    return (CardSuit) (c & 0x0F);
}

//==============================================================================
//==============================================================================


// Returns index of a certain card
int32_t index_of (Hand *hand, Card c);

// Checks if card in hand
inline bool has_card (Hand *hand, Card c)  {   return index_of(hand, c) >= 0; }

// Returns index of the first card of a certain suit
int32_t first_index_of (Hand *hand, CardSuit c);

// Adds a card to the hand
void add_card (Hand *hand, Card c);

// Removes a card to the hand
void remove_card (Hand *hand, int32_t index);

// Removes a card to the hand
inline Card get (Hand *hand, int32_t index) {   return hand->cards[index];  }

//==============================================================================
//==============================================================================

Card chars_to_card (const char *c);
void card_to_chars (Card c, char *css);

Bid chars_to_bid (const char *c);

TrumpSuit char_to_trump (char c);
void trump_to_char (TrumpSuit cs, char *css);

CardSuit char_to_card_suit (char c);
void card_suit_to_char (CardSuit cs, char *css);

std::string hand_to_string (Hand *h);
std::string card_name (Card c);
std::string trump_name (TrumpSuit c);
std::string suit_name (CardSuit c);

//==============================================================================
//==============================================================================

Card bits_to_card(uint8_t b);
uint8_t card_to_bits(Card c);

//==============================================================================
//==============================================================================

int32_t count_suit_in_hand (Hand *hand, CardSuit s);
void count_suits_in_hand (Hand *hand, int32_t suits[4]);
float weighted_sum_for_suit(CardSuit suit, Hand *my_cards);

//==============================================================================
//==============================================================================

class AIException: public std::runtime_error {
    public:
        AIException (const std::string &what)
            :   runtime_error (what)
        {}

        AIException (const char *what)
            :   runtime_error (what)
        {}

};

//==============================================================================
// Neural network types
//==============================================================================

#define MUTATION_RATE 0.015f
#define MUTATION_SCALE 1.0f

#define INPUT_SIZE 298
#define INTERNAL_SIZE 128
#define OUTPUT_SIZE 32

struct Weight {
    float w;
    float b;
};

struct Network {  // [input/height][output/width]
    Weight l0[INPUT_SIZE][INTERNAL_SIZE];
    Weight l1[INTERNAL_SIZE][INTERNAL_SIZE];
    Weight l2[INTERNAL_SIZE][INTERNAL_SIZE];
    Weight l3[INTERNAL_SIZE][OUTPUT_SIZE];
};

void init_network(Network *n);
void save_network(const std::string &path, Network *n);
bool load_network(const std::string &path, Network *n);

void evaluate(Network *n, float *in, float *out);

void breed(Network *n_out, Network *n1, Network *n2);
void initialize_layer(Weight *l_out, int w, int h);

//==============================================================================
//==============================================================================
