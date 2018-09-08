/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

#include "Hooking.Patterns.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <algorithm>

#if PATTERNS_USE_HINTS
#include <map>
#endif

#if PATTERNS_USE_HINTS

// from boost someplace
template <std::uint64_t FnvPrime, std::uint64_t OffsetBasis>
struct basic_fnv_1
{
    std::uint64_t operator()(const char *text) const
    {
        std::uint64_t hash = OffsetBasis;

        while (*text != 0)
        {
            hash *= FnvPrime;
            hash ^= *(uint8_t *)text;

            ++text;
        }

        return hash;
    }
};

const std::uint64_t fnv_prime = 1099511628211u;
const std::uint64_t fnv_offset_basis = 14695981039346656037u;

typedef basic_fnv_1<fnv_prime, fnv_offset_basis> fnv_1;

#endif

namespace hook
{
    ptrdiff_t baseAddressDifference;

    // sets the base to the process main base
    void set_base()
    {
        set_base((uintptr_t)GetModuleHandle(nullptr));
    }


#if PATTERNS_USE_HINTS
static std::multimap<uint64_t, uintptr_t> g_hints;
#endif

static void TransformPattern(const char *pattern, std::vector<uint8_t>& data, std::vector<uint8_t>& mask)
{
    auto tol = [](char ch) -> uint8_t
    {
        if (ch >= 'A' && ch <= 'F') return uint8_t(ch - 'A' + 10);
        if (ch >= 'a' && ch <= 'f') return uint8_t(ch - 'a' + 10);
        return uint8_t(ch - '0');
    };

    auto is_digit = [](char ch) -> bool
    {
        return (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f') || (ch >= '0' && ch <= '9');
    };

    char temp_string[2]{ 0, 0 };

    data.clear();
    mask.clear();

    if (!pattern)
    {
        return;
    }

    const char *patit = pattern;
    const char *patend = (pattern + strlen(pattern) + 1);

    while (patit != patend)
    {
        char ch = *patit;

        if (ch == ' ' || ch == 0)
        {
            if (!temp_string[0] && !temp_string[1]) //Continous delimiter
            {
                
            }
            else if (temp_string[0] == '?' && (temp_string[1] == '?' || temp_string[1] == 0)) //??
            {
                data.push_back(0);
                mask.push_back(0u);
            }
            else if (temp_string[0] == '?' && is_digit(temp_string[1])) //?x
            {
                data.push_back(tol(temp_string[1]));
                mask.push_back(0x0Fu);
            }
            else if (temp_string[1] == '?' && is_digit(temp_string[0])) //x?
            {
                data.push_back(tol(temp_string[0]) << 4);
                mask.push_back(0xF0u);
            }
            else if (is_digit(temp_string[0]) && is_digit(temp_string[1])) //xx
            {
                data.push_back((tol(temp_string[0]) << 4) | tol(temp_string[1]));
                mask.push_back(0xFFu);
            }
            else
            {
                data.clear();
                mask.clear();
                return;
            }

            temp_string[0] = 0;
            temp_string[1] = 0;
        }
        else
        {
            if (temp_string[0] == 0)
            {
                temp_string[0] = ch;
            }
            else if (temp_string[1] == 0)
            {
                temp_string[1] = ch;
            }
            else
            {
                data.clear();
                mask.clear();
                return;
            }
        }

        ++patit;
    }
}

class executable_meta
{
private:
    uintptr_t m_begin;
    uintptr_t m_end;

public:
    template<typename TReturn, typename TOffset>
    TReturn* getRVA(TOffset rva)
    {
        return (TReturn*)(m_begin + rva);
    }

    explicit executable_meta(void* module)
        : m_begin((uintptr_t)module), m_end(0)
    {
        static auto getSection = [](const PIMAGE_NT_HEADERS nt_headers, unsigned section) -> PIMAGE_SECTION_HEADER
        {
            return reinterpret_cast<PIMAGE_SECTION_HEADER>(
                (UCHAR*)nt_headers->OptionalHeader.DataDirectory +
                nt_headers->OptionalHeader.NumberOfRvaAndSizes * sizeof(IMAGE_DATA_DIRECTORY) +
                section * sizeof(IMAGE_SECTION_HEADER));
        };

        PIMAGE_DOS_HEADER dosHeader = getRVA<IMAGE_DOS_HEADER>(0);
        PIMAGE_NT_HEADERS ntHeader = getRVA<IMAGE_NT_HEADERS>(dosHeader->e_lfanew);

        for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
        {
            auto sec = getSection(ntHeader, i);
            auto secSize = sec->SizeOfRawData != 0 ? sec->SizeOfRawData : sec->Misc.VirtualSize;
            //if (sec->Characteristics & IMAGE_SCN_MEM_EXECUTE)
                m_end = m_begin + sec->VirtualAddress + secSize;

            if ((i == ntHeader->FileHeader.NumberOfSections - 1) && m_end == 0)
                m_end = m_begin + sec->PointerToRawData + secSize;
        }
    }

    executable_meta(uintptr_t begin, uintptr_t end)
        : m_begin(begin), m_end(end)
    {
    }

    inline uintptr_t begin() const { return m_begin; }
    inline uintptr_t end() const   { return m_end; }
};

void pattern::Initialize(const char* pattern)
{
    // get the hash for the base pattern
#if PATTERNS_USE_HINTS
    m_hash = fnv_1()(pattern);
#endif

    // transform the base pattern from IDA format to canonical format
    TransformPattern(pattern, m_bytes, m_mask);

    m_size = m_mask.size();

#if PATTERNS_USE_HINTS
    // if there's hints, try those first
    if (m_module == GetModuleHandle(nullptr))
    {
        auto range = g_hints.equal_range(m_hash);

        if (range.first != range.second)
        {
            std::for_each(range.first, range.second, [&] (const std::pair<uint64_t, uintptr_t>& hint)
            {
                ConsiderMatch(hint.second);
            });

            // if the hints succeeded, we don't need to do anything more
            if (!m_matches.empty())
            {
                m_matched = true;
                return;
            }
        }
    }
#endif
}

void pattern::EnsureMatches(uint32_t maxCount)
{
    if (m_matched)
        return;

    if (!m_rangeStart && !m_rangeEnd && !m_module)
        return;

    // scan the executable for code
    executable_meta executable = m_rangeStart != 0 && m_rangeEnd != 0 ? executable_meta(m_rangeStart, m_rangeEnd) : executable_meta(m_module);

    auto matchSuccess = [&] (uintptr_t address)
    {
#if PATTERNS_USE_HINTS
        g_hints.emplace(m_hash, address);
#else
        (void)address;
#endif

        return (m_matches.size() == maxCount);
    };

    ptrdiff_t BadCharacter[256];

    std::ptrdiff_t index;

    const std::uint8_t *pbytes = m_bytes.data();
    const std::uint8_t *pmask = m_mask.data();

    for (std::uint32_t bc = 0; bc < 256; ++bc)
    {
        for (index = m_size - 1; index >= 0; --index)
        {
            if ((pbytes[index] & pmask[index]) == (bc & pmask[index]))
            {
                break;
            }
        }

        BadCharacter[bc] = index;
    }

    __try
    {
        for (uintptr_t i = executable.begin(), end = executable.end() - m_size; i <= end;)
        {
            uint8_t* ptr = reinterpret_cast<uint8_t*>(i);

            for (index = m_size - 1; index >= 0; --index)
            {
                if ((pbytes[index] & pmask[index]) != (ptr[index] & pmask[index]))
                {
                    break;
                }
            }

            if (index == -1)
            {
                m_matches.emplace_back(ptr);

                if (matchSuccess(i))
                {
                    break;
                }

                i += m_size;
            }
            else
            {
                i += max(index - BadCharacter[ptr[index]], 1);
            }
        }
    }
    __except ((GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) 
    { }
    m_matched = true;
}

bool pattern::ConsiderMatch(uintptr_t offset)
{
    const uint8_t* pattern = m_bytes.data();
    const uint8_t* mask = m_mask.data();

    char* ptr = reinterpret_cast<char*>(offset);

    for (size_t i = 0; i < m_size; i++)
    {
        if ((pattern[i] & mask[i]) != (ptr[i] & mask[i]))
        {
            return false;
        }
    }

    m_matches.emplace_back(ptr);

    return true;
}

#if PATTERNS_USE_HINTS
void pattern::hint(uint64_t hash, uintptr_t address)
{
    auto range = g_hints.equal_range(hash);

    for (auto it = range.first; it != range.second; it++)
    {
        if (it->second == address)
        {
            return;
        }
    }

    g_hints.emplace(hash, address);
}
#endif
}
