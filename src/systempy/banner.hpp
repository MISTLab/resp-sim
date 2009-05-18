#ifndef BANNER_HPP
#define BANNER_HPP

#include <iostream>
#include <string>

std::string banner = std::string("\n") +
"      ___           ___           ___           ___ \n" +
"     /  /\\         /  /\\         /  /\\         /  /\\ \n" +
"    /  /::\\       /  /:/_       /  /:/_       /  /::\\ \n" +
"   /  /:/\\:\\     /  /:/ /\\     /  /:/ /\\     /  /:/\\:\\ \n" +
"  /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\\   /  /:/~/:/ \n" +
" /__/:/ /:/___ /__/:/ /:/ /\\ /__/:/ /:/\\:\\ /__/:/ /:/ \n" +
" \\  \\:\\/:::::/ \\  \\:\\/:/ /:/ \\  \\:\\/:/~/:/ \\  \\:\\/:/ \n" +
"  \\  \\::/~~~~   \\  \\::/ /:/   \\  \\::/ /:/   \\  \\::/ \n" +
"   \\  \\:\\        \\  \\:\\/:/     \\__\\/ /:/     \\  \\:\\ \n" +
"    \\  \\:\\        \\  \\::/        /__/:/       \\  \\:\\ \n" +
"     \\__\\/         \\__\\/         \\__\\/         \\__\\/ \n\n" +
"  v0.3.1 - Politecnico di Milano, European Space Agency \n" +
"     This tool is distributed under the GPL License \n\n";

#define PRINT_BANNER() std::cerr << banner << std::endl;

#endif
