#include "./filtered_string_view.h"
#include <catch2/catch.hpp>

// Helper function to test properties of filtered_string_view
void test_filtered_string_view_properties(fsv::filtered_string_view& string_view, const char* expected_pointer, size_t expected_size) {
    REQUIRE(string_view.predicate());
    fsv::filter string_view_predicate = string_view.predicate();

    // Testing it has the default predicate 
    for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
        bool predicate_return = string_view_predicate(c);
        REQUIRE(predicate_return == true);
    }
    REQUIRE(string_view.data() == expected_pointer);
    REQUIRE(string_view.size() == expected_size);
}

void default_predicate_test(const fsv::filter& string_view_predicate) {
	for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
		REQUIRE(string_view_predicate(c));
	}
}

TEST_CASE("Default predicate") {
	SECTION("Always True") {
		for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
			REQUIRE(fsv::filtered_string_view::default_predicate(c));
		}
	}
}

TEST_CASE("Constructors") {
	SECTION("Default constructor") {
		auto string_view = fsv::filtered_string_view();
		test_filtered_string_view_properties(string_view, nullptr, 0);
	}

	SECTION("Implicit String constructor") {
		std::string hello_string = "Hello world can you see me?";
		const std::string &hello_ref = hello_string;
		const char* hello_pointer = hello_string.c_str();

		auto string_view = fsv::filtered_string_view(hello_ref);
		test_filtered_string_view_properties(string_view, hello_pointer, hello_string.size());
	}

	SECTION("String Constructor with Predicate: Default") {
		std::string hello_string = "Hello world can you see me?";
		const std::string &hello_ref = hello_string;
		const char* hello_pointer = hello_string.c_str();
		fsv::filter hello_predicate = [](const char &) {
			return true;
		};

		auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);
		test_filtered_string_view_properties(string_view, hello_pointer, hello_string.size());
	}

	SECTION("String Constructor with Predicate: Custom") {
		SECTION("Hide some") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;
			const char* hello_pointer = hello_string.c_str();
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'e'; 
			};

			auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);

			// Testing string view property
			// Test whether the predicate is the same by REQUIREing the outcome of that predicate
			fsv::filter string_view_predicate = string_view.predicate();
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				bool predicate_return = string_view_predicate(c);
				if (c == 'e') {
					REQUIRE(predicate_return == true);
				} else {
					REQUIRE(predicate_return == false);
				}
			}
			REQUIRE(string_view.data() == hello_pointer);
			REQUIRE(string_view.size() == 4);

		}

		SECTION("Hide all") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;
			const char* hello_pointer = hello_string.c_str();
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'z'; 
			};

			auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);

			// Testing string view property
			// Test whether the predicate is the same by REQUIREing the outcome of that predicate
			fsv::filter string_view_predicate = string_view.predicate();
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				bool predicate_return = string_view_predicate(c);
				if (c == 'z') {
					REQUIRE(predicate_return == true);
				} else {
					REQUIRE(predicate_return == false);
				}
			}
			REQUIRE(string_view.data() == hello_pointer);
			REQUIRE(string_view.size() == 0);

		}

		SECTION("String Constructor with Predicate: Custom - hide notthing") {
			std::string hello_string = "eeeeeeeeeeeeeeeeeeeeeee";
			const std::string &hello_ref = hello_string;
			const char* hello_pointer = hello_string.c_str();
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'e'; 
			};

			auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);

			// Testing string view property
			// Test whether the predicate is the same by REQUIREing the outcome of that predicate
			fsv::filter string_view_predicate = string_view.predicate();
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				bool predicate_return = string_view_predicate(c);
				if (c == 'e') {
					REQUIRE(predicate_return == true);
				} else {
					REQUIRE(predicate_return == false);
				}
			}
			REQUIRE(string_view.data() == hello_pointer);
			REQUIRE(string_view.size() == hello_string.size());

		}
	
	}

	SECTION("Implicit Null-Terminated String Constructor:") {
		SECTION("Implicit Null-Terminated String Constructor: Long string") {
			char hello_string[] = {'H', 'e', 'l', 'l', 'o', '\0'};
			auto string_view = fsv::filtered_string_view(hello_string);
			test_filtered_string_view_properties(string_view, hello_string, strlen(hello_string));
		}

		SECTION("Implicit Null-Terminated String Constructor: only 1 \0") {
			char hello_string[] = {'\0'};
			auto string_view = fsv::filtered_string_view(hello_string);
			test_filtered_string_view_properties(string_view, hello_string, strlen(hello_string));
			REQUIRE(string_view.size() == 0);

		}

		SECTION("Implicit Null-Terminated String Constructor: all \0") {
			char hello_string[] = {'\0', '\0','\0','\0'};
			auto string_view = fsv::filtered_string_view(hello_string);
			test_filtered_string_view_properties(string_view, hello_string, strlen(hello_string));
			REQUIRE(string_view.size() == 0);
		}

		SECTION("Implicit Null-Terminated String Constructor: only 1 \0 in middle") {
			char hello_string[] = {'H', 'e', '\0', 'l', 'l', 'o', '\0'};
			auto string_view = fsv::filtered_string_view(hello_string);
			test_filtered_string_view_properties(string_view, hello_string, strlen(hello_string));
			REQUIRE(string_view.size() == 2);

		}

	}
	
	SECTION("Null-Terminated String with Predicate Constructor:") {
		SECTION("Default predicate + string end with \0") {
			char hello_string[] = {'H', 'e', 'l', 'l', 'o', '\0'};
			fsv::filter hello_predicate = [](const char &) {
				return true; 
			};

			auto string_view = fsv::filtered_string_view(hello_string, hello_predicate);
			test_filtered_string_view_properties(string_view, hello_string, strlen(hello_string));
			test_filtered_string_view_properties(string_view, hello_string, 5);

		}

		SECTION("Custom predicate + string end with \0") {
			char hello_string[] = {'H', 'e', 'l', 'l', 'o', '\0'};
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'e'; 
			};

			auto string_view = fsv::filtered_string_view(hello_string, hello_predicate);

			// Testing string view property
			// Test whether the predicate is the same by REQUIREing the outcome of that predicate
			fsv::filter string_view_predicate = string_view.predicate();
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				bool predicate_return = string_view_predicate(c);
				if (c == 'e') {
					REQUIRE(predicate_return == true);
				} else {
					REQUIRE(predicate_return == false);
				}
			}
			
			REQUIRE(string_view.size() == 1);
			REQUIRE(string_view.data() == hello_string);
		}

		SECTION("Default predicate + string only 1 \0") {
			char hello_string[] = {'\0'};
			fsv::filter hello_predicate = [](const char &) {
				return true; 
			};

			auto string_view = fsv::filtered_string_view(hello_string, hello_predicate);
			test_filtered_string_view_properties(string_view, hello_string, strlen(hello_string));
			test_filtered_string_view_properties(string_view, hello_string, 0);

		}

		SECTION("Custom predicate + string only 1 \0") {
			char hello_string[] = {'\0'};
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'e'; 
			};

			auto string_view = fsv::filtered_string_view(hello_string, hello_predicate);

			// Testing string view property
			// Test whether the predicate is the same by REQUIREing the outcome of that predicate
			fsv::filter string_view_predicate = string_view.predicate();
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				bool predicate_return = string_view_predicate(c);
				if (c == 'e') {
					REQUIRE(predicate_return == true);
				} else {
					REQUIRE(predicate_return == false);
				}
			}
			
			REQUIRE(string_view.size() == 0);
			REQUIRE(string_view.size() == strlen(hello_string));
			REQUIRE(string_view.data() == hello_string);

		}

		// TODO: modify this test case into 2 more tests case: default and custom predicate
		SECTION("Implicit Null-Terminated String Constructor: all \0") {
			char hello_string[] = {'\0', '\0','\0','\0'};
			auto string_view = fsv::filtered_string_view(hello_string);
			test_filtered_string_view_properties(string_view, hello_string, strlen(hello_string));
			REQUIRE(string_view.size() == 0);
		}
		
		// TODO: modify this test case into 2 more tests case: default and custom predicate
		SECTION("Implicit Null-Terminated String Constructor: only 1 \0 in middle") {
			char hello_string[] = {'H', 'e', '\0', 'l', 'l', 'o', '\0'};
			auto string_view = fsv::filtered_string_view(hello_string);
			test_filtered_string_view_properties(string_view, hello_string, strlen(hello_string));
			REQUIRE(string_view.size() == 2);

		}

	}

		
	SECTION("Copy constructor") {
		SECTION ("Initialised by default constructor") {
			auto string_view = fsv::filtered_string_view();
			auto string_view2 = fsv::filtered_string_view(string_view);
			REQUIRE(string_view.size() == string_view2.size());
			REQUIRE(string_view.data() == string_view2.data());
			REQUIRE(string_view.size() == 0);
			REQUIRE(string_view2.size() == 0);
			REQUIRE(string_view.data() == nullptr);
			REQUIRE(string_view2.data() == nullptr);
			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				REQUIRE(predicate_string_view(c));
				REQUIRE(predicate_string_view2(c));
			}
		}

		SECTION ("Initialised by Implicit String Constructor with default predicate") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;

			auto string_view = fsv::filtered_string_view(hello_ref);
			auto string_view2 = fsv::filtered_string_view(string_view);
			REQUIRE(string_view.size() == string_view2.size());
			REQUIRE(string_view.data() == string_view2.data());
			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				REQUIRE(predicate_string_view(c));
				REQUIRE(predicate_string_view2(c));
			}
		}
		
		SECTION ("Initialised by Implicit String Constructor with custom predicate") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'z'; 
			};

			auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);
			auto string_view2 = fsv::filtered_string_view(string_view);
			REQUIRE(string_view.size() == string_view2.size());
			REQUIRE(string_view.size() == 0);
			REQUIRE(string_view2.size() == 0);

			REQUIRE(string_view.data() == string_view2.data());
			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				if (c == 'z') {
					REQUIRE(predicate_string_view(c));
					REQUIRE(predicate_string_view2(c));
				} else {
					REQUIRE(!predicate_string_view(c));
					REQUIRE(!predicate_string_view2(c));
				}
			}
		}
	}

	SECTION("Move constructor") {
		SECTION ("Initialised by default constructor") {
			auto string_view = fsv::filtered_string_view();
			auto string_view2 = std::move(string_view);
			REQUIRE(string_view.size() == string_view2.size());
			REQUIRE(string_view.data() == string_view2.data());
			REQUIRE(string_view.size() == 0);
			REQUIRE(string_view2.size() == 0);
			REQUIRE(string_view.data() == nullptr);
			REQUIRE(string_view2.data() == nullptr);
			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				REQUIRE(predicate_string_view(c));
				REQUIRE(predicate_string_view2(c));
			}
		}

		SECTION ("Initialised by Implicit String Constructor with default predicate") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;

			auto string_view = fsv::filtered_string_view(hello_ref);
			auto string_view2 = std::move(string_view);
			REQUIRE(string_view.size() != string_view2.size());
			REQUIRE(string_view.data() != string_view2.data());
			REQUIRE(string_view.size() == 0);
			REQUIRE(string_view.data() == nullptr);
			REQUIRE(string_view2.size() == hello_string.size());
			REQUIRE(string_view2.data() == hello_string);

			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				REQUIRE(predicate_string_view(c));
				REQUIRE(predicate_string_view2(c));
			}
		}
		
		SECTION ("Initialised by Implicit String Constructor with custom predicate") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'e'; 
			};

			auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);
			auto string_view2 = std::move(string_view);
			REQUIRE(string_view.size() == 0);
			REQUIRE(string_view2.size() == 4);
			REQUIRE(string_view.data() == nullptr);
			REQUIRE(string_view2.data() == hello_string);

			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				if (c == 'e') {
					REQUIRE(predicate_string_view(c));
					REQUIRE(predicate_string_view2(c));
				} else {
					REQUIRE(predicate_string_view(c));
					REQUIRE(!predicate_string_view2(c));
				}
			}
		}
	}

}

TEST_CASE("Member Operator") {
	SECTION("= operator copy") {
		SECTION ("Initialised by default constructor") {
			auto string_view = fsv::filtered_string_view();
			auto string_view2 = fsv::filtered_string_view();
			string_view2 = string_view;
			REQUIRE(string_view.size() == string_view2.size());
			REQUIRE(string_view.data() == string_view2.data());
			REQUIRE(string_view.size() == 0);
			REQUIRE(string_view2.size() == 0);
			REQUIRE(string_view.data() == nullptr);
			REQUIRE(string_view2.data() == nullptr);
			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				REQUIRE(predicate_string_view(c));
				REQUIRE(predicate_string_view2(c));
			}

		}

		SECTION ("Initialised by Implicit String Constructor with default predicate") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;

			auto string_view = fsv::filtered_string_view(hello_ref);
			auto string_view2 = fsv::filtered_string_view();
			string_view2 = string_view;
			REQUIRE(string_view.size() == string_view2.size());
			REQUIRE(string_view.data() == string_view2.data());
			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				REQUIRE(predicate_string_view(c));
				REQUIRE(predicate_string_view2(c));
			}
		}
			
		SECTION ("Initialised by Implicit String Constructor with custom predicate") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'z'; 
			};

			auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);
			auto string_view2 = fsv::filtered_string_view();
			string_view2 = string_view;
			REQUIRE(string_view.size() == string_view2.size());
			REQUIRE(string_view.size() == 0);
			REQUIRE(string_view2.size() == 0);

			REQUIRE(string_view.data() == string_view2.data());
			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				if (c == 'z') {
					REQUIRE(predicate_string_view(c));
					REQUIRE(predicate_string_view2(c));
				} else {
					REQUIRE(!predicate_string_view(c));
					REQUIRE(!predicate_string_view2(c));
				}
			}
		}

		SECTION ("Self-copy") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'e'; 
			};

			
			// Before copy behaviour
			auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);
			REQUIRE(string_view.size() == 4);
			REQUIRE(string_view.data() == hello_string);
			auto predicate_string_view = string_view.predicate();
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				if (c == 'e') {
					REQUIRE(predicate_string_view(c));
				} else {
					REQUIRE(!predicate_string_view(c));
				}
			}

			// After self copy
			string_view = string_view;
			REQUIRE(string_view.size() == 4);
			REQUIRE(string_view.data() == hello_string);
			predicate_string_view = string_view.predicate();
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				if (c == 'e') {
					REQUIRE(predicate_string_view(c));
				} else {
					REQUIRE(!predicate_string_view(c));
				}
			}
		}

	}

	SECTION("= operator move") {
		SECTION ("Initialised by default constructor") {
			auto string_view = fsv::filtered_string_view();
			auto string_view2 = fsv::filtered_string_view();
			string_view2 = std::move(string_view);
			REQUIRE(string_view.size() == string_view2.size());
			REQUIRE(string_view.data() == string_view2.data());
			REQUIRE(string_view.size() == 0);
			REQUIRE(string_view2.size() == 0);
			REQUIRE(string_view.data() == nullptr);
			REQUIRE(string_view2.data() == nullptr);
			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				REQUIRE(predicate_string_view(c));
				REQUIRE(predicate_string_view2(c));
			}

		}

		SECTION ("Initialised by Implicit String Constructor with default predicate") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;

			auto string_view = fsv::filtered_string_view(hello_ref);
			auto string_view2 = fsv::filtered_string_view();
			string_view2 = std::move(string_view);

			REQUIRE(string_view.size() != string_view2.size());
			REQUIRE(string_view.data() != string_view2.data());
			REQUIRE(string_view.size() == 0);
			REQUIRE(string_view2.size() == hello_string.size());
			REQUIRE(string_view.data() == nullptr);
			REQUIRE(string_view2.data() == hello_string);


			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				REQUIRE(predicate_string_view(c));
				REQUIRE(predicate_string_view2(c));
			}
		}
			
		SECTION ("Initialised by Implicit String Constructor with custom predicate") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'e'; 
			};

			auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);
			auto string_view2 = fsv::filtered_string_view();
			string_view2 = std::move(string_view);

			REQUIRE(string_view.size() != string_view2.size());
			REQUIRE(string_view.size() == 0);
			REQUIRE(string_view2.size() == 4);

			REQUIRE(string_view.data() != string_view2.data());
			REQUIRE(string_view.data() == nullptr);
			REQUIRE(string_view2.data() == hello_string);


			auto predicate_string_view = string_view.predicate();
			auto predicate_string_view2 = string_view2.predicate();

			// Compare behaviour of those 2 predicate should be the same
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				if (c == 'e') {
					REQUIRE(predicate_string_view(c));
					REQUIRE(predicate_string_view2(c));
				} else {
					REQUIRE(predicate_string_view(c));
					REQUIRE(!predicate_string_view2(c));
				}
			}
		}

	}

	SECTION("[] operator") {
		SECTION("n is random in the middle of the string") {
			auto pred = [](const char &c) { return c == '9' || c == '0' || c == ' '; };
			auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};
			REQUIRE (fsv1[2] == '0');
		}

		SECTION("n is the first char") {
			auto pred = [](const char &c) { return c == '9' || c == '0' || c == 'o'; };
			auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};
			REQUIRE (fsv1[0] == 'o');
		}

		SECTION("n is the last char") {
			auto pred = [](const char &c) { return c == '9' || c == '0' || c == 'o'; };
			auto fsv1 = fsv::filtered_string_view{"o09o", pred};
			REQUIRE (fsv1[3] == 'o');
		}

	}

	SECTION("operator std::string") {
		SECTION("Empty string with no predicate") {
			auto sv = fsv::filtered_string_view("");
			auto s = static_cast<std::string>(sv);

			// This only compares the content
			REQUIRE(sv.data() == s);

			// This compares pointer to buffer of 2 string so it should not be the same
			REQUIRE(sv.data() != s.data());

			// REQUIRE if it is copied and can be modified after cast
			auto new_string1 = "";
			REQUIRE(s == new_string1);

			// REQUIRE if it is copied and can be modified after cast
			s+=" hala Madrid";
			auto new_string = " hala Madrid";
			REQUIRE(s == new_string);

		}

		SECTION("Non empty string with no predicate") {
			auto sv = fsv::filtered_string_view("vizsla");
			auto s = static_cast<std::string>(sv);

			// This only compares the content
			REQUIRE(sv.data() == s);

			// This compares pointer to buffer of 2 string so it should not be the same
			REQUIRE(sv.data() != s.data());

			// REQUIRE if it is copied and can be modified after cast
			s+=" hala Madrid";
			auto new_string = "vizsla hala Madrid";
			REQUIRE(s == new_string);

		}

		SECTION("Non empty string with custom predicate") {
			auto predicate = [](const auto &c) {
				return c == 'a';
			};

			auto sv = fsv::filtered_string_view("vizsla", predicate);
			auto s = static_cast<std::string>(sv);


			// This only compares the content
			REQUIRE(sv.data() != s);

			// This compares pointer to buffer of 2 string so it should not be the same
			REQUIRE(sv.data() != s.data());

			// REQUIRE if it is copied and can be modified after cast
			s+=" hala Madrid";
			auto new_string = "a hala Madrid";
			REQUIRE(s == new_string);

		}

		SECTION("Const correct: can work with const input") {
			const fsv::filtered_string_view sv = fsv::filtered_string_view("vizsla");
			auto s = static_cast<std::string>(sv);

			// This only compares the content
			REQUIRE(sv.data() == s);

			// This compares pointer to buffer of 2 string so it should not be the same
			REQUIRE(sv.data() != s.data());

			// REQUIRE if it is copied and can be modified after cast
			s+=" hala Madrid";
			auto new_string = "vizsla hala Madrid";
			REQUIRE(s == new_string);

		}

		SECTION("Const correct: can work with referebce of const input") {
			const fsv::filtered_string_view sv_ori = fsv::filtered_string_view("vizsla");
			const fsv::filtered_string_view &sv = sv_ori;
			auto s = static_cast<std::string>(sv);

			// This only compares the content
			REQUIRE(sv.data() == s);

			// This compares pointer to buffer of 2 string so it should not be the same
			REQUIRE(sv.data() != s.data());

			// REQUIRE if it is copied and can be modified after cast
			s+=" hala Madrid";
			auto new_string = "vizsla hala Madrid";
			REQUIRE(s == new_string);

		}
	}
}

TEST_CASE("Non member operator") {
	SECTION("== operator") {
		SECTION ("Normal case") {
			auto const lo = fsv::filtered_string_view{"aaa"};
			auto const lo1 = fsv::filtered_string_view{"aaa"};
			auto const hi = fsv::filtered_string_view{"zzz"};
			REQUIRE(lo != hi);
			REQUIRE(lo == lo1);

		}

		SECTION ("Empty case") {
			auto const lo = fsv::filtered_string_view{};
			auto const lo1 = fsv::filtered_string_view{""};
			auto const lo2 = fsv::filtered_string_view{"\0"};
			auto const hi = fsv::filtered_string_view{"zzz"};
			REQUIRE(lo == lo1);
			REQUIRE(lo == lo2);
			REQUIRE(lo1 == lo2);
			REQUIRE(lo != hi);
			REQUIRE(lo1 != hi);
			REQUIRE(lo2 != hi);

		}

		SECTION ("Filtered case") {
			auto const lo = fsv::filtered_string_view{};
			auto predicate = [](auto const &c) {
				return c == 'z';
			};
			auto const lo1 = fsv::filtered_string_view{"aaa", predicate};
			
			auto const hi = fsv::filtered_string_view{"zzz"};
			auto const hi1 = fsv::filtered_string_view{"zzz"};
			REQUIRE(lo != hi);
			REQUIRE(lo1 != hi);
			REQUIRE(lo1 != hi1);
			REQUIRE(lo != hi1);

			REQUIRE(lo == lo1);
			REQUIRE(hi1 == hi);

		}

		SECTION ("Same char, different order") {
			auto const lo = fsv::filtered_string_view{"bac"};
			auto const lo1 = fsv::filtered_string_view{"aba"};
			auto const hi = fsv::filtered_string_view{"zzz"};
			REQUIRE(lo != hi);
			REQUIRE(lo != lo1);

		}

		
	}

	SECTION("<=> operator") {
		SECTION ("<") {
			SECTION ("Normal case") {
				auto const lo = fsv::filtered_string_view{"aaa"};
				auto const hi = fsv::filtered_string_view{"zzz"};
				REQUIRE(lo < hi);
				REQUIRE(lo <= hi);
				REQUIRE((lo > hi) == false);
				REQUIRE((lo >= hi) == false);
				REQUIRE(lo <=> hi == std::strong_ordering::less);

			}

			SECTION ("Different length") {
				auto const lo = fsv::filtered_string_view{""};
				auto const lo1 = fsv::filtered_string_view{};
				auto const hi = fsv::filtered_string_view{"zzz"};
				REQUIRE(lo < hi);
				REQUIRE(lo <= hi);
				REQUIRE((lo > hi) == false);
				REQUIRE((lo >= hi) == false);
				REQUIRE(lo <=> hi == std::strong_ordering::less);

				REQUIRE((lo < lo1) == false);
				REQUIRE((lo <= lo1) == true);
				REQUIRE((lo > lo1) == false);
				REQUIRE((lo >= lo1) == true);
				REQUIRE((lo == lo1) == true);
				REQUIRE(lo <=> lo1 == std::strong_ordering::equal);
			}
		}

		SECTION (">") {
			SECTION ("Normal case") {
				auto const lo = fsv::filtered_string_view{"aaa"};
				auto const hi = fsv::filtered_string_view{"zzz"};
				REQUIRE(hi > lo);
				REQUIRE(hi >= lo);
				REQUIRE((hi < lo) == false);
				REQUIRE((hi <= lo) == false);
				REQUIRE(hi <=> lo == std::strong_ordering::greater);

			}

			SECTION ("Different length") {
				auto const lo = fsv::filtered_string_view{""};
				auto const lo1 = fsv::filtered_string_view{};
				auto const hi = fsv::filtered_string_view{"zzz"};
				REQUIRE(hi > lo);
				REQUIRE(hi >= lo);
				REQUIRE((hi < lo) == false);
				REQUIRE((hi <= lo) == false);
				REQUIRE(hi <=> lo == std::strong_ordering::greater);

				REQUIRE((lo1 > lo) == false);
				REQUIRE((lo1 >= lo) == true);
				REQUIRE((lo1 < lo) == false);
				REQUIRE((lo1 <= lo) == true);
				REQUIRE((lo1 == lo) == true);
				REQUIRE(lo1 <=> lo == std::strong_ordering::equal);
			}
		}
	}

	SECTION("<< operator") {
		SECTION("Normal string with custom predicate") {
			auto s = "c++";
			auto fsv = fsv::filtered_string_view{"c++ > rust > java", [](const char &c){ return c == 'c' || c == '+'; }};
			auto stream = std::stringstream{};
			stream << fsv;
			REQUIRE(stream.str() == s);
		}

		SECTION("Empty string with custom predicate") {
			auto s = "";
			auto fsv = fsv::filtered_string_view{"", [](const char &c){ return c == 'c' || c == '+'; }};
			auto stream = std::stringstream{};
			stream << fsv;
			REQUIRE(stream.str() == s);
		}

	}
}

TEST_CASE("Iterators"){
	SECTION("Begin") {
		// With predicate which removes lowercase vowels:
		auto fsv = fsv::filtered_string_view{"samoyed", [](const char &c) {
			return !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
		}};
		auto it = fsv.begin();
		const char expected = 's';
		REQUIRE(*it == expected);
	}

	SECTION("++ -- begin end without predicate") {
		const auto fsv1 = fsv::filtered_string_view{"bunny"};
		auto iter = fsv1.begin();

		REQUIRE(*iter == 'b');
		++iter;
		REQUIRE(*iter == 'u');
		++iter;
		REQUIRE(*iter == 'n');
		++iter;
		REQUIRE(*iter == 'n');
		++iter;
		REQUIRE(*iter == 'y');
		++iter;
		REQUIRE(iter == fsv1.end());

		--iter;
		REQUIRE(*iter == 'y');
		--iter;
		REQUIRE(*iter == 'n');
		--iter;
		REQUIRE(*iter == 'n');
		--iter;
		REQUIRE(*iter == 'u');
		--iter;
		REQUIRE(*iter == 'b');
		REQUIRE(iter == fsv1.begin());
	}

	SECTION("++ -- begin end with predicate") {
		const auto is_upper = [](const char& c) { return std::isupper(static_cast<unsigned char>(c)); };
		const auto fsv1 = fsv::filtered_string_view{"Shawty Destroyer of all time ___", is_upper};
		auto iter = fsv1.begin();
		REQUIRE(*iter == 'S');
		++iter;
		REQUIRE(*iter == 'D');
		++iter;
		REQUIRE(iter == fsv1.end());
		--iter;
		REQUIRE(*iter == 'D');
		--iter;
		REQUIRE(*iter == 'S');
		REQUIRE(iter == fsv1.begin());
	}

	SECTION("next") {
		const auto fsv = fsv::filtered_string_view{"shy"};
		const auto it = fsv.begin();
		REQUIRE(*it == 's');
		REQUIRE(*std::next(it) == 'h');
		REQUIRE(*std::next(it, 2) == 'y');
	}

	SECTION("prev") {
		const auto str = std::string("tohsaka");
		const auto s = fsv::filtered_string_view{str};
		const auto it = s.cend();
		REQUIRE(*std::prev(it) == 'a');
		REQUIRE(*std::prev(it, 2) == 'k');
		REQUIRE(*std::prev(it, 3) == 'a');

	}

	SECTION("empty string") {
		const auto s = fsv::filtered_string_view{};
		const auto v = std::vector<char>{s.begin(), s.end()};
		REQUIRE(v.empty());
	}

	SECTION("empty string with predicate") {
		const auto s = fsv::filtered_string_view{"the king comes back", [](const char&) { return false; }};
		const auto v = std::vector<char>{s.begin(), s.end()};
		REQUIRE(v.empty());
	}

	SECTION("rbegin and rend") {
		const auto s = fsv::filtered_string_view{"Rin Tohsaka", [](const char& c) { return !(c == 'i' || c == 'o'); }};
		const auto v = std::vector<char>{s.rbegin(), s.rend()};
		REQUIRE(v[0] == 'a');
		REQUIRE(v[1] == 'k');
	}

}

TEST_CASE("Functions") {
	SECTION("Function: size") {
		SECTION("Default constructor") {
			auto string_view = fsv::filtered_string_view();
			REQUIRE(string_view.size() == 0);
		}

		SECTION("Filtered string view single match") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'c'; 
			};
			auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);
			REQUIRE(string_view.size() == 1);

		}

		SECTION("Filtered string view many matches") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;
			fsv::filter hello_predicate = [](const char &c) {
				return c == 'e'; 
			};
			auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);
			REQUIRE(string_view.size() == 4);

		}
		
		SECTION("Filtered string view all matches - with predicate: Default") {
			std::string hello_string = "Hello world can you see me?";
			const std::string &hello_ref = hello_string;
			fsv::filter hello_predicate = [](const char &) {
				return true;
			};

			auto string_view = fsv::filtered_string_view(hello_ref, hello_predicate);
			REQUIRE(string_view.size() == hello_string.size());
			
		}

		SECTION("Filtered string view all matches - string has 1 type character only") {
			std::string e_string = "eeeeeeeeeeeeeeeeee";
			const std::string &e_ref = e_string;
			fsv::filter e_predicate = [](const char &c) {
				return c == 'e'; 
			};

			auto string_view = fsv::filtered_string_view(e_ref, e_predicate);
			REQUIRE(string_view.size() == e_string.size());
			
		}
	}


	SECTION("at") {
		SECTION ("Valid cases") {
			SECTION("n is random in the middle of the string") {
				auto pred = [](const char &c) { return c == '9' || c == '0' || c == ' '; };
				auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};
				REQUIRE (fsv1.at(2) == '0');
			}

			SECTION("n is the first char") {
				auto pred = [](const char &c) { return c == '9' || c == '0' || c == 'o'; };
				auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};
				REQUIRE (fsv1.at(0) == 'o');
			}

			SECTION("n is the last char") {
				auto pred = [](const char &c) { return c == '9' || c == '0' || c == 'o' || c == 'h'; };
				auto fsv1 = fsv::filtered_string_view{"o09h", pred};
				REQUIRE (fsv1.at(3)== 'h');
			}
		}

		SECTION ("Error cases") {
			SECTION("n is negative") {
				auto pred = [](const char &c) { return c == '9' || c == '0' || c == ' '; };
				auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};
				REQUIRE_THROWS_AS(fsv1.at(-1), std::domain_error);
			}

			SECTION("n is out of upperbound") {
				auto pred = [](const char &c) { return c == '9' || c == '0' || c == ' '; };
				auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};
				REQUIRE_THROWS_AS(fsv1.at(100), std::domain_error);
			}
		}

	}


	SECTION("empty") {
		SECTION ("empty case") {
			auto empty_sv = fsv::filtered_string_view{};
			REQUIRE(empty_sv.empty());
		}
		
		SECTION ("non empty case") {
			SECTION("Default predicate") {
				auto sv = fsv::filtered_string_view{"Australian Shephard"};
				REQUIRE(!sv.empty());
			}

			SECTION("Custom predicate") {
				auto sv = fsv::filtered_string_view{"Border Collie", [](const char &c) {
					return c == 'z';
				}};
				REQUIRE(sv.empty());
			}
		}
	}

	SECTION("predicate") {
		SECTION("Default") {
			const auto print_and_return_true = [](const char &) {
				return true;
			};
			const auto s = fsv::filtered_string_view{"doggo", print_and_return_true};

			const auto& predicate = s.predicate();
			default_predicate_test(predicate);
		}

			SECTION("Custom") {
			const auto print_and_return_true = [](const char &c) {
				return c == 'c';
			};
			const auto s = fsv::filtered_string_view{"doggo", print_and_return_true};

			const auto& predicate = s.predicate();
			for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
				if (c == 'c') {
					REQUIRE(predicate(c));
				} else {
					REQUIRE(!predicate(c));
				}
			}
		}
	}

	SECTION("data") {
		SECTION("non empty string: Sum 42") {
			auto s = "Sum 42";
			auto sv = fsv::filtered_string_view{s, [](const char &){ return false; }};
			REQUIRE(sv.data() == s);
		} 

		SECTION("empty string") {
			auto s = "";
			auto sv = fsv::filtered_string_view{s, [](const char &){ return false; }};
			REQUIRE(sv.data() == s);
		} 

		SECTION("null terminated string") {
			const char * s = "\0";
			auto sv = fsv::filtered_string_view{s, [](const char &){ return false; }};
			REQUIRE(sv.data() == s);
		} 
	}
}

TEST_CASE("Compose") {
	SECTION("Test compose with empty string") {
		const auto fs = fsv::filtered_string_view{};
		const auto vf = std::vector<fsv::filter>{[](const char& c) { return c == 'g'; },
		                                         [](const char& c) { return c == 'a'; }};
		const auto sv = fsv::compose(fs, vf);
		REQUIRE(sv == "");
	}

	SECTION("No matches") {
		const auto best_languages = fsv::filtered_string_view{"c++ and java"};
		const auto c_predicate = [](const char& c) {
			return c == 'c' || c == '+';
		};
		const auto java_predicate = [](const char& c) {
			return c == 'j' || c == 'a' || c == 'v';
		};
		const auto vf = std::vector<fsv::filter>{c_predicate, java_predicate};
		const auto sv = fsv::compose(best_languages, vf);
		REQUIRE(sv == "");
	}

	SECTION("Have matches") {
		const auto best_languages = fsv::filtered_string_view{"c++ and java"};
		const auto c_predicate = [](const char& c) {
			return c == 'c' || c == '+';
		};
		const auto java_predicate = [](const char& c) {
			return c > ' ';
		};
		const auto vf = std::vector<fsv::filter>{c_predicate, java_predicate};
		const auto sv = fsv::compose(best_languages, vf);
		REQUIRE(sv == "c++");
	}

	SECTION("Empty vector of filters") {
		const auto say = fsv::filtered_string_view{"say my name"};
		const auto vf = std::vector<fsv::filter>{};
		const auto sv = fsv::compose(say, vf);
		REQUIRE(sv == "say my name");
	}

	SECTION("Stop when the first filter returns false") {
		const auto pred = [](const char& c) { return c == 'c'; };
		const auto fs = fsv::filtered_string_view{"all the stars around you", pred};
		const auto vf = std::vector<fsv::filter>{[](const char& c) { return c == 'd'; },
												[](const char& c) { return c == 'a' || c == 'd'; }};
		const auto sv = fsv::compose(fs, vf);
		REQUIRE(sv == "d");
	}


}

TEST_CASE("Split") {
	SECTION("No predicate") {
		const auto sv = fsv::filtered_string_view{"0xAIOXIMA / 0xHAHAHA"};
		const auto tok = fsv::filtered_string_view{" / "};
		const auto v = fsv::split(sv, tok);
		const auto sv2 = fsv::filtered_string_view{"0xAIOXIMA"};
		const auto sv3 = fsv::filtered_string_view{"0xHAHAHA"};
		REQUIRE(v[0] == sv2);
		REQUIRE(v[1] == sv3);
	}
	
	SECTION("No predicate: delimeter at the start and end") {
		const auto sv = fsv::filtered_string_view{"|a|"};
		const auto tok = fsv::filtered_string_view{"|"};
		const auto v = fsv::split(sv, tok);
		const auto expected = std::vector<fsv::filtered_string_view>{"", "a", ""};
		CHECK(v == expected);
	}

	SECTION("No predicate: strings contains only delimeters") {
		const auto sv = fsv::filtered_string_view{"||||"};
		const auto tok = fsv::filtered_string_view{"|"};
		const auto v = fsv::split(sv, tok);
		const auto expected = std::vector<fsv::filtered_string_view>{"", "", "", "", ""};
		CHECK(v == expected);
	}

	SECTION("No predicate: empty token") {
		const auto sv = fsv::filtered_string_view{"The new moon has risen"};
		const auto tok = fsv::filtered_string_view{};
		const auto v = fsv::split(sv, tok);
		const auto expected = std::vector<fsv::filtered_string_view>{"The new moon has risen"};
		CHECK(v == expected);
	}

	SECTION("Has predicate") {
		const auto predicate_no_0x = [](const auto &c) {
			return c != '0' and c != 'x';
		};
		const auto sv = fsv::filtered_string_view{"0xAIOXIMA / 0xHAHAHA", predicate_no_0x};
		const auto tok = fsv::filtered_string_view{" / "};
		const auto v = fsv::split(sv, tok);
		const auto sv2 = fsv::filtered_string_view{"AIOXIMA"};
		const auto sv3 = fsv::filtered_string_view{"HAHAHA"};
		REQUIRE(v[0] == sv2);
		REQUIRE(v[1] == sv3);
	}
}

TEST_CASE("Substring") {
	SECTION("No predicate") {
		const auto sv1 = fsv::filtered_string_view{"Siberian Husky"};
		const auto sv2 = fsv::filtered_string_view{"Siberian"};
		const auto sv3= fsv::filtered_string_view{"Husky"};
		REQUIRE(fsv::substr(sv1, 0, 8) == sv2);
		REQUIRE(fsv::substr(sv1, 9) == sv3);

	}

	SECTION("No predicate: return whole string") {
		const auto sv1 = fsv::filtered_string_view{"Holla Joey"};
		const auto sv2 = fsv::filtered_string_view{"Holla Joey"};
		CHECK(fsv::substr(sv1, 0, 10) == sv2);
	}

	SECTION("Custom predicate") {
		const auto is_lower = [](const char& c) { return std::islower(static_cast<unsigned char>(c)); };
		const auto sv1 = fsv::filtered_string_view{"this is all you CANT SEE", is_lower};
		const auto sv2 = fsv::filtered_string_view{"this"};
		REQUIRE(fsv::substr(sv1, 0, 4) == sv2);
	}

	SECTION("Pos 0, count 0 with default predicate") {
		const auto predicate = [](const char&) {
			return false;
		};
		const auto sv1 = fsv::filtered_string_view{"Sled Dog", predicate};
		const auto sv2 = fsv::filtered_string_view{""};
		CHECK(fsv::substr(sv1, 0, 0) == sv2);
	}



}
