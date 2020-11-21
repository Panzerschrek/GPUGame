#include "Host.hpp"


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

int main()
{
	return GPUGame::Main();
}
