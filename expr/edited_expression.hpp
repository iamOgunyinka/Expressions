#ifndef EDIT_EXPRESSIONS_H_INCLUDED
#define EDIT_EXPRESSIONS_H_INCLUDED

#include <memory>
#include <cassert>
#include <cmath>
#include <string>

namespace Expression
{    
    enum class expression_type
    {
        e_none,
        e_constant,
        e_variable ,
        e_binary_operator,
        e_unary_func
    };
    
    class expr
    {
    public:
        
        using expr_ptr = std::shared_ptr< expr >;
        using const_expr_ptr = std::shared_ptr< expr const >;
        
        virtual size_t size( void ) const = 0;
        virtual double get_expr_value() const = 0;
        virtual const_expr_ptr get_children( size_t i ) const = 0;
        virtual expr_ptr get_children( size_t i ) = 0;
        virtual void set_children( size_t , expr_ptr e ) = 0;
        virtual expression_type get_type( void ) const = 0;
        virtual std::string to_string( void ) const = 0;
    };
    
    using expr_ptr = expr::expr_ptr;
    using const_expr_ptr = expr::const_expr_ptr;
    
    class terminal_expr : public expr
    {
    public:
        
        size_t size( void ) const override { return 0; }
        const_expr_ptr get_children( size_t i ) const override { assert( false ); return nullptr; }
        expr_ptr get_children( size_t i ) override { assert( false ); return nullptr; }
        void set_children( size_t i , expr_ptr e ) override { assert( false ); }
    };
    
    class unary_expr : public expr
    {
    public:
        
        unary_expr( expr_ptr child )
        : m_child{ child } {}
        
        size_t size( void ) const override { return 1; }
        const_expr_ptr get_children( size_t i ) const override { assert( i == 0 ) ;return m_child; }
        expr_ptr get_children( size_t i ) override { assert( i == 0 ) ;return m_child; }
        double get_expr_value () const override { return m_child->get_expr_value(); }
        void set_children( size_t i , expr_ptr e ) override { assert( i == 0 ); m_child = e; }
    protected:
    
        expr_ptr m_child;
    };
    
    
    class binary_expr : public expr
    {
    public:
        
        binary_expr( expr_ptr left , expr_ptr right )
        : m_children{} { m_children[0] = left; m_children[1] = right; }
        
        size_t size( void ) const override { return 2; }
        const_expr_ptr get_children( size_t i ) const override { assert( i < 2 ); return m_children[i]; }
        expr_ptr get_children( size_t i ) override { assert( i < 2 ); return m_children[i]; }        
        void set_children( size_t i , expr_ptr e ) override { assert( i < 2 ); m_children[i] = e; }

    protected:
       
        expr_ptr m_children[2];
    };
    
    class constant : public terminal_expr
    {
    public:
        
        constant( double const & c ) : m_c( c ) {}
        double get_expr_value() const { return m_c; }
        expression_type get_type( void ) const override { return expression_type::e_constant; }
        std::string to_string( void ) const override
        {
            return std::to_string( m_c );
        }
        
    private:
        
        double m_c;
    };
    
    template< size_t I = 0>
    class variable : public terminal_expr
    {
        std::string variable_name;
        constant const variable_value;
    public:
        variable( std::string const & name, double const & const_value ): variable_name( name ), variable_value( const_value ) { }
        variable( double const & const_value = 0 ): variable_name{ "var" + std::to_string( I ) }, variable_value( 0.0 ) { }
        expression_type get_type( void ) const override { return expression_type::e_variable; }
        double get_expr_value() const override { return variable_value.get_expr_value(); }
        std::string to_string( void ) const override { return variable_name; }
    };
    
    class sin_node : public unary_expr
    {
    public:
        sin_node( expr_ptr ptr ) : unary_expr{ ptr } { }
        double get_expr_value () const override { return std::sin( m_child->get_expr_value() ); }
        expression_type get_type( void ) const override { return expression_type::e_unary_func; }
        std::string to_string( void ) const override { return std::string{ "sin" }; }
    };
    
    class cos_node : public unary_expr
    {
    public:
        cos_node( expr_ptr child ) : unary_expr{ child } { }
        double get_expr_value () const override { return std::cos( m_child->get_expr_value() ); }
        expression_type get_type( void ) const override { return expression_type::e_unary_func; }
        std::string to_string( void ) const override { return std::string{ "cos" }; }
    };
    
    template<char op>
    class binary_op_node: public binary_expr
    {
        char binary_op_type;
    public:
        binary_op_node( expr_ptr left, expr_ptr right ): binary_expr { left, right }, binary_op_type{ op } { }
        expression_type get_type( ) const { return expression_type::e_binary_operator; }
        std::string to_string( ) const override { return std::string( 1, binary_op_type ); }
        double get_expr_value () const override {
            switch ( binary_op_type ) {
                case '+': return m_children[0]->get_expr_value() + m_children[1]->get_expr_value();
                case '-': return m_children[0]->get_expr_value() - m_children[1]->get_expr_value();
                case '*': return m_children[0]->get_expr_value() * m_children[1]->get_expr_value();
                case '/': default: return m_children[0]->get_expr_value() / m_children[1]->get_expr_value();
            }
        }
    };
    
    expr_ptr make_constant( double c ) { return std::make_shared< constant >( c ); }
    template< size_t I = 0>
    expr_ptr make_variable( std::string const & str = "var", double const & c = 0.0 ) { return std::make_shared< variable< I > >( str, c ); }

    template< size_t I = 0>
    expr_ptr make_variable( double const & c ) { return std::make_shared< variable< I > >( c ); }

    
    expr_ptr make_variable( std::string const & str, double const & a = 0.0 ) { return std::make_shared< variable<> >( str, a ); }
    
    expr_ptr make_sin( expr_ptr child ) { return std::make_shared< sin_node >( child ); }
    expr_ptr make_cos( expr_ptr child ) { return std::make_shared< cos_node >( child ); }
    expr_ptr make_plus( expr_ptr left , expr_ptr right ) { return std::make_shared<binary_op_node<'+'> >( left , right ); }
    expr_ptr make_minus( expr_ptr left , expr_ptr right ) { return std::make_shared< binary_op_node<'-'> >( left , right ); }
    expr_ptr make_multiplies( expr_ptr left , expr_ptr right ) { return std::make_shared< binary_op_node<'*'> >( left , right ); }
    expr_ptr make_divided( expr_ptr left , expr_ptr right ) { return std::make_shared<binary_op_node<'/'> >( left , right ); }
    
} //namespace Expression

#endif // EXPRESSIONS_H_INCLUDED
