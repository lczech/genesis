#include <string>
#include <iostream>

#include "../utils/log.hh"
#include "../utils/utils.hh"
#include "../utils/lexer.hh"

using namespace genesis;
using namespace genesis::utils;

/**
 * Prints the header information on screen when starting the program.
 */
std::string print_header()
{
    return
"                                     ,    \n\
        __    __    __    __   __     __   \n\
      /   ) /___) /   ) /___) (_ ` / (_ `  \n\
     (___/ (___  /   / (___  (__) / (__)   \n\
        /                                  \n\
    (_ /            2014 by Lucas Czech    \n";
}

int main (int argc, char* argv[])
{
    Log::AddOutputStream(std::cout);

    LOG_BOLD << print_header();

    LOG_TIME << "am start";

    utils::Lexer l;
    l.Analyze(utils::ReadFile("/home/lucas/Dropbox/HITS/genesis/data/placement.json"));
    //~ l.Analyze("((A:0.2{0},B:0.09{1}):0.7{2},C:0.5{3}){4};");
    LOG_BOLD << l.Dump();
    //~ LOG_INFO << "brackets " << l.ValidateBrackets();

    //~ for (LexerToken t : l) {
        //~ std::cout << t.value() << std::endl;
    //~ }

    //~ for (size_t i = 0; i < l.size(); i++) {
        //~ LexerToken t = l[i];
        //~ std::cout << t.value() << std::endl;
    //~ }

    //~ for (Lexer::TokenIterator t = l.begin(); t != l.end(); ++t) {
        //~ std::cout << t->value() << std::endl;
    //~ }

    LexerToken f = l.back();
    std::cout << std::endl << "last " << f.TypeToStr() << " at " << f.line()
    << ":" << f.column() << " with " << f.value() << std::endl;


    std::cout << argc << " ";
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i];
    }
    std::cout << std::endl;

    LOG_TIME << "durch";

    return 0;
}
