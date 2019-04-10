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

#include <stdint.h>
#include <cstring>
#if DEBUG_LOG
    #include <iostream>
#endif
#include <limits>
#include <cstddef>
#include <random>

int32_t get_winner (CardSuit first_suit, TrumpSuit trump_suit, uint8_t played_cards[4]);

//==============================================================================
//==============================================================================

Network *networks[4] = { nullptr };

//==============================================================================
// Returns the best cards to pass. out_cards must point to array of at least
// rule_pass_cards in size
//==============================================================================

Card ai_get_play_cards_ml (int32_t     rule_pass_cards,
                           int32_t     rule_no_trump_bidout,
                           int32_t     rule_minimum_bid,
    
                           int32_t     highest_bidder,
                           Bid         highest_bid,

                           Hand        *p0_played_cards,
                           Hand        *p1_played_cards,
                           Hand        *p2_played_cards,
                           Hand        *p3_played_cards,

                           Hand        *my_passed_cards,

                           Hand        *my_cards,
                           int32_t     my_index,
                           Network     *network = nullptr)
{

    // Load network
//    if (networks[my_index] == nullptr) {
//        networks[my_index] = new Network();
//        if (!load_network(std::string("net0")+std::to_string(my_index)+".bin", networks[my_index]))
//            throw AIException("rule_pass_cards out of range");
//    }

    // Validate inputs
    if (rule_pass_cards != 0 && rule_pass_cards != 1) {
        throw AIException("rule_pass_cards out of range");
    }

    if (rule_no_trump_bidout != 52 && rule_no_trump_bidout != 62) {
        throw AIException("rule_no_trump_bidout out of range");
    }

    if (rule_minimum_bid < 5 || rule_minimum_bid >8) {
        throw AIException("rule_minimum_bid out of range");
    }


    if (p0_played_cards->num_cards > 8) {
        throw AIException("Played cards out of range");
    }
    if (p1_played_cards->num_cards > 8) {
        throw AIException("Played cards out of range");
    }
    if (p2_played_cards->num_cards > 8) {
        throw AIException("Played cards out of range");
    }
    if (p3_played_cards->num_cards > 8) {
        throw AIException("Played cards out of range");
    }
    
    if (rule_pass_cards) {
        if (my_passed_cards->num_cards != 2) {
            throw AIException("Passed cards out of range");
        }
    }
    
    if (my_cards->num_cards > 8) {
        throw AIException("My cards out of range");
    }

    if (highest_bidder < 0 || highest_bidder > 3) {
        throw AIException("Highest bidder out of range");
    }
    if (my_index < 0 || my_index > 3) {
        throw AIException("Highest bidder out of range");
    }
    
    Card        first_card = 0;
    CardSuit    first_suit = card_to_suit (first_card);
    CardFace    first_face = card_to_face (first_card);
    
    //
    // Play the game forward from the start
    //
    
    uint8_t current_player = highest_bidder;
    uint8_t first_player = current_player;
    uint8_t num_played = 0;
    uint8_t played_cards[4] = { 0 };
    
    bool five_was_played = false;
    bool three_was_played = false;
    
    Hand p0_played_cards_copy = *p0_played_cards;
    Hand p1_played_cards_copy = *p1_played_cards;
    Hand p2_played_cards_copy = *p2_played_cards;
    Hand p3_played_cards_copy = *p3_played_cards;
    
    while (true) {
    
        // Get correct player
        Hand *p_played_cards;
        
        switch (current_player) {
            case 0: p_played_cards = &p0_played_cards_copy;
                    break;
            case 1: p_played_cards = &p1_played_cards_copy;
                    break;
            case 2: p_played_cards = &p2_played_cards_copy;
                    break;
            case 3: p_played_cards = &p3_played_cards_copy;
                    break;
            default:
                throw AIException("Internal error (1)");
        }
        
        // Check done
        if (p0_played_cards_copy.num_cards == 0 && 
            p1_played_cards_copy.num_cards == 0 && 
            p2_played_cards_copy.num_cards == 0 && 
            p3_played_cards_copy.num_cards == 0)
            break;
        
        // Record played card
        Card        played_card = p_played_cards->cards[0];
        CardSuit    played_suit = card_to_suit (played_card);

        played_cards[current_player] = played_card;
        
        // Remove the played cards
        remove_card (p_played_cards, 0);

        // Remember 3 or 5 played
        five_was_played = (played_card == card (CARD_5, SUIT_H));
        three_was_played = (played_card == card (CARD_3, SUIT_S));
        
        ++num_played;
        
        // First card played
        if (num_played == 1) {
            first_card = played_card;
            first_suit = card_to_suit (first_card);
            first_face = card_to_face (first_card);

        }
                
        // Check for winner and reset for next round
        if (num_played == 4) {
            current_player = get_winner(first_suit, highest_bid.trump, played_cards);
            first_player = current_player;
            
            first_card = 0;
            num_played = 0;
            played_cards[0] = played_cards[1] = played_cards[2] = played_cards[3] = 0;
        } else {
            // Player is done turn
            current_player = (current_player+1) % 4;
        }
        
    }   
    
    // Remove any cards that I cannot play
    if (first_card && count_suit_in_hand (my_cards, first_suit) > 0) {
        // Remove all but first_suit. Do it in reverse order because of the way deleting works
        for (int32_t c = my_cards->num_cards-1; c >= 0; --c) {
            if (card_to_suit(my_cards->cards[c]) != first_suit) {
                remove_card (my_cards, c);
            }
        }
    }
 
    //
    // AI Time
    //
    
    
    // Rotate played cards so p0 is me
    for (int i = 0; i < my_index; ++i) {
        Hand *temp = p0_played_cards;
        p0_played_cards = p1_played_cards;
        p1_played_cards = p2_played_cards;
        p2_played_cards = p3_played_cards;
        p3_played_cards = temp;
        
        uint8_t temp2 = played_cards[0];
        played_cards[0] = played_cards[1];
        played_cards[1] = played_cards[2];
        played_cards[2] = played_cards[3];
        played_cards[3] = temp2;
    }
    
    // Map parameters to inputs
    
    // 32 - my_cards
    // 32 - my_passed_cards
    // 
    // 32 - p0_played_cards (me)
    // 32 - p1_played_cards
    // 32 - p2_played_cards
    // 32 - p3_played_cards
    //
    // 32 - current_p1_played_cards
    // 32 - current_p2_played_cards
    // 32 - current_p3_played_cards
    // 
    // 1 - highest_bidder P0 (me)
    // 1 - highest_bidder P1
    // 1 - highest_bidder P2
    // 1 - highest_bidder P3
    // 
    // 1 - highest_bidder_bid_suit
    // 1 - highest_bidder_bid_suit
    // 1 - highest_bidder_bid_suit
    // 1 - highest_bidder_bid_suit
    // 1 - highest_bidder_bid_no
    // 1 - highest_bidder_bid_amount
    // 
    // 1 - rule_pass_cards
    // 1 - rule_no_trump_bidout
    // 1 - rule_minimum_bid
    
    float in[INPUT_SIZE] = { 0.0f };
    float out[OUTPUT_SIZE] = { 0.0f };
    
    for (uint8_t i = 0; i < my_cards->num_cards; ++i)
        in[card_to_bits(my_cards->cards[i]) + 32*0] = 1.0f;

    for (uint8_t i = 0; i < my_passed_cards->num_cards; ++i)
        in[card_to_bits(my_passed_cards->cards[i]) + 32*1] = 1.0f;

    for (uint8_t i = 0; i < p0_played_cards->num_cards; ++i)
        in[card_to_bits(p0_played_cards->cards[i]) + 32*2] = 1.0f;

    for (uint8_t i = 0; i < p1_played_cards->num_cards; ++i)
        in[card_to_bits(p1_played_cards->cards[i]) + 32*3] = 1.0f;

    for (uint8_t i = 0; i < p2_played_cards->num_cards; ++i)
        in[card_to_bits(p2_played_cards->cards[i]) + 32*4] = 1.0f;

    for (uint8_t i = 0; i < p3_played_cards->num_cards; ++i)
        in[card_to_bits(p3_played_cards->cards[i]) + 32*5] = 1.0f;

    if (played_cards[1])
      in[card_to_bits(played_cards[1]) + 32*6] = 1.0f;
    if (played_cards[2])
      in[card_to_bits(played_cards[2]) + 32*7] = 1.0f;
    if (played_cards[3])
      in[card_to_bits(played_cards[3]) + 32*8] = 1.0f;

    in[32*9+0] = (highest_bidder == 0) ? 1.0f : 0.0f;
    in[32*9+1] = (highest_bidder == 1) ? 1.0f : 0.0f;
    in[32*9+2] = (highest_bidder == 2) ? 1.0f : 0.0f;
    in[32*9+3] = (highest_bidder == 3) ? 1.0f : 0.0f;

    in[32*9+4] = (highest_bid.trump == TRUMP_H) ? 1.0f : 0.0f;
    in[32*9+5] = (highest_bid.trump == TRUMP_S) ? 1.0f : 0.0f;
    in[32*9+6] = (highest_bid.trump == TRUMP_D) ? 1.0f : 0.0f;
    in[32*9+7] = (highest_bid.trump == TRUMP_C) ? 1.0f : 0.0f;
    in[32*9+8] = (highest_bid.trump == TRUMP_N) ? 1.0f : 0.0f;
    in[32*9+9] = highest_bid.bid;
    
//    in[32*9+10] = rule_pass_cards;
//    in[32*9+11] = rule_no_trump_bidout;
//    in[32*9+12] = rule_minimum_bid;
    
    // Run NN
    evaluate(network ? network : networks[my_index], in, out);
    
    // Find highest non-zero card
    float max_val = -std::numeric_limits<float>::infinity();
    int8_t max_index = -1;
    for (int oo = 0; oo < OUTPUT_SIZE; ++oo) {
        if (out[oo] != 0.0f && out[oo] > max_val && index_of(my_cards, bits_to_card(oo)) >= 0) {
            max_val = out[oo];
            max_index = oo;
        }
    }
    
    // Check for errors, return first card
    if (max_index < 0 || max_index >= OUTPUT_SIZE)
        return my_cards->cards[0];
    
    // Return highest ranking card
    return bits_to_card(max_index);
}

//==============================================================================
//==============================================================================
