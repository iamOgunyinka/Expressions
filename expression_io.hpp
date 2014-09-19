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

#include "expressions.hpp"
#include <sstream>

namespace Expression
{
    
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

