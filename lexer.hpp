#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include "expressions.hpp"

namespace EditedExpression
{
    inline namespace Functions
    {
        struct is_alphabet
        {
            inline bool operator() ( int const & ch ) {
                return ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) || ch == '_';
            }
        };
        struct is_numeric_constant
        {
            inline bool operator ()( int const & ch ){
                return ( ch >= '0' && ch <= '9') || ( ch == '.' );
            }
        };
        struct is_alphanumeric
        {
            inline bool operator ()( int const & ch ){
                return ( is_alphabet{}( ch ) || is_numeric_constant{}( ch ) );
            }
        };
        [[noreturn]] void panic( std::string const & str, int const & column ){
            std::string space ( column - 1, ' ' );
            printf( "Invalid expression near column: %d\n", column );
            puts( str.c_str() );
            printf( "%s^\n", space.c_str() );

            std::exit( -1 );
        }
    } // Functions

    constexpr size_t EOF_F = -1;
    using Expression::expression_type;


    struct Symbol
    {
        Symbol( std::string && lex ): m_lexeme( std::move( lex ) ) { }
        Symbol( char const *lex ): m_lexeme( lex ) { }
        Symbol( std::string const & lex = "None" ): m_lexeme( lex ) { }

        std::string &get_lexeme() { return m_lexeme; }
        const std::string lexeme() const { return m_lexeme; }
    private:
        std::string m_lexeme;
    };
    
    struct Lexer
    {
    private:
        std::string str_lex; //since we're dealing with a very small string, using a std::ostream would be an overkill
        std::string::size_type current_index;
        size_t current_character, end_of_file;

        inline void update_current_token() {
            if( current_index >= end_of_file ){
                current_character = EOF_F;
                return;
            }
            current_character = str_lex[current_index];
            ++current_index;
        }

    public:
        using token_type = std::pair<Symbol, expression_type>;
        
        Lexer() = delete;
        explicit Lexer( std::string const & lex_str ): str_lex( lex_str ),
            current_index { 0 }, current_character { }, end_of_file{ str_lex.size() }
        {
            assert( end_of_file > 0 );
            update_current_token();
        }
        explicit Lexer( char const *lex_str ): str_lex( lex_str ), current_index { 0 },
            current_character { }, end_of_file{ str_lex.size() }
        {
            assert( end_of_file > 0 );
            update_current_token();
        }
        inline void reset(){
            current_index = 0;
            current_character = ' ';
        }
        inline void set_string( char const *str )
        {
            reset();
            str_lex = std::string { str };
            end_of_file = str_lex.size();
        }

        inline bool eof() {
            return current_character == EOF_F;
        }
        token_type get_negated_constant_or_unary_minus()
        {
            std::string str_buf( 1, current_character );
            update_current_token();
            bool dot_found = false;
            
            if( is_numeric_constant{} ( current_character ) ){
                while( is_numeric_constant{}( current_character ) ){
                    if( current_character == '.' && !dot_found ){
                        dot_found = true;
                    } else if ( current_character == '.' ){
                        panic( str_lex, current_index );
                    }
                    str_buf.push_back( current_character );
                    update_current_token();
                }
                return { Symbol( str_buf ), expression_type::Constant };
            } else {
                return { Symbol( str_buf ), expression_type::Minus };
            }
        }

        token_type get_constant_token()
        {
            std::string str_buf( 1, current_character );
            update_current_token();
            bool dot_found = false;

            while( is_numeric_constant{} ( current_character ) ){
                if( current_character == '.' && !dot_found ){
                    dot_found = true;
                } else if ( current_character == '.' ){
                    update_current_token();
                    continue;
                }
                str_buf.push_back( current_character );
                update_current_token();
            }
            return { Symbol { str_buf }, expression_type::Constant };
        }
        
        token_type get_variable_token()
        {
            std::string str_buf { };
            is_alphabet is_alpha {};
            if( is_alpha ( current_character ) ){
                str_buf.push_back( current_character );
                update_current_token();
                
                while( is_alphanumeric{} ( current_character ) ){
                    str_buf.push_back( current_character );
                    update_current_token();
                }
            }
            update_current_token();
            return { Symbol { str_buf }, expression_type::Variable };
        }
        //TODO
        
        token_type get_token()
        {
            for( ; ; ){
                switch( current_character ){
                    case EOF_F:
                        update_current_token();
                        return { Symbol {}, expression_type::None };
                    case '|': case ' ':
                        update_current_token();
                        break;
                    case '*':
                        update_current_token();
                        return { Symbol { "*" }, expression_type::Multiplies };
                    case '/':
                        update_current_token();
                        return { Symbol { "/" }, expression_type::Divides };
                    case '+':
                        update_current_token();
                        return { Symbol { "+" }, expression_type::Plus };
                    case '-':
                        return get_negated_constant_or_unary_minus();
                    case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
                        return get_constant_token();
                    default:
                        return get_variable_token();
                }
            }
        }
    };
} // namespace BuildExpression

#endif
