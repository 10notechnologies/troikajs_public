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

#include "troika_common.hpp"
#include <cstring>
#include <random>
#include <fstream>
#if DEBUG_LOG
    #include <iostream>
#endif

//==============================================================================
//==============================================================================

const float BID_WEIGHTS[] = {
    0.0f,   // 0 - filler
    0.0f,   // 1 - filler
    0.0f,   // 2
    0.00153846153846154f,
    0.00615384615384615f,
    0.0138461538461538f,
    0.0246153846153846f,
    0.0384615384615385f,
    0.0553846153846154f,
    0.0753846153846154f,
    0.0984615384615385f,
    0.124615384615385f,
    0.153846153846154f,
    0.186153846153846f,
    0.221538461538462f
};

const float BID_MUL = 6.5f;

//==============================================================================
//==============================================================================

int32_t count_suit_in_hand (Hand *hand, CardSuit s)
{
    int32_t n = 0;

    for (int32_t i = 0; i < hand->num_cards; ++i) {
        if (card_to_suit(hand->cards[i]) == s)
            ++n;
    }

    return n;
}

void count_suits_in_hand (Hand *hand, int32_t suits[4])
{
    suits[0] = suits[1] = suits[2] = suits[3] = 0;
    
    for (int32_t i = 0; i < hand->num_cards; ++i) {
        ++suits[card_to_suit(hand->cards[i])];
    }

}

float weighted_sum_for_suit(CardSuit suit, Hand *my_cards)
{
    float sum = 0.0F;
    
    for (uint8_t i = 0; i < my_cards->num_cards; ++i) {
        if (card_to_suit(my_cards->cards[i]) == suit)
            sum += BID_WEIGHTS[(uint32_t) card_to_face(my_cards->cards[i])] * BID_MUL;
    }
    
    return sum;
}

//==============================================================================
//==============================================================================

int32_t index_of (Hand *hand, Card c)
{
    for (int32_t i = 0; i < hand->num_cards; ++i) {
        if (c == hand->cards[i])
            return i;
    }

    return -1;
}

int32_t first_index_of (Hand *hand, CardSuit c)
{
    for (int32_t i = 0; i < hand->num_cards; ++i) {
        if (c == card_to_suit(hand->cards[i]))
            return i;
    }

    return -1;
}

//==============================================================================
//==============================================================================

void add_card (Hand *hand, Card c)
{
    if (hand->num_cards >= NUM_CARDS_IN_HAND)
        throw AIException("Too many cards in hand");
    
    hand->cards[hand->num_cards] = c;
    ++hand->num_cards;
}

void remove_card (Hand *hand, int32_t index)
{
    if (index >= hand->num_cards)
        throw AIException("index out of range");

    for (int32_t i = index; i < (hand->num_cards-1); ++i) {
        hand->cards[i] = hand->cards[i+1];
    }
    
    --hand->num_cards;
}

//==============================================================================
//==============================================================================

std::string hand_to_string (Hand *h)
{
    std::string s;
    
    for (uint32_t c = 0; c < h->num_cards; ++c) {
        if (c != 0)
            s+= ", ";
        s += card_name(h->cards[c]);
    }
    
    return s;
}

//==============================================================================
//==============================================================================

std::string card_name (Card c)
{
    std::string s;
    
    CardFace cf = card_to_face(c);
    CardSuit cs = card_to_suit(c);
    
    switch (cf) {
        case CARD_2:    s += "2";   break;
        case CARD_3:    s += "3";   break;
        case CARD_4:    s += "4";   break;
        case CARD_5:    s += "5";   break;
        case CARD_6:    s += "6";   break;
        case CARD_7:    s += "7";   break;
        case CARD_8:    s += "8";   break;
        case CARD_9:    s += "9";   break;
        case CARD_T:    s += "T";   break;
        case CARD_J:    s += "J";   break;
        case CARD_Q:    s += "Q";   break;
        case CARD_K:    s += "K";   break;
        case CARD_A:    s += "A";   break;
        default:
            throw AIException("Invalid Card");
    };
    
    switch (cs) {
        case SUIT_H:   s += "\u2665";    break;
        case SUIT_S:   s += "\u2660";    break;
        case SUIT_D:   s += "\u2666";    break;
        case SUIT_C:   s += "\u2663";    break;
        default:
            throw AIException("Invalid Card");
    };
    
    return s;
}

//==============================================================================
//==============================================================================

std::string trump_name (TrumpSuit c)
{
    std::string s;
    
    switch (c) {
        case TRUMP_H:  s += "\u2665";  break;
        case TRUMP_S:  s += "\u2660";  break;
        case TRUMP_D:  s += "\u2666";  break;
        case TRUMP_C:  s += "\u2663";  break;
        case TRUMP_N:  s += "No";      break;
        case TRUMP_UNKNOWN: s += "Unknown"; break;
        default:
            throw AIException("Invalid trump Suit");
    };
    
    return s;
}

//==============================================================================
//==============================================================================

std::string suit_name (CardSuit c)
{
    std::string s;
    
    switch (c) {
        case SUIT_H:   s += "\u2665";  break;
        case SUIT_S:   s += "\u2660";  break;
        case SUIT_D:   s += "\u2666";  break;
        case SUIT_C:   s += "\u2663";  break;
        default:
            throw AIException("Invalid card Suit");
    };
    
    return s;
}

//==============================================================================
//==============================================================================

Card chars_to_card (const char *c)
{
    if (!c) {
        throw AIException("Card string must not be NULL");
    }

    size_t len = ::strlen(c);
    if (len != 2) {
        throw AIException("Card string must have length of two characters");
    }

    char cf_c = c[0];
    char cs_c = c[1];
    
    CardFace cf;
    CardSuit cs;

    switch (cf_c) {
        case '2':   cf = CARD_2;    break;
        case '3':   cf = CARD_3;    break;
        case '4':   cf = CARD_4;    break;
        case '5':   cf = CARD_5;    break;
        case '6':   cf = CARD_6;    break;
        case '7':   cf = CARD_7;    break;
        case '8':   cf = CARD_8;    break;
        case '9':   cf = CARD_9;    break;
        case 'T':   cf = CARD_T;   break;
        case 'J':   cf = CARD_J;    break;
        case 'Q':   cf = CARD_Q;    break;
        case 'K':   cf = CARD_K;    break;
        case 'A':   cf = CARD_A;    break;
        default:
            throw AIException("Invalid card string");
    };

    switch (cs_c) {
        case 'H':   cs = SUIT_H;   break;
        case 'S':   cs = SUIT_S;   break;
        case 'D':   cs = SUIT_D; break;
        case 'C':   cs = SUIT_C;    break;
        default:
            throw AIException("Invalid card string");
    };
    
    return card(cf, cs);
}

void card_to_chars (Card c, char *css)
{    
    CardFace cf = card_to_face(c);
    CardSuit cs = card_to_suit(c);
  
#if DEBUG_LOG
    std::cout << "CardFace: " << (int) cf << std::endl;
    std::cout << "CardSuit: " << (int) cs << std::endl;
#endif

    switch (cf) {
        case CARD_2:    css[0] = '2';    break;
        case CARD_3:    css[0] = '3';    break;
        case CARD_4:    css[0] = '4';    break;
        case CARD_5:    css[0] = '5';    break;
        case CARD_6:    css[0] = '6';    break;
        case CARD_7:    css[0] = '7';    break;
        case CARD_8:    css[0] = '8';    break;
        case CARD_9:    css[0] = '9';    break;
        case CARD_T:    css[0] = 'T';    break;
        case CARD_J:    css[0] = 'J';    break;
        case CARD_Q:    css[0] = 'Q';    break;
        case CARD_K:    css[0] = 'K';    break;
        case CARD_A:    css[0] = 'A';    break;
        default:
            throw AIException("Invalid card");
    };

    switch (cs) {
        case SUIT_H:    css[1] = 'H';   break;
        case SUIT_S:    css[1] = 'S';   break;
        case SUIT_D:    css[1] = 'D';   break;
        case SUIT_C:    css[1] = 'C';   break;
        default:
            throw AIException("Invalid card");
    };
    
}

//==============================================================================
//==============================================================================

Bid chars_to_bid (const char *c)
{
    if (!c) {
        throw AIException("Bid string must not be NULL");
    }

    size_t len = ::strlen(c);
    
    if (len == 4 && c[0] == 'p' && c[1] == 'a' && c[2] == 's' && c[3] == 's') {
        Bid bid;
        bid.bid = 0;
        bid.trump = TRUMP_UNKNOWN;
        
        return bid;
        
    } else {
        if (len != 1 && len != 2 && len != 3) {
            throw AIException("Bid string must have length of two or three characters.");
        }

        Bid bid;

        // Check if 2 digits for bid
        if (c[0] == '1' && (len == 2 || len == 3)) {
            bid.bid = (c[0] - '0') * 10 + (c[1] - '0');
            if (len == 3)
                bid.trump = char_to_trump(c[2]);
            else
                bid.trump = TRUMP_UNKNOWN;
        } else if (len == 1 || len == 2) {
            bid.bid = c[0] - '0';
            if (len == 2)
                bid.trump = char_to_trump(c[1]);
            else
                bid.trump = TRUMP_UNKNOWN;
        } else {
            throw AIException("Bid formatted incorrectly");
        }
        
        if (bid.bid < 0 || bid.bid > 12) {
            throw AIException("Bid amount not valid");
        }
        
        return bid;
    }
}

//==============================================================================
//==============================================================================

TrumpSuit char_to_trump (char c)
{
    switch (c) {
        case 'H':   return TRUMP_H;
        case 'S':   return TRUMP_S;
        case 'D':   return TRUMP_D;
        case 'C':   return TRUMP_C;
        case 'N':   return TRUMP_N;
        default:
            throw AIException("Invalid trump string");
    }
    
}

void trump_to_char (TrumpSuit ts, char *css)
{
    switch (ts) {
        case TRUMP_H:   css[0] = 'H'; break;
        case TRUMP_S:   css[0] = 'S'; break;
        case TRUMP_D:   css[0] = 'D'; break;
        case TRUMP_C:   css[0] = 'C'; break;
        case TRUMP_N:   css[0] = 'N'; break;
        default:
            throw AIException("Invalid trump character");
    };
}

//==============================================================================
//==============================================================================

CardSuit char_to_card_suit (char c)
{
    switch (c) {
        case 'H':   return SUIT_H;
        case 'S':   return SUIT_S;
        case 'D':   return SUIT_D;
        case 'C':   return SUIT_C;
        default:
            throw AIException("Invalid trump string");
    }
}

void card_suit_to_char (CardSuit cs, char *css)
{
    switch (cs) {
        case SUIT_H:  css[0] = 'H'; break;
        case SUIT_S:  css[0] = 'S'; break;
        case SUIT_D:css[0] = 'D'; break;
        case SUIT_C:   css[0] = 'C'; break;
        default:
            throw AIException("Invalid trump character");
    };
}

//==============================================================================
//==============================================================================

uint8_t card_to_bits(Card c)
{
    switch (c) {
        case (CARD_3 << 4 | SUIT_S):  return 0;
        case (CARD_5 << 4 | SUIT_H):  return 1;
        case (CARD_7 << 4 | SUIT_C):  return 2;
        case (CARD_7 << 4 | SUIT_D):  return 3;
        
        case (CARD_8 << 4 | SUIT_H):  return 4;
        case (CARD_8 << 4 | SUIT_S):  return 5;
        case (CARD_8 << 4 | SUIT_D):  return 6;
        case (CARD_8 << 4 | SUIT_C):  return 7;

        case (CARD_9 << 4 | SUIT_H):  return 8;
        case (CARD_9 << 4 | SUIT_S):  return 9;
        case (CARD_9 << 4 | SUIT_D):  return 10;
        case (CARD_9 << 4 | SUIT_C):  return 11;

        case (CARD_T << 4 | SUIT_H):  return 12;
        case (CARD_T << 4 | SUIT_S):  return 13;
        case (CARD_T << 4 | SUIT_D):  return 14;
        case (CARD_T << 4 | SUIT_C):  return 15;

        case (CARD_J << 4 | SUIT_H):  return 16;
        case (CARD_J << 4 | SUIT_S):  return 17;
        case (CARD_J << 4 | SUIT_D):  return 18;
        case (CARD_J << 4 | SUIT_C):  return 19;

        case (CARD_Q << 4 | SUIT_H):  return 20;
        case (CARD_Q << 4 | SUIT_S):  return 21;
        case (CARD_Q << 4 | SUIT_D):  return 22;
        case (CARD_Q << 4 | SUIT_C):  return 23;

        case (CARD_K << 4 | SUIT_H):  return 24;
        case (CARD_K << 4 | SUIT_S):  return 25;
        case (CARD_K << 4 | SUIT_D):  return 26;
        case (CARD_K << 4 | SUIT_C):  return 27;

        case (CARD_A << 4 | SUIT_H):  return 28;
        case (CARD_A << 4 | SUIT_S):  return 29;
        case (CARD_A << 4 | SUIT_D):  return 30;
        case (CARD_A << 4 | SUIT_C):  return 31;
    }
    
    return 0;
}

Card bits_to_card(uint8_t b)
{
    switch (b) {
        case 0: return (CARD_3 << 4 | SUIT_S);
        case 1: return (CARD_5 << 4 | SUIT_H);
        case 2: return (CARD_7 << 4 | SUIT_C);
        case 3: return (CARD_7 << 4 | SUIT_D);
        
        case 4: return (CARD_8 << 4 | SUIT_H);
        case 5: return (CARD_8 << 4 | SUIT_S);
        case 6: return (CARD_8 << 4 | SUIT_D);
        case 7: return (CARD_8 << 4 | SUIT_C);

        case 8: return (CARD_9 << 4 | SUIT_H);
        case 9: return (CARD_9 << 4 | SUIT_S);
        case 10: return (CARD_9 << 4 | SUIT_D);
        case 11: return (CARD_9 << 4 | SUIT_C);

        case 12: return (CARD_T << 4 | SUIT_H);
        case 13: return (CARD_T << 4 | SUIT_S);
        case 14: return (CARD_T << 4 | SUIT_D);
        case 15: return (CARD_T << 4 | SUIT_C);

        case 16: return (CARD_J << 4 | SUIT_H);
        case 17: return (CARD_J << 4 | SUIT_S);
        case 18: return (CARD_J << 4 | SUIT_D);
        case 19: return (CARD_J << 4 | SUIT_C);

        case 20: return (CARD_Q << 4 | SUIT_H);
        case 21: return (CARD_Q << 4 | SUIT_S);
        case 22: return (CARD_Q << 4 | SUIT_D);
        case 23: return (CARD_Q << 4 | SUIT_C);

        case 24: return (CARD_K << 4 | SUIT_H);
        case 25: return (CARD_K << 4 | SUIT_S);
        case 26: return (CARD_K << 4 | SUIT_D);
        case 27: return (CARD_K << 4 | SUIT_C);

        case 28: return (CARD_A << 4 | SUIT_H);
        case 29: return (CARD_A << 4 | SUIT_S);
        case 30: return (CARD_A << 4 | SUIT_D);
        case 31: return (CARD_A << 4 | SUIT_C);
    }
    
    return 0;
}

//==============================================================================
// Load and save NN's
//==============================================================================

void initialize_layer(Weight *l_out, int w, int h)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    int n = w*h;
    for (int nn = 0; nn < n; ++n) {
        l_out[nn].w = (dis(gen) - 0.5f) * MUTATION_SCALE;
        l_out[nn].b = (dis(gen) - 0.5f) * MUTATION_SCALE;
    }
}

void init_network(Network *n)
{
    initialize_layer(&(n->l0[0][0]), INPUT_SIZE, INTERNAL_SIZE);
    initialize_layer(&(n->l1[0][0]), INTERNAL_SIZE, INTERNAL_SIZE);
    initialize_layer(&(n->l2[0][0]), INTERNAL_SIZE, INTERNAL_SIZE);
    initialize_layer(&(n->l3[0][0]), INTERNAL_SIZE, INTERNAL_SIZE);
    initialize_layer(&(n->l4[0][0]), INTERNAL_SIZE, OUTPUT_SIZE);
}

void save_network(const std::string &path, Network *n)
{
    std::ofstream file = std::ofstream(path, std::ios::out | std::ios::binary);
    file.write((char*)n,sizeof(Network));
}

bool load_network(const std::string &path, Network *n)
{
    std::ifstream file = std::ifstream(path, std::ios::in | std::ios::binary);
    if (file.bad())
        return false;
    
    file.seekg (0, std::ios::end);
    uint32_t length = file.tellg();
    file.seekg (0, std::ios::beg);
    
    if (length != sizeof(Network)) {
        return false;
    }
    
    file.read ((char*)n,sizeof(Network));
    return true;
}

//==============================================================================
// Breeding functions
//==============================================================================

void breed_layer(Weight *l_out, Weight *l1, Weight *l2, int w, int h)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    int n = w*h;
    for (int nn = 0; nn < n; ++n) {
        
        if (dis(gen) <= MUTATION_RATE) {
            l_out[nn].w += (dis(gen) - 0.5f) * MUTATION_SCALE;
            l_out[nn].b += (dis(gen) - 0.5f) * MUTATION_SCALE;
        } else {
            if (dis(gen) >= 0.5f) {
                l_out[nn].w = l1[nn].w;
                l_out[nn].b = l1[nn].b;
            } else {
                l_out[nn].w = l2[nn].w;
                l_out[nn].b = l2[nn].b;
            }
        }
    }
}

void breed(Network *n_out, Network *n1, Network *n2)
{
    breed_layer(&(n_out->l0[0][0]), &(n1->l0[0][0]), &(n2->l0[0][0]), INPUT_SIZE, INTERNAL_SIZE);
    breed_layer(&(n_out->l1[0][0]), &(n1->l1[0][0]), &(n2->l1[0][0]), INTERNAL_SIZE, INTERNAL_SIZE);
    breed_layer(&(n_out->l2[0][0]), &(n1->l2[0][0]), &(n2->l2[0][0]), INTERNAL_SIZE, INTERNAL_SIZE);
    breed_layer(&(n_out->l3[0][0]), &(n1->l3[0][0]), &(n2->l3[0][0]), INTERNAL_SIZE, INTERNAL_SIZE);
    breed_layer(&(n_out->l4[0][0]), &(n1->l4[0][0]), &(n2->l4[0][0]), INTERNAL_SIZE, OUTPUT_SIZE);
}

//==============================================================================
// Evaluation functions
//==============================================================================

void evaluate_layer(Weight *l, float *in, float *out, int w, int h)
{
    ::memset(out, 0, sizeof(float) * w);

    // Convolve NN
    for (int hh = 0; hh < h; ++hh) {        
        for (int ww = 0; ww < w; ++ww) {
            out[ww] += in[hh] * l[hh * w + ww].w;
        }
    }

    // Bias NN
    for (int hh = 0; hh < h; ++hh) {        
        for (int ww = 0; ww < w; ++ww) {
            out[ww] += l[hh * w + ww].b;
        }
    }
    
    // Activation functions
    for (int ww = 0; ww < w; ++ww) {
        out[ww] = 1.0f / (1.0f + std::exp(-out[ww]));
    }
}

void evaluate(Network *n, float *in, float *out)
{
    float out_0[INTERNAL_SIZE];
    evaluate_layer(&(n->l0[0][0]), in, out_0, INPUT_SIZE, INTERNAL_SIZE);
    
    float out_1[INTERNAL_SIZE];
    evaluate_layer(&(n->l1[0][0]), out_0, out_1, INTERNAL_SIZE, INTERNAL_SIZE);

    float out_2[INTERNAL_SIZE];
    evaluate_layer(&(n->l2[0][0]), out_1, out_2, INTERNAL_SIZE, INTERNAL_SIZE);

    float out_3[INTERNAL_SIZE];
    evaluate_layer(&(n->l3[0][0]), out_2, out_3, INTERNAL_SIZE, INTERNAL_SIZE);

    evaluate_layer(&(n->l4[0][0]), out_3, out, INTERNAL_SIZE, OUTPUT_SIZE);
}

//==============================================================================
//==============================================================================

