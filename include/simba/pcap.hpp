//
// This file is part of simba spectra library
// Copyright 2025 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT
// PCAP file format: https://datatracker.ietf.org/doc/id/draft-gharris-opsawg-pcap-00.html
// Net protocol headers: https://pingfu.net/reference/ethernet-ip-tcp-udp-icmp-protocol-header-cheatsheets/
//

#pragma once


#include <bit>
#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <span>
#include <system_error>
#include <vector>


static_assert(std::endian::native != std::endian::big,
  "This should work only for little endian machines");

namespace simba::pcap {

  enum struct error {
    success,
    unsupported_magic_number,
    unsupported_packet_checksums
  }; //  error

  class error_category final : public std::error_category {
  public:

    char const* name() const noexcept override {
      return "pcap";
    }

    constexpr std::string message(int code) const override {
      switch (static_cast<error>(code)) {
        case error::success:
          return "Success";
        case error::unsupported_magic_number:
          return "Unsupported magic number";
        case error::unsupported_packet_checksums:
          return "Unsupported packet checksums";
      }
      return "Unknown error";
    }
  }; // error_category

  static inline error_category error_category;

  inline std::error_code make_error_code(error code) noexcept {
    return {static_cast<int>(code), error_category};
  }

  // https://github.com/the-tcpdump-group/libpcap/blob/732df312d1d30b4e4f235c628cbff78ef7f3434a/pcap/pcap.h#L299
  struct pcap_header {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t captured_size;
    uint32_t original_size;
  }; // pcap_header

  struct ethernet_header {
    uint8_t destination_mac[6];
    uint8_t source_mac[6];
    uint16_t type;
  }; // ethernet_header
  static_assert(sizeof(ethernet_header) == 14);

  // https://datatracker.ietf.org/doc/html/rfc791
  struct ip_header {
    uint8_t internet_header_length: 4, version:4;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t identifier;
    uint16_t flags;
    uint8_t time_to_live;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t source;
    uint32_t destination;
  }; // ip_header
  static_assert(sizeof(ip_header) == 20);

  struct udp_header {
    uint16_t source_port;
    uint16_t destination_port;
    uint16_t length;
    uint16_t checksum;
  }; // udp_header
  static_assert(sizeof(udp_header) == 8);

  struct packet {
    pcap_header pcap;
    ethernet_header const* ethernet;
    ip_header const* ip;
    udp_header const* udp;
    std::span<uint8_t const> data;
  }; // packet

  class file {
    using handle_ptr = std::unique_ptr<FILE, decltype(&fclose)>;

    handle_ptr handle_;
    std::vector<uint8_t> packet_data_;

    file(handle_ptr handle, size_t packet_capacity)
    : handle_(std::move(handle)) {
      packet_data_.reserve(packet_capacity);
    }


  public:

    file() = delete; // use function 'open'
    file(file const&) = delete;
    file& operator=(file const&) = delete;
    file(file&&) = default;
    file& operator=(file&&) = default;

    /**
     * Open file to read
     * @param path Path to file
     * @return file or error code
     */
    [[nodiscard]] static std::expected<file, std::error_code>
    open(std::string const& path) noexcept {
      // https://github.com/the-tcpdump-group/libpcap/blob/732df312d1d30b4e4f235c628cbff78ef7f3434a/pcap/pcap.h#L204C8-L204C25
      struct file_header {
        uint32_t magic_number;
        uint16_t version_major;
        uint16_t version_minor;
        uint32_t this_zone;
        uint32_t timestamp_accuracy;
        uint32_t snap_length;
        uint32_t network_type;
      };

      using namespace std;
      auto handle = handle_ptr{fopen(path.data(), "rb"), &fclose};
      if (!handle)
        return unexpected(error_code{errno, generic_category()});

      auto fh = file_header{};
      auto const read_count = fread(&fh, sizeof(fh), 1, handle.get());
      if (read_count != 1)
        return unexpected(make_error_code(errc::io_error));

      if (fh.magic_number != 0xA1B23C4D && fh.magic_number != 0xA1B2C3D4)
        return unexpected(make_error_code(error::unsupported_magic_number));

      if ((fh.network_type & 0xF0000000) != 0)
        return unexpected(make_error_code(error::unsupported_packet_checksums));

      return file(std::move(handle), fh.snap_length);
    }

    /**
     * Read next packet
     * @return packet or error code, error code will be empty for end of file
     */
    std::expected<packet, std::error_code>
    read_packet() noexcept {
      using namespace std;
      auto pcap = pcap_header{};
      auto const header_read_count = fread(&pcap, sizeof(pcap), 1, handle_.get());
      if (header_read_count != 1) {
        if (feof(handle_.get()))
          return unexpected(error_code{});
        else
          return unexpected(error_code{errno, generic_category()});
      }

      packet_data_.resize(pcap.captured_size);
      auto const packet_read_count = fread(packet_data_.data(), packet_data_.size(), 1, handle_.get());
      if (packet_read_count != 1)
        return unexpected(error_code{errno, generic_category()});

      auto const* marker = packet_data_.data();
      auto const* ethernet = reinterpret_cast<ethernet_header const*>(marker);
      marker += sizeof(ethernet_header);
      auto const* ip = reinterpret_cast<ip_header const*>(marker);
      marker += ip->internet_header_length * 4u; // from 32-bit words to bytes
      auto const* udp = reinterpret_cast<udp_header const*>(marker);
      marker += sizeof(udp_header);
      auto const data_size = packet_data_.size() - (marker - packet_data_.data());

      return packet{
        .pcap = pcap,
        .ethernet = ethernet,
        .ip = ip,
        .udp = udp,
        .data = std::span{marker, data_size}
      };
    }

  }; // file

} // namespace simba::pcap

template<>
struct std::is_error_code_enum<simba::pcap::error> : true_type {};