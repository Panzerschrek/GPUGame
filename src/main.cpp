#include "Host.hpp"
#include <SDL.h>

namespace GPUGame
{

namespace
{

int Main()
{

	Host host;
	while(!host.Loop()){}
	return 0;
}

} // namespace

} // namespace GPUGame

extern "C" int main( int argc, char *argv[] )
{
	return GPUGame::Main();
}
