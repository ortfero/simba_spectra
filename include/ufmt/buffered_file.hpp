// This file is part of ufmt library
// Copyright 2020-2022 Andrei Ilin <ortfero@gmail.com>
// SPDX-License-Identifier: MIT
 
#pragma once

#include <optional>
#include <string>
#include <system_error>

#include <errno.h>
#include <stdio.h>

#include "json.hpp"
#include "text.hpp"


namespace ufmt {
	
	
	template<class T = text> class buffered_file {
		
		text text_;
		FILE* handle_{nullptr};
		
		explicit buffered_file(FILE* handle) noexcept: handle_{handle} { }
		
		template<std::size_t N>
		static std::optional<buffered_file> open(std::string const& path, char const (&mode)[N],
		                                         std::error_code& ec) {
			auto* opened = fopen(path.data(), mode);
			if(!opened) {
				ec = {errno, std::system_category()};
				return std::nullopt;
			}
			return buffered_file{opened};
		}

		bool write(char const* data, size_t size, std::error_code& ec) noexcept {
			auto const written =
#if defined(_WIN32)
#if __MSVCRT_VERSION__ >= 0x800
				_fwrite_nolock(data, size, 1, handle_) == 1;
#else
			fwrite(data, size, 1, handle_) == 1;
#endif
#else
			fwrite_unlocked(data, size, 1, handle_) == 1;
#endif
			if (!written) {
				ec = {errno, std::system_category()};
				return false;
			}
			return true;
		}
		
	public:
	
		~buffered_file() noexcept { close(); }
		buffered_file(buffered_file const&) = delete;
		buffered_file& operator = (buffered_file const&) = delete;
		
		
		static std::optional<buffered_file> create_always(std::string const& path, std::error_code& ec) {
			return open(path, "w+b", ec);
		}


		static std::optional<buffered_file> open_existing(std::string const& path, std::error_code& ec) {
			return open(path, "wb", ec);
		}
		
		
		static std::optional<buffered_file> open_always_to_append(std::string const& path, std::error_code& ec) {
			return open(path, "a+b", ec);
		}
		
		
		explicit operator bool () const noexcept {
			return handle_ != nullptr;
		}
		
		
		buffered_file(buffered_file&& other) noexcept: handle_{other.handle_} {
			other.handle_ = nullptr;
		}
		
		
		buffered_file& operator = (buffered_file&& other) noexcept {
			close();
			handle_ = other.handle_; other.handle_ = nullptr;
			return *this;
		}
	
		
		void close() noexcept {
			if(handle_ == nullptr)
				return;
			fclose(handle_);
			handle_ = nullptr;
		}
		
		
		bool write(std::string_view sv, std::error_code& ec) noexcept {
			return write(sv.data(), sv.size(), ec);
		}
		
		
		template<typename... Args>
		void print(Args&&... args) {
			text_.clear();
			text_.format(std::forward<Args>(args)..., '\n');
			auto ec = std::error_code{};
			write(text_.data(), text_.size(), ec);
		}

	}; // buffered_file
	
	
} // namespace ufmt
