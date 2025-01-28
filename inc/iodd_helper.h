#ifndef IODD_HELPER_H
#define IODD_HELPER_H

#include "types.h"
#include "ezxml.h"

#define T_INT       1
#define T_FLOAT     2

inline u8 HEXSTR2INT(char c){
	if (c >= '0'  && c <= '9'){
		return c - '0';
	}
	else if( c >= 'a' && c <= 'f'){
		return c - 'a' + 10;
	}
	else if( c >= 'A' && c <= 'F'){
		return c - 'A' + 10;
	}else{
        return 0;
    }
	return 0;
}

inline int STR2INT(IODDStr s) {
	return atoi(s.c_str());
}

inline float STR2FLOAT(IODDStr s) {
	return atof(s.c_str());
}

inline IODDStrVect SPLIT(const IODDStr& str, const char delim = ',')
{
    IODDStrVect ret;
    size_t start;
    size_t end = 0;
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
        end = str.find(delim, start);
        ret.push_back(str.substr(start, end - start));
    }
    return ret;
}

template <typename T>
inline T StringToNumber ( const std::string &Text )
{
    std::istringstream ss(Text);
    T result;
    return ss >> result ? result : 0;
}

template <class Type>
inline void XMLGetNumeric(ezxml_t xml, IODDConstChar tag, Type& val, int type = 1) {
    IODDConstChar data = ezxml_attr(xml, tag);
    if(data != NULL) {
        IODDStr retStr(data);
        if(type == T_INT) {
            val = StringToNumber<int>(retStr);
        } else if(type == T_FLOAT) {
            val = StringToNumber<float>(retStr);
        }
    } else {
        val = 0;
    }
}

inline void XMLGetStr(ezxml_t xml, IODDConstChar tag, IODDStr& val) {
    IODDConstChar data = ezxml_attr(xml, tag);
    if(data != NULL) {
        val = std::string(data);
    }
}

template <typename T, typename Collection>
inline T* FindById(Collection& collection, const IODDStr& id, const std::function<IODDStr(const T&)>& idGetter) {
    auto it = std::find_if(collection.begin(), collection.end(), [&id, &idGetter](const T& item) {
        return idGetter(item) == id;
    });
    if (it != collection.end()) {
        return &(*it); // Return pointer to the found element
    }
    return nullptr; // Return nullptr if not found
}

inline bool IODD_Helper_ParseRegisterValue(IODDStr value, int bitlength, u32& ret) {
    if (bitlength % 4 != 0) {
        IODD_ERROR("Bit-length invalid (not a multiple of 4): %d", bitlength);
        return false;
    }

    int words = bitlength / 4; // Number of hex characters
    if (words > value.length()) {
        IODD_ERROR("Bit-length > value-length");
        return false;
    }

    // Check if the raw value is all zeros
    if (std::all_of(value.begin(), value.end(), [](char c) { return c == '0'; })) {
        IODD_DEBUG("Raw value is all zeros, returning 0");
        ret = 0;
        return true;
    }

    // Check the rightmost 8 bits
    IODDStr rightmost = value.substr(value.length() - words, words);
    bool isRightZero = std::all_of(rightmost.begin(), rightmost.end(), [](char c) { return c == '0'; });

    if (isRightZero) {
        // Extract from the leftmost `bitlength` bits
        IODDStr leftmost = value.substr(0, words);
        char* p;
        ret = strtoul(leftmost.c_str(), &p, 16);
        if (*p != 0) {
            IODD_ERROR("ERROR ParseRegisterValue (leftmost): not a number");
            return false;
        }
        IODD_DEBUG("Extracted leftmost value: %s, Parsed: %u", leftmost.c_str(), ret);
    } else {
        // Extract from the rightmost `bitlength` bits
        char* p;
        ret = strtoul(rightmost.c_str(), &p, 16);
        if (*p != 0) {
            IODD_ERROR("ERROR ParseRegisterValue (rightmost): not a number");
            return false;
        }
        IODD_DEBUG("Extracted rightmost value: %s, Parsed: %u", rightmost.c_str(), ret);
    }

    return true;
}

inline IODDStr IODD_Helper_ParseStringValue(const IODDStr& hex){
    IODDStr result;
    result.reserve(hex.length() / 2);
    for (size_t i = 0; i < hex.length(); i += 2) {
        // Convert each pair of hex characters to a single character
        std::string byte = hex.substr(i, 2);
        char chr = static_cast<char>(std::stoi(byte, nullptr, 16));
        result.push_back(chr);
    }
    // Trim trailing null characters
    result.erase(result.find_last_not_of('\0') + 1);
    return result;
}

inline bool IODD_Helper_ParseRecordValue(IODDStr type, int bitLength, int bitOffset,  IODDStr value, float& ret) {
    // Input validation
    if (value.empty() || (value.length() % 2) != 0) {
        IODD_ERROR("Invalid hex string length");
        return false;
    }

    // Set default bitLength based on type
    if(bitLength == 0) {
        if (type == RECORD_TYPE_FLOAT) bitLength = 32;
        else if (type == RECORD_TYPE_BOOL) bitLength = 1;
        else if (type == RECORD_TYPE_INT || type == RECORD_TYPE_UINT) bitLength = 16;
        else {
            IODD_ERROR("Unknown type: %s", type.c_str());
            return false;
        }
    }

    // Bounds checking
    if(bitLength + bitOffset > value.length() * 4) {
        IODD_ERROR("Record exceeds data bounds: offset=%d, length=%d, data_bits=%lu", 
                  bitOffset, bitLength, value.length() * 4);
        return false;
    }

    // Calculate positions and lengths
    const u8 posB = bitOffset / 8;          // Byte offset
    const u8 posb = bitOffset % 8;          // Bit offset within byte
    u8 lenB = bitLength / 8;                // Full bytes needed
    const u8 lenb = bitLength % 8;          // Remaining bits
    const size_t bytesLen = value.length() / 2;
    
    // Adjust byte length for partial bytes
    if(lenb > 0 || posb > 0) lenB++;
    if((posb + lenb) > 8) lenB++;

    // Convert hex string to bytes
    std::vector<u8> bytes(bytesLen);
    for(size_t i = 0; i < bytesLen; i++) {
        bytes[i] = (HEXSTR2INT(value[2*i]) << 4) | HEXSTR2INT(value[2*i + 1]);
    }

    // Calculate position from end (maintaining your original byte order logic)
    const int pos = bytesLen - posB - 1;

    // Extract value
    u64 result = 0;
    for(u8 i = 0; i < lenB && (pos - i) >= 0; i++) {
        result |= (static_cast<u64>(bytes[pos - i]) << (8 * i));
    }

    // Apply bit offset and create mask
    result >>= posb;
    result &= ((1ULL << bitLength) - 1);

    // Handle different types
    if(type == RECORD_TYPE_FLOAT) {
        if(bitLength != 32) {
            IODD_ERROR("Float must be 32 bits");
            return false;
        }
        ret = *reinterpret_cast<float*>(&result);
    }
    else if(type == RECORD_TYPE_BOOL) {
        ret = result ? 1.0f : 0.0f;
    }
    else if(type == RECORD_TYPE_INT) {
        // Sign extend if necessary
        if(result & (1ULL << (bitLength - 1))) {
            result |= ~((1ULL << bitLength) - 1);
        }
        ret = static_cast<float>(static_cast<int64_t>(result));
    }
    else { // RECORD_TYPE_UINT
        ret = static_cast<float>(result);
    }

    IODD_DEBUG("Parsed value: type=%s, offset=%d, length=%d, value=%f", 
              type.c_str(), bitOffset, bitLength, ret);
    return true;
}

template <class T>
inline IODDStr IODD_Helper_FormatDisplay(T& value, IODDStr fmt) {
    char tmp[64];
    bool isNA = false;
    if(fmt == "Bin") {
        std::stringstream ss;
        ss << std::bitset<sizeof(T) * 8>(value);
        return ss.str();
    } else if(fmt == "Hex") {
        int valueInt = (int)value;
        snprintf(tmp, 64, "%Xh", valueInt);
    } else if(fmt == "Dec") {
        int valueInt = (int)value;
        snprintf(tmp, 64, "%d", valueInt);
    } else {
        IODDStrVect fmtVec = SPLIT(fmt, '.');
        if(fmtVec.size() == 2) {
            if(fmtVec.at(0) == "Dec") {
                int precision = STR2INT(fmtVec.at(1));
                snprintf(tmp, 64, "%0.*f", precision, (double)value);
            } else {
                snprintf(tmp, 64, "%f", (double)value);
            }
        } else {
            isNA = true;
        }
    }
    IODDStr str(tmp);
    if(isNA) {
        str = "0";
    }
    // printf("DISPLAY_FORMAT %s - isNA %d fmt %s ", str.c_str(), isNA, fmt.c_str());
    return str;
}

#endif // IODD_HELPER_H