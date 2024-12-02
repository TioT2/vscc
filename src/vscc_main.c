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
    VsccRule *rules[] = {
        vsccRuleTerminal("amogus"),
        vsccRuleTerminal("sus"),
        vsccRuleEnd()
    };

    VsccRule *seq = vsccRuleOptional(vsccRuleSequence(rules, 3));

    printf("seq ::= ");
    vsccRulePrint(stdout, seq);
    printf("\n");

    vsccRuleDtor(seq);

    return 0;
} // main

// vscc_main.c
