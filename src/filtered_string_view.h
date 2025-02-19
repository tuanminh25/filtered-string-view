#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <compare>
#include <functional>
#include <iterator>
#include <optional>
#include <string>
#include <iostream>
#include <cstring>
namespace fsv {
	using filter = std::function<bool(const char &)>;
	class filtered_string_view {
		class iter {
			friend filtered_string_view;

		 public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using reference = const char &;
			using pointer = void;
			using difference_type = std::ptrdiff_t;

			iter() = default;

			auto operator*() const -> reference {
				return (*fsv_ptr).at(idx);
			}; 

			auto operator->() const -> pointer {
				
			};

			auto operator++() -> iter& {
				++idx;
            	return *this;
			};

			auto operator++(int) -> iter {
				auto newIter = iter(fsv_ptr, idx + 1);
				return newIter;
			};

			auto operator--() -> iter& {
				--idx;
            	return *this;
			};

			auto operator--(int) -> iter {
				auto newIter = iter(fsv_ptr, idx + 1);
				return newIter;
			};

			friend auto operator==(const iter& lhs, const iter& rhs) -> bool {
				return lhs.idx == rhs.idx and lhs.fsv_ptr == rhs.fsv_ptr;
			};

			friend auto operator!=(const iter& lhs , const iter& rhs) -> bool {
				return lhs.idx != rhs.idx or lhs.fsv_ptr != rhs.fsv_ptr;
			};

		 private:
			/* Implementation-specific private members */
			iter(const filtered_string_view *ptr, const int idx) : idx(idx), fsv_ptr(ptr) {}; 
			int idx;
			const filtered_string_view *fsv_ptr;
		};

	public:
		using const_iterator = iter;
		using iterator = const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		auto begin()-> iterator {
			return { this, 0 };
		}

		auto end()-> iterator {
			return { this, static_cast<int>(size()) };
		}

		auto begin() const -> const_iterator {
			return { this, 0 };
		}

		auto end() const -> const_iterator {
			return { this, static_cast<int>(size()) };
		}

		auto cbegin() const -> const_iterator {
			return begin();
		}

		auto cend() const -> const_iterator {
			return end();
		}

		// reverse iterators
		auto rbegin() -> reverse_iterator {
			return reverse_iterator{ end() };
		}
		
		auto rend() -> reverse_iterator {
			return reverse_iterator{ begin() };
		}
		
		auto rbegin() const -> const_reverse_iterator {
			return const_reverse_iterator{ end() };
		}
		
		auto rend() const -> const_reverse_iterator {
			return const_reverse_iterator{ begin() };
		}
		
		auto crbegin() const -> const_reverse_iterator {
			return rbegin();
		}
		
		auto crend() const -> const_reverse_iterator {
			return rend();
		}

		/**
		 * Should this be public or not? 
		 * There are systems which make this public
		 * There are systems that don't
		 * We do it here because we need to test it
		*/ 
		static filter default_predicate;

		/**
		 * the pointer set to nullptr, 
		 * the length set to 0, 
		 * and the predicate set to the true predicate.
		 */
		filtered_string_view()
			: pointer(nullptr),
			length(0) 
		{

		};

		/**
		 * the pointer set to the string's underlying data,
		 * and the predicate set to the true predicate.
		 *
		 * Use c_str() for C compability, avoid writting on string, have null termination
		 * Use data() will allow write over string + not guranteed "\o" -> not using it.
		 */
		filtered_string_view(const std::string &str)
			: pointer(str.c_str()), 
			  length(str.size()) 
		{

		};
		
		/**
		 * Same as the Implicit String Constructor, 
		 * but with the predicate set to the given one.
		 */
		filtered_string_view(const std::string &str, filter predicate)
			: pointer(str.c_str()), 
			  length(str.size()),
			  str_predicate(predicate)
		{

		};

		/**
		 * the pointer set to the given null-terminated string,
		 * and the predicate set to the true predicate.
		 * 
		 * This constructor should be implicit 
		 * because it's actually desirable to enable implicit conversions from 
		 * const char * to filtered_string_view
		 * 
		 * You can assume that the passed in string pointer is validly null-terminated
		 */
		filtered_string_view(const char *str) 			
			: pointer(str), 
			  length(std::strlen(str))
		{

		};

		/**
		 * Same as the implicit null-terminated string pointer constructor, 
		 * but with the predicate set to the given one.
		 */
		filtered_string_view(const char *str, filter predicate) 			
			: pointer(str), 
			  length(std::strlen(str)),
			  str_predicate(predicate)
		{

		};

		/**
		 * The newly constructed object must compare equal to the copied object.
		 * A member-wise copy is sufficient.
		 * The underlying character buffer should not be deep-copied.
		 */
		filtered_string_view(const filtered_string_view &other)
			: pointer(other.data()), 
			  length(other.size()),
			  str_predicate(other.predicate()) 
		{

		};

		/**
		 * The newly constructed object must be identical to 
		 * the moved-from object before it was moved.
		 * 
		 * The moved from object should be in the same state 
		 * as a default constructed fsv::filtered_string_view.
		 */
		filtered_string_view(filtered_string_view &&other)
			: pointer(std::move(other.pointer)), 
			  length(std::move(other.length)),
			  str_predicate(std::move(other.str_predicate)) 
		{
			other.pointer = nullptr;
			other.length = 0;
			other.str_predicate = default_predicate;
		};

		/**
		 * Important: You must explicitly declare the destructor as default.
		 */
		~filtered_string_view() = default;

		/**
		 * Copies at least the length, data and predicate of other 
		 * so that after the copy-assignment this filtered_string_view and 
		 * other compare equal via operator==
		 * 
		 * In the case of self-copy, the object should remain unchanged.
		 */
		filtered_string_view operator=(const filtered_string_view &other);

		/**
		 * The moved from object should be left in a valid state 
		 * equivalent to a default-constructed filtered_string_view, 
		 * 
		 * except in the case of self-assignment, 
		 * in which the moved from object should remain unchanged.
		 */
		filtered_string_view operator=(filtered_string_view &&other) noexcept;

		/**
		 * Allows reading a character from the filtered string given its index.
		 * 
		 * Hint: Remember that the underlying string data should be read-only, 
		 * and it should not be possible for clients of filtered_string_view to mutate 
		 * it through the class.
		 */
		auto operator[](int n) const -> const char &;


		friend auto operator==(const fsv::filtered_string_view &lhs, 
								const fsv::filtered_string_view &rhs) -> bool {
			return (lhs <=> rhs) == std::strong_ordering::equal;
		};


		friend auto operator<=>(const filtered_string_view &lhs,
								const filtered_string_view &rhs) -> std::strong_ordering {
			auto lhs_string = static_cast<std::string>(lhs);
			auto rhs_string = static_cast<std::string>(rhs);
			return lhs_string <=> rhs_string;
		};

		friend auto operator<<(std::ostream &os, const filtered_string_view &fsv) -> std::ostream& {
			auto fsv_string = static_cast<std::string>(fsv);
			return os << fsv_string;
		}

		// Member functions
		/**
		 * Returns the size of the filtered string
		 */
		auto size() const -> std::size_t { 
			if (pointer == nullptr) {
				return 0;
			}
			size_t sv_len = 0;
			const char *temp_pointer = pointer;
			while (*temp_pointer != '\0') {
				if (str_predicate(*temp_pointer)) {
					sv_len++;
				}
				temp_pointer++;
			}
			return sv_len; 
		}

		auto at(int index) const -> const char & {
			if (0 <= index && static_cast<size_t>(index) < size()) {
				auto temp_ptr = pointer;
				auto idx = 0;
				while (*temp_ptr != '\0') {
					if (str_predicate(*temp_ptr)) {
						if (idx == index) {
							break;
						}
						++idx;
					}
					++temp_ptr;
				}
				return *temp_ptr;
			} else {
				throw std::domain_error{"filtered_string_view::at(" + std::to_string(index) + "): invalid index"};
			}
		}
		
		auto empty() const -> bool {
			return size() == 0;
		}
		
		/**
		 * Enables type casting a filtered_string_view to a std::string. 
		 * 
		 * @return std::string should be a copy of the filtered string, 
		 * so any characters which are filtered out should not be present. 
		 * 
		 * The returned std::string must be a copy 
		 * so that modifications to it 
		 * do not mutate the underyling data backing the filtered_string_view.
		 */
		explicit operator std::string() const;

		filter predicate() const { return str_predicate; };
		const char* data() const { return pointer; }
		
	private:
		const char* pointer; // raw pointer to the first char of the string
		std::size_t length;
		filter str_predicate = default_predicate;
	};

	auto compose(const filtered_string_view &fsv, const std::vector<filter> &filts) -> filtered_string_view;
	auto split(const filtered_string_view &fsv, const filtered_string_view &tok) -> std::vector<filtered_string_view>;
	auto substr(const filtered_string_view &fsv, int pos = 0, int count = 0) -> filtered_string_view;

} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
