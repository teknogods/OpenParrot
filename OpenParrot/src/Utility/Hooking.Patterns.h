/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

#pragma once

#define PATTERNS_USE_HINTS 0

#include <cassert>
#include <vector>

namespace hook
{
    extern ptrdiff_t baseAddressDifference;

    // sets the base address difference based on an obtained pointer
    inline void set_base(uintptr_t address)
    {
#ifdef _M_IX86
        uintptr_t addressDiff = (address - 0x400000);
#elif defined(_M_AMD64)
        uintptr_t addressDiff = (address - 0x140000000);
#endif

        // pointer-style cast to ensure unsigned overflow ends up copied directly into a signed value
        baseAddressDifference = *(ptrdiff_t*)&addressDiff;
    }

    // sets the base to the process main base
    void set_base();

    template<typename T>
    inline T* getRVA(uintptr_t rva)
    {
        set_base();
#ifdef _M_IX86
        return (T*)(baseAddressDifference + 0x400000 + rva);
#elif defined(_M_AMD64)
        return (T*)(baseAddressDifference + 0x140000000 + rva);
#endif
    }

    class pattern_match
    {
    private:
        void* m_pointer;

    public:
        inline pattern_match(void* pointer)
            : m_pointer(pointer)
        {
        }

        template<typename T>
        T* get(ptrdiff_t offset = 0) const
        {
            char* ptr = reinterpret_cast<char*>(m_pointer);
            return reinterpret_cast<T*>(ptr + offset);
        }
    };

    class pattern
    {
    private:
        std::vector<uint8_t> m_bytes;
        std::vector<uint8_t> m_mask;

#if PATTERNS_USE_HINTS
        uint64_t m_hash;
#endif

        size_t m_size;

        std::vector<pattern_match> m_matches;

        bool m_matched;

        union
        {
            void* m_module;
            struct
            {
                uintptr_t m_rangeStart;
                uintptr_t m_rangeEnd;
            };
        };

    protected:
        inline pattern(void* module)
            : m_module(module), m_rangeEnd(0), m_matched(false)
        {
        }

        inline pattern(uintptr_t begin, uintptr_t end)
            : m_rangeStart(begin), m_rangeEnd(end), m_matched(false)
        {
        }

        void Initialize(const char* pattern);

    private:
        bool ConsiderMatch(uintptr_t offset);

        void EnsureMatches(uint32_t maxCount);

        inline const pattern_match& _get_internal(size_t index)
        {
            return m_matches[index];
        }

    public:
        pattern()
            : m_matched(true)
        {
        }

        pattern(const char *pattern)
            : pattern(getRVA<void>(0))
        {
            Initialize(pattern);
        }

        pattern(std::string& pattern)
            : pattern(getRVA<void>(0))
        {
            Initialize(pattern.c_str());
        }

        inline pattern& count(uint32_t expected)
        {
            EnsureMatches(expected);
            assert(m_matches.size() == expected);
            return *this;
        }

        inline pattern& count_hint(uint32_t expected)
        {
            EnsureMatches(expected);
            return *this;
        }

        inline pattern& clear(void* module = nullptr)
        {
            if (module)
                m_module = module;
            m_matches.clear();
            m_matched = false;
            return *this;
        }

        inline size_t size()
        {
            EnsureMatches(UINT32_MAX);
            return m_matches.size();
        }

        inline bool empty()
        {
            return size() == 0;
        }

        inline const pattern_match& get(size_t index)
        {
            EnsureMatches(UINT32_MAX);
            return _get_internal(index);
        }

        inline const pattern_match& get_one()
        {
            return count(1)._get_internal(0);
        }

        template<typename T = void>
        inline auto get_first(ptrdiff_t offset = 0)
        {
            return get_one().get<T>(offset);
        }

        template <typename Fn>
        pattern &for_each_result(Fn Pr)
        {
            EnsureMatches(UINT32_MAX);

            for (auto &result : this->m_matches)
            {
                Pr(result);
            }

            return *this;
        }

    public:
#if PATTERNS_USE_HINTS
        // define a hint
        static void hint(uint64_t hash, uintptr_t address);
#endif
    };

    class module_pattern
        : public pattern
    {
    public:
        module_pattern(void* module, const char *pattern)
            : pattern(module)
        {
            Initialize(pattern);
        }

        module_pattern(void* module, std::string& pattern)
            : pattern(module)
        {
            Initialize(pattern.c_str());
        }
    };

    class range_pattern
        : public pattern
    {
    public:
        range_pattern(uintptr_t begin, uintptr_t end, const char *pattern)
            : pattern(begin, end)
        {
            Initialize(pattern);
        }

        range_pattern(uintptr_t begin, uintptr_t end, std::string& pattern)
            : pattern(begin, end)
        {
            Initialize(pattern.c_str());
        }
    };


    template<typename T = void>
    auto get_pattern(const char *pattern_string, ptrdiff_t offset = 0)
    {
        return pattern(pattern_string).get_first<T>(offset);
    }
}
