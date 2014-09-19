#ifndef EDIT_EXPRESSIONS_IO_H_INCLUDED
#define EDIT_EXPRESSIONS_IO_H_INCLUDED

#include <sstream>
#include "edited_expression.hpp"

namespace EditedExpression {
    
    double evaluate_expr( const_expr_ptr ptr ){
        return ptr->get_expr_value();
    }
    
    inline void to_polish( std::ostream& out , const_expr_ptr e , std::string const& separator = "|" )
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
