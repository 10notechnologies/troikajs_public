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

#include <iostream>
#include <vector>
#include <random>
#include <future>
#include "troika_common.hpp"

extern Network *networks[4];
#define NUM_HANDS 256
#define NUM_NETWORKS 64

//==============================================================================
//==============================================================================

void ai_get_pass_cards (int32_t     rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        Hand        *my_cards,
                        Card        *out_cards);

TrumpSuit ai_get_trump  (int32_t    rule_pass_cards,
                        int32_t     rule_no_trump_bidout,
                        int32_t     rule_minimum_bid,
                        Hand        *my_cards);

Bid ai_get_bid (int32_t     rule_pass_cards,
                int32_t     rule_no_trump_bidout,
                int32_t     rule_minimum_bid,
                Bid         bids[4],
                Hand        *my_cards,
                int32_t     my_index,
                int32_t     dealer_index);

Card ai_get_play_cards (int32_t     rule_pass_cards,
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
                        int32_t     my_index);

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
                        Network     *network);

//==============================================================================
//==============================================================================

int run_ml_hand (Network *network)
{
    int32_t rule_pass_cards = 1;
    int32_t rule_no_trump_bidout = 62;
    int32_t rule_minimum_bid = 7;

    // Build and shuffle the deck
    std::vector<Card> deck;
    for (uint8_t i = 0; i < 32; ++i)
        deck.push_back(bits_to_card(i));
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(deck.begin(), deck.end(), std::default_random_engine(seed));

    // Deal the cards
    Hand hand[4] = {0};
    for (uint8_t p = 0; p < 32; ) {
        add_card(&hand[0], deck[p++]);
        add_card(&hand[1], deck[p++]);
        add_card(&hand[2], deck[p++]);
        add_card(&hand[3], deck[p++]);
    }

    // Pass cards
    Card passed_cards_raw[4][2] = {0};
    Hand passed_cards[4] = {0};
    if (rule_pass_cards) {
        // Choose cards to swap
        for (uint8_t p = 0; p < 4; ++p) {
            Hand hand_copy = hand[p];
            ai_get_pass_cards (rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, &hand_copy, (Card*) &(passed_cards_raw[p]));
        }
        
        // Swap the cards
        for (uint8_t p = 0; p < 4; ++p) {
            uint8_t ppi = (p + 2)%4;
            remove_card(&hand[p], index_of(&hand[p], passed_cards_raw[p][0]));
            remove_card(&hand[p], index_of(&hand[p], passed_cards_raw[p][1]));
        }
        for (uint8_t p = 0; p < 4; ++p) {
            uint8_t ppi = (p + 2)%4;
            add_card(&hand[ppi], passed_cards_raw[p][0]);
            add_card(&hand[ppi], passed_cards_raw[p][1]);
            add_card(&passed_cards[p], passed_cards_raw[p][0]);
            add_card(&passed_cards[p], passed_cards_raw[p][1]);
        }
    }
    
    int8_t dealer_index = std::rand() % 4;
    int8_t player_turn_index = dealer_index;

    // Bidding
    Bid player_bids[4];
    int8_t bid_winner_amount = 0;
    int8_t bid_winner_index = -1;

    for (uint8_t i = 0; i < 4; ++i) {
        uint8_t pi = (dealer_index + i + 1) % 4;
        player_bids[pi] = ai_get_bid(rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, player_bids, &hand[pi], pi, dealer_index);
        
        uint8_t bid_amount = player_bids[pi].bid + (player_bids[pi].trump == TRUMP_N) ? 1 : 0;
        if (pi != dealer_index && bid_amount > bid_winner_amount)
            bid_winner_index = pi;
        else if (pi == dealer_index && bid_amount >= bid_winner_amount)
            bid_winner_index = pi;
    }

    // Do Trump
    player_turn_index = bid_winner_index;
    
    if (player_bids[bid_winner_index].trump != TRUMP_N) {
        TrumpSuit trump = ai_get_trump(rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, &hand[bid_winner_index]);
        player_bids[bid_winner_index].trump = trump;
    }
    
    // 8 rounds
    int8_t round_winner = bid_winner_index;
    Card selected_card = 0;
    Hand played_cards[4] = {0};
    int round_scores[2] = {0};

    for (uint8_t i = 0; i < 8; ++i) {
    
        // One card each
        Card first_card = 0;
        Card played_card[4] = {0};
        bool has_three = false;
        bool has_five = false;

        for (uint8_t j = 0; j < 4; ++j) {
            uint8_t pi = (round_winner + j) % 4;

            // Enable card interactions
            Hand played_cards_copy[4];
            played_cards_copy[0] = played_cards[0];
            played_cards_copy[1] = played_cards[1];
            played_cards_copy[2] = played_cards[2];
            played_cards_copy[3] = played_cards[3];
            Hand passed_cards_copy = passed_cards[pi];
            Hand hand_copy = hand[pi];

            if (pi % 2 == 0) {
                selected_card = ai_get_play_cards_ml(rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid,
                                                    bid_winner_index,
                                                    player_bids[bid_winner_index],
                                                    &played_cards_copy[0], &played_cards_copy[1], &played_cards_copy[2], &played_cards_copy[3],
                                                    &passed_cards_copy,
                                                    &hand_copy,
                                                    pi,
                                                    network);
            } else {
                selected_card = ai_get_play_cards(  rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid,
                                                    bid_winner_index,
                                                    player_bids[bid_winner_index],
                                                    &played_cards_copy[0], &played_cards_copy[1], &played_cards_copy[2], &played_cards_copy[3],
                                                    &passed_cards_copy,
                                                    &hand_copy,
                                                    pi);
            }

            played_card[pi] = selected_card;
            
            // Update 3 or 5 flags
            if (card_to_face(selected_card) == CARD_3)
                has_three = true;
            if (card_to_face(selected_card) == CARD_5)
                has_five = true;

            // Update player cards
            int32_t index = index_of(&hand[pi], selected_card);
            remove_card(&hand[pi], index);
            add_card(&played_cards[pi], selected_card);

            if (!first_card)
                first_card = selected_card;
        }
        
        round_winner = -1;

        // Get round winner
        if (player_bids[bid_winner_index].trump != TRUMP_N) {
            for (uint8_t j = 0; j < 4; ++j) {
                if ((uint8_t) card_to_suit(played_card[j]) != (uint8_t) player_bids[bid_winner_index].trump)
                    continue;
                if (round_winner == -1)
                    round_winner = j;
                if (card_to_face(played_card[j]) > card_to_face(played_card[round_winner]))
                    round_winner = j;
            }
        }
        
        // If nobody played trump
        if (round_winner == -1) {
            for (uint8_t j = 0; j < 4; ++j) {
                if (card_to_suit(played_card[j]) != card_to_suit(first_card))
                    continue;
                if (round_winner == -1)
                    round_winner = j;
                if (card_to_face(played_card[j]) > card_to_face(played_card[round_winner]))
                    round_winner = j;
            }
        }
        
        // Update round score
        int8_t delta_score = 1;
        if (has_three)  delta_score -= 3;
        if (has_five)   delta_score += 5;

        int8_t winner = round_winner % 2;
        round_scores[winner] += delta_score;
    }
    
    // Update Final scores
    int8_t bidding_team = bid_winner_index % 2;
    int8_t opposing_team = (bid_winner_index+1) % 2;

    int8_t multiplier = 1;
    if (player_bids[bid_winner_index].trump == TRUMP_N) {
        multiplier = 2;
    }

    // Bidding team
    int scores[2] = {0};
    
    if (round_scores[bidding_team] >= player_bids[bid_winner_index].bid) {
        scores[bidding_team] += round_scores[bidding_team] * multiplier;
    } else {
        scores[bidding_team] -= player_bids[bid_winner_index].bid * multiplier;
    }

    // Opposing team
    scores[opposing_team] += round_scores[opposing_team];

    return scores[0] - scores[1];
}

//==============================================================================
//==============================================================================

void test_population ()
{
    struct Sample {
        Network *network;
        float score;
    };
    
    std::cout << "Building networks..." << std::endl;

    // Build networks
    std::vector<Sample> samples(NUM_NETWORKS);
    for (int i = 0; i < samples.size(); ++i) {
        Network *network = new Network();
        if (!load_network("net" + std::to_string(i) + ".bin", network))
            init_network(network);
        
        samples[i] = Sample{network, 0.0f};
    }
    
    for (int g = 1; g < 100000; ++g) {
        std::cout << "GENERATION: " << g << std::endl;
        
        std::vector<std::future<float>> futures(samples.size());
        
        // Test networks
        for (int i = 0; i < samples.size(); ++i) {
            futures[i] = std::async(std::launch::async,
                [&samples,i]()->float{
                    float sum = 0.0f;
                    int count = 0;
                
                    for (int j = 0; j < NUM_HANDS; ++j) {
                        int score = run_ml_hand(samples[i].network);
                        sum += score;
                        count += 1;
                    }
                    
                    return sum/count;
                }
            );
        }
        
        // Wait
        for (int i = 0; i < samples.size(); ++i) {
            float score = futures[i].get();
            samples[i].score = score;
        }

        // Sort by score
        std::sort(samples.begin(), samples.end(),
            [](const Sample &a, const Sample &b)
            {
                return a.score > b.score;
            }
        );
        
        // Print top results
        float sum = 0.0f;
        for (int i = 0; i < 8; ++i) {
            sum += samples[i].score;
            std::cout << "  " << i << " - Score: " << samples[i].score << std::endl;
        }
        std::cout << "  Average score: " << (sum/8) << std::endl;

        // Breed networks
        for (int o = samples.size()/2, i = 0; o < samples.size(); ++o, ++i) {
            if (i > 8)
                i = 0;
            breed(samples[o].network, samples[i].network, samples[i+1].network);
        }
        
        // Wipe out very bottom networks
        for (int i = samples.size()*7/8; i < samples.size(); ++i) {
            init_network(samples[i].network);
        }
        
        // Save state
        for (int i = 0; i < samples.size(); ++i) {
            save_network("net" + std::to_string(i) + ".bin", samples[i].network);
        }
    }
    
}

//==============================================================================
//==============================================================================
