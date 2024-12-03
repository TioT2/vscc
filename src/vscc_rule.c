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
 * @note resulting rule and additional data are safe to free by applying free() to ruleDst
 * @note resulting rule and additional data are zeroed
 * @note this function **is not** setting defasult values to ruleDst or additionalDataDst in case if function fails.
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
    assert(count > 0);

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
    assert(count > 0);

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

VsccRule * vsccRuleStringTerminalFromSlice( const char *terminalBegin, const char *terminalEnd ) {
    const size_t length = terminalEnd - terminalBegin;
    char *resultTerminal = NULL;
    VsccRule *result = NULL;

    if (!vsccRuleAlloc(length + 1, &result, (void **)&resultTerminal))
        return NULL;

    // vsccRuleAlloc result is zeroed
    memcpy(resultTerminal, terminalBegin, length);

    result->type = VSCC_RULE_STRING_TERMINAL;
    result->stringTerminal = resultTerminal;

    return result;
} // vsccRuleStringTerminalFromSlice

VsccRule * vsccRuleStringTerminal( const char *terminal ) {
    return vsccRuleStringTerminalFromSlice(terminal, terminal + strlen(terminal));
} // vsccRuleStringTerminal

VsccRule * vsccRuleCharTerminal( const VsccRuleCharRange *ranges, size_t count ) {
    VsccRuleCharRange *resultRanges = NULL;
    VsccRule *result = NULL;

    if (!vsccRuleAlloc(count * sizeof(VsccRuleCharRange), &result, (void **)&resultRanges))
        return NULL;
    memcpy(resultRanges, ranges, sizeof(VsccRuleCharRange) * count);

    result->type = VSCC_RULE_CHAR_TERMINAL;
    result->charTerminal.count = count;
    result->charTerminal.ranges = resultRanges;

    return result;
} // vsccRuleCharTerminal

VsccRule * vsccRuleReferernceFromSlice( const char *refBegin, const char *refEnd ) {
    const size_t length = refEnd - refBegin;
    char *resultReference = NULL;
    VsccRule *result = NULL;

    if (!vsccRuleAlloc(length + 1, &result, (void **)&resultReference))
        return NULL;

    memcpy(resultReference, refBegin, length);

    result->type = VSCC_RULE_REFERENCE;
    result->reference = resultReference;

    return result;
} // vsccRuleReferernceFromSlice

VsccRule * vsccRuleReference( const char *reference ) {
    return vsccRuleReferernceFromSlice(reference, reference + strlen(reference));
} // vsccRuleReference

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

    case VSCC_RULE_OPTIONAL:
        return vsccRuleOptional(vsccRuleClone(rule->optional));

    case VSCC_RULE_REPEAT:
        return vsccRuleRepeat(vsccRuleClone(rule->repeat.rule), rule->repeat.atLeastOnce);

    case VSCC_RULE_STRING_TERMINAL:
        return vsccRuleStringTerminal(rule->stringTerminal);

    case VSCC_RULE_CHAR_TERMINAL:
        return vsccRuleCharTerminal(rule->charTerminal.ranges, rule->charTerminal.count);

    case VSCC_RULE_REFERENCE:
        return vsccRuleReference(rule->reference);

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
        break;

    case VSCC_RULE_VARIANT:
        for (size_t i = 0; i < rule->variant.count; i++)
            vsccRuleDtor(rule->variant.rules[i]);
        break;

    case VSCC_RULE_OPTIONAL:
        vsccRuleDtor(rule->optional);
        break;

    case VSCC_RULE_REPEAT:
        vsccRuleDtor(rule->repeat.rule);
        break;

    case VSCC_RULE_REFERENCE:
    case VSCC_RULE_CHAR_TERMINAL:
    case VSCC_RULE_STRING_TERMINAL:
    case VSCC_RULE_END:
    case VSCC_RULE_EMPTY:
        break;
    }

    free(rule);
} // vsccRuleDtor

void vsccRulePrint( FILE *out, const VsccRule *rule ) {
    switch (rule->type) {
    case VSCC_RULE_SEQUENCE: {
        fprintf(out, "{");

        assert(rule->sequence.count > 0);
        vsccRulePrint(out, rule->sequence.rules[0]);

        for (size_t i = 1; i < rule->sequence.count; i++) {
            fprintf(out, " ");
            vsccRulePrint(out, rule->sequence.rules[i]);
        }
        fprintf(out, "}");
        break;
    }

    case VSCC_RULE_VARIANT: {
        fprintf(out, "{");

        assert(rule->sequence.count > 0);
        vsccRulePrint(out, rule->variant.rules[0]);

        for (size_t i = 1; i < rule->variant.count; i++) {
            fprintf(out, " | ");
            vsccRulePrint(out, rule->variant.rules[i]);
        }
        fprintf(out, "}");
        break;
    }

    case VSCC_RULE_OPTIONAL: {
        bool surround = true
            && rule->optional->type != VSCC_RULE_VARIANT
            && rule->optional->type != VSCC_RULE_SEQUENCE
            && rule->optional->type != VSCC_RULE_REPEAT
            && rule->optional->type != VSCC_RULE_OPTIONAL
        ;

        if (surround) fprintf(out, "{");
        vsccRulePrint(out, rule->optional);
        if (surround) fprintf(out, "}");
        fprintf(out, "?");
        break;
    }

    case VSCC_RULE_REPEAT: {
        bool surround = true
            && rule->repeat.rule->type != VSCC_RULE_VARIANT
            && rule->repeat.rule->type != VSCC_RULE_SEQUENCE
            && rule->repeat.rule->type != VSCC_RULE_REPEAT
            && rule->repeat.rule->type != VSCC_RULE_OPTIONAL
        ;

        if (surround) fprintf(out, "{");
        vsccRulePrint(out, rule->repeat.rule);
        if (surround) fprintf(out, "}");
        fprintf(
            out,
            rule->repeat.atLeastOnce
                ? "+"
                : "*"
        );
        break;
    }

    case VSCC_RULE_STRING_TERMINAL:
        fprintf(out, "\"%s\"", rule->stringTerminal);
        break;

    case VSCC_RULE_CHAR_TERMINAL:
        fprintf(out, "[");
        for (size_t i = 0; i < rule->charTerminal.count; i++) {
            VsccRuleCharRange range = rule->charTerminal.ranges[i];

            if (range.first == range.last)
                fprintf(out, "%s%c",
                    range.first == '-' ? "\\" : "",
                    range.first
                );
            else
                fprintf(out, "%s%c-%s%c",
                    range.first == '-' ? "\\" : "",
                    range.first,
                    range.last == '-' ? "\\" : "",
                    range.last
                );
        }
        fprintf(out, "]");
        break;

    case VSCC_RULE_REFERENCE:
        fprintf(out, "%s", rule->reference);
        break;

    case VSCC_RULE_END:
        fprintf(out, "$");
        break;

    case VSCC_RULE_EMPTY:
        // literally empty
        break;
    }
} // vsccRulePrint

// vscc_rule.c
