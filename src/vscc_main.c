/**
 * @brief main project file
 */

#include <stdio.h>

#include "vscc.h"

/**
 * @brief main project function
 *
 * @return exit status
 */
int main( void ) {
    VsccRuleCharRange ranges[] = {
        {'a', 'z'},
        {'A', 'Z'},
        {'0', '9'},
        {'_', '_'},
    };
    VsccRule *rules[] = {
        vsccRuleStringTerminal("amogus"),
        vsccRuleStringTerminal("sus"),
        vsccRuleCharTerminal(ranges, 4),
        vsccRuleEnd()
    };

    VsccRule *seq = vsccRuleOptional(vsccRuleSequence(rules, 4));

    printf("seq ::= ");
    vsccRulePrint(stdout, seq);
    printf("\n");

    vsccRuleDtor(seq);

    return 0;
} // main

// vscc_main.c
