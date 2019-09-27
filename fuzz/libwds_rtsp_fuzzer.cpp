#include <assert.h>

#include "libwds/rtsp/driver.h"
#include "libwds/rtsp/message.h"
#include "libwds/rtsp/reply.h"
#include "libwds/rtsp/getparameter.h"

using namespace std;
using namespace wds::rtsp;
using wds::rtsp::Driver;

namespace
{
	struct RtspFuzzer {
		static const unsigned s_numFuzzers = 4;

		static void parseHeader(const string& buffer) {
			unique_ptr<Message> message;
			Driver::Parse(buffer, message);
		}

		static void parseReply(const string& buffer) {
			unique_ptr<Message> message;
			message.reset(new wds::rtsp::Reply());
			Driver::Parse(buffer, message);
		}

		static void parseRequest(const string& buffer) {
			unique_ptr<Message> message;
			message.reset(new wds::rtsp::GetParameter("rtsp://localhost/wfd1.0"));
			Driver::Parse(buffer, message);
		}

		static void parseError(const string& buffer) {
			unique_ptr<Message> message;
			message.reset(new wds::rtsp::Reply(303));
			Driver::Parse(buffer, message);
		}
	};
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
	if (size == 0)
		return 0;

	string buffer(data, data + size - 1);
	RtspFuzzer fuzzer;

	switch (size % fuzzer.s_numFuzzers)
	{
		case 0:
			fuzzer.parseHeader(buffer);
			break;
		case 1:
			fuzzer.parseRequest(buffer);
			break;
		case 2:
			fuzzer.parseReply(buffer);
			break;
		case 3:
			fuzzer.parseError(buffer);
			break;
		default:
			assert(false);
	}
	return 0;
}