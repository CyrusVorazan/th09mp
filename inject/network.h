#pragma once

#include <string>
#include <map>
#include <vector>

namespace th09mp {

	namespace network
	{
		void ProcessPackets();
		void SendInputData(std::map<unsigned int, unsigned short> inputData);
		void SendRngSeed(unsigned int seed);

		void Connect(std::string ip, int port);
		void Host(int port);

		int GetPing();
	}
}