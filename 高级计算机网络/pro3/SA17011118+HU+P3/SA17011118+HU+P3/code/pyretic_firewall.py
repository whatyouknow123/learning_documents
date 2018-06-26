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

# insert the name of the module and policy you want to import
from pyretic.examples.pyretic_switch import act_like_switch
import os
import csv
from csv import DictReader
policy_file = "%s/pyretic/pyretic/examples/firewall_policies.csv" % os.environ[ 'HOME']

def main():
    # start with a policy that doesn't match any packets
    not_allowed = none
    # and sdd traffic that isn't allowed
    with open(policy_file, "r") as csvfile:
        dictreader = DictReader(csvfile)
        for d in dictreader:
            not_allowed = not_allowed + 
            (match(srcmac=MAC(d['mac_0']))&match(dstmac=MAC(d['mac_1']))) + 
            (match(srcmac=MAC(d['mac_1']))&match(dstmac=MAC(d['mac_0'])))
    
    #express allowed traffic in terms of not_allowed - hint use'~'
    allowed = ~not_allowed

    # and only send allowed traffic to the mac learning (act_like_switch) logic
    return allowed>>act_like_switch()
