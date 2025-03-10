//
// This file is part of simba spectra library
// Copyright 2025 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT
// JSON encoder for simba spectra types
// https://ftp.moex.com/pub/SIMBA/Spectra/prod/doc/spectra_simba_en.pdf
//

#pragma once


#include <ufmt/fixed_string.hpp>
#include <ufmt/json.hpp>

#include <simba/types.hpp>


namespace simba::types {

  template<class S> ufmt::basic_json<S>&
  operator << (ufmt::basic_json<S>& json, types::decimal5 d5) {
    return json << ufmt::short_string::from_number(d5.mantissa);
  }

  template<class S> ufmt::basic_json<S>&
  operator << (ufmt::basic_json<S>& json, types::decimal5null d5n) {
    return json << ufmt::short_string::from_number(d5n.mantissa);
  }

  template<class S> ufmt::basic_json<S>&
  operator << (ufmt::basic_json<S>& json, types::order_update const& ou) {
    using string = ufmt::short_string;
    return json << ufmt::object(
      "TemplateID", types::order_update::template_id,
      "MDEntryID", string::from_number(ou.md_entry_id),
      "MDEntryPx", ou.md_entry_px,
      "MDEntrySize", string::from_number(ou.md_entry_size),
      "MDFlags", string::from_number(static_cast<uint64_t>(ou.md_flags)),
      "MDFlags2", string::from_number(static_cast<uint64_t>(ou.md_flags2)),
      "SecurityID", ou.security_id,
      "RptSeq", ou.rpt_seq,
      "MDUpdateAction", static_cast<char>(ou.md_update_action),
      "MDEntryType", static_cast<char>(ou.md_entry_type));
  }

  template<class S> ufmt::basic_json<S>&
  operator << (ufmt::basic_json<S>& json, types::order_execution const& oe) {
    using string = ufmt::short_string;
    return json << ufmt::object(
      "TemplateID", types::order_execution::template_id,
      "MDEntryID", string::from_number(oe.md_entry_id),
      "MDEntryPx", oe.md_entry_px,
      "MDEntrySize", string::from_number(oe.md_entry_size),
      "LastPx", oe.last_px,
      "LastQty", string::from_number(oe.last_qty),
      "TradeID", string::from_number(oe.trade_id),
      "MDFlags", string::from_number(static_cast<uint64_t>(oe.md_flags)),
      "MDFlags2", string::from_number(static_cast<uint64_t>(oe.md_flags2)),
      "SecurityID", oe.security_id,
      "RptSeq", oe.rpt_seq,
      "MDUpdateAction", static_cast<char>(oe.md_update_action),
      "MDEntryType", static_cast<char>(oe.md_entry_type));
  }

  template<class S> ufmt::basic_json<S>&
  operator << (ufmt::basic_json<S>& json, types::order_book_snapshot::entry const& e) {
    using string = ufmt::short_string;
    return json << ufmt::object(
      "MDEntryID", string::from_number(e.md_entry_id),
      "TransactTime", string::from_number(e.transact_ts_nsec),
      "MDEntryPx", e.md_entry_px,
      "MDEntrySize", string::from_number(e.md_entry_size),
      "TradeID", string::from_number(e.trade_id),
      "MDFlags", string::from_number(static_cast<uint64_t>(e.md_flags)),
      "MDFlags2", string::from_number(static_cast<uint64_t>(e.md_flags2)),
      "MDEntryType", static_cast<char>(e.md_entry_type));
  }

  template<class S> ufmt::basic_json<S>&
  operator << (ufmt::basic_json<S>& json, types::order_book_snapshot const& obs) {
    using string = ufmt::short_string;
    return json << ufmt::object(
      "TemplateID", types::order_book_snapshot::template_id,
      "SecurityID", obs.security_id,
      "LastMsgSeqNumProcessed", obs.last_msg_seq_num,
      "RptSeq", obs.rpt_seq,
      "ExchangeTradingSessionID", obs.trading_session_id,
      "NoMDEntries", obs.no_md_entries.num_in_group,
      "MDEntries", std::span{obs.entries, obs.no_md_entries.num_in_group});
  }

} // namespace simba::json
