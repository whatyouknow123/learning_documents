################################################################################
# The Pyretic Project                                                          #
# frenetic-lang.org/pyretic                                                    #
# author: Joshua Reich (jreich@cs.princeton.edu)                               #
################################################################################
# Licensed to the Pyretic Project by one or more contributors. See the         #
# NOTICES file distributed with this work for additional information           #
# regarding copyright and ownership. The Pyretic Project licenses this         #
# file to you under the following license.                                     #
#                                                                              #
# Redistribution and use in source and binary forms, with or without           #
# modification, are permitted provided the following conditions are met:       #
# - Redistributions of source code must retain the above copyright             #
#   notice, this list of conditions and the following disclaimer.              #
# - Redistributions in binary form must reproduce the above copyright          #
#   notice, this list of conditions and the following disclaimer in            #
#   the documentation or other materials provided with the distribution.       #
# - The names of the copyright holds and contributors may not be used to       #
#   endorse or promote products derived from this work without specific        #
#   prior written permission.                                                  #
#                                                                              #
# Unless required by applicable law or agreed to in writing, software          #
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT    #
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the     #
# LICENSE file distributed with this work for specific language governing      #
# permissions and limitations under the License.                               #
################################################################################


from pyretic.lib.corelib import *
from pyretic.lib.std import *

# The @dynamic decorator means that the function below will create a new dynamic
# policy class with the name "act_like_switch"
@dynamic   
def act_like_switch(self):
    """
    Implement switch-like behavior.
    """
    
    # Set up the initial forwarding behavior for your mac learning switch to flood 
    # all packets
    self.forward = flood()

    # Set up a query that will receive new incoming packets
    self.query = packets(limit=1,group_by=['srcmac','switch'])

    # Set the initial internal policy value (each dynamic policy has a member 'policy'
    # when this member is assigned, the dynamic policy updates itself)
    self.policy = self.forward + self.query
    # hint: '+' operator on policies is shorthand for parallel composition

    # Function to take each new packet pkt and update the forwarding policy
    # so subsequent incoming packets on this switch whose dstmac matches pkt's srcmac 
    # (accessed like in a dictionary pkt['srcmac']) will be forwarded out  pkt's inport 
    # (pyretic packets are located, so this value is accessed just like srcmac -  
    # i.e., p['inport'])
    def learn_from_a_packet(pkt):
        # Set the forwarding policy
        self.forward = if_(match(dstmac=pkt['srcmac'],
                                 switch=pkt['switch']), fwd(pkt['inport']),
                           self.policy)  # hint use 'match', '&', 'if_', and 'fwd' 
        # Update the policy
        self.policy = self.forward + self.query # hint you've already written this
        print self.policy 

    # learn_from_a_packet is called back every time our query sees a new packet
    self.query.register_callback(learn_from_a_packet)
    
    
def main():
    return act_like_switch()