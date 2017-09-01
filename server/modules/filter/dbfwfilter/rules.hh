#pragma once
/*
 * Copyright (c) 2016 MariaDB Corporation Ab
 *
 * Use of this software is governed by the Business Source License included
 * in the LICENSE.TXT file and at www.mariadb.com/bsl11.
 *
 * Change Date: 2020-01-01
 *
 * On the date above, in accordance with the Business Source License, use
 * of this software will be governed by version 2 or later of the General
 * Public License.
 */

#include "dbfwfilter.hh"

#include <maxscale/pcre2.hh>

/**
 * A structure used to identify individual rules and to store their contents
 *
 * Each type of rule has different requirements that are expressed as void pointers.
 * This allows to match an arbitrary set of rules against a user.
 */
class Rule
{
    Rule(const Rule&);
    Rule& operator=(const Rule&);

public:
    Rule(std::string name);
    virtual ~Rule();
    virtual bool matches_query(FW_SESSION* session, GWBUF* buffer, char** msg);
    virtual bool need_full_parsing(GWBUF* buffer) const;
    bool matches_query_type(GWBUF* buffer);

    void*          data;          /*< Actual implementation of the rule */
    std::string    name;          /*< Name of the rule */
    ruletype_t     type;          /*< Type of the rule */
    uint32_t       on_queries;    /*< Types of queries to inspect */
    int            times_matched; /*< Number of times this rule has been matched */
    TIMERANGE*     active;        /*< List of times when this rule is active */
};

/**
 * Matches if a query uses the wildcard character, `*`.
 */
class WildCardRule: public Rule
{
    WildCardRule(const WildCardRule&);
    WildCardRule& operator=(const WildCardRule&);

public:
    WildCardRule(std::string name):
        Rule(name)
    {
    }

    ~WildCardRule()
    {
    }

    bool need_full_parsing(GWBUF* buffer) const
    {
        return true;
    }

    bool matches_query(FW_SESSION* session, GWBUF* buffer, char** msg);
};

/**
 * Matches if a query has no WHERE clause
 */
class NoWhereClauseRule: public Rule
{
    NoWhereClauseRule(const NoWhereClauseRule&);
    NoWhereClauseRule& operator=(const NoWhereClauseRule&);

public:
    NoWhereClauseRule(std::string name):
        Rule(name)
    {
    }

    ~NoWhereClauseRule()
    {
    }

    bool need_full_parsing(GWBUF* buffer) const
    {
        return true;
    }

    bool matches_query(FW_SESSION* session, GWBUF* buffer, char** msg);

};

class ValueListRule: public Rule
{
    ValueListRule(const ValueListRule&);
    ValueListRule& operator=(const ValueListRule&);

public:
    bool need_full_parsing(GWBUF* buffer) const
    {
        return true;
    }

protected:
    ValueListRule(std::string name, const ValueList& values):
        Rule(name),
        m_values(values)
    {
    }

    ValueList m_values;
};

/**
 * Matches if a query uses one of the columns
 */
class ColumnsRule: public ValueListRule
{
    ColumnsRule(const ColumnsRule&);
    ColumnsRule& operator=(const ColumnsRule&);

public:
    ColumnsRule(std::string name, const ValueList& values):
        ValueListRule(name, values)
    {
    }

    bool matches_query(FW_SESSION* session, GWBUF* buffer, char** msg);
};

/**
 * Matches if a query uses one of the functions
 */
class FunctionRule: public ValueListRule
{
    FunctionRule(const FunctionRule&);
    FunctionRule& operator=(const FunctionRule&);

public:
    FunctionRule(std::string name, const ValueList& values):
        ValueListRule(name, values)
    {
    }

    bool matches_query(FW_SESSION* session, GWBUF* buffer, char** msg);
};


/**
 * Matches if a query uses any functions
 */
class FunctionUsageRule: public ValueListRule
{
    FunctionUsageRule(const FunctionUsageRule&);
    FunctionUsageRule& operator=(const FunctionUsageRule&);

public:
    FunctionUsageRule(std::string name, const ValueList& values):
        ValueListRule(name, values)
    {
    }

    bool matches_query(FW_SESSION* session, GWBUF* buffer, char** msg);
};


/**
 * Matches if a queries are executed too quickly
 */
class LimitQueriesRule: public Rule
{
    LimitQueriesRule(const LimitQueriesRule&);
    LimitQueriesRule& operator=(const LimitQueriesRule&);

public:
    LimitQueriesRule(std::string name, int max, int timeperiod, int holdoff):
        Rule(name),
        m_max(max),
        m_timeperiod(timeperiod),
        m_holdoff(holdoff)
    {
    }

    ~LimitQueriesRule()
    {
    }

    bool need_full_parsing(GWBUF* buffer) const
    {
        return true;
    }

    bool matches_query(FW_SESSION* session, GWBUF* buffer, char** msg);

private:
    int m_max;
    int m_timeperiod;
    int m_holdoff;
};

/**
 * Matches if a queries matches a pattern
 */
class RegexRule: public Rule
{
    RegexRule(const RegexRule&);
    RegexRule& operator=(const RegexRule&);

public:
    RegexRule(std::string name, pcre2_code* re):
        Rule(name),
        m_re(re)
    {
    }

    ~RegexRule()
    {
    }

    bool need_full_parsing(GWBUF* buffer) const
    {
        return false;
    }

    bool matches_query(FW_SESSION* session, GWBUF* buffer, char** msg);

private:
    mxs::Closer<pcre2_code*> m_re;
};

typedef std::tr1::shared_ptr<Rule> SRule;
typedef std::list<SRule>           RuleList;
