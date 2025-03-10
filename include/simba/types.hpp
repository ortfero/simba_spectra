//
// This file is part of simba spectra library
// Copyright 2025 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT
// Simba spectra protocol types
// https://ftp.moex.com/pub/SIMBA/Spectra/prod/doc/spectra_simba_en.pdf
//

#pragma once


#include <cstdint>
#include <type_traits>


namespace simba::types {

  struct decimal5 {
    int64_t mantissa;

    static constexpr int exponent = -5;
  }; // decimal5

  struct decimal5null {
    int64_t mantissa;

    static constexpr int exponent = -5;
    static constexpr int64_t min_value = -9223372036854775808llu;
    static constexpr int64_t max_value = 9223372036854775806llu;
    static constexpr int64_t null_value = 9223372036854775807llu;
  }; // decimal5null

  enum struct md_flags_set: uint64_t {};

  enum struct md_flags2_set: uint64_t {};

  enum struct md_update_action: uint8_t {};

  enum struct md_entry_type: char {};

  struct order_update {
    static constexpr uint16_t template_id = 15;

    int64_t md_entry_id;
    decimal5 md_entry_px;
    int64_t md_entry_size;
    md_flags_set md_flags;
    md_flags2_set md_flags2;
    int32_t security_id;
    uint32_t rpt_seq;
    enum md_update_action md_update_action;
    enum md_entry_type md_entry_type;

    static constexpr size_t size = sizeof(md_entry_id)
      + sizeof(md_entry_px) + sizeof(md_entry_size) + sizeof(md_flags)
      + sizeof(md_flags2) + sizeof(security_id) + sizeof(rpt_seq)
      + sizeof(md_update_action) + sizeof(md_entry_type);
  }; // order_update
  static_assert(order_update::size == 50);

  struct order_execution {
    static constexpr uint16_t template_id = 16;

    int64_t md_entry_id;
    decimal5null md_entry_px;
    int64_t md_entry_size;
    decimal5 last_px;
    int64_t last_qty;
    int64_t trade_id;
    md_flags_set md_flags;
    md_flags2_set md_flags2;
    int32_t security_id;
    uint32_t rpt_seq;
    enum md_update_action md_update_action;
    enum md_entry_type md_entry_type;

    static constexpr size_t size = sizeof(md_entry_id)
      + sizeof(md_entry_px) + sizeof(md_entry_size) + sizeof(last_px)
      + sizeof(last_qty) + sizeof(trade_id) + sizeof(md_flags)
      + sizeof(md_flags2) + sizeof(security_id) + sizeof(rpt_seq)
      + sizeof(md_update_action) + sizeof(md_entry_type);
  }; // order_execution
  static_assert(order_execution::size == 74);

  struct group_size {
    uint16_t block_length;
    uint8_t num_in_group;

    static constexpr size_t size = sizeof(block_length)
      + sizeof(num_in_group);
  }; // group_size
  static_assert(group_size::size == 3);

  struct order_book_snapshot {
    static constexpr uint16_t template_id = 17;

    int32_t security_id;
    uint32_t last_msg_seq_num;
    uint32_t rpt_seq;
    uint32_t trading_session_id;
    group_size no_md_entries;

    static constexpr size_t size = sizeof(security_id)
        + sizeof(last_msg_seq_num) + sizeof(rpt_seq)
        + sizeof(trading_session_id) + group_size::size;

    struct entry {
      int64_t md_entry_id;
      uint64_t transact_ts_nsec;
      decimal5null md_entry_px;
      int64_t md_entry_size;
      int64_t trade_id;
      md_flags_set md_flags;
      uint64_t md_flags2;
      enum md_entry_type md_entry_type;

      static constexpr size_t size = sizeof(md_entry_id)
        + sizeof(transact_ts_nsec) + sizeof(md_entry_px)
        + sizeof(md_entry_size) + sizeof(trade_id)
        + sizeof(md_flags) + sizeof(md_flags2)
        + sizeof(md_entry_type);
    }; // entry

    static constexpr size_t max_entries = 32;

    entry entries[max_entries];
  }; // order_book_snapshot
  static_assert(order_book_snapshot::size == 19);
  static_assert(order_book_snapshot::entry::size == 57);

} // namespace simba::types
