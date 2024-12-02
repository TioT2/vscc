/**
 * @brief main project header
 */

#ifndef VSCC_H_
#define VSCC_H_

#include <stdbool.h>
#include <stdio.h>

/// @brief rule type ('tag')
typedef enum __VsccRuleType {
    VSCC_RULE_SEQUENCE,        ///< first and second           ... ...
    VSCC_RULE_VARIANT,         ///< first or second            ... | ...
    VSCC_RULE_OPTIONAL,        ///< once or none               ... ?
    VSCC_RULE_REPEAT,          ///< repeat n times             ...* or ...+
    VSCC_RULE_STRING_TERMINAL, ///< string terminal symbol     "..."
    VSCC_RULE_CHAR_TERMINAL,   ///< character terminal symbol  [...]
    VSCC_RULE_REFERENCE,       ///< reference to another rule  ...
    VSCC_RULE_END,             ///< sequence end               $
    VSCC_RULE_EMPTY,           ///< empty rule                 
} VsccRuleType;

/// @brief grammar rule structure forward declaration
typedef struct __VsccRule VsccRule;

/// @brief character range
typedef struct __VsccRuleCharRange {
    char first; ///< first matched character
    char last;  ///< last matched character
} VsccRuleCharRange;

/// @brief grammar rule representation structure
struct __VsccRule {
    VsccRuleType type; ///< rule type

    union {
        struct {
            VsccRule ** rules; ///< first element
            size_t      count; ///< count of rules in sequence
        } sequence;

        struct {
            VsccRule ** rules; ///< rules
            size_t      count; ///< count of rules
        } variant;

        struct {
            bool       atLeastOnce; ///< it's required to repeat the rule at least once
            VsccRule * rule;        ///< repeated rule
        } repeat;

        struct {
            VsccRuleCharRange * ranges; ///< character ranges
            size_t              count;  ///< count of matched character ranges
        } charTerminal;

        VsccRule *optional;         ///< optional rule
        const char *stringTerminal; ///< string constant
        const char *reference;      ///< reference to another rule
    };
}; // struct __VsccRule

/**
 * @brief sequence rule constructor
 * 
 * @param[in] rules rules to build sequence of array (non-null)
 * @param[in] count count of rules in rule set (>= 1)
 * 
 * @note function gathers ownership of **elements** of 'rules' array, but not of 'rules' array itself
 * 
 * @return rule that represents concatenation of 'rules' rules.
 */
VsccRule * vsccRuleSequence( VsccRule **rules, size_t count );

/**
 * @brief variant rule constructor
 * 
 * @param[in] rules rules to build sequence of array (non-null)
 * @param[in] count count of rules in rule set (>= 1)
 * 
 * @note function gathers ownership of **elements** of 'rules' array, but not of 'rules' array itself
 * 
 * @return rule that represents variant of 'rules' rules.
 */
VsccRule * vsccRuleVariant( VsccRule **rules, size_t count );

/**
 * @brief optional rule construction function
 * 
 * @param[in] rule rule to create optional for (non-null)
 * 
 * @return rule that represents option of 'rule' rule.
 */
VsccRule * vsccRuleOptional( VsccRule *rule );

/**
 * @brief repeating rule create function
 * 
 * @param[in] rule        rule to create repeat of
 * @param[in] atLeastOnce should this rule be repeated at least one time
 * 
 * @return repeat rule
 */
VsccRule * vsccRuleRepeat( VsccRule *rule, bool atLeastOnce );

/**
 * @brief terminal symbol rule constructor
 * 
 * @param[in] terminal terminal symbol to construct rule based on (non-null, null-terminated)
 * 
 * @return created rule
 */
VsccRule * vsccRuleStringTerminal( const char *terminal );

/**
 * @brief terminal symbol rule constructor
 * 
 * @param[in] ranges supported character range array (non-null)
 * @param[in] count  count of ranges in array (>= 1)
 * 
 * @return created rule
 */
VsccRule * vsccRuleCharTerminal( const VsccRuleCharRange *ranges, size_t count );

/**
 * @brief referential rule constructor
 * 
 * @param[in] reference referenced rule name
 * 
 * @return created rule
 */
VsccRule * vsccRuleReference( const char *reference );

/**
 * @brief rule that signals about sequence end create function
 * 
 * @return end rule
 */
VsccRule * vsccRuleEnd( void );

/**
 * @brief empty rule create function
 * 
 * @return empty rule
 */
VsccRule * vsccRuleEmpty( void );

/**
 * @brief rule cloning function
 * 
 * @param[in] rule rule to clone
 * 
 * @return exact copy of 'rule' rule
 */
VsccRule * vsccRuleClone( const VsccRule *rule );

/**
 * @brief rule destructor
 * 
 * @param[in] rule rule to destroy (nullable)
 */
void vsccRuleDtor( VsccRule *rule );

/**
 * @brief rule display function
 * 
 * @param[in] out  text file to write rule to
 * @param[in] rule rule to display (non-null)
 */
void vsccRulePrint( FILE *out, const VsccRule *rule );

/// @brief rule parsing status
typedef enum __VsccRuleParseStatus {
    VSCC_RULE_PARSE_OK,             ///< parsing succeeded
    VSCC_RULE_PARSE_INTERNAL_ERROR, ///< internal error occured
} VsccRuleParseStatus;

/// @brief rule parsing result
typedef struct __VsccRuleParseResult {
    VsccRuleParseStatus status; ///< operation status

    union {
        struct {
            const char * rest;   ///< rest of string to be parsed
            VsccRule   * result; ///< parsed rule
        } ok; ///< successful result
    };
} VsccRuleParseResult;

/**
 * @brief rule parsing function
 * 
 * @param[in]  strBegin start of string slice to parse rule from
 * @param[in]  strEnd   end of string slice to parse rule from
 * 
 * @return true if parsed, false if not
 */
VsccRuleParseResult vsccRuleParse( const char *strBegin, const char *strEnd );

/// @brief name-rule pair
typedef struct __VsccGrammarPair {
    const char * name; ///< rule name
    VsccRule   * rule; ///< rule itself
} VsccGrammarPair;

/// @brief grammar representation structure
typedef struct __VsccGrammar {
    size_t            ruleCount; ///< count of rules
    VsccGrammarPair * rules;     ///< rules themselves
} VsccGrammar;

#endif // !defined(VSCC_H_)

// vscc.h
