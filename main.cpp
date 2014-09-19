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
    expr_ptr root1 =  make_plus( make_sin(  make_variable<0>( 11.1 ) ) ,  make_constant( 2.0 ) );
    std::ofstream fout( "expr1.dot" );
    fout << to_graphviz( root1 );

    std::cout << "Expr1 : " << to_polish( root1 ) << "\n";
    auto d = std::sin( 11.1 ) + 2.0;
    std::cout << "Evaluation yields: " << evaluate_expr( root1 ) << " " << d << std::endl;

    expr_ptr root2 =
    make_plus(
        make_plus( make_variable<0>(), make_constant( 1.0 ) ) ,
        make_plus(
            make_plus( make_constant( -2.0 ) , make_variable<1>( 11.1 ) ) ,
            make_sin( make_variable<2>( 14.1 ) )
        ) );
    //Manual arrangement of root2 tree
    d = ( ( 0.0 + 1.0 ) + ( ( -2.0 + 11.1 ) + std::sin( 14.1 ) ) );
    std::ofstream fout2( "expr2.dot" );
    fout2 << to_graphviz( root2 );
    std::cout << "Expr2 : " << to_polish( root2 ) << "\n";
    std::cout << "Evaluation of Expr2: " << evaluate_expr( root2 ) << " " << d << std::endl;
    return 0;
}

