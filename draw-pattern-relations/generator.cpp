#include "gen.h"
#include "fs_io.hpp"
#include "system.hpp"

int main(int argc, char** argv)
{
    return !::gen::io::fs::Save(
		gen::PatternRelationSystem(std::cin).GeneratePatternImage(),
		gen::Init(argc, argv).filepath);
}
