/**
 * @brief main project header
 */

#ifndef VSCC_H_
#define VSCC_H_

#include <stddef.h>
#include <stdbool.h>

/// @brief rule type ('tag')
typedef enum __VsccRuleType {
    VSCC_RULE_SEQUENCE,   ///< ... , ... (first and second)
    VSCC_RULE_VARIANT,    ///< ... | ... (first or second)
    VSCC_RULE_OPTIONAL,   ///< [ ... ] (once or none)
    VSCC_RULE_REPEAT,     ///< { ... } (repeat n times)
    VSCC_RULE_TERMINAL,   ///< terminal symbol
    VSCC_RULE_END,        ///< sequence end
    VSCC_RULE_EMPTY,      ///< empty rule
} VsccRuleType;

/// @brief grammar rule structure forward declaration
typedef struct __VsccRule VsccRule;

/// @brief grammar rule representation structure
struct __VsccRule {
    VsccRuleType type; ///< rule type

    union {
        struct {
            VsccRule * rules; ///< first element
            size_t     count; ///< count of rules in sequence
        } sequence;

        struct {
            VsccRule * rules; ///< rules
            size_t     count; ///< count of rules
        } variant;

        struct {
            bool       atLeastOnce; ///< it's required to repeat the rule at least once
            VsccRule * rule;        ///< repeated rule
        } repeat;

        VsccRule *option;     ///< optional rule
        const char *terminal; ///< string constant
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
VsccRule * vsccRuleTerminal( const char *terminal );

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

/// @brief generate C parser for some kind of 
typedef struct __VsccGenerateInfo {
    VsccGrammar * grammar; ///< grammar to generate text by
    const char  * prefix;  ///< generated function prefix
} VsccGenerateInfo;

#endif // !defined(VSCC_H_)

// vscc.h
