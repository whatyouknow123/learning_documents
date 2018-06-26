
from pyretic.lib.corelib import *
from pyretic.lib.std import *
from pyretic.examples.pyretic_switch import act_like_switch

import os, csv
from csv import DictReader

policy_file = "%s/pyretic/pyretic/examples/firewall-policies.csv" % os.environ[ 'HOME']

def main():
# start with a policy that doesn't match any packets
    not_allowed = none
    # read data from policy file
    with open(policy_file, "r") as policy_content:
        dictReader = csv.DictReader(policy_content)
        # add the forbidden policy and not allow the two side communication		
        for d in dictReader:    
            not_allowed = not_allowed + (match(srcmac=MAC(d['mac_0']))&match(dstmac=MAC(d['mac_1']))) + (match(srcmac=MAC(d['mac_1']))&match(d=MAC(count['mac_0'])))
    # add the allowed rules
	allowed = ~not_allowed
	# print allowed
	print allowed
	# regard the allowed switch input as the act_like_switch of pyretic_switch
    return allowed>>act_like_switch() 
