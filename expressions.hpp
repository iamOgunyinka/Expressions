#ifndef EXPRESSIONS_H_INCLUDED
#define EXPRESSIONS_H_INCLUDED

#include <memory>
#include <cassert>
#include <cmath>

namespace Expression
{
    
    enum expression_type
    {
        None,
        Constant ,
        Variable ,
        Plus ,
        Minus ,
        Divides ,
        Multiplies ,
        UnaryFunc
    };
    
    class expr
    {
    public:
        
        using expr_ptr = std::shared_ptr< expr >;
        using const_expr_ptr = std::shared_ptr< expr const >;
        
        virtual size_t num_children( void ) const = 0;
        virtual const_expr_ptr get_children( size_t i ) const = 0;
        virtual expr_ptr get_children( size_t i ) = 0;
        virtual void set_children( size_t , expr_ptr e ) = 0;
        virtual expression_type get_type( void ) const = 0;
        virtual std::string to_string( void ) const = 0;
        virtual double eval() const = 0;
    };
    
    using expr_ptr = expr::expr_ptr;
    using const_expr_ptr = expr::const_expr_ptr;
    
    class terminal_expr : public expr
    {
    public:

        size_t num_children( void ) const override { return 0; }
        const_expr_ptr get_children( size_t i ) const override { assert( false ); return nullptr; }
        expr_ptr get_children( size_t i ) override { assert( false ); return nullptr; }
        void set_children( size_t i , expr_ptr e ) override { assert( false ); }
    };
    
    class unary_expr : public expr
    {
    public:
        
        unary_expr( expr_ptr child )
        : m_child{ child } {}

        virtual double eval() const { return 0.0; }
        size_t num_children( void ) const override { return 1; }
        const_expr_ptr get_children( size_t i ) const override { assert( i == 0 ) ;return m_child; }
        expr_ptr get_children( size_t i ) override { assert( i == 0 ) ;return m_child; }
        void set_children( size_t i , expr_ptr e ) override { assert( i == 0 ); m_child = e; }
        
    protected:
    
        expr_ptr m_child;
    };
    
    class binary_expr : public expr
    {
    public:
        
        binary_expr( expr_ptr left , expr_ptr right )
        : m_children{} { m_children[0] = left; m_children[1] = right; }

        double eval() const override { return 0.0; }
        size_t num_children( void ) const override { return 2; }
        const_expr_ptr get_children( size_t i ) const override { assert( i < 2 ); return m_children[i]; }
        expr_ptr get_children( size_t i ) override { assert( i < 2 ); return m_children[i]; }        
        void set_children( size_t i , expr_ptr e ) override { assert( i < 2 ); m_children[i] = e; }
        
    protected:
       
        expr_ptr m_children[2];
    };
    
    class constant : public terminal_expr
    {
    public:
        
        constant( double c ) : m_c( c ) {}
        double eval( ) const { return m_c; }
        expression_type get_type( void ) const final { return Constant; }
        std::string to_string( void ) const final
        {
            return std::to_string( m_c );
        }
        
    private:
        
        double m_c;
    };
    
    struct context_expr_eval
    {
        double get ( int const & index) const noexcept
        {
            assert( index < 20 );
            return elements[index];
        }
        virtual ~context_expr_eval() { }

    private:
        std::array<double, 20> elements { { 11.8, 15,9, 11.9, 1, 12, 12, 24, 6, 45, 45, 6 } };
    };
    
    template< size_t I >
    class variable : public terminal_expr, context_expr_eval
    {
        int m_index;
    public:
        variable( int const & index = I ): terminal_expr {}, context_expr_eval {}, m_index { index } { }
        double eval() const override { return context_expr_eval::get( m_index ); }
        expression_type get_type( void ) const override { return Variable; }
        std::string to_string( void ) const override {
            return std::string { "var" + std::to_string( m_index ) };
        }
    };
    
    class sin_node : public unary_expr
    {
    public:
        sin_node( expr_ptr ptr ) : unary_expr{ ptr } { }
        expression_type get_type( void ) const override { return UnaryFunc; }
        std::string to_string( void ) const override { return "sin"; }
    };
    
    class cos_node : public unary_expr
    {
    public:
        cos_node( expr_ptr child ) : unary_expr{ child } { }
        expression_type get_type( void ) const override { return UnaryFunc; }
        std::string to_string( void ) const override { return "cos"; }
    };
    
    class plus_node : public binary_expr
    {
    public:
        plus_node( expr_ptr left , expr_ptr right ) : binary_expr{ left , right } { }
        expression_type get_type( void ) const override { return Plus; }
        std::string to_string( void ) const override { return "+"; }
    };
    
    class multiplies_node : public binary_expr
    {
    public:
        
        multiplies_node( expr_ptr left , expr_ptr right ) : binary_expr{ left , right } { }
        expression_type get_type( void ) const override { return Multiplies; }
        std::string to_string( void ) const override { return "*"; }
    };
    
    class minus_node : public binary_expr
    {
    public:
        
        minus_node( expr_ptr left , expr_ptr right ) : binary_expr{ left , right } { }
        expression_type get_type( void ) const override { return Minus; }
        std::string to_string( void ) const override { return "-"; }
    };
    
    class divides_node : public binary_expr
    {
    public:
        
        divides_node( expr_ptr left , expr_ptr right ) : binary_expr{ left , right } { }
        expression_type get_type( void ) const override { return Divides; }
        std::string to_string( void ) const override { return "/"; }
    };
    
    expr_ptr make_constant( double c ) { return std::make_shared< constant >( c ); }
    
    template< size_t I>
    expr_ptr make_variable( ) { return std::make_shared< variable< I > >( ); }

    template< size_t I = 0>
    expr_ptr make_variable_with_index( int const & index ) { return std::make_shared< variable< I > >( index ); }
    
    expr_ptr make_sin( expr_ptr child ) { return std::make_shared< sin_node >( child ); }
    expr_ptr make_cos( expr_ptr child ) { return std::make_shared< cos_node >( child ); }
    expr_ptr make_plus( expr_ptr left , expr_ptr right ) { return std::make_shared< plus_node >( left , right ); }
    expr_ptr make_minus( expr_ptr left , expr_ptr right ) { return std::make_shared< minus_node >( left , right ); }
    expr_ptr make_multiplies( expr_ptr left , expr_ptr right ) { return std::make_shared< multiplies_node >( left , right ); }
    expr_ptr make_divided( expr_ptr left , expr_ptr right ) { return std::make_shared< divides_node >( left , right ); }
}
#endif // EXPRESSIONS_H_INCLUDED

