#ifndef EDIT_EXPRESSIONS_IO_H_INCLUDED
#define EDIT_EXPRESSIONS_IO_H_INCLUDED

#include <sstream>
#include "lexer.hpp"

namespace Expression {

    double evaluate_expr( const_expr_ptr ptr ){
        return ptr->get_expr_value();
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
            token.second = expression_type::e_none;
        }
    }
    expr_ptr build_unary_operator_or_variable_from( Lexer &lex, Lexer::token_type const & token )
    {
        if( token.first.lexeme() == std::string { "cos" } ){
            return make_cos( nullptr );
        } else if( token.first.lexeme() == std::string { "sin" } ){
            return make_sin( nullptr );
        } else {
            return make_variable( token.first.lexeme() );
        }
    }
    expr_ptr build_binary_operator_from( Lexer::token_type const & token )
    {
        std::string token_kind = token.first.lexeme();
        
        if( token_kind == std::string { "/" } ){
            return make_divided( nullptr, nullptr );
        } else if ( token_kind == std::string{ "*" } ){
            return make_multiplies( nullptr, nullptr );
        } else if ( token_kind == std::string { "+" } ){
            return make_plus( nullptr, nullptr );
        } else {
            return make_minus( nullptr, nullptr );
        }
    }
    
    expr_ptr convert_token_to_expression( Lexer &lex, Lexer::token_type const & token )
    {
        switch( token.second ){
            case expression_type::e_constant:           return make_constant( to_double( token.first.lexeme() ) );
            case expression_type::e_unary_func:
            case expression_type::e_variable:           return build_unary_operator_or_variable_from( lex, token );
            case expression_type::e_binary_operator:    return build_binary_operator_from( token );
            case expression_type::e_none: default:      return nullptr;
        }
    }
    inline expr_ptr get_root_node( Lexer & lex )
    {
        return convert_token_to_expression( lex, lex.get_token() );
    }
    expr_ptr insert_child( expr_ptr node_to_insert, Lexer & lex, Lexer::token_type & token )
    {
        auto curr_ptr = node_to_insert;
        if( curr_ptr ){
            for( size_t i = 0; i != curr_ptr->size(); ++i ){
                update_current_token( lex, token );
                node_to_insert = convert_token_to_expression( lex, token );
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
            for( size_t i = 0; i != root->size(); ++i ){
                update_current_token( lex, token );
                auto what_to_insert = convert_token_to_expression( lex, token );
                root->set_children( i, insert_child( what_to_insert, lex, token ) );
            }
        }
        return root;
    }
    inline void to_polish( std::ostream& out , const_expr_ptr e , std::string const& separator )
    {
        assert( e );
        out << e->to_string();
        for( size_t i = 0 ; i < e->size() ; ++i )
        {
            out << separator;
            to_polish( out , e->get_children( i ) , separator );
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
                
            for( size_t i=0 ; i < e->size() ; ++i )
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
} //namespace Expression
#endif
