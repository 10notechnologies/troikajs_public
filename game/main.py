#==============================================================================
#
# Copyright (c) 2019 Smells Like Donkey Software Inc.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
#==============================================================================

import random
from ai_module import AIModule
from ai_user import AIUser
from text_colors import TextColor
from utils import *

VALID_CARDS = [ "3S","5H",
                "7C","7D",
                "8C","8D","8H","8S",
                "9C","9D","9H","9S",
                "TC","TD","TH","TS",
                "JC","JD","JH","JS",
                "QC","QD","QH","QS",
                "KC","KD","KH","KS",
                "AC","AD","AH","AS"]

# This array determines which AI each player is using. Set it to AIUser to manually play.
PLAYER_AIS = [
    AIModule(),   # Player 0
    AIModule(),   # Player 1
    AIModule(),   # Player 2
    AIUser()    # Player 3
]

SETTING_MIN_BID = 7
SETTING_PASS_CARDS = False
SETTING_NO_TRUMP_BIDOUT = 62
SETTING_GAME_OVER_AT_MINUS_52 = True

class PlayerState:
    def __init__(self):
        self.hand = []
        self.played_card = None
        self.played_cards = []
        self.bid = None

def main():
    print(TextColor.UNDERLINE + "Welcome the the Troika AI Test bench" + TextColor.ENDC)

    dealer_index = None
    player_02_score = 0
    player_13_score = 0

    while True:

        cards = VALID_CARDS[:]   # Make a copy

        player_02_round_score = 0
        player_13_round_score = 0

        highest_bidder = None
        highest_bid = "pass"

        if dealer_index is None:
            dealer_index = random.randint(0, 3)
        else:
            dealer_index = (dealer_index + 1) % 4

        #
        # Shuffle the cards and deal
        #

        print(TextColor.HEADER + "Dealer is " + str(dealer_index) + TextColor.ENDC)
        print("Dealing...")

        random.seed() # see https://docs.python.org/2/library/random.html
        random.shuffle(cards)

        player_state = [ PlayerState(), PlayerState(), PlayerState(), PlayerState() ]
        player_state[0].hand = cards[0:8]
        player_state[1].hand = cards[8:16]
        player_state[2].hand = cards[16:24]
        player_state[3].hand = cards[24:32]

        #
        # Pass cards (if option is turned on)
        #

        if SETTING_PASS_CARDS:
            passed_cards = [None,None,None,None]

            for player_index in range(0,4):
                print(TextColor.HEADER + "Passing cards, player " + str(player_index) + TextColor.ENDC)
                passed_cards[player_index] = PLAYER_AIS[player_index].get_pass_cards(SETTING_PASS_CARDS, SETTING_NO_TRUMP_BIDOUT, SETTING_MIN_BID, player_state[player_index].hand)

            # Do actual swapping
            for player_index in range(0,4):
                player_index_partner = (player_index + 2) % 4

                # Take cards from partners hand
                player_state[player_index].hand.append(passed_cards[player_index_partner][0])
                player_state[player_index_partner].hand.remove(passed_cards[player_index_partner][0])

                player_state[player_index].hand.append(passed_cards[player_index_partner][1])
                player_state[player_index_partner].hand.remove(passed_cards[player_index_partner][1])

        #
        # Bidding
        #

        for pi in range(0,4):
            player_index = (pi + dealer_index + 1) % 4  # Bidder index starts at the player after the bidder

            # Check if the bid is forced
            force = False
            if player_index == dealer_index and highest_bidder == None:
                force = True

            bids = [
                player_state[0].bid,
                player_state[1].bid,
                player_state[2].bid,
                player_state[3].bid
            ]

            print(TextColor.HEADER + "Bidding, player " + str(player_index) + TextColor.ENDC)
            bid = PLAYER_AIS[player_index].get_bid(SETTING_PASS_CARDS, SETTING_NO_TRUMP_BIDOUT, SETTING_MIN_BID, bids, player_state[player_index].hand, player_index, dealer_index)

            if force and bid == "pass":
                bid = str(SETTING_MIN_BID);

            if bid != "pass":
                player_state[player_index].bid = bid
    
                # Special case dealer can match
                if force or (player_index == dealer_index and highest_bid == bid):
                    highest_bid = bid
                    highest_bidder = player_index
    
                # Update highest bid
                elif highest_bid == None or (get_better_bid(bid, highest_bid) != highest_bid):
                    highest_bid = bid
                    highest_bidder = player_index
    
                else:
                    bid = "pass"

            print("Bid: " + bid)

        print((TextColor.OKGREEN + "Highest Bidder %d with bid %s" + TextColor.ENDC) % (highest_bidder, highest_bid))

        #
        # Bidding Trump if not "No" trump
        #

        bid, suit = decode_bid(highest_bid)
        if suit != "N":
            print(TextColor.HEADER + "Bidding trump, player " + str(highest_bidder) + TextColor.ENDC)

            bid = PLAYER_AIS[highest_bidder].get_trump(SETTING_PASS_CARDS, SETTING_NO_TRUMP_BIDOUT, SETTING_MIN_BID, player_state[highest_bidder].hand)

            player_state[highest_bidder].bid = player_state[highest_bidder].bid + bid[1]
            highest_bid = highest_bid + bid[1]

        print((TextColor.OKGREEN + "Highest Bidder %d with bid %s" + TextColor.ENDC) % (highest_bidder, highest_bid))

        #
        # Finally we can start playing
        #

        round_winner = highest_bidder

        for r in range(0,8):

            first_played_card = None

            for pi in range(0,4):
                player_index = (pi + round_winner) % 4  # Bidder index starts at the player after the bidder

                print(TextColor.HEADER + "Playing card, player " + str(player_index) + TextColor.ENDC)

                selected_card = PLAYER_AIS[player_index].get_play_cards(SETTING_PASS_CARDS, SETTING_NO_TRUMP_BIDOUT, SETTING_MIN_BID,
                                                                       highest_bidder, highest_bid,
                                                                       player_state[0].played_cards,
                                                                       player_state[1].played_cards,
                                                                       player_state[2].played_cards,
                                                                       player_state[3].played_cards,
                                                                       player_state[player_index].hand,
                                                                       player_index)

                print("Played: " + selected_card)

                player_state[player_index].played_card = selected_card
                player_state[player_index].played_cards.append(selected_card)
                player_state[player_index].hand.remove(selected_card)

                if first_played_card is None:
                    first_played_card = selected_card


            # Figure out winner of trick
            highest_value, highest_suit = decode_bid(highest_bid)
            first_value, first_suit = decode_card(first_played_card)

            played_cards = [ player_state[0].played_card, player_state[1].played_card, player_state[2].played_card, player_state[3].played_card ]

            winner = get_winner(first_suit, highest_suit, played_cards)
            has_three = False
            has_five = False

            for p in range(0, 4):
                # Check best card
                if p == winner:
                    player_index = p

                # Update 3 or 5 flags
                value, suit = decode_card(player_state[p].played_card)
                if value == 3:
                    has_three = True
                if value == 5:
                    has_five = True

                player_state[p].played_card = None

            # Update score
            delta_score = 1
            if has_three:
                delta_score -= 3
            if has_five:
                delta_score += 5

            if player_index in (0,2):
                player_02_round_score += delta_score
            else:
                player_13_round_score += delta_score

            print( (TextColor.OKGREEN + "Score 02: %d/%d  13: %d/%d\n" + TextColor.ENDC) % (player_02_round_score, player_02_score, player_13_round_score, player_13_score))

            # Reset for next trick
            first_played_card = None
            round_winner = winner
            
            
        #
        # Tally score
        #

        multiplier = 1
        end_score = 52
        if highest_suit == 'N':
            multiplier = 2
            end_score = SETTING_NO_TRUMP_BIDOUT

        thresh = 52 - SETTING_MIN_BID

        if highest_bidder in (0, 2):

            # Bidding Team
            if player_02_round_score >= int(highest_value):
                player_02_score += player_02_round_score * multiplier
            else:
                player_02_score -= int(highest_value) * multiplier

            # Opposing Team
            if player_13_score < thresh or player_13_round_score < 0:
                player_13_score += player_13_round_score

        else:

            # Winner players
            if player_13_round_score >= int(highest_value):
                player_13_score += player_13_round_score * multiplier
            else:
                player_13_score -= int(highest_value) * multiplier

            # Loser players
            if player_02_score < thresh or player_02_round_score < 0:
                player_02_score += player_02_round_score

        # Check wins
        if (highest_bidder in (0, 2) and player_02_score >= end_score) or \
           (highest_bidder in (0, 2) and player_02_round_score == 12 and int(highest_value) == 12 and highest_suit == 'N') or \
           (highest_bidder in (1, 3) and player_13_round_score != 12 and int(highest_value) == 12 and highest_suit == 'N') or \
           (SETTING_GAME_OVER_AT_MINUS_52 and player_13_score <= -52):

            # Player 0 and 2 win
            print("Players 02 wins")
            return

        elif (highest_bidder in (1, 3) and player_13_score >= end_score) or \
             (highest_bidder in (1, 3) and player_13_round_score == 12 and int(highest_value) == 12 and highest_suit == 'N') or \
             (highest_bidder in (0, 2) and player_02_round_score != 12 and int(highest_value) == 12 and highest_suit == 'N') or \
             (SETTING_GAME_OVER_AT_MINUS_52 and player_13_score <= -52):

            # Player 1 and 3 win
            print("Players 13 wins")
            return

        player_02_round_score = 0
        player_13_round_score = 0

        print( (TextColor.OKGREEN + "Score 02: %d/%d  13: %d/%d\n" + TextColor.ENDC) % (player_02_round_score, player_02_score, player_13_round_score, player_13_score))

if __name__ == "__main__":
    main()
