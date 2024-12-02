/**
 * @brief main rule-related functions implementation file
 */

#include "vscc.h"

VsccRule * vsccRuleSequence( VsccRule **rules, size_t count ) {
    return NULL;
} // vsccRuleSequence

VsccRule * vsccRuleVariant( VsccRule **rules, size_t count ) {
    return NULL;
} // vsccRuleVariant

VsccRule * vsccRuleOptional( VsccRule *rule ) {
    return NULL;
} // vsccRuleOptional

VsccRule * vsccRuleRepeat( VsccRule *rule, bool atLeastOnce ) {
    return NULL;
} // vsccRuleRepeat

VsccRule * vsccRuleTerminal( const char *terminal ) {
    return NULL;
} // vsccRuleTerminal

VsccRule * vsccRuleEnd( void ) {
    return NULL;
} // vsccRuleEnd

VsccRule * vsccRuleEmpty( void ) {
    return NULL;
} // vsccRuleEmpty

VsccRule * vsccRuleClone( const VsccRule *rule ) {
    return NULL;
} // vsccRuleClone

void vsccRuleDtor( VsccRule *rule ) {
} // vsccRuleDtor
