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

//==============================================================================
//==============================================================================

uint8_t VALID_INDICES[] = {
    card(CARD_3, SUIT_S),
    card(CARD_8, SUIT_S),
    card(CARD_9, SUIT_S),
    card(CARD_T, SUIT_S),
    card(CARD_J, SUIT_S),
    card(CARD_Q, SUIT_S),
    card(CARD_K, SUIT_S),
    card(CARD_A, SUIT_S),
    
    card(CARD_5, SUIT_H),
    card(CARD_8, SUIT_H),
    card(CARD_9, SUIT_H),
    card(CARD_T, SUIT_H),
    card(CARD_J, SUIT_H),
    card(CARD_Q, SUIT_H),
    card(CARD_K, SUIT_H),
    card(CARD_A, SUIT_H),

    card(CARD_7, SUIT_D),
    card(CARD_8, SUIT_D),
    card(CARD_9, SUIT_D),
    card(CARD_T, SUIT_D),
    card(CARD_J, SUIT_D),
    card(CARD_Q, SUIT_D),
    card(CARD_K, SUIT_D),
    card(CARD_A, SUIT_D),

    card(CARD_7, SUIT_C),
    card(CARD_8, SUIT_C),
    card(CARD_9, SUIT_C),
    card(CARD_T, SUIT_C),
    card(CARD_J, SUIT_C),
    card(CARD_Q, SUIT_C),
    card(CARD_K, SUIT_C),
    card(CARD_A, SUIT_C)
};

//==============================================================================
//==============================================================================

float get_cost (Card c, bool i_can_win, TrumpSuit trump_suit,
                bool five_was_played, bool three_was_played,
                uint8_t count_suit,
                uint8_t trick_number,
                uint8_t num_played,
                Hand *my_cards,
                uint8_t my_index,
                float probabilities[255][4])
{
    CardFace card_face = card_to_face(c);
    CardSuit card_suit = card_to_suit(c);
    
    //bool has_3 = has_card(my_cards, card(CARD_3, SUIT_S));
    bool has_5 = has_card(my_cards, card(CARD_5, SUIT_H));

    // If we're going to lose, play normal
    float cost = card_face;
    if (!i_can_win)
        return cost;
    
    uint8_t opponent_index_0 = (my_index + 1) % 4;
    uint8_t opponent_index_1 = (my_index + 3) % 4;

    // Play best cards if we have all the cards up to the ace... but not for trump
    if ((uint32_t) card_suit != (uint32_t) trump_suit || five_was_played) {
        bool is_run_to_ace = true;
        for (uint8_t c = card_face; c <= CARD_A; ++c)
            is_run_to_ace &= has_card(my_cards, card((CardFace) c, card_suit)) ||
                                (probabilities[card((CardFace)c,card_suit)][opponent_index_0] == 0.0f &&
                                probabilities[card((CardFace)c,card_suit)][opponent_index_1] == 0.0f);
        if (is_run_to_ace)
            cost = CARD_A + (CARD_A - card_face);
    }
        
    // If playing for 5
    if (trump_suit != TRUMP_N && !has_5 && !five_was_played) {
    
        // Make other hearts more likely to get played
        if (card_suit == SUIT_H)
            cost *= 1.01f;
        
    }
    
    // Save trumps
    if ( (uint32_t) card_suit == (uint32_t) trump_suit)
        cost *= 1.1F;
    
    return cost;
}

float expected (float p_score, bool i_can_win, Card c, TrumpSuit trump_suit,
                bool five_was_played, bool three_was_played,
                uint8_t count_suit,
                uint8_t trick_number,
                uint8_t num_played,
                Hand *my_cards,
                uint8_t my_index,
                float probabilities[255][4])
{
    CardFace card_face = card_to_face(c);
    CardSuit card_suit = card_to_suit(c);
    
    bool has_3 = has_card(my_cards, card(CARD_3, SUIT_S));
    bool has_5 = has_card(my_cards, card(CARD_5, SUIT_H));

    // If we're going to lose, play normal
    if (!i_can_win)
        return p_score;
    
    // Bleed out one trump
    // 1 - If AI bids (regular) - in clubs, diamonds or spades trump
    // 2- if AI bids (regular) - in hearts *and has the 5 as well
    // 3-if AI has a min of 3 trump cards in its hand
    // 4- AI does not have the Ace
    // 5-will lead out in lowest card (in hearts not the 5 obviously) so would be 8 and up. This will “bleed out” *some trump!
    if ((uint8_t) card_suit == (uint8_t) trump_suit && count_suit>=3 && !has_card(my_cards, card(CARD_A, card_suit)) ) {
        if ( (card_suit == SUIT_C || card_suit == SUIT_D || card_suit == SUIT_S) ||
             (card_suit == SUIT_H && has_5)) {
            
            // Check if this is the lowest trump card in the hand
            CardFace lowest_face = CARD_A;
            for (uint8_t i = 0; i < my_cards->num_cards; ++i) {
                CardFace test_face = card_to_face(my_cards->cards[i]);
                CardSuit test_suit = card_to_suit(my_cards->cards[i]);
                if (test_suit == card_suit && test_face < lowest_face) {
                    lowest_face = test_face;
                }
            }
            
            if (lowest_face == card_face)
                return p_score + 2.0f;    // Force this card
        }
    }

    // If playing for 5
    if (trump_suit != TRUMP_N && !has_5 && !five_was_played) {
    
        // Play Ace of Hearts right away
        if (trick_number == 0 && c == card(CARD_A,SUIT_H) && num_played==0)
            return p_score + 2.0f;    // Force this card

        // Play other aces right away
        if (trick_number == 0 &&
            card_face == CARD_A &&
            (uint8_t) card_suit != (uint8_t) trump_suit &&
            (card_suit == SUIT_C || card_suit == SUIT_D || (has_3 && card_suit == SUIT_S)))
            return p_score + 1.5f;    // Force this card

        // Save hearts
        if ((trick_number > 0 || num_played == 0) && (uint32_t) card_suit == SUIT_H)
            return p_score - 0.2f;

        // Save trumps by modifying probability
        if ((uint32_t) card_suit == (uint32_t) trump_suit) {
            if (trick_number < 2) {
                return p_score - 0.1f - (int32_t) card_face * 0.019f;
            } else {
                return p_score - 0.2f;
            }
        }

        // Play single suits
        if ((uint32_t) card_suit != (uint32_t) trump_suit &&
            (uint32_t) card_suit != (uint32_t) TRUMP_H &&
            (card_suit != SUIT_S || card_face <= CARD_9) &&
            trick_number == 0 &&
            count_suit <= 1)
            return p_score + 1.0f;    // Force this card
        
    } else if (trump_suit != TRUMP_N && !has_5 && five_was_played) {
    
        // Play Rest of Hearts right away
        if (trump_suit != TRUMP_H && card_suit == SUIT_H && card_face != CARD_A)
            return p_score + 1.0f;    // Force this card

    }
    
    // Don't lead with high spades
    if (!has_3 && !three_was_played && num_played == 0 && card_suit == SUIT_S && (uint32_t) card_face >= 10) {
        return p_score * 0.5f;
    }

    return p_score;
}

//==============================================================================
//==============================================================================

int32_t get_winner (CardSuit first_suit, TrumpSuit trump_suit, uint8_t played_cards[4])
{
    // Calculate winner if trump
    int32_t winner = -1;
    if (trump_suit != TRUMP_N) {
        for (uint32_t i = 0; i < 4; ++i) {
            CardSuit card_suit = card_to_suit(played_cards[i]);
            
            if ( (uint32_t) card_suit != (uint32_t) trump_suit)
                continue;
            
            if (winner == -1) {
                winner = i;
            } else if (card_to_face(played_cards[winner]) < card_to_face(played_cards[i])) {
                winner = i;
            }
        }
    }

    // Calculate winner if not trumped
    if (winner == -1) {
        for (uint32_t i = 0; i < 4; ++i) {
            CardSuit card_suit = card_to_suit(played_cards[i]);
            
            if ( card_suit != first_suit)
                continue;
            
            if (winner == -1) {
                winner = i;
            } else if (card_to_face(played_cards[winner]) < card_to_face(played_cards[i])) {
                winner = i;
            }
        }
    }

    if (winner < 0 || winner > 3)
        throw AIException("Internal error (3)");
    
    return winner;

}

int32_t get_winner_rankings (CardSuit first_suit, TrumpSuit trump_suit, uint8_t played_cards[4], float probability = 0.0f, int32_t *score_p0_p2 = NULL, int32_t *score_p1_p3 = NULL)
{
    uint8_t winner = 0;
    uint32_t winner_score = 0;
    
    bool had_5 = false;
    uint8_t who_had_5 = 0;

    bool had_3 = false;
    uint8_t who_had_3 = 0;

    for (uint8_t i = 0; i < 4; ++i) {
        
        uint32_t i_score = (uint32_t) card_to_face(played_cards[i]);
        
        if (i_score == CARD_3) {
            had_3 = true;
            who_had_3 = i;
        } else if (i_score == CARD_5) {
            had_5 = true;
            who_had_5 = i;
        }
        
        if (trump_suit == (TrumpSuit) card_to_suit(played_cards[i])) {
            i_score += 200;
        } else if (first_suit == card_to_suit(played_cards[i])) {
            i_score += 100;
        }
    
        if (i_score > winner_score) {
            winner = i;
            winner_score = i_score;
        }
        
    }
    
    
    // Team 0 or 1
    uint8_t winner_team = winner % 2;
    who_had_3 = who_had_3 % 2;
    who_had_5 = who_had_5 % 2;

    int loser_had_5  = (had_5 && (who_had_5 != winner_team)) ? 1 : 0;
    int winner_had_3 = (had_3 && (who_had_3 == winner_team)) ? 1 : 0;
    int loser_had_3 = (had_3 && (who_had_3 != winner_team)) ? 1 : 0;

    const int32_t WINNER_PENALTY_5 = trump_suit == TRUMP_N ? 3 : 9;    // 9 works for not leading with 5
    const int32_t WINNER_PENALTY_3 = trump_suit == TRUMP_N ? 3 : 4;
    const int32_t LOSER_PENALTY_3 = trump_suit == TRUMP_N ? 0 : 1;

    if (winner_team == 0) {
        *score_p0_p2 = 1 + (had_3 ? -3 : 0) + (had_5 ? +5 : 0) - (winner_had_3 * 3 * WINNER_PENALTY_3) - (loser_had_3 * 3 * LOSER_PENALTY_3);
        *score_p1_p3 = 0 + (had_3 ? +0 : 0) + (had_5 ? +0 : 0) - (loser_had_5 * 5 * WINNER_PENALTY_5);
    } else {
        *score_p1_p3 = 1 + (had_3 ? -3 : 0) + (had_5 ? +5 : 0) - (winner_had_3 * 3 * WINNER_PENALTY_3) - (loser_had_3 * 3 * LOSER_PENALTY_3);
        *score_p0_p2 = 0 + (had_3 ? +0 : 0) + (had_5 ? +0 : 0) - (loser_had_5 * 5 * WINNER_PENALTY_5);
    }

    return winner;
}

//==============================================================================
//==============================================================================

void set_probabilities (Card card, float p0, float p1, float p2, float p3, float probabilities[255][4])
{
    probabilities[card][0] = p0;
    probabilities[card][1] = p1;
    probabilities[card][2] = p2;
    probabilities[card][3] = p3;
}

void change_suit_probability (uint8_t player, CardSuit suit, float p_delta, float probabilities[255][4])
{
    for (int32_t c = CARD_2; c <= CARD_A; ++c) {
        float &p = probabilities[card((CardFace) c, suit)][player];
    
        if (p > 0.0F) {
            p += p_delta;
        }
    }
}

void change_card_probability (uint8_t player, Card card, float p_delta, float probabilities[255][4])
{
    float &p = probabilities[card][player];

    if (p > 0.0F) {
        p += p_delta;
    }
}

void set_card_probability (uint8_t player, Card card, float value, float probabilities[255][4])
{
    float &p = probabilities[card][player];
    p = value;
}

void set_suit_probability (uint8_t player, CardSuit suit, float value, float probabilities[255][4])
{
    for (int32_t c = CARD_2; c <= CARD_A; ++c) {
        float &p = probabilities[card((CardFace) c, suit)][player];
        p = value;
    }
}

//==============================================================================
//==============================================================================

void calc_probabilities (   CardSuit first_suit,
                            TrumpSuit trump_suit,
                            Hand *my_cards,
                            uint8_t my_index,
                            uint8_t index,
                            Card card_buffer[4],
                            float probability,
                            uint8_t depth,
                            uint32_t &can_win,
                            float &p_score_p0_p2,
                            float &p_score_p1_p3,
                            uint32_t &p_count,
                            float probabilities[255][4])
{
    if (depth >= 3) {
    
        int32_t score_p0_p2;
        int32_t score_p1_p3;
        int32_t winner = get_winner_rankings(first_suit, trump_suit, card_buffer, probability, &score_p0_p2, &score_p1_p3);
        
        // Lower probability of large score differences
        // NOTE: DON'T DO THIS... It negates the penalties of losing 5
        //probability /= std::abs(score_p0_p2 - score_p1_p3);
        
        can_win |= (1 << winner);

        p_score_p0_p2 += probability * score_p0_p2;
        p_score_p1_p3 += probability * score_p1_p3;
        p_count += 1;

//#if DEBUG_LOG
//        std::cout << "  Try: ";
//        for (uint32_t i = 0; i < 4; ++i) {
//            std::cout << card_name(card_buffer[i]) << " ";
//        }
//        std::cout << can_win << " ";
//        std::cout << score_p0_p2 << " ";
//        std::cout << score_p1_p3 << " ";
//        std::cout << " prob " << probability << std::endl;
//#endif

        
    } else {
        index = (index + 1) % 4;
        
        for (uint8_t c = 0; c < ARRAY_SIZE(VALID_INDICES); ++c) {
            // Restore the buffer
            
            Card card_buffer_copy[4] = { card_buffer[0], card_buffer[1], card_buffer[2], card_buffer[3] };
            Card ci = VALID_INDICES[c];  // Same as card
            
            bool skip = false;
            
            // Make sure card isn't in our hand
            if (has_card(my_cards, ci))
                skip |= true;

            // Make sure card isn't played
            for (uint8_t cc = 0; cc < 4; ++cc) {
                if (card_buffer_copy[cc] == ci) {
                    skip |= true;
                }
            }

            if (!skip) {
                float cp = probabilities[ci][index];
                card_buffer_copy[index] = ci;
                
                calc_probabilities(first_suit, trump_suit, my_cards, my_index, index, card_buffer_copy, probability * cp, depth+1, can_win, p_score_p0_p2, p_score_p1_p3, p_count, probabilities);
            }
            
        }
        
    }
}

//==============================================================================
// Returns the best cards to pass. out_cards must point to array of at least
// rule_pass_cards in size
//==============================================================================

Card ai_get_play_cards (int32_t     rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
    
                        int32_t     highest_bidder,
                        Bid         highest_bid,

                        Hand        *p0_played_cards,
                        Hand        *p1_played_cards,
                        Hand        *p2_played_cards,
                        Hand        *p3_played_cards,
                        
                        Hand        *my_cards,
                        int32_t     my_index)
{
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
    if (my_cards->num_cards > 8) {
        throw AIException("My cards out of range");
    }

    if (highest_bidder < 0 || highest_bidder > 3) {
        throw AIException("Highest bidder out of range");
    }
    if (my_index < 0 || my_index > 3) {
        throw AIException("Highest bidder out of range");
    }

#if DEBUG_LOG
    std::cout << "rule_pass_cards: " << (int) rule_pass_cards << std::endl;
    std::cout << "rule_no_trump_bidout: " << (int) rule_no_trump_bidout << std::endl;
    std::cout << "rule_minimum_bid: " << (int) rule_minimum_bid << std::endl;
    std::cout << "highest_bidder: " << (int) highest_bidder << std::endl;
    std::cout << "my_index: " << (int) highest_bidder << std::endl;

    std::cout << "bid->bid: " << (int) highest_bid.bid << std::endl;
    std::cout << "bid->trump: " << (int) highest_bid.trump << std::endl;
    
    std::cout << "p0_played_cards: ";
    for (int i = 0; i < p0_played_cards->num_cards; ++i)
        std::cout << card_name(p0_played_cards->cards[i]) << " ";
    std::cout << std::endl;

    std::cout << "p1_played_cards: ";
    for (int i = 0; i < p1_played_cards->num_cards; ++i)
        std::cout << card_name(p1_played_cards->cards[i]) << " ";
    std::cout << std::endl;

    std::cout << "p2_played_cards: ";
    for (int i = 0; i < p2_played_cards->num_cards; ++i)
        std::cout << card_name(p2_played_cards->cards[i]) << " ";
    std::cout << std::endl;

    std::cout << "p3_played_cards: ";
    for (int i = 0; i < p3_played_cards->num_cards; ++i)
        std::cout << card_name(p3_played_cards->cards[i]) << " ";
    std::cout << std::endl;

    std::cout << "my_cards: ";
    for (int i = 0; i < my_cards->num_cards; ++i)
        std::cout << card_name(my_cards->cards[i]) << " ";
    std::cout << std::endl;
    std::cout << std::flush;
#endif

    float       probabilities[255][4]; // indexed by [card][player]
    
    Card        first_card = 0;
    CardSuit    first_suit = card_to_suit (first_card);
    CardFace    first_face = card_to_face (first_card);
    
    //
    // Setup probabilities
    //
    
    ::memset(probabilities, 0, sizeof(probabilities));
    for (uint32_t i = 0; i < ARRAY_SIZE(VALID_INDICES); ++i) {
        set_probabilities (VALID_INDICES[i], 0.25F, 0.25F, 0.25F, 0.25F, probabilities);
    }


    //
    // Play the game forward from the start
    //
    
    uint8_t current_player = highest_bidder;
    uint8_t first_player = current_player;
    uint8_t num_played = 0;
    uint8_t played_cards[4] = { 0 };
    uint8_t dumped_cards[4] = { 0 };
    uint8_t trick_number = 0;
    
    bool five_was_played = false;
    bool three_was_played = false;
    
    
    while (true) {
    
#if DEBUG_LOG
        //std::cout << "FooA" << std::endl;
#endif

        // Get correct player
        Hand *p_played_cards;
        
        switch (current_player) {
            case 0: p_played_cards = p0_played_cards;
                    break;
            case 1: p_played_cards = p1_played_cards;
                    break;
            case 2: p_played_cards = p2_played_cards;
                    break;
            case 3: p_played_cards = p3_played_cards;
                    break;
            default:
#if DEBUG_LOG
                std::cout << "Internal error (1): current_player: " << (int) current_player << std::endl;
#endif
                throw AIException("Internal error (1)");
        }
        
        // Check done
        if (p0_played_cards->num_cards == 0 && 
            p1_played_cards->num_cards == 0 && 
            p2_played_cards->num_cards == 0 && 
            p3_played_cards->num_cards == 0)
            break;
        
        // Record played card
        Card        played_card = p_played_cards->cards[0];
        CardSuit    played_suit = card_to_suit (played_card);
        //CardFace    played_face = card_to_face (played_card);
        
#if DEBUG_LOG
        std::cout << "Mark played: " << card_name(played_card) << " (" << int(played_card) << ")" << std::endl;
#endif

        played_cards[current_player] = played_card;
        set_probabilities (played_cards[current_player], 0.0F, 0.0F, 0.0F, 0.0F, probabilities);
        
        // Remove the played cards
        remove_card (p_played_cards, 0);

        // Remember 3 or 5 played
        five_was_played = (played_card == card (CARD_5, SUIT_H));
        three_was_played = (played_card == card (CARD_3, SUIT_S));
        
        ++num_played;
        
#if DEBUG_LOG
        //std::cout << "FooC" << std::endl;
#endif

        // First card played
        if (num_played == 1) {
            first_card = played_card;
            first_suit = card_to_suit (first_card);
            first_face = card_to_face (first_card);

        } else {
            
            // Check if card was dumped
            if (played_suit != first_suit) {
            
                if (!dumped_cards[current_player]) {
                    dumped_cards[current_player] = played_card;
                    
                    // Increase probability that this player has this card
                    //change_suit_probability (current_player, played_suit, 0.05F, probabilities); // Kevin said Ace only
                    change_card_probability (current_player, card(CARD_A, played_suit), 0.05F, probabilities);
                }

                // If Player plays a different card then we know they don't have any of the suit
                // so zero the probabilities.
                set_suit_probability (current_player, first_suit, 0.0F, probabilities);
            }
        
        }
        
        // Special strategy update
        if (trick_number == 0) {
            // If player started with Ace of hearts and partner didn't play it then they don't have it
        
            if (played_cards[0] == card(CARD_A, SUIT_H) && !five_was_played)
                set_card_probability (2, card(CARD_5, SUIT_H), 0.0f, probabilities);
            if (played_cards[1] == card(CARD_A, SUIT_H) && !five_was_played)
                set_card_probability (3, card(CARD_5, SUIT_H), 0.0f, probabilities);
            if (played_cards[2] == card(CARD_A, SUIT_H) && !five_was_played)
                set_card_probability (0, card(CARD_5, SUIT_H), 0.0f, probabilities);
            if (played_cards[3] == card(CARD_A, SUIT_H) && !five_was_played)
                set_card_probability (1, card(CARD_5, SUIT_H), 0.0f, probabilities);

        }
                
        // Check for winner and reset for next round
        if (num_played == 4) {
            current_player = get_winner(first_suit, highest_bid.trump, played_cards);
            first_player = current_player;
            
            first_card = 0;
            num_played = 0;
            played_cards[0] = played_cards[1] = played_cards[2] = played_cards[3] = 0;
            ++trick_number;

        } else {
            // Player is done turn
            current_player = (current_player+1) % 4;
        }
        
    }
    
    //
    // Cleanup the dataset
    //
   
#if DEBUG_LOG
    std::cout << "my_index: " << (int) my_index << std::endl;
    std::cout << "current_player: " << (int) current_player << std::endl;
#endif

    // Make sure we are calculating the correct player
    if (my_index != current_player)
        throw AIException("Internal error (2)");
    
    // Adjust probabilities for all of my cards
    for (uint32_t c = 0; c < my_cards->num_cards; ++c) {
        set_probabilities (my_cards->cards[c], 0.0F, 0.0F, 0.0F, 0.0F, probabilities);  // Yes I don't care about my probabilities too much
    }
    
    // Rebalance all probabilities
    for (uint32_t p = 0; p < 255; ++p) {
        float sum = probabilities[p][0] + probabilities[p][1] + probabilities[p][2] + probabilities[p][3];
        if (sum > 0.0F) {
            probabilities[p][0] /= sum;
            probabilities[p][1] /= sum;
            probabilities[p][2] /= sum;
            probabilities[p][3] /= sum;
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
    // Do AI
    //
    
    uint8_t best_card = 0;
    float   best_score = -std::numeric_limits<float>::infinity();
    float   best_score_cost = std::numeric_limits<float>::infinity();

    for (uint32_t c = 0; c < my_cards->num_cards; ++c) {
        
        uint8_t count_suit = count_suit_in_hand(my_cards, card_to_suit(my_cards->cards[c]));
    
        played_cards[my_index] = my_cards->cards[c];
        
        // First card info because this card leads a new hand
        if (num_played == 0) {
            first_card = played_cards[my_index];
            first_suit = card_to_suit (first_card);
            first_face = card_to_face (first_card);
        }
        
        uint32_t p_count = 0;
        uint32_t can_win = 0;
        float p_score_p0_p2 = 0.0F;
        float p_score_p1_p3 = 0.0F;

        // Calc probability of card winning
        calc_probabilities(first_suit, highest_bid.trump, my_cards, my_index, my_index, played_cards, 1.0F, num_played, can_win, p_score_p0_p2, p_score_p1_p3, p_count, probabilities);
        p_score_p0_p2 /= p_count;
        p_score_p1_p3 /= p_count;

        float p_score = (my_index == 0 || my_index == 2) ? (p_score_p0_p2 - p_score_p1_p3) : (p_score_p1_p3 - p_score_p0_p2);
        bool i_can_win = can_win & (1 << my_index);
        
        p_score = expected (p_score, i_can_win, my_cards->cards[c], highest_bid.trump, five_was_played, three_was_played, count_suit, trick_number, num_played, my_cards, my_index, probabilities);
        float p_cost = get_cost (my_cards->cards[c], i_can_win, highest_bid.trump, five_was_played, three_was_played, count_suit, trick_number, num_played, my_cards, my_index, probabilities);

#if DEBUG_LOG
        std::cout << " Probability: " << card_name(my_cards->cards[c]) << ", can_win=" << can_win << ", p_score=" << p_score << ", cost=" << p_cost << ", p_score_p0_p2=" << p_score_p0_p2 << ", p_score_p1_p3=" << p_score_p1_p3 << std::endl;
#endif

        if (p_score > best_score || (p_score == best_score && p_cost < best_score_cost)) {
            best_card = my_cards->cards[c];
            best_score = p_score;
            best_score_cost = p_cost;
        }
    }
    
    return best_card;
}

//==============================================================================
//==============================================================================
