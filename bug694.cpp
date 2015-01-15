/**
 * @file bug694.cpp  - regression test for bug694 ("RWSplit: SELECT @a:=@a+1 as a, test.b FROM test breaks client session")
 *
 * - set use_sql_variables_in=all in MaxScale.cnf
 * - connect to readwritesplit router and execute:
 * @verbatim
CREATE TABLE test (b integer);
SELECT @a:=@a+1 as a, test.b FROM test;
USE test
@endverbatim
 * - check if MaxScale alive
 */

#include <my_config.h>
#include <iostream>
#include "testconnections.h"
#include "mariadb_func.h"

int main()
{
    TestConnections * Test = new TestConnections();
    int global_result = 0;

    Test->ReadEnv();
    Test->PrintIP();

    Test->ConnectMaxscale();

    printf("Trying SELECT @a:=@a+1 as a, test.b FROM test\n"); fflush(stdout);
    global_result += execute_query(Test->conn_rwsplit, "CREATE TABLE test (b integer);");
    if (execute_query(Test->conn_rwsplit, "SELECT @a:=@a+1 as a, test.b FROM test;") == 0) {
        printf("Query succeded, bu expected to fail. Test FAILED!\n"); fflush(stdout);
        global_result++;
    }
    printf("Trying USE test\n"); fflush(stdout);
    global_result += execute_query(Test->conn_rwsplit, "USE test");

    printf("Checking if MaxScale alive\n"); fflush(stdout);
    Test->CloseMaxscaleConn();

    printf("Checking logs\n"); fflush(stdout);
    global_result    += CheckLogErr((char *) "Warning : The query can't be routed to all backend servers because it includes SELECT and SQL variable modifications which is not supported", TRUE);
    global_result    += CheckLogErr((char *) "SELECT with session data modification is not supported if configuration parameter use_sql_variables_in=all", TRUE);

    global_result += CheckMaxscaleAlive();

    return(global_result);
}

