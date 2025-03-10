#include <exception>

#include <ufmt/buffered_file.hpp>
#include <ufmt/json.hpp>
#include <ufmt/print.hpp>

#include <simba/decoder.hpp>
#include <simba/json.hpp>
#include <simba/pcap.hpp>


int main(int argc, char* argv[])
  try {
    using namespace ufmt;
    if (argc != 3)
      return print_with(0, "SIMBA spectra pcap to json converter, use `simba_spectra <pcap-file> <json-file>`");

    using namespace simba;

    // open pcap file
    auto maybe_pcap_file = pcap::file::open(argv[1]);
    if (!maybe_pcap_file)
      return error_with(-1, "Unable to open pcap file: ", maybe_pcap_file.error().message());
    auto& pcap_file = *maybe_pcap_file;

    // create json file
    auto ec = std::error_code{};
    auto maybe_json_file = buffered_file<>::create_always(argv[2], ec);
    if (!maybe_json_file)
      return error_with(-1, "Unable to create json file: ", ec.message());
    auto& json_file = *maybe_json_file;

    // initialize buffer for json formatter
    auto packets_count = 0zu;
    auto json = ufmt::json{};
    json.reserve(8192); // big enough, so we don't bother for reallocations

    // read each packet, decode it, format to json and write to file
    auto maybe_packet = pcap_file.read_packet();
    for (; maybe_packet; maybe_packet = pcap_file.read_packet()) {
      auto const& packet = *maybe_packet;
      decoder::decode(packet.data, [&json, &json_file, &ec](auto const& message) {
        json.clear();
        json << message << ln{};
        if (!json_file.write(json.view(), ec))
          error("Unable to write json file: ", ec.message());
      });
      ++packets_count;
    }
    if (maybe_packet.error())
      return error_with(-1, "Unable to read pcap file: ", maybe_packet.error().message());

    print(packets_count, " packets processed");
    return 0;
  } catch (std::exception const& e) {
    return ufmt::error_with(-1, "Exception occurred: ", std::string_view(e.what()));
  } catch (...) {
    return ufmt::error_with(-1, "Unknown exception");
  }
