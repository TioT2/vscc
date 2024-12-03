/**
 * @brief main project header
 */

#ifndef VSCC_H_
#define VSCC_H_

#include <stdbool.h>
#include <stdio.h>

/// @brief dynamic array representation structure
typedef struct __VsccArrayImpl * VsccArray;

/**
 * @brief array constructor
 * 
 * @param[in] elementSize single array element size ( > 0)
 * 
 * @return created array (may be NULL)
 */
VsccArray vsccArrayCtor( size_t elementSize );

/**
 * @brief array destructor
 * 
 * @param[in] array array to destroy (nullable)
 */
void vsccArrayDtor( VsccArray array );

/**
 * @brief array current capacity getting function
 * 
 * @param[in] array arrat to get capacity of (non-null)
 * 
 * @return size of array
 */
size_t vsccArraySize( const VsccArray array );

/**
 * @brief array data getting function
 * 
 * @param[in] array array to get data of (non-null)
 * 
 * @return array data start pointer
 * 
 * @note it's ok for user to read/write elementSize * vsccArraySize(...) bytes to this array.
 */
void * vsccArrayData( VsccArray array );

/**
 * @brief array element by index getting function
 * 
 * @param[in] array arrat to get pointer to element of
 */
void * vsccGetArrayElement( VsccArray array, size_t index );

/**
 * @brief pushing function
 * 
 * @param[in,out] array array to push data to (non-null)
 * @param[in]     data  data to push (non-null, at least elementSize bytes readable)
 * 
 * @return true if operation succeeded, false if not
 * 
 * @note array repains unmodified in case if operation don't succeeded
 */
bool vsccArrayPush( VsccArray *array, const void *data );

/**
 * @brief popping function
 * 
 * @param[in,out] array array to pop data from (non-null)
 * @param[out]    data  data to pop destination (nullable, at least elementSize bytes writable)
 * 
 * @return true if operation succeeded, false if not
 */
bool vsccArrayPop( VsccArray *array, void *data );

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
 * @brief string terminal from string slice constructor
 * 
 * @param[in] terminalBegin begin of string slice to construct terminal from (inclusive, non-null)
 * @param[in] terminalEnd   end of string slice to construct terminal from (exclusive, non-null, >= terminalBegin)
 * 
 * @return created rule
 */
VsccRule * vsccRuleStringTerminalFromSlice( const char *terminalBegin, const char *terminalEnd );

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
 * @brief refererntial rule from string slice constructor
 * 
 * @param[in] refBegin begin of string slice to construct reference from (inclusive, non-null)
 * @param[in] refEnd   end of string slice to construct reference from (exclusive, non-null, >= refBegin)
 * 
 * @return created rule
 */
VsccRule * vsccRuleReferernceFromSlice( const char *refBegin, const char *refEnd );

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
    VSCC_RULE_PARSE_OK,                   ///< parsing succeeded
    VSCC_RULE_PARSE_INTERNAL_ERROR,       ///< internal error occured
    VSCC_RULE_PARSE_UNEXPECTED_TEXT_END, ///< unexpected end of text slice 
} VsccRuleParseStatus;

/// @brief rule parsing result
typedef struct __VsccRuleParseResult {
    VsccRuleParseStatus status; ///< operation status

    union {
        VsccRule * ok; ///< parsed rule
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
