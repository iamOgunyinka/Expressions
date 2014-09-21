/*
 * main.cpp
 * Date: 2014-09-08
 * Author: Karsten Ahnert (karsten.ahnert@gmx.de)
 * Copyright: Karsten Ahnert
 *
 *
 */

#include "expression_io.hpp"

#include <iostream>
#include <fstream>

using namespace Expression;

int main( int argc , char *argv[] )
{
    expr_ptr root1 =  make_plus( make_sin( make_variable<0>( "t", 1245 ) ) ,  make_constant( 2.0 ) );
    std::ofstream fout( "expr1.dot" );
    fout << to_graphviz( root1 );

    std::cout << "Expr1 : " << to_polish( root1 ) << "\n";
    std::cout << "Evaluation yields: " << evaluate_expr( root1 ) << std::endl;
    
    expr_ptr root2 =
        make_plus(
            make_plus( make_variable<0>("u"), make_constant( 1.0 ) ) ,
            make_plus(
                make_plus( make_constant( 11 ) , make_variable<1>( ) ) ,
                make_sin( make_variable<2>( "x", 90 ) )
            ) );
    
    std::ofstream fout2( "expr2.dot" );
    fout2 << to_graphviz( root2 );
    std::cout << "Expr2 : " << to_polish( root2 ) << "\n";
    std::cout << "Evaluation of Expr2: " << evaluate_expr( root2 ) << std::endl;

    //~ Must have the same tree as root2 above
    auto root3 = from_polish( to_polish( root1 ) );
    std::cout << "Evaluation of Expr3: " << evaluate_expr( root3 ) << std::endl;
    std::cout << to_polish( root3 ) << std::endl;
    return 0;
}

