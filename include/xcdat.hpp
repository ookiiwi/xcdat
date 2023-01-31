#pragma once

#include <fstream>
#include <string>

#include "xcdat/bc_vector_15.hpp"
#include "xcdat/bc_vector_16.hpp"
#include "xcdat/bc_vector_7.hpp"
#include "xcdat/bc_vector_8.hpp"
#include "xcdat/load_visitor.hpp"
#include "xcdat/mmap_visitor.hpp"
#include "xcdat/save_visitor.hpp"
#include "xcdat/size_visitor.hpp"
#include "xcdat/trie.hpp"

namespace xcdat {

//! The trie type with standard DACs using 8-bit integers
using trie_8_type = trie<bc_vector_8>;

//! The trie type with standard DACs using 16-bit integers
using trie_16_type = trie<bc_vector_16>;

//! The trie type with pointer-based DACs using 7-bit integers (for the 1st layer)
using trie_7_type = trie<bc_vector_7>;

//! The trie type with pointer-based DACs using 15-bit integers (for the 1st layer)
using trie_15_type = trie<bc_vector_15>;

//! Set the continuous memory block to a new trie instance (for a memory-mapped file).
template <class Trie>
[[maybe_unused]] Trie mmap(const char* address) {
    mmap_visitor visitor(address);

    std::uint32_t type_id;
    visitor.visit(type_id);
    XCDAT_THROW_IF(type_id != Trie::type_id, "The input dictionary type is different.");

    Trie idx;
    visitor.visit(idx);
    return idx;
}

//! Load the trie dictionary from the file.
template <class Trie>
[[maybe_unused]] Trie load(const std::string& filepath) {
    load_visitor visitor(filepath);

    std::uint32_t type_id;
    visitor.visit(type_id);
    XCDAT_THROW_IF(type_id != Trie::type_id, "The input dictionary type is different.");

    Trie idx;
    visitor.visit(idx);
    return idx;
}

//! Save the trie dictionary to the file and returns the file size in bytes.
//! The identifier of the trie type will be written in the first 4 bytes.
template <class Trie>
[[maybe_unused]] std::uint64_t save(const Trie& idx, const std::string& filepath) {
    save_visitor visitor(filepath);
    visitor.visit(static_cast<std::uint32_t>(Trie::type_id));  // identifier
    visitor.visit(const_cast<Trie&>(idx));
    return visitor.bytes();
}

template <class Trie>
[[maybe_unused]] std::uint64_t save(const Trie& idx, std::stringstream& ss) {
    save_visitor visitor(&ss);
    visitor.visit(static_cast<std::uint32_t>(Trie::type_id));  // identifier
    visitor.visit(const_cast<Trie&>(idx));
    return visitor.bytes();
}

//! Get the dictionary size in bytes.
template <class Trie>
[[maybe_unused]] std::uint64_t memory_in_bytes(const Trie& idx) {
    size_visitor visitor;
    visitor.visit(static_cast<std::uint32_t>(Trie::type_id));  // identifier
    visitor.visit(const_cast<Trie&>(idx));
    return visitor.bytes();
}

//! Get the identifier of the trie type embedded by the function 'save'.
//! The identifier corresponds to trie::type_id and will be used to detect the trie type.
[[maybe_unused]] std::uint32_t get_type_id(const std::string& filepath) {
    std::ifstream ifs(filepath);
    XCDAT_THROW_IF(!ifs.good(), "Cannot open the input file");

    std::uint32_t type_id;
    ifs.read(reinterpret_cast<char*>(&type_id), sizeof(type_id));
    return type_id;
}

}  // namespace xcdat
