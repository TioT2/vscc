/**
 * @brief main rule-related functions implementation file
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "vscc.h"

/**
 * @brief rule allocation function
 * 
 * @param[in]  additionalDataSize size of additional space required for rule
 * @param[out] ruleDst            rule allocation destination (non-null)
 * @param[out] additionalDataDst  additional data destination (nullable if additionalDataSize == 0)
 * 
 * @return true if allocated successfully, false if allocation failed.
 */
static bool vsccRuleAlloc( size_t additionalDataSize, VsccRule **ruleDst, void **additionalDataDst ) {
    assert(false
        || additionalDataSize != 0 && additionalDataDst != NULL
        || additionalDataSize == 0
    );

    const size_t alignedRuleSize = 0
        + sizeof(VsccRule) / sizeof(size_t) * sizeof(size_t)
        + (sizeof(VsccRule) % sizeof(size_t) != 0)
    ;
    void *data = calloc(alignedRuleSize + additionalDataSize, 1);

    if (data == NULL)
        return false;

    *ruleDst = (VsccRule *)data;
    if (additionalDataSize != 0)
        *additionalDataDst = (uint8_t *)data + alignedRuleSize;

    return true;
} // vsccRuleAlloc

VsccRule * vsccRuleSequence( VsccRule **rules, size_t count ) {
    VsccRule **array = NULL;
    VsccRule *result = NULL;

    if (!vsccRuleAlloc(count * sizeof(VsccRule *), &result, (void **)&array)) {
        for (size_t i = 0; i < count; i++)
            vsccRuleDtor(rules[i]);
        return NULL;
    }
    memcpy(array, rules, count * sizeof(VsccRule *));

    result->type = VSCC_RULE_SEQUENCE;
    result->sequence.count = count;
    result->sequence.rules = array;

    return result;
} // vsccRuleSequence

VsccRule * vsccRuleVariant( VsccRule **rules, size_t count ) {
    VsccRule **array = NULL;
    VsccRule *result = NULL;

    if (!vsccRuleAlloc(count * sizeof(VsccRule *), &result, (void **)&array)) {
        for (size_t i = 0; i < count; i++)
            vsccRuleDtor(rules[i]);
        return NULL;
    }
    memcpy(array, rules, count * sizeof(VsccRule *));

    result->type = VSCC_RULE_VARIANT;
    result->variant.count = count;
    result->variant.rules = array;

    return result;
} // vsccRuleVariant

VsccRule * vsccRuleOptional( VsccRule *rule ) {
    VsccRule *result = NULL;

    if (!vsccRuleAlloc(0, &result, NULL)) {
        vsccRuleDtor(rule);
        return NULL;
    }

    result->type = VSCC_RULE_OPTIONAL;
    result->optional = rule;

    return result;
} // vsccRuleOptional

VsccRule * vsccRuleRepeat( VsccRule *rule, bool atLeastOnce ) {
    VsccRule *result = NULL;

    if (!vsccRuleAlloc(0, &result, NULL)) {
        vsccRuleDtor(rule);
        return NULL;
    }
    
    result->type = VSCC_RULE_REPEAT;
    result->repeat.rule = rule;
    result->repeat.atLeastOnce = atLeastOnce;

    return result;
} // vsccRuleRepeat

VsccRule * vsccRuleTerminal( const char *terminal ) {
    const size_t length = strlen(terminal);
    char *resultTerminal = NULL;
    VsccRule *result = NULL;

    if (!vsccRuleAlloc(length + 1, &result, (void **)&resultTerminal))
        return NULL;

    memcpy(resultTerminal, terminal, length);

    result->type = VSCC_RULE_TERMINAL;
    result->terminal = resultTerminal;

    return result;
} // vsccRuleTerminal

VsccRule * vsccRuleEnd( void ) {
    VsccRule *result = NULL;

    if (!vsccRuleAlloc(0, &result, NULL))
        return NULL;

    result->type = VSCC_RULE_END;

    return result;
} // vsccRuleEnd

VsccRule * vsccRuleEmpty( void ) {
    VsccRule *result = NULL;

    if (!vsccRuleAlloc(0, &result, NULL))
        return NULL;

    result->type = VSCC_RULE_EMPTY;

    return result;
} // vsccRuleEmpty

VsccRule * vsccRuleClone( const VsccRule *rule ) {
    switch (rule->type) {
    case VSCC_RULE_SEQUENCE : {
        // allocate temporary array
        VsccRule **tmp = (VsccRule **)calloc(rule->sequence.count, sizeof(VsccRule *));

        if (tmp == NULL)
            return NULL;

        for (size_t i = 0; i < rule->sequence.count; i++)
            tmp[i] = vsccRuleClone(rule->sequence.rules[i]);

        VsccRule *result = vsccRuleSequence(tmp, rule->sequence.count);
        free(tmp);
        return result;
    }

    case VSCC_RULE_VARIANT  : {
        // allocate temporary array
        VsccRule **tmp = (VsccRule **)calloc(rule->variant.count, sizeof(VsccRule *));

        if (tmp == NULL)
            return NULL;

        for (size_t i = 0; i < rule->variant.count; i++)
            tmp[i] = vsccRuleClone(rule->variant.rules[i]);

        VsccRule *result = vsccRuleVariant(tmp, rule->variant.count);
        free(tmp);
        return result;

    }

    case VSCC_RULE_OPTIONAL :
        return vsccRuleOptional(vsccRuleClone(rule->optional));

    case VSCC_RULE_REPEAT   :
        return vsccRuleRepeat(vsccRuleClone(rule->repeat.rule), rule->repeat.atLeastOnce);

    case VSCC_RULE_TERMINAL :
        return vsccRuleTerminal(rule->terminal);

    case VSCC_RULE_END      :
        return vsccRuleEnd();

    case VSCC_RULE_EMPTY    :
        return vsccRuleEmpty();
    }

    assert(false && "Unreachable case reached.");
    return NULL;
} // vsccRuleClone

void vsccRuleDtor( VsccRule *rule ) {
    switch (rule->type) {
    case VSCC_RULE_SEQUENCE:
        for (size_t i = 0; i < rule->sequence.count; i++)
            vsccRuleDtor(rule->sequence.rules[i]);

    case VSCC_RULE_VARIANT:
        for (size_t i = 0; i < rule->variant.count; i++)
            vsccRuleDtor(rule->variant.rules[i]);

    case VSCC_RULE_OPTIONAL:
        vsccRuleDtor(rule->optional);

    case VSCC_RULE_REPEAT:
        vsccRuleDtor(rule->repeat.rule);

    case VSCC_RULE_TERMINAL:
    case VSCC_RULE_END:
    case VSCC_RULE_EMPTY:
        break;
    }

    free(rule);
} // vsccRuleDtor
