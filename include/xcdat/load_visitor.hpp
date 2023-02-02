#pragma once

#include <string>
#include <type_traits>

#include "exception.hpp"
#include "immutable_vector.hpp"

namespace xcdat {

class load_visitor {
  private:
    std::ifstream m_ifs;
    std::stringstream* m_ss;

  public:
    load_visitor(const std::string& filepath) : m_ifs(filepath, std::ios::binary), m_ss(nullptr) {
        XCDAT_THROW_IF(!m_ifs.good(), "Cannot open the input file");
    }

    load_visitor(std::stringstream* ss) : m_ss(ss) {
        XCDAT_THROW_IF(!m_ss, "String stream pointer must not be null");
    }

    virtual ~load_visitor() {
        m_ifs.close();
    }

    template <class T>
    void visit(immutable_vector<T>& vec) {
        if (m_ss) {
            vec.load(*m_ss);
        } else {
            vec.load(m_ifs);
        }
    }

    template <class T>
    void visit(T& obj) {
        if constexpr (std::is_pod_v<T>) {
            if (m_ss) {
                m_ss->read(reinterpret_cast<char*>(&obj), sizeof(T));
            } else {
                m_ifs.read(reinterpret_cast<char*>(&obj), sizeof(T));
            }
        } else {
            obj.visit(*this);
        }
    }

    std::uint64_t bytes() {
        return m_ifs.tellg();
    }
};

}  // namespace xcdat
