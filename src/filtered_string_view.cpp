#include "./filtered_string_view.h"
#include "filtered_string_view.h"

// Implement here
// Member operator
// Initialization for static member
fsv::filter fsv::filtered_string_view::default_predicate = [](const char &) {
    return true;
};

// Copy assignment
fsv::filtered_string_view fsv::filtered_string_view::operator=(const filtered_string_view &other) {
    if (this == &other) {
        return *this;
    }

    pointer = other.pointer;
    length = other.length;
    str_predicate = other.str_predicate;

    return *this;
};

// Move assignment
fsv::filtered_string_view fsv::filtered_string_view::operator=(filtered_string_view &&other) noexcept {
    if (this == &other) {
        return *this;
    }
    pointer = std::move(other.pointer);
    length = std::move(other.length);
    str_predicate = std::move(other.str_predicate);
    other.pointer = nullptr;
    other.length = 0;
    other.str_predicate = default_predicate;
    return *this;
};

// Subscript
auto fsv::filtered_string_view::operator[](int n) const -> const char &{
    return at(n);
}

// String Type Conversion
fsv::filtered_string_view::operator std::string() const {
    std::string return_string = "";
    if (pointer == nullptr) {
        return return_string;
    }
    size_t idx = 0;
    while (idx < strlen(pointer)) {
        if (str_predicate(pointer[idx])) {
            return_string.push_back(pointer[idx]);
        }
        ++idx;
    }
    return return_string;
};

// Compose
auto fsv::compose(const fsv::filtered_string_view &fsv, const std::vector<fsv::filter> &filts) -> fsv::filtered_string_view {
    if (fsv.data() == nullptr) {
        return fsv;
    };

    const auto ult_predicate = [filts](const char &c) {
        for (const auto &filt : filts) {
            if (not filt(c)) {
                return false;
            }
        }
        return true;
    };

    return fsv::filtered_string_view(fsv.data(), ult_predicate);
};

// Split
void cut_and_add_segments(std::vector<size_t> filter_position, 
                            std::size_t start, std::size_t end, 
                            fsv::filtered_string_view fsv, 
                            std::vector<fsv::filtered_string_view> &return_vector) {
    auto segment_start_idx = filter_position[start];
    auto segment_pass_end_idx = filter_position[end];
    auto segment_predicate = [fsv, segment_start_idx, segment_pass_end_idx](const char &c) {
        std::size_t char_postion = static_cast<std::size_t>(&c - fsv.data());
        return char_postion >= segment_start_idx and 
                char_postion < segment_pass_end_idx and
                fsv.predicate()(c);
    };
    return_vector.emplace_back(fsv.data(), segment_predicate);
}
auto fsv::split(const fsv::filtered_string_view &fsv, const fsv::filtered_string_view &tok) -> std::vector<fsv::filtered_string_view> {
    std::vector<fsv::filtered_string_view> return_vector;
    if (tok.size() == 0) {
        const fsv::filtered_string_view fsv_copy = fsv;
        return_vector.push_back(fsv_copy);
        return return_vector;
    }
    auto fsv_data = fsv.data();
    auto fsv_str = static_cast<std::string>(fsv);
    auto tok_str = static_cast<std::string>(tok);
    auto tok_len = tok_str.size();
    std::size_t start = 0;
    std::size_t end = 0;
    // Store the location of each valid character in fsv (passing through predicate)
    std::vector<size_t> filter_position;
    for (size_t i = 0; i < strlen(fsv_data); ++i) {
        if (fsv.predicate()(fsv_data[i])) {
            filter_position.push_back(i);
        }
    }

    // Splitting fsv into segments 
    while ((end = fsv_str.find(tok_str, start)) != std::string::npos) {
        cut_and_add_segments(filter_position, start, end, fsv, return_vector);
        start = end + tok_len;
    }

    // Adding the last segments
    cut_and_add_segments(filter_position, start, strlen(fsv_data), fsv, return_vector);
    return return_vector;
};

// Substring
auto fsv::substr(const fsv::filtered_string_view &fsv, int pos, int count) -> fsv::filtered_string_view {
    if (fsv.size() == 0) {
        auto return_fs = fsv::filtered_string_view("");
        return return_fs;
    }
    auto rcount = 0;
    if (count <= 0) {
        rcount = static_cast<int>(fsv.size()) - pos;
    } else {
        rcount = count;
    }
    auto segment_predicate = [fsv, pos, rcount](const char &c) {
        const char * char_ptr = fsv.data() +  (&c - fsv.data());
        return char_ptr >= &fsv[pos] and 
                char_ptr <= &fsv[pos + rcount - 1] and
                fsv.predicate()(c);
    };
    return fsv::filtered_string_view{fsv.data(), segment_predicate};
};
