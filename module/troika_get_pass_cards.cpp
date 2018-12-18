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
#if DEBUG_LOG
    #include <iostream>
#endif
#include <limits>

//==============================================================================
// Returns the best cards to pass. out_cards must point to array of at least
// 2 cards in size
//==============================================================================

void ai_get_pass_cards (int32_t     rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        Hand        *my_cards,
                        Card        *out_cards)
{
    // Validate inputs
    if (rule_pass_cards != 1) {
        throw AIException("rule_pass_cards out of range");
    }

    if (rule_no_trump_bidout != 52 && rule_no_trump_bidout != 62) {
        throw AIException("rule_no_trump_bidout out of range");
    }

    if (rule_minimum_bid < 5 || rule_minimum_bid >8) {
        throw AIException("rule_minimum_bid out of range");
    }


#if DEBUG_LOG
    std::cout << "my_cards: ";
    for (int32_t i = 0; i < my_cards->num_cards; ++i) {
        std::cout << card_name(my_cards->cards[i]) << " ";
    }
    std::cout << std::endl;
#endif

    // Candidates
    Hand candidates;
    candidates.num_cards = 0;

    //
    // Check for any loners in a certain suit
    //

    int32_t suit_counts[NUM_SUITS];
    count_suits_in_hand (my_cards, suit_counts);
    
#if DEBUG_LOG
    std::cout << "Num Hearts:   " << suit_counts[SUIT_H] << std::endl;
    std::cout << "Num Spades:   " << suit_counts[SUIT_S] << std::endl;
    std::cout << "Num Diamonds: " << suit_counts[SUIT_D] << std::endl;
    std::cout << "Num Clubs:    " << suit_counts[SUIT_C] << std::endl;
#endif

    for (int32_t s = 0; s < NUM_SUITS; ++s) {

        if (suit_counts[s] == 2) {
            int32_t index = first_index_of (my_cards, (CardSuit) s);    // Get the first of the two
            add_card(&candidates, get(my_cards,index));                 // Add to a new hand for storage
            remove_card(my_cards, index);                               // Remove it from current hand

            index = first_index_of (my_cards, (CardSuit) s);            // Get the second of the two
            add_card(&candidates, get(my_cards,index));                 // Add to a new hand for storage
            remove_card(my_cards, index);                               // Remove it from current hand
            break;
            
        } else if (suit_counts[s] == 1) {
            int32_t index = first_index_of (my_cards, (CardSuit) s);    // Get the loner
            add_card(&candidates, get(my_cards,index));                 // Add to a new hand for storage
            remove_card(my_cards, index);                               // Remove it from current hand
        }

    }
    
    while (candidates.num_cards < 2) {
    
        float value = std::numeric_limits<float>::infinity();
        int32_t value_i = 0;
        
        for (int32_t i = 0; i < my_cards->num_cards; ++i) {
            //CardSuit card_suit = card_to_suit (my_cards->cards[i]);
            CardFace card_face = card_to_face (my_cards->cards[i]);
            float test_value = card_face;
            if (test_value < value) {
                value = test_value;
                value_i = i;
            }
        }
    
        add_card(&candidates, get(my_cards,value_i));                 // Add to a new hand for storage
        remove_card(my_cards, value_i);                               // Remove it from current hand
    }
    
    // Copy card results
    for (int32_t i = 0; i < 2; ++i)
        out_cards[i] = candidates.cards[i];

}

//==============================================================================
//==============================================================================
