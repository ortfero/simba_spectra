//
// This file is part of simba spectra library
// Copyright 2025 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT
// Binary decoder for simba spectra types
// https://ftp.moex.com/pub/SIMBA/Spectra/prod/doc/spectra_simba_en.pdf
//

#pragma once


#include <cstdint>
#include <span>
#include <type_traits>

#include <ufmt/print.hpp>

#include <simba/types.hpp>


namespace simba::decoder {

  namespace detail {

    template<typename T> T
    from_bytes(uint8_t const*& marker, size_t size = sizeof(T)) noexcept {
      static_assert(std::is_standard_layout_v<T>, "Should only be used with standard layout types");
      T result;
      memcpy(&result, marker, size);
      marker += size;
      return result;
    }

    struct market_data_packet_header {
      uint32_t sequence_number;
      uint16_t size;
      uint16_t
        is_last_fragment:1,
        is_snapshot_start:1,
        is_snapshot_end:1,
        is_incremental:1,
        is_poss_dup:1,
        flags:11;
      uint64_t ts_nsec;
    }; // market_data_packet_header
    static_assert(sizeof(market_data_packet_header) == 16);

    struct incremental_packet_header {
      uint64_t transact_ts_nsec;
      uint32_t trading_session_id;
    }; // incremental_packet_header
    static_assert(sizeof(incremental_packet_header) == 16);

    struct sbe_header {
      uint16_t block_length;
      uint16_t template_id;
      uint16_t schema_id;
      uint16_t version;
    }; // sbe_header
    static_assert(sizeof(sbe_header) == 8);

    template<typename T, typename F> void
    decode(uint8_t const*& marker, F&& f) {
      auto const data = from_bytes<T>(marker, T::size);
      f(data);
    }

    template<typename F> void
    decode_order_book_snapshot(uint8_t const*& marker, F&& f) {
      using type = types::order_book_snapshot;
      auto obs = from_bytes<types::order_book_snapshot>(marker, type::size);
      auto const n = obs.no_md_entries.num_in_group > type::max_entries
        ? type::max_entries : obs.no_md_entries.num_in_group;
      for (auto i = 0uz; i != n; ++i) {
        memcpy(obs.entries + i, marker, type::entry::size);
        marker += type::entry::size;
      }
      f(obs);
    }

    template<typename F> void
    decode(sbe_header const& header, uint8_t const*& marker, F&& f) {
      switch(header.template_id) {
        case types::order_execution::template_id:
          decode<types::order_execution>(marker, std::forward<F>(f));
        return;
        case types::order_update::template_id:
          decode<types::order_update>(marker, std::forward<F>(f));
        return;
        case types::order_book_snapshot::template_id:
          decode_order_book_snapshot(marker, std::forward<F>(f));
        return;
        default:
          marker += header.block_length;
      }
    }

  } // namespace detail

  template<typename F> void
  decode(std::span<uint8_t const> payload, F&& f) {
    auto const* marker = payload.data();
    auto const market_data_header = detail::from_bytes<detail::market_data_packet_header>(marker);
    if (!market_data_header.is_incremental) {
      // decoding snapshot message
      auto const sbe_header = detail::from_bytes<detail::sbe_header>(marker);
      return detail::decode(sbe_header, marker, std::forward<F>(f));
    }
    // decoding incremental messages
    marker += sizeof(detail::incremental_packet_header);
    while (marker < payload.data() + payload.size()) {
      auto const sbe_header = detail::from_bytes<detail::sbe_header>(marker);
      detail::decode(sbe_header, marker, std::forward<F>(f));
    }
  }

} // namespace simba::decoder
