#pragma once

#include <string>
#include <type_traits>

#include "exception.hpp"
#include "immutable_vector.hpp"

namespace xcdat {

class load_visitor {
  private:
    std::istream& m_is;
    std::ifstream m_ifs;

  public:
    load_visitor(const std::string& filepath) : m_is(m_ifs), m_ifs(filepath, std::ios::binary) {
        XCDAT_THROW_IF(!m_ifs.good(), "Cannot open the input file");
    }

    load_visitor(std::istream& ss) : m_is(ss) {}

    virtual ~load_visitor() {
        m_ifs.close();
    }

    template <class T>
    void visit(immutable_vector<T>& vec) {
        vec.load(m_is);
    }

    template <class T>
    void visit(T& obj) {
        if constexpr (std::is_pod_v<T>) {
            m_is.read(reinterpret_cast<char*>(&obj), sizeof(T));
        } else {
            obj.visit(*this);
        }
    }

    std::uint64_t bytes() {
        return m_is.tellg();
    }
};

}  // namespace xcdat
