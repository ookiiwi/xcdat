#pragma once

#include <string>
#include <type_traits>

#include "exception.hpp"
#include "immutable_vector.hpp"

namespace xcdat {

class save_visitor {
  private:
    std::ofstream m_ofs;
    std::stringstream* m_ss;

  public:
    save_visitor(const std::string& filepath) : m_ofs(filepath, std::ios::binary), m_ss(NULL) {
        XCDAT_THROW_IF(!m_ofs.good(), "Cannot open the input file");
    }

    save_visitor(std::stringstream* ss) : m_ss(ss) {
        XCDAT_THROW_IF(!ss, "Pointer must not be null");
    }

    virtual ~save_visitor() {
        m_ofs.close();
    }

    template <typename T>
    void visit(const immutable_vector<T>& vec) {
        if (m_ss) vec.save(*m_ss);
        else vec.save(m_ofs);
    }

    template <typename T>
    void visit(const T& obj) {
        if constexpr (std::is_pod_v<T>) {
            if (m_ss) {
                m_ss->write(reinterpret_cast<const char*>(&obj), sizeof(T));
            } else {
                m_ofs.write(reinterpret_cast<const char*>(&obj), sizeof(T));
            }
        } else {
            const_cast<T&>(obj).visit(*this);
        }
    }

    std::uint64_t bytes() {
        return m_ss ? m_ss->tellp() : m_ofs.tellp();
    }
};

}  // namespace xcdat