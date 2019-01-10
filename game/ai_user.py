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

from text_colors import TextColor
from utils import *

class AIUser:

    def prompt_input (self, prompt, valid_inputs):
        val = raw_input(prompt)
        while val not in valid_inputs:
            print(TextColor.FAIL + "Invalid choice" + TextColor.ENDC)
            val = raw_input(prompt)
        return val


    def get_pass_cards (self, rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, my_cards):
        print("Your hand: %s" % ','.join(my_cards))

        mc = my_cards[:]

        pass_1 = self.prompt_input("Enter first card to pass:", mc)
        mc.remove(pass_1) # Make sure we can't pass the same card

        pass_2 = self.prompt_input("Enter second card to pass:", mc)

        return [pass_1, pass_2]


    def get_bid (self, rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, bids, my_cards, my_index, dealer_index):
        print("Your hand: %s" % ','.join(my_cards))

        valid_bids = ['pass']
        for b in range(rule_minimum_bid, 12+1):
            valid_bids.append(str(b))
            valid_bids.append(str(b)+'N')

        bid = self.prompt_input("Enter your bid (i.e. '7', '8N', 'pass'):", valid_bids)

        return bid


    def get_trump(self, rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, my_cards):
        print("Your hand: %s" % ','.join(my_cards))

        valid_trump = ['S','H','D','C']
        trump = self.prompt_input("Enter trump (i.e. 'S','H','D','C'):", valid_trump)
        return '0' + trump  # Note: For historical reasons, trump looks like a bid hence the 0


    def get_play_cards(self, rule_pass_cards, rule_no_trump_bidout, rule_minimum_bid, highest_bidder, highest_bid,
                        p0_played_cards, p1_played_cards, p2_played_cards, p3_played_cards, my_cards, my_index):


        # Okay, this requires some explanation. The AI is stateless so when it's playing it actually has to play the
        # entire hand from the start so it can build up a list of all the cards, etc.  This module really isn't an AI
        # but we still need to play the game forward to the current state so we can show the allowed cards.

        # Make copies of played cards
        player_cards = [ p0_played_cards[:], p1_played_cards[:], p2_played_cards[:], p3_played_cards[:] ]

        # Play hands forward
        def play_forward():
            highest_value, highest_suit = decode_bid(highest_bid)
            round_winner = highest_bidder

            for r in range(0,8):

                first_played_card = None
                played_cards = [None,None,None,None]

                for pi in range(0,4):
                    player_index = (pi + round_winner) % 4

                    if len(player_cards[player_index]) == 0:
                        return first_played_card

                    selected_card = player_cards[player_index].pop(0)
                    if not first_played_card:
                        first_played_card = selected_card

                    played_cards[player_index] = selected_card

                first_value, first_suit = decode_card(first_played_card)
                round_winner = get_winner (first_suit, highest_suit, played_cards)

        # Play this forward
        first_played_card = play_forward()

        #
        # Build a list of allowed cards
        #

        allowed_cards = []
        if first_played_card is None:
            # All allowed
            allowed_cards = my_cards
        else:
            first_value, first_suit = decode_card(first_played_card)

            for c in my_cards:
                card_value, card_suit = decode_card(c)

                if first_suit == card_suit:
                    allowed_cards.append(c)

            # If, after we restrict playable cards to suit, there still aren't any then allow all
            if len(allowed_cards) == 0:
                allowed_cards = my_cards

        #
        # Okay, now we can prompt
        #

        print("Valid cards: %s" % ','.join(allowed_cards))
        selected_card = self.prompt_input("Enter card to play:", allowed_cards)
        return selected_card

