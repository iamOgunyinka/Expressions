/*
 * expression_io.h
 * Date: 2014-09-11
 * Author: Karsten Ahnert (karsten.ahnert@gmx.de)
 * Copyright: Karsten Ahnert
 *
 *
 */

#ifndef EXPRESSION_IO_H_INCLUDED
#define EXPRESSION_IO_H_INCLUDED

#include "lexer.hpp"
#include <sstream>
#include <deque>

using namespace EditedExpression;

namespace Expression
{
    
    inline expr_ptr build_tree_for( expression_type const & expr_constant )
    {
        switch( expr_constant ){
            case expression_type::Plus: default: return make_plus( nullptr, nullptr );
        }
    }
    
    void attach( expr_ptr const &a, std::deque<expression_type> &operator_deq, std::deque<expr_ptr> &children_deq )
    {
        for( size_t i = 0; i != a->num_children(); ++i ){
            expression_type type_of_expr = a->get_children( i )->get_type();
            if( type_of_expr == expression_type::Plus ){
                operator_deq.push_back( type_of_expr );
                attach( a->get_children( i ), operator_deq, children_deq );
            } else {
                children_deq.push_back( a->get_children( i ) );
            }
        }
    }
    
    expr_ptr build_rightmost_tree( expr_ptr const &a, expr_ptr &node, expression_type in )
    {
        expr_ptr right_tree = build_tree_for( in );
        right_tree->set_children( 1, a );
    
        right_tree.swap( node );
    
        node->set_children( 0, right_tree );
        return node->get_children( 1 );
    }
    
    expr_ptr linearize( expr_ptr const & expression )
    {
        std::deque<expr_ptr> child_deq {};
        std::deque<expression_type> op_deq { expression->get_type() };
        
        expr_ptr tree_to_build = nullptr, ptr = nullptr;
        expr *x = tree_to_build.get();
    
        attach( expression, op_deq, child_deq );
        while( !op_deq.empty() ){
            if( !tree_to_build ){
                tree_to_build = build_tree_for( op_deq.front() ); op_deq.pop_front();
                tree_to_build->set_children( 0, child_deq.front() );
                child_deq.pop_front();
                
                tree_to_build->set_children( 1, child_deq.front() );
                child_deq.pop_front();
                
                ptr = tree_to_build->get_children( 1 );
                x = tree_to_build.get();
            } else {
                auto foo = build_rightmost_tree( child_deq.front(), ptr, op_deq.front() );
                op_deq.pop_front();
                child_deq.pop_front();
                x->set_children( 1, ptr );
    
                x = x->get_children( 1 ).get();
    
                ptr = foo;
            }
        }
        return tree_to_build;
    }
    inline double to_double ( std::string const & str )
    {
        return std::stod( str );
    }
    
    inline void update_current_token( Lexer &lex, Lexer::token_type &token )
    {
        if( !lex.eof() ){
            token = lex.get_token();
        } else {
            token.first.get_lexeme().clear();
            token.second = expression_type::None;
        }
    }
    
/*
 * 
 * name: get_index_from
 * @param: string
 * @return: integer
 * This function is an hack, why? Making variable requires a compile-time template parameter constant I, extracting the index from a
 * polish notation, such as var0, requires a runtime calculation in order to get the last digit 0 or 10 in case it is var10, this is
 * why I created this function and an accompanying function make_variable_with_index( int ), which will serve as an index.
 */
    inline int get_index_from( std::string const & str )
    {
        std::string::size_type found = str.find_first_of( "1234567890" );
        return std::stoi( std::string { str.begin() + found, str.end() } );
    }
    expr_ptr build_unary_operator_or_variable_from( Lexer::token_type const & token )
    {
        if( token.first.lexeme() == std::string { "cos" } ){
            return make_cos( nullptr );
        } else if( token.first.lexeme() == std::string { "sin" } ){
            return make_sin( nullptr );
        } else {
            unsigned int const index = get_index_from( token.first.lexeme() );
            return make_variable_with_index<>( index );
        }
    }
    
    expr_ptr convert_token_to_expression( Lexer::token_type const & token )
    {
        switch( token.second ){
            case expression_type::None: default:        return nullptr;
            case expression_type::Constant:             return make_constant( to_double( token.first.lexeme() ) );
            case expression_type::Variable:
            case expression_type::UnaryFunc:            return build_unary_operator_or_variable_from( token );
            case expression_type::Plus:                 return make_plus( nullptr, nullptr );
            case expression_type::Minus:                return make_minus( nullptr, nullptr );
            case expression_type::Divides:              return make_divided( nullptr, nullptr );
            case expression_type::Multiplies:           return make_multiplies( nullptr, nullptr );
        }
    }
    inline expr_ptr get_root_node( Lexer & lex )
    {
        return convert_token_to_expression( lex.get_token() );
    }
    
    expr_ptr insert_child( expr_ptr node_to_insert, Lexer & lex, Lexer::token_type & token )
    {
        auto curr_ptr = node_to_insert;
        if( curr_ptr ){
            for( size_t i = 0; i != curr_ptr->num_children(); ++i ){
                update_current_token( lex, token );
                node_to_insert = convert_token_to_expression( token );
                curr_ptr->set_children( i, insert_child( node_to_insert, lex, token ) );
            }
        }
        return curr_ptr;
    }
    expr_ptr from_polish( std::string const & str, std::string const & separator = "|" )
    {
        Lexer lex ( str );
        Lexer::token_type token;
        
        expr_ptr root = get_root_node( lex );
        
        if( !lex.eof() ){
            for( size_t i = 0; i != root->num_children(); ++i ){
                update_current_token( lex, token );
                auto what_to_insert = convert_token_to_expression( token );
                root->set_children( i, insert_child( what_to_insert, lex, token ) );
            }
        }
        return root;
    }
    inline void to_polish( std::ostream& out , const_expr_ptr e , std::string const& separator = "|" )
    {
        assert( e );
        out << e->to_string();
        for( size_t i=0 ; i<e->num_children() ; ++i )
        {
            out << separator;
            to_polish( out , e->get_children(i) , separator );
        }
    }
    
    inline double process_unary_function( const_expr_ptr e );

    inline double evaluate_expr( double const & c )
    {
        return c;
    }
    inline double evaluate_expr( const_expr_ptr e )
    {
        switch ( e->get_type() ) {
            case Constant:
                return e->eval();
            case Variable:
                return e->eval();
            case Plus:
                return evaluate_expr( e->get_children( 0 ) ) + evaluate_expr( e->get_children( 1 ) );
            case Minus:
                return evaluate_expr( e->get_children( 0 ) ) - evaluate_expr( e->get_children( 1 ) );
            case Divides:
                return evaluate_expr( e->get_children( 0 ) ) / evaluate_expr( e->get_children( 1 ) );
            case Multiplies:
                return evaluate_expr( e->get_children( 0 ) ) * evaluate_expr( e->get_children( 1 ) );
            case UnaryFunc: default:
                return process_unary_function( e );
        }
    }
    
    inline double process_unary_function( const_expr_ptr e )
    {
        if( e->to_string() == std::string { "cos" } ){
            return std::cos( evaluate_expr( e->get_children( 0 ) ) );
        } else {
            return std::sin( evaluate_expr( e->get_children( 0 ) ) );
        }
    }
    inline std::string to_polish( const_expr_ptr e , std::string const& separator = "|" )
    {
        std::ostringstream str;
        to_polish( str , e , separator );
        return str.str();
    }
    
    namespace detail {
    
    inline void to_graphviz_impl( std::ostream& out , const_expr_ptr e , size_t& index )
    {
        assert( e );
        
        size_t current_index = index;
        out << "NODE" << current_index << " [ label = \"" << e->to_string() << "\" ]\n";
            
        for( size_t i=0 ; i<e->num_children() ; ++i )
        {
            ++index;
            out << "NODE" << current_index << " -> " << "NODE" << index << "\n";
            to_graphviz_impl( out , e->get_children(i) , index );
        }
    }
    
    } // namespace detail
    
    inline void to_graphviz( std::ostream& out , const_expr_ptr e )
    {
        out << "digraph G\n";
        out << "{\n";
        size_t index = 0;
        if( e ) detail::to_graphviz_impl( out , e , index );
        out << "}\n";
    }
    
    inline std::string to_graphviz( const_expr_ptr e )
    {
        std::ostringstream str;
        to_graphviz( str , e );
        return str.str();
    }
}
#endif // EXPRESSION_IO_H_INCLUDED

