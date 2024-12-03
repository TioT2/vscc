/**
 * @brief rule parser implementation file
 */

#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "vscc.h"

/// @brief parser representation structure
typedef struct __VsccRuleParser {
    const char         * strRest; ///< rest of string to be parsed
    const char         * strEnd;  ///< end of substring to be parsed
} VsccRuleParser;

static void vsccRuleParserSkipSpaces( VsccRuleParser *self ) {
    while (self->strRest < self->strEnd && isspace(*self->strRest))
        self->strRest++;
} // vsccRuleParserSkipSpaces

static VsccRuleParseResult vsccRuleParseTerminal( VsccRuleParser *self ) {
    vsccRuleParserSkipSpaces(self);

    if (self->strRest >= self->strEnd)
        return (VsccRuleParseResult) { .status = VSCC_RULE_PARSE_UNEXPECTED_TEXT_END };
    
    // currently processed character
    char current = *self->strRest;

    switch (current) {
    case '[': {
        assert(false && "Character range parsing isn't supported yet(");
        break;
    }

    case '{': {
        assert(false && "Group parsing isn't implemented yet");
        break;
    }

    case '\"': {
        // parse string terminal
        const char *termBegin = self->strRest + 1;

        while (self->strRest < self->strEnd && *self->strRest != '\"')
            self->strRest++;

        VsccRule *rule = vsccRuleStringTerminalFromSlice(termBegin, self->strRest);

        return rule == NULL
            ? (VsccRuleParseResult) { .status = VSCC_RULE_PARSE_INTERNAL_ERROR }
            : (VsccRuleParseResult) {
                .status = VSCC_RULE_PARSE_OK,
                .ok = rule,
            }
        ;
    }

    default: {
        VsccRule *result = NULL;

        const char *anyChar = "__char__";
        const size_t anyCharLen = strlen(anyChar);

        if (self->strRest + anyCharLen <= self->strEnd && strncmp(self->strRest, anyChar, anyCharLen) == 0) {
            self->strRest += anyCharLen;

            const VsccRuleCharRange range = { .first = '\x00', .last = '\xFF' };

            result = vsccRuleCharTerminal(&range, 1);
        } else {
            const char *identStart = self->strRest;

            // parse ident
            while (self->strRest < self->strEnd && (isalnum(*self->strRest) || *self->strRest == '_'))
                self->strRest++;

            result = vsccRuleReferernceFromSlice(identStart, self->strRest);
        }

        return result == NULL
            ? (VsccRuleParseResult) { .status = VSCC_RULE_PARSE_INTERNAL_ERROR }
            : (VsccRuleParseResult) {
                .status = VSCC_RULE_PARSE_OK,
                .ok = result
            }
        ;
    }
    }

} // vsccRuleParseTerminal

static VsccRuleParseResult vsccRuleParseImpl( VsccRuleParser *self ) {
    return vsccRuleParseTerminal(self); // Tough Mega Parser solution, actually
} // vsccRuleParseImpl

VsccRuleParseResult vsccRuleParse( const char *strBegin, const char *strEnd ) {
    VsccRuleParser parser = (VsccRuleParser) {
        .strRest = strBegin,
        .strEnd  = strEnd,
    };

    // skip initial spaces
    vsccRuleParserSkipSpaces(&parser);

    assert(false && "rule parsing is not implemented yet");
    return (VsccRuleParseResult) {};
} // vsccRuleParse

// vscc_rule_parse.c
